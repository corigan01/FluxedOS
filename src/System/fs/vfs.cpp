/*
 *       ______            __ __                 __
 *      / __/ /_ ____ __  / //_/__ _______  ___ / /
 *     / _// / // /\ \ / / ,< / -_) __/ _ \/ -_) /
 *    /_/ /_/\_,_//_\_\ /_/|_|\__/_/ /_//_/\__/_/
 *
 *   copyright (c) 2021 Gavin Kellam (aka corigan01)
 *
 *   Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"),
 *   to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense,
 *   and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 *   The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *   WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *
 */

#include "vfs.hpp"
#include <lib/vector/KernelVector.hpp>
#include <System/fs/ext2.hpp>

using namespace System;

K_Vector<fs::fs_node_t> NodeList;
const char* root_mount;

void System::fs::init(const char* root_mount_location) {
    root_mount = root_mount_location;

    NodeList.ChangePointer(Memory::kmalloc(sizeof(fs::fs_node_t) * 1010));
}

void System::fs::add_node(System::fs::fs_node_t node) {

    kout << "Added fs node!" << endl;

    if (ext2::test_node(node)) {
        node.fs_type = fs::fs_node_t::EXT2;
    }

    NodeList.push_back(node);
}

void System::fs::remove_node(const char *mount_point) {
    for (int i = 0; i < NodeList.size(); i ++) {
        if (NodeList[i].mount_point == mount_point) {
            NodeList.pop_at(i);
            break;
        }
    }
}

void fs::CreateDir(fs::dir_t dir) {

}

void fs::DeleteDir(fs::dir_t dir) {

}

// path must in the form of "/file1/file2/file3"
// root path must also still input "/"
K_Vector<fs::dir_t> fs::ListDirectories(fs::dir_t parent) {
    ASSERT(strlen(parent));
    ASSERT(parent[0] == '/');
    ASSERT(parent[strlen(parent) - 1] == '/')

    // Get the node that is mounted closest to the parent
    // If there is only one node that is mounted then it will be returned,
    // but if there are multiple nodes with mount points inside other
    // directories, then we need to find the node that is the highest up the
    // chain of mounted disks.
    auto node = fs::GetParentNode(parent);

    // Now we need to parse the parent for separators ('/')
    K_Vector<int> separators;

    // So we go through the array looking for any / characters, and then we
    // append the index at which they were found.
    for (int i = 0; i < strlen(parent); i++) {
        if (parent[i] == '/')
            separators.push_back(i);
    }
    separators.push_back(strlen(parent) - 1);

    // This is where things get interesting, we now need to split the dir
    // into little parts. Each part is the part inside the '/'.
    // An example of this would be as follows:
    // &> input  = "/Home/User/Test/"
    // &> output = {"Home", "User", "Test"}
    K_Vector<char*> directoryStrings;
    for (int i = 0; i < separators.size() - 1; i++) {
        size_t betweenSeperators = separators[i + 1] - separators[i];

        if (betweenSeperators > 0) {
            char* string = (char*) Memory::kmalloc(betweenSeperators + 1);
            string[betweenSeperators] = '\0';

            for (int e = separators[i]; e < separators[i + 1] - 1; e++) {
                string[e - separators[i]] = parent[e + 1];
            }

            directoryStrings.push_back(string);
        }
    }

    // Now that we have the directories in little bite size pieces, we
    // can ask the file system driver if it has any directories with the
    // same name.
    //
    // This gets a little complex because we need to check this same
    // process for each little piece we have. Once we check a piece,
    // we then check if the next piece is inside the previous piece's
    // directory.
    //
    // An example of what this snippet does can be described as follows:
    // input = {"Home", "User", "Test"}
    //
    // FileSystem Root = {"Home", "etc", "root"}
    //                    "Home" <-- Found the first element
    //
    // FileSystem "Home" = {"..", "Test", "User"}
    //                                    "User" <-- Found the second element
    //
    // FileSystem "User" = {"Test"}
    //                      "Test" <-- Found the last element
    //
    // FileSystem "Test" = {"MyDir", "MyFolders", "Documents"}
    //
    // Now that we have the filesystem tell us what's inside that directory,
    // we are done! We just need to return its findings :)
    //
    // output = {"MyDir", "MyFolders", "Documents"};
    K_Vector<fs::dir_t> directories;
    if (node.fs_type == fs::fs_node_t::EXT2) {
        auto workingDirectory = ext2::get_root_directory(node);
        bool FoundDirectory = false;


        // Just return the root directory
        if (directoryStrings.size() == 0 && strlen(parent) == 1) {
            FoundDirectory = true;
        }
        else {

            // Goes through each piece of what we put into 'parent'
            for (int i = 0; i < directoryStrings.size(); i++) {

                // Looks to find if any of the actual files looks like
                // the piece we put into our 'directoryStrings'
                for (int e = 0; e < workingDirectory.size(); e++) {
                    if (strcmp(workingDirectory[e]->name, directoryStrings[i]) == 0) {
                        FoundDirectory = true;

                        auto newdir = ext2::get_directories(node, workingDirectory[e]);

                        // Make sure to clean everything up
                        for (int f = 0; f < workingDirectory.size(); f++) {
                            Memory::kfree(workingDirectory[f]->name);
                            Memory::kfree(workingDirectory[f]);
                        }
                        workingDirectory.delete_all();

                        // Copy over the new vector
                        for (int f = 0; f < newdir.size(); f++ ) {
                            workingDirectory.push_back(newdir[f]);
                        }

                        break;
                    }
                }
            }
        }

        // Now just return the names
        if (FoundDirectory) {
            for (int i = 0; i < workingDirectory.size(); i++) {
                char* name = (char *)(Memory::kmalloc(workingDirectory[i]->nameleng + 1));
                name[workingDirectory[i]->nameleng] = '\0';
                memcpy(name, workingDirectory[i]->name, workingDirectory[i]->nameleng + 1);

                directories.push_back(name);
            }
        }

        // Some more cleanup
        for (int i = 0; i < workingDirectory.size(); i++) {
            Memory::kfree(workingDirectory[i]->name);
            Memory::kfree(workingDirectory[i]);
        }
    }

    // our last and final cleanup
    for (int i = 0; i < directoryStrings.size(); i++) {
        Memory::kfree(directoryStrings[i]);
    }

    // finally, return the nice little vector we made
    return directories;
}

fs::fs_node_t fs::GetParentNode(const char *path) {
    auto closest_path = fs::GetRootNode();

    for (size_t i = 0; i < NodeList.size(); i++) {
        auto &node = NodeList[i];

        ASSERT(closest_path.mount_point);
        ASSERT(node.mount_point);

        if (strcmp(closest_path.mount_point, path) < strcmp(node.mount_point, path))
            closest_path = node;
    }

    return closest_path;
}

K_Vector<fs::File> fs::ListAllFiles(fs::dir_t parent) {
    return {};
}

void fs::CreateFile(fs::dir_t parent, const char *name) {

}

fs::fs_node_t fs::GetRootNode() {
    for (size_t i = 0; i < NodeList.size(); i++) {
        if (strcmp(NodeList[i].mount_point, "/") == 0)
            return NodeList[i];
    }

    return {};
}


void fs::File::operator<<(const char *data) {

}

void fs::File::operator>>(void *data) {

}

void fs::File::RemoveAllContents() {

}

void fs::File::AppendData(fs::data_block_t data) {

}

void fs::File::WriteEntireFile(fs::data_block_t data) {

}

fs::data_block_t fs::File::ReadEntireFile() {
    return {};
}

fs::data_block_t fs::File::ReadBlock(size_t block) {
    return {};
}

fs::data_block_t fs::File::ReadLine(size_t line) {
    return {};
}

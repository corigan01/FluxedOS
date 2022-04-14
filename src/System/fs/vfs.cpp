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
using namespace fs;
using namespace vfs;

K_Vector<void*> prv_data;
K_Vector<void*> RequestServer;
K_Vector<fs::fs_node_t> nodes;

const char* root_mount;

void System::fs::init(const char* root_mount_location) {
    root_mount = root_mount_location;

    nodes.construct_pointer();
    RequestServer.construct_pointer();
    prv_data.construct_pointer();
}

void System::fs::remove_node(const char *mount_point) {
    for (int i = 0; i < nodes.size(); i ++) {
        if (nodes[i].mount_point == mount_point) {
            auto response = vfs::RunRequest(i, vfs::request::DISCONNECT);

            if (response.responseStatus == vfs::request::OK) {
                kout << "Safely removed node!" << endl;
                nodes.pop_at(i);
            }
            else {
                kout << "Node not removed!" << endl;
            }
            break;
        }
    }
}

void fs::CreateDir(fs::dir_t dir) {

}

void fs::DeleteDir(fs::dir_t dir) {

}

K_Vector<char*> fs::PathToVector(fs::dir_t path) {
    // Now we need to parse the parent for separators ('/')
    K_Vector<size_t> separators;

    // So we go through the array looking for any '/' characters, and then we
    // append the index at which they were found.
    for (int i = 0; i < strlen(path); i++) {
        if (path[i] == '/')
            separators.push_back(i);
    }
    separators.push_back(strlen(path) - 1);

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

            for (size_t e = separators[i]; e < separators[i + 1] - 1; e++) {
                string[e - separators[i]] = path[e + 1];
            }

            directoryStrings.push_back(string);
        }
    }


    return directoryStrings;
}

// path must in the form of "/file1/file2/file3"
// root path must also still input "/"
K_Vector<fs::dir_t> fs::ListEntires(fs::dir_t parent) {
    ASSERT(strlen(parent));
    ASSERT(parent[0] == '/');
    ASSERT(parent[strlen(parent) - 1] == '/')

    kout << "Listing Entires" << endl;

    auto node = GetChildNode(parent);
    auto request = GetChildLocalPath(parent);

    kout << "Local Path of node: \'" << (char*)request.storage << "\'" << endl;

    auto rep = vfs::RunRequest(node.index, vfs::request::LIST_ENTRIES, request);

    K_Vector<fs::dir_t> dir;

    if (rep.responseStatus == vfs::request::OK) {
        auto unpacked_response = (request::buffer_t*)rep.buffer.storage;

        for (int i = 0; i < rep.buffer.size; i++) {
            auto unpacked_entry = unpacked_response[i];
            dir.push_back((fs::dir_t)unpacked_entry.storage);
        }

        Memory::kfree(unpacked_response);
    }

    Memory::kfree(rep.buffer.storage);
    Memory::kfree(request.storage);

    return dir;
}

fs::fs_node_t fs::GetChildNode(path_t path) {
    auto closest_path = fs::GetRootNode();

    for (size_t i = 0; i < nodes.size(); i++) {

        auto node = nodes[i];

        ASSERT(closest_path.mount_point);
        ASSERT(node.mount_point);

        if (strcmp(closest_path.mount_point, path) < strcmp(node.mount_point, path))
            closest_path = node;
    }

    return closest_path;
}

void fs::CreateFile(fs::dir_t parent, const char *name) {

}

fs::fs_node_t fs::GetRootNode() {
    ASSERT(nodes.size());

    for (size_t i = 0; i < nodes.size(); i++) {
        if (strcmp(nodes[i].mount_point, "/") == 0)
            return nodes[i];
    }

    return {};
}



fs::File fs::OpenFile(fs::dir_t path) {
    ASSERT(strlen(path));
    ASSERT(path[0] == '/');

    // Get the node that is mounted closest to the parent
    // If there is only one node that is mounted then it will be returned,
    // but if there are multiple nodes with mount points inside other
    // directories, then we need to find the node that is the highest up the
    // chain of mounted disks.
    auto node = fs::GetChildNode(path);

    auto directoryString = PathToVector(path);


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

u32 fs::File::GetFileSize() {
    return 0;
}

const char *fs::File::GetFileName() {
    return nullptr;
}

fs::dir_t fs::File::GetPath() {
    return nullptr;
}


vfs::vfs_response_t System::fs::vfs::RunRequest(size_t index, vfs::request::type request, vfs::request::buffer_t buffer) {
    auto private_data = prv_data[index];
    auto server_func = RequestServer[index];
    auto server_node = nodes[index];

    kout << "[VFS]: Calling Handler!" << endl;

    auto handler = (System::fs::vfs::vfs_response_t (*)(
            void*, fs_node_t, request::type, request::buffer_t))(server_func);


    return handler(private_data, server_node, request, buffer);
}

K_Vector<void *> &vfs::get_prv_data() {
    return prv_data;
}

K_Vector<void *> &vfs::get_RequestServer() {
    return RequestServer;
}

K_Vector<fs::fs_node_t> &vfs::get_nodes() {
    return nodes;
}

request::buffer_t vfs::GetChildLocalPath(dir_t parent) {
    auto node = fs::GetChildNode(parent);

    request::buffer_t request;

    request.size = (strlen(parent) - strlen(node.mount_point)) + 1;
    request.storage = (u8*)Memory::kmalloc(request.size + 1);

    for (size_t i = strlen(node.mount_point) - 1; i < strlen(parent); i++) {
        request.storage[i - (strlen(node.mount_point) - 1)] = parent[i];
    }

    // Make sure the first and last element are '/'
    request.storage[0] = '/';
    request.storage[request.size - 1] = '/';

    // Close the string with a null char
    request.storage[request.size] = '\0';

    return request;
}

vfs::vfs_file_info_t vfs::get_file_info(dir_t path) {
    ASSERT(strlen(path));
    ASSERT(path[0] == '/');
    ASSERT(path[strlen(path) - 1] == '/')

    kout << "Listing Entires" << endl;

    auto node = GetChildNode(path);
    auto request = GetChildLocalPath(path);

    kout << "Local Path of node: \'" << (char*)request.storage << "\'" << endl;

    auto rep = vfs::RunRequest(node.index, vfs::request::GET_ENTRY, request);

    return {};
}

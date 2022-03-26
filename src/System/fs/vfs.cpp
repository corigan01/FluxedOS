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

K_Vector<fs::dir_t> fs::ListDirectories(fs::dir_t parent) {
    auto node = fs::GetParentNode(parent);

    if (node.fs_type == fs::fs_node_t::EXT2) {

    }
}

fs::fs_node_t fs::GetParentNode(const char *path) {
    auto closest_path = fs::GetRootNode();

    for (size_t i = 0; i < NodeList.size(); i++) {
        auto &node = NodeList[i];

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
        if (strcmp(NodeList[i].mount_point, "/") != 0)
            return NodeList[i];
    }

    return {};
}


void fs::File::operator<<(const char *data) {

}

void fs::File::operator>>(void *data) {

}

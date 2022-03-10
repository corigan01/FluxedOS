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
    ext2::test_node(node);
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

System::fs::file_t System::fs::ReadFile(const char *loc) {
    return System::fs::file_t();
}

void System::fs::WriteFile(System::fs::file_t file) {

}

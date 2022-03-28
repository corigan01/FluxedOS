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

#pragma once

#include <lib/core/core.h>
#include <System/Disk/vDisk.hpp>
#include <lib/vector/k_vector/k_vector.hpp>

#include "mbt.hpp"

namespace System {
    namespace fs {

        typedef struct {
            partition_t *partition;
            enum FS_TYPE {
                EXT2 = 0
            } fs_type;
            char* mount_point;
        } fs_node_t;

        typedef char* path_t;
        typedef path_t dir_t;

        typedef struct {
            u8* block;
            u32 size;
        } data_block_t;

        class File {
        private:
            u32 size;
            char* name;
            dir_t parent;

        public:
            void operator <<(const char* data);
            void operator >>(void* data);

            data_block_t ReadLine(size_t line);
            data_block_t ReadBlock(size_t block);
            data_block_t ReadEntireFile();

            void WriteEntireFile(data_block_t data);
            void AppendData(data_block_t data);

            void RemoveAllContents();

            u32 GetFileSize();
            const char* GetFileName();
            dir_t GetPath();
        };

        void init(const char* root_mount_location);

        fs_node_t GetRootNode();
        fs_node_t GetParentNode(path_t path);

        K_Vector<char*> PathToVector(path_t path);

        K_Vector<dir_t> ListEntires(path_t path);


        File OpenFile(dir_t path);

        void add_node(fs_node_t node);
        void remove_node(const char* mount_point);

        void CreateDir(dir_t dir);
        void DeleteDir(dir_t dir);

        void CreateFile(dir_t parent, const char* name);

        namespace vfs {
            namespace response {
                enum status {
                    ERROR = -1,
                    OK = 0,
                    WAIT = 1,
                    DOES_NOT_EXIST = 2
                };

                typedef struct {
                    u8* storage;
                    size_t size;
                } buffer_t;

                enum type {
                    LIST_ENTRIES = 0,
                    GET_ENTRY,
                    MKDIR,
                    TOUCH,
                    APPEND,
                    WRITE,
                    READ
                };
            }

            typedef struct {
                response::status responseStatus;
                response::buffer_t buffer;
            } vfs_response_t;

            template<class T, typename server>
            bool AddNode(server s) {

            }
        }
        namespace ext2 {
            vfs::vfs_response_t FileSystemServer (void* prv_data, fs_node_t node, vfs::response::type request,
                                                  vfs::response::buffer_t buffer = {.size = 0}) {

            }

            typedef struct {
                u8 example_data_that_ext2_would_need_to_store_in_a_buffer_to_make_it_work;
            } file_system_info;
        }




        void test() {
            vfs::AddNode<ext2::file_system_info>(ext2::FileSystemServer);
        }


    }
}
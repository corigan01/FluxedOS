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
            size_t index = 0;
        } fs_node_t;

        typedef char* path_t;
        typedef path_t dir_t;
        typedef path_t file_name_t;

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
        fs_node_t GetChildNode(path_t path);

        K_Vector<char*> PathToVector(path_t path);

        K_Vector<dir_t> ListEntires(path_t path);


        File OpenFile(dir_t path);

        void add_node(fs_node_t node);
        void remove_node(const char* mount_point);

        void CreateDir(dir_t dir);
        void DeleteDir(dir_t dir);

        void CreateFile(dir_t parent, const char* name);

        namespace vfs {
            namespace request {
                enum status {
                    ERROR = -1,
                    OK = 0,
                    WAIT = 1,
                    DOES_NOT_EXIST = 2,
                    VALID = 3,
                    BAD_REQUEST = 4,
                    NOT_VALID = 5
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
                    READ,
                    INIT,
                    DISCONNECT
                };

                enum file_type {
                    DIR = 0,
                    FILE,
                    DISK,
                    VDEV,
                    UNKNOWN
                };
            }

            typedef struct {
                file_name_t fileName;
                size_t size;
                path_t path;
                request::file_type fileType;
            } vfs_file_info_t;

            typedef struct {
                request::status responseStatus;
                request::buffer_t buffer;
            } vfs_response_t;



            vfs_response_t RunRequest(size_t index, vfs::request::type request, vfs::request::buffer_t buffer = {});

            K_Vector<void*>& get_prv_data();
            K_Vector<void*>& get_RequestServer();
            K_Vector<fs::fs_node_t>& get_nodes();

            request::buffer_t GetChildLocalPath(dir_t parent);

            vfs_file_info_t get_file_info(dir_t path);

            template<class T, typename server>
            bool AddNode(server s, fs_node_t node) {
                vfs::vfs_response_t (*handler)(void *prv_data, fs_node_t node, vfs::request::type request,
                        vfs::request::buffer_t buffer) = nullptr;

                auto &RequestServer = get_RequestServer();
                auto &prv_data = get_prv_data();
                auto &nodes = get_nodes();

                kout << endl;

                if ( (u32)((void*)s) > 0 ) {
                    handler = s;

                    auto private_data_buffer = Memory::kmalloc(sizeof(T));

                    kout << "[VFS]: Adding node to DB." << endl;

                    RequestServer.push_back((void*)handler);
                    prv_data.push_back(private_data_buffer);

                    node.index = nodes.size();

                    nodes.push_back(node);

                    kout << "[VFS]: Running init request on FS." << endl;

                    auto response = vfs::RunRequest(RequestServer.size() - 1, request::INIT);

                    if (response.responseStatus == request::VALID) {
                        kout << kout.GREEN << "[VFS]: Valid Node added!" << kout.YELLOW << endl;

                        return true;
                    }
                    else {
                        kout << kout.RED << "[VFS]: File System did not respond with a \'VALID\' response!" << kout.YELLOW << endl;

                        return false;
                    }
                }

                return false;
            }
        }



    }
}
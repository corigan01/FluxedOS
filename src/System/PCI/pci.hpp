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
#include <System/kout/kout.hpp>

// Ports
#define PCI_CONFIG_ADDRESS  0xCF8
#define PCI_CONFIG_DATA     0xCFC

// Config Address Register

// Offset
#define PCI_VENDOR_ID            0x00
#define PCI_DEVICE_ID            0x02
#define PCI_COMMAND              0x04
#define PCI_STATUS               0x06
#define PCI_REVISION_ID          0x08
#define PCI_PROG_IF              0x09
#define PCI_SUBCLASS             0x0a
#define PCI_CLASS                0x0b
#define PCI_CACHE_LINE_SIZE      0x0c
#define PCI_LATENCY_TIMER        0x0d
#define PCI_HEADER_TYPE          0x0e
#define PCI_BIST                 0x0f
#define PCI_BAR0                 0x10
#define PCI_BAR1                 0x14
#define PCI_BAR2                 0x18
#define PCI_BAR3                 0x1C
#define PCI_BAR4                 0x20
#define PCI_BAR5                 0x24
#define PCI_INTERRUPT_LINE       0x3C
#define PCI_SECONDARY_BUS        0x09

// Device type
#define PCI_HEADER_TYPE_DEVICE  0
#define PCI_HEADER_TYPE_BRIDGE  1
#define PCI_HEADER_TYPE_CARDBUS 2
#define PCI_TYPE_BRIDGE 0x0604
#define PCI_TYPE_SATA   0x0106
#define PCI_NONE 0xFFFF

#define DEVICE_PER_BUS           32
#define FUNCTION_PER_DEVICE      32


namespace System
{
    namespace PCI
    {
        typedef union pci_dev {
            i32 bits;
            struct {
                i32 always_zero    : 2;
                i32 field_num      : 6;
                i32 function_num   : 3;
                i32 device_num     : 5;
                i32 bus_num        : 8;
                i32 reserved       : 7;
                i32 enable         : 1;
            };
        } PICdev_T;

        i32 read(PICdev_T dev, i32 field);
        void write(PICdev_T dev, i32 field, i32 value);

        i32 get_device_type(PICdev_T dev);
        i32 get_secondary_bus(PICdev_T dev);
        i32 reach_end(PICdev_T dev);

        PICdev_T scan_function(i16 vendor_id, i16 device_id, i32 bus, i32 device, i32 function, int device_type);
        PICdev_T scan_device(i16 vendor_id, i16 device_id, i32 bus, i32 device, int device_type);
        PICdev_T scan_bus(i16 vendor_id, i16 device_id, i32 bus, int device_type);

        PICdev_T get_device(i16 vendor_id, i16 device_id, int device_type);

        void pci_init();




    } // namespace PCI
    
} // namespace System

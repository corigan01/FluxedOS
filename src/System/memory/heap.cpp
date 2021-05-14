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

#include "heap.hpp"
#include <System/memory/pmm/pmm.hpp>

void* heapStart;
void* heapEnd;
HeapSegment* LastHdr;

using namespace System;
using namespace System::Memory;

void initHeap(void* heapAddress, size_t pageCount){
    void* pos = heapAddress;

    for (size_t i = 0; i < pageCount; i++){
        //g_PageTableManager.MapMemory(pos, GlobalAllocator.RequestPage());
        pos = (void*)((size_t)pos + 0x1000);
    }

    size_t heapLength = pageCount * 0x1000;

    heapStart = heapAddress;
    heapEnd = (void*)((size_t)heapStart + heapLength);
    HeapSegment* startSeg = (HeapSegment*)heapAddress;
    startSeg->length = heapLength - sizeof(HeapSegment);
    startSeg->next = NULL;
    startSeg->last = NULL;
    startSeg->free = true;
    LastHdr = startSeg;
}

void free(void* address){
    HeapSegment* segment = (HeapSegment*)address - 1;
    segment->free = true;
    segment->CombineForward();
    segment->CombineBackward();
}

void* malloc(size_t size){
    if (size % 0x10 > 0){ // it is not a multiple of 0x10
        size -= (size % 0x10);
        size += 0x10;
    }

    if (size == 0) return NULL;

    HeapSegment* currentSeg = (HeapSegment*) heapStart;
    while(true){
        if(currentSeg->free){
            if (currentSeg->length > size){
                currentSeg->Split(size);
                currentSeg->free = false;
                return (void*)((uint64)currentSeg + sizeof(HeapSegment));
            }
            if (currentSeg->length == size){
                currentSeg->free = false;
                return (void*)((uint64)currentSeg + sizeof(HeapSegment));
            }
        }
        if (currentSeg->next == NULL) break;
        currentSeg = currentSeg->next;
    }
    ExpandHeap(size);
    return malloc(size);
}

HeapSegment* HeapSegment::Split(size_t splitLength){
    if (splitLength < 0x10) return NULL;
    int splitSegLength = length - splitLength - (sizeof(HeapSegment));
    if (splitSegLength < 0x10) return NULL;

    HeapSegment* newSplitHdr = (HeapSegment*) ((size_t)this + splitLength + sizeof(HeapSegment));
    next->last = newSplitHdr; // Set the next segment's last segment to our new segment
    newSplitHdr->next = next; // Set the new segment's next segment to out original next segment
    next = newSplitHdr; // Set our new segment to the new segment
    newSplitHdr->last = this; // Set our new segment's last segment to the current segment
    newSplitHdr->length = splitSegLength; // Set the new header's length to the calculated value
    newSplitHdr->free = free; // make sure the new segment's free is the same as the original
    length = splitLength; // set the length of the original segment to its new length

    if (LastHdr == this) LastHdr = newSplitHdr;
    return newSplitHdr;
}

void ExpandHeap(size_t length){
    if (length % 0x1000) {
        length -= length % 0x1000;
        length += 0x1000;
    }

    size_t pageCount = length / 0x1000;
    HeapSegment* newSegment = (HeapSegment*)heapEnd;

    for (size_t i = 0; i < pageCount; i++){
        //g_PageTableManager.MapMemory(heapEnd, GlobalAllocator.RequestPage());
        heapEnd = (void*)((size_t)heapEnd + 0x1000);
    }

    newSegment->free = true;
    newSegment->last = LastHdr;
    LastHdr->next = newSegment;
    LastHdr = newSegment;
    newSegment->next = NULL;
    newSegment->length = length - sizeof(HeapSegment);
    newSegment->CombineBackward();

}

void HeapSegment::CombineForward(){
    if (next == NULL) return;
    if (!next->free) return;

    if (next == LastHdr) LastHdr = this;

    if (next->next != NULL){
        next->next->last = this;
    }

    

    length = length + next->length + sizeof(HeapSegment);

    next = next->next;
}

void HeapSegment::CombineBackward(){
    if (last != NULL && last->free) last->CombineForward();
}

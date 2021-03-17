#ifndef LIST__H___
#define LIST__H___

#include "../core/core.h"
#include "../VGA/VGA.h"
#include "../IO/port.h"
#include "../mem/mem.h"

typedef struct listnode {
	struct listnode * prev;
	struct listnode * next;
	void * val;
}listnode_t;

typedef struct list{
	listnode_t * head;
	listnode_t * tail;
	uint32_t size;
}list_t;

#define foreach(t, list) for(listnode_t * t = list->head; t != NULL; t = t->next)

namespace LIST {
    list_t * create();
    uint32_t size(list_t * list);

    listnode_t * InsertFront(list_t * list, void * val);
    void InsertBack(list_t * list, void * val);

    void * RemoveNode(list_t * list, listnode_t * node);
    void * RemoveFront(list_t * list);
    void * RemoveBack(list_t * list);

    void push(list_t * list, void * val);
    listnode_t * pop(list_t * list);
    void enqueue(list_t * list, void * val);
    listnode_t * dequeue(list_t * list);

    void * PeekFront(list_t * list);
    void * PeekBack(list_t * list);

    void destroy(list_t * list);
    void NodeDestroy(listnode_t * node);

    int contain(list_t * list, void * val);
    listnode_t * GetByIndex(list_t * list, int index);
    void * RemoveByIndex(list_t * list, int index);
}




#endif // !LIST__H___
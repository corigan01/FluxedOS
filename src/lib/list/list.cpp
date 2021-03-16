#include "list.h"

list_t * LIST::create() {
	list_t * list = (list_t *)malloc(sizeof(list_t) * 4096);
	return list;
}

/*
 * Get list size
 * */
uint32_t LIST::size(list_t * list) {
    if(!list) return 0;
	return list->size;
}

/*
 * Given a listnode, remove it from lis
 * */
void * LIST::RemoveNode(list_t * list, listnode_t * node) {
    void * val = node->val;
    if(list->head == node)
        return LIST::RemoveFront(list);
    else if(list->tail == node)
        return LIST::RemoveBack(list);
    else {
        node->next->prev = node->prev;
        node->prev->next = node->next;
        list->size--;
        free(node);
    }
    return val;
}
/*
 * Insert a value at the front of list
 * */
listnode_t * LIST::InsertFront(list_t * list, void * val) {
	listnode_t * t = (listnode_t *)malloc(sizeof(listnode_t) * 1);
	list->head->prev = t;
    t->next = list->head;
	t->val = val;

	// If it's the first element, then it's both head and tail
	if(!list->head)
		list->tail = t;

	list->head = t;
	list->size++;
	return t;
}

/*
 * Insert a value at the back of list
 * */
void LIST::InsertBack(list_t * list, void * val) {
	listnode_t * t = (listnode_t *)malloc(sizeof(listnode_t) * 1);
	t->prev = list->tail;
    if(list->tail)
        list->tail->next = t;
	t->val = val;

	if(!list->head)
		list->head = t;

	list->tail = t;
	list->size++;
}

/*
 * Remove a value at the front of list
 * */
void * LIST::RemoveFront(list_t * list) {
	if(!list->head) return (void*)0;
	listnode_t * t = list->head;
    void * val = t->val;
	list->head = t->next;
	if(list->head)
		list->head->prev = NULL;
	free(t);
	list->size--;
    return val;
}

/*
 * Remove a value at the back of list
 * */
void * LIST::RemoveBack(list_t * list) {
	if(!list->head) return(void*)0;
	listnode_t * t = list->tail;
    void * val = t ->val;
	list->tail = t->prev;
	if(list->tail)
		list->tail->next = NULL;
	free(t);
	list->size--;
    return val;
}

/*
 * Insert after tail of list(same as insert back)
 * */
void LIST::push(list_t * list, void * val) {
	LIST::InsertBack(list, val);
}

/*
 * Remove and return tail of list(user is responsible for freeing the returned node and the value)
 * */
listnode_t * LIST::pop(list_t * list) {
	if(!list->head) return NULL;
	listnode_t * t = list->tail;
	list->tail = t->prev;
	if(list->tail)
		list->tail->next = NULL;
	list->size--;
	return t;
}

/*
 * Insert before head of list(same as insert front)
 * */
void LIST::enqueue(list_t * list, void * val) {
	LIST::InsertFront(list, val);
}

/*
 * Remove and return tail of list(same as LIST::pop
 * */
listnode_t * LIST::dequeue(list_t * list) {
	return LIST::pop(list);
}

/*
 * Get the value of the first element but not remove it
 * */
void * LIST::PeekFront(list_t * list) {
	if(!list->head) return NULL;
	return list->head->val;
}

/*
 * Get the value of the last element but not remove it
 * */
void * LIST::PeekBack(list_t * list) {
	if(!list->tail) return NULL;
	return list->tail->val;
}


/*
 * Does the list contain a value
 * Return -1 if list element is not found
 * */

int LIST::contain(list_t * list, void * val) {
    int idx = 0;
    foreach(listnode, list) {
        if(listnode->val == val)
            return idx;
        idx++;
    }
    return -1;
}

listnode_t * LIST::GetByIndex(list_t * list, int index) {
    if(index < 0 || index >= LIST::size(list))
        return NULL;
    int curr = 0;
    foreach(listnode, list) {
        if(index == curr) return listnode;
        curr++;
    }
    return NULL;
}

void * LIST::RemoveByIndex(list_t * list, int index) {
    listnode_t * node = LIST::GetByIndex(list, index);
    return LIST::RemoveNode(list, node);
}

void LIST::destroy(list_t * list) {
	// Free each node's value and the node itself
	listnode_t * node = list->head;
	while(node != NULL) {
		listnode_t * save = node;
		node = node->next;
		free(save);
	}
	// Free the list
	free(list);
}

void LIST::NodeDestroy(listnode_t * node) {
	free(node);
}

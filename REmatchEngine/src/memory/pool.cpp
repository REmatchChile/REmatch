#include "pool.hpp"

#include "structures.hpp"

// Template specialization for Node datastruct
template<>
void Pool<Node>::update_free_list() {
	Node *listHead, *adyacentNext;

	// Pointer labeling
	listHead = free_head_->start;
	adyacentNext = free_head_->next;

	// Append to freelist new garbage
	if(listHead != nullptr && listHead->refCount == 0 && !listHead->isNodeEmpty()) {
		listHead->nextFree = free_head_->nextFree;
		free_head_->nextFree = listHead;
	}
	if(adyacentNext != nullptr && adyacentNext->refCount == 0 && !listHead->isNodeEmpty()) {
		adyacentNext->nextFree = free_head_->nextFree;
		free_head_->nextFree = adyacentNext;
	}
}
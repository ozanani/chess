#include "ArrayList.h"

ArrayList* arrayListCreate(int maxSize) {
	if (maxSize <= 0) return NULL;

	ArrayList *al = (ArrayList*)(malloc(sizeof(ArrayList)));
	if ((void*)al == NULL) return NULL;

	al->elements = (HistoryElement*)(malloc(sizeof(HistoryElement) * maxSize));
	if ((void*)(al->elements) == NULL) {
		free(al);
		return NULL;
	}

	al->maxSize = maxSize;
	al->actualSize = 0;

	return al;
}


/*
* Copies array elements to other array.
* Assumes number_of_elements < dst.length, src.length
*/

/*
static void copyElements(int* src, int* dst, int number_of_elements) {
	int i;

	for (i = 0; i < number_of_elements; i++) {
		dst[i] = src[i];
	}
}

SPArrayList* spArrayListCopy(SPArrayList* src) {
	if ((void*)src == NULL) {
		return NULL;
	}

	SPArrayList *al = (SPArrayList*)(malloc(sizeof(SPArrayList)));
	if ((void*)al == NULL) {
		return NULL;
	}

	al->elements = (int*)(malloc(sizeof(int) * src->maxSize));
	if ((void*)(al->elements) == NULL) {
		free(al);
		return NULL;
	}

	copyElements(src->elements, al->elements, src->actualSize);

	al->maxSize = src->maxSize;
	al->actualSize = src->actualSize;

	return al;
}
*/

void arrayListDestroy(ArrayList* src) {
	if ((void*)src == NULL) return;

	free(src->elements);
	free(src);
}

/*
SP_ARRAY_LIST_MESSAGE spArrayListClear(SPArrayList* src) {
	int i;

	if ((void*)src == NULL) {
		return ARRAY_LIST_INVALID_ARGUMENT;
	}

	// clear the array
	for (i = 0; i < src->actualSize; i++) {
		(src->elements)[i] = 0;
	}

	src->actualSize = 0;

	return ARRAY_LIST_SUCCESS;
}
*/

ARRAY_LIST_MESSAGE arrayListAddAt(ArrayList* src, HistoryElement elem, int index) {
	if ((void*)src == NULL || index < 0 || index > src->actualSize) {
		return ARRAY_LIST_INVALID_ARGUMENT;
	}

	if (src->actualSize == src->maxSize) return ARRAY_LIST_FULL;

	// shift the array!
	for (int i = src->actualSize - 1; i >= index; i--) {
		(src->elements)[i + 1] = (src->elements)[i];
	}

	// insert new val
	(src->elements)[index] = elem;
	(src->actualSize)++;

	return ARRAY_LIST_SUCCESS;
}

ARRAY_LIST_MESSAGE arrayListAddFirst(ArrayList* src, HistoryElement elem) {
	return arrayListAddAt(src, elem, 0);
}

ARRAY_LIST_MESSAGE arrayListAddLast(ArrayList* src, HistoryElement elem) {
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

	if (src->actualSize == src->maxSize) return ARRAY_LIST_FULL;

	return arrayListAddAt(src, elem, src->actualSize);
}

ARRAY_LIST_MESSAGE arrayListRemoveAt(ArrayList* src, int index) {
	if ((void*)src == NULL || index < 0 || index >= src->actualSize) {
		return ARRAY_LIST_INVALID_ARGUMENT;
	}

	// redundant but specified 
	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

	// shift the array!
	for (int i = index + 1; i < src->actualSize; i++) {
		(src->elements)[i - 1] = (src->elements)[i];
	}

	(src->actualSize)--;

	return ARRAY_LIST_SUCCESS;
}

ARRAY_LIST_MESSAGE arrayListRemoveFirst(ArrayList* src) {
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

	return arrayListRemoveAt(src, 0);
}

ARRAY_LIST_MESSAGE arrayListRemoveLast(ArrayList* src) {
	if ((void*)src == NULL) return ARRAY_LIST_INVALID_ARGUMENT;

	if (src->actualSize == 0) return ARRAY_LIST_EMPTY;

	return arrayListRemoveAt(src, src->actualSize - 1);
}

HistoryElement arrayListGetAt(ArrayList* src, int index) {
	if ((void*)src == NULL || index < 0 || index >= src->actualSize) {
		return (HistoryElement) { .oldSquare = { -1, -1 } };
	}

	return src->elements[index];
}

HistoryElement arrayListGetFirst(ArrayList* src) {
	return arrayListGetAt(src, 0);
}

HistoryElement arrayListGetLast(ArrayList* src) {
	if ((void*)src == NULL) return (HistoryElement) { .oldSquare = { -1, -1 } };

	return arrayListGetAt(src, src->actualSize - 1);
}

int arrayListMaxCapacity(ArrayList* src) {
	if ((void*)src == NULL) return -1;

	return src->maxSize;
}

int arrayListSize(ArrayList* src) {
	if ((void*)src == NULL) return -1;

	return src->actualSize;
}

bool arrayListIsFull(ArrayList* src) {
	if ((void*)src == NULL) return false;

	return src->actualSize == src->maxSize;
}

bool arrayListIsEmpty(ArrayList* src) {
	if ((void*)src == NULL) return false;

	return src->actualSize == 0;
}
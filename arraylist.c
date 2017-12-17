#include"arraylist.h"
#include "filesystem.h"
#include"trackedmemory.h"
#include<stdio.h>

typedef struct _arraylist AL;
struct _arraylist{
  List li;
  void ** A;
  size_t max;
  size_t size;
  size_t start;
};

List * ALprepend(List*, void*);
List * ALappend(List*, void*);
List * ALremovefirst(List*, void**);
List * ALremovelast(List*, void**);
List * ALset(List*, size_t, void*);
List * ALget(List*, size_t, void**);
void * ALgetElement(List*, size_t, void**);
fstree * ALgetFSTreeAt(List*, size_t);
List * ALinsert(List*, size_t, void*);
List * ALremove(List*, size_t, void**);
size_t ALsize(List*);
int ALalreadyExists(List*, char*);
size_t ALgetIndexOf(List*, char*);
//List * ALsort(List*, int (*comp)(void*, void*));
List * ALsort(List*);
void * ALfold(List*, void*, void* (*f)(void*,void*));
void ALprintall(List*);
void ALdestroy(List*);

List * ArrayList(size_t max){
  AL * al = (AL*) MALLOC(sizeof(AL));
  al->li.prepend = ALprepend;
  al->li.append = ALappend;
  al->li.removefirst = ALremovefirst;
  al->li.removelast = ALremovelast;
  al->li.set = ALset;
  al->li.get = ALget;
  al->li.getElement = ALgetElement;
  al->li.getFSTreeAt = ALgetFSTreeAt;
  al->li.insert = ALinsert;
  al->li.remove = ALremove;
  al->li.alreadyExists = ALalreadyExists;
  al->li.getIndexOf = ALgetIndexOf;
  al->li.size = ALsize;
  al->li.sort = ALsort;
  al->li.fold = ALfold;
  al->li.printall = ALprintall;
  al->li.destroy = ALdestroy;
  al->max = max;
  al->A = (void **)CALLOC(sizeof(void*),al->max);
  al->size = 0;
  al->start = 0;
  return (List*)al;
}

size_t alindex(AL * al, size_t i){ return (al->start + i)% al->max;}

size_t getSize(AL * al) {
	return al->size;
}

void resize(AL *al){
  if(al->size == al->max){
    size_t newmax = al->max *2;
    //printf("resizing from %lu to %lu\n", al->max, newmax);
    void ** newA = (void **)CALLOC(sizeof(void*),newmax);
    int i;
    for(i = 0; i < al->size; i++)
      newA[i] = al->A[alindex(al,i)];
    FREE(al->A);
    al->A = newA;
    al->max = newmax;
    al->start = 0;
  }
}

List * ALprepend(List*l, void*e){
  AL * al = (AL *) l;
  resize(al);
  al->start = alindex(al, -1);
  al->A[al->start] = e;
  al->size++;
  return l;
}

size_t ALgetIndexOf(List*l, char *name){
	AL * al = (AL *) l;
	size_t i = 0;
	for(i = 0; i < al->size; i++){
		//printf("getindex: comparing %s to %s\n", name, ((fstree*)(al->A[alindex(al,i)]))->name);
		if(strcmp(name, ((fstree*)(al->A[alindex(al,i)]))->name) == 0) {
			return i;
		}
	}
	return -1;
}

int ALalreadyExists(List*l, char *name){
	AL * al = (AL *) l;
	int i;
	for(i = 0; i < al->size; i++){
		if(strcmp(name, ((fstree*)al->A[alindex(al,i)])->name) == 0)
			return 1;
	}
	return 0;
}

List * ALappend(List*l, void*e){
  AL * al = (AL *) l;
  resize(al);
  al->A[alindex(al, al->size)] = e;
  al->size++;
  return l;
}

List * ALremovefirst(List*l, void**e){
  AL * al = (AL *) l;
  if(al->size == 0)
    *e = NULL;
  else {
    *e = al->A[al->start];
    al->start = alindex(al, 1);
    al->size--;
  }
  return l;
}

List * ALremovelast(List*l, void**e){
  AL * al = (AL *) l;
  if(al->size == 0)
    NULL;
  else {
    al->A[alindex(al, al->size-1)];
    al->size--;
  }
  return l;
}

List * ALset(List*l, size_t i, void*e){
  AL * al = (AL *) l;
  if(i >= al->size) return ALappend(l, e);
  al->A[alindex(al, i)] = e;
  return l;
}

fstree * ALgetFSTreeAt(List*l, size_t i){
  AL * al = (AL *) l;
  if(i >= al->size) {
    //printf("index %lu is null element on list of size %lu, returning\n", i, al->size);
    return NULL;
  } else {
    //printf("returning element\n"); 
    return (fstree*)al->A[alindex(al, i)];
  }
  //return e;
}

void * ALgetElement(List*l, size_t i, void**e){
  AL * al = (AL *) l;
  if(i >= al->size) {
    //printf("returning null element\n");
    return NULL;
  } else {
    //printf("returning element\n"); 
    return &al->A[alindex(al, i)];
  }
  //return e;
}

List * ALget(List*l, size_t i, void**e){
  AL * al = (AL *) l;
  if(i >= al->size) {
    //printf("returning null element\n");
    *e = NULL;
  } else {
    //printf("returning element\n"); 
    *e = al->A[alindex(al, i)];
  }
  return l;
}

List * ALinsert(List*l, size_t i, void*e){
  AL * al = (AL *) l;
  resize(al);
  int j;
  for(j = al->size; j > i; j--)
     al->A[alindex(al, j)] = al->A[alindex(al, j-1)];
  al->A[alindex(al, i)] = e;
  al->size++;
  return l;
}

List * ALremove(List*l, size_t i, void**e){
  AL * al = (AL *) l;
  if(i >= al->size)
    *e = NULL;
  else {
    //*e = al->A[alindex(al, i)];
    al->A[alindex(al, i)];
    al->size--;
    int j;
    for(j = i; j < al->size; j++)
      al->A[alindex(al, j)] = al->A[alindex(al, j+1)];
  }
  return l;
}

size_t ALsize(List* l){
  AL * al = (AL *) l;
  return al->size;
}

int fstreeCompare(fstree *a, fstree *b) {
	char *aname = a->name;
	char *bname = b->name;
	size_t index;
	//printf("comparing %s to %s\n", aname, bname);
	for(index = 0; index<strlen(aname); index++) {
		if(index >= strlen(bname)) { //a is longer than b
			return 1;
		}
		if(strncmp(aname+index, "\0", 1) == 0) {
			return -1;
		}
		if(strncmp(aname+index, ".", 1) == 0 && strncmp(bname+index, ".", 1) != 0) {
			return -1;
		}
		if(strncmp(aname+index, ".", 1) != 0 && strncmp(bname+index, ".", 1) == 0) {
			return 1;
		}
		if(strncmp(aname+index, "-", 1) == 0 && strncmp(bname+index, "-", 1) != 0) {
			return -1;
		}
		if(strncmp(aname+index, "-", 1) != 0 && strncmp(bname+index, "-", 1) == 0) {
			return 1;
		}
		if(strncmp(aname+index, "_", 1) == 0 && strncmp(bname+index, "_", 1) != 0) {
			return -1;
		}
		if(strncmp(aname+index, "_", 1) != 0 && strncmp(bname+index, "_", 1) == 0) {
			return 1;
		}
		if (strncmp(aname+index, bname+index, 1) != 0) {
			return strncmp(aname+index, bname+index, 1);
		}	
	}
	return -1; //ran out of characters in a, but not b
}

//List * ALsort(List*l, int (*comp)(void*, void*)){
List * ALsort(List*l){
	//expect that only the first element is unsorted
	//priority: ’.’ < ’-’ < ’_’ < (0-9) < (A-Z) < (a-z)
	AL * al = (AL *) l;
	if(al->size < 2)
		return l;
	fstree* first = (fstree*)al->A[alindex(al, 0)];
	size_t x;
	for(x = 1; x < al->size; x++) {
		fstree *candidate = (fstree*)al->A[alindex(al,x)];
		int diff = fstreeCompare(first, candidate);
		//printf("returned diff of %d\n", diff);
		if(diff < 0) {
			//printf("1. setting %lu to %s\n", x-1, first->name);
			//l->set(l,x-1,first);
			break;
		} else {
			//printf("2. setting %lu to %s\n", x-1, candidate->name);
			l->set(l,x-1,candidate);
		}
	}
	l->set(l,x-1,first);
	//x is the position we need to insert candidate at
	//printf("need to insert first element into position %lu\n", x);
	return l;
}

void ALprintall(List*l){
  AL * al = (AL *) l;
  int i;
  for(i = 0; i < al->size; i++){
    printf("%s\n", ((fstree*)al->A[alindex(al,i)])->name);
  }
}

void * ALfold(List*l, void*a, void* (*f)(void*,void*)){
  AL * al = (AL *) l;
  int i;
  for(i = 0; i < al->size; i++){
    a  = f(a, al->A[alindex(al,i)]);
  }
  return a;
}

void ALdestroy(List*l){
  //printf("list destroy called\n");
  AL * al = (AL *) l;
  //printf("casting list\n");
  FREE(al->A);
  //printf("array in list freed\n");
  FREE(al);
  //printf("list freed; returning\n");
}



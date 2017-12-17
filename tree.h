#ifndef ___BSTREE_HEADER__
#define ___BSTREE_HEADER__

#include "arraylist.h"
#include "common.h"
#include "element.h"
#include "trackedmemory.h"

//typedef struct _fstree_interface fstree;
struct _fstree_interface{
  //fstree * (*insert)(fstree *, Element);
  //fstree * (*remove)(fstree *, Element, int*);
  //BSTree * (*inorder) (BSTree *, void*, void* (*f)(void*, Element));
  //BSTree * (*preorder) (BSTree *, void*, void* (*f)(void*, Element));
  //BSTree * (*postorder) (BSTree *, void*, void* (*f)(void*, Element));
  void (*destroy)(fstree*);
  char * (*getName)(fstree*);
  void (*printName)(fstree*);
  Element e;
  char name[21];
  fstree *parent;
  fsys *fs;
  List *childdirs;
  List *childfiles;
  //BSTree *rightNode;
};

fstree * newFSTree(char*, fstree*);

#endif

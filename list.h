#ifndef __list_h_
#define __list_h_

#include<stdlib.h>
#include "common.h"
#include "tree.h"

//typedef struct _list_interface List;
struct _list_interface{
   List * (*prepend)(List*, void*);
   List * (*append)(List*, void*);
   List * (*removefirst)(List*, void**);
   List * (*removelast)(List*, void**);
   List * (*set)(List*, size_t, void*);
   List * (*get)(List*, size_t, void**);
   void * (*getElement)(List*, size_t, void**);
   fstree * (*getFSTreeAt)(List*, size_t);
   List * (*insert)(List*, size_t, void*);
   List * (*remove)(List*, size_t, void**);
   int (*alreadyExists)(List*, char*);
   size_t (*getIndexOf)(List*, char*);
   size_t (*size)(List*);
   //List * (*sort)(List*, int (*comp)(void*, void*));
   List * (*sort)(List*);
   void * (*fold)(List*, void*, void* (*f)(void*,void*));
   void (*printall)(List*);
   void (*destroy)(List*);
};

#endif

#include "tree.h"

//BSTree * bstree_insert(BSTree*, Element);
//BSTree * bstree_remove(BSTree*, Element, int*);
//BSTree * bstree_inorder(BSTree *, void*, void* (*f)(void*, Element));
//BSTree * bstree_preorder(BSTree *, void*, void* (*f)(void*, Element));
//BSTree * bstree_postorder(BSTree *, void*, void* (*f)(void*, Element));
char * getName(fstree*);
void printName(fstree*);
void fstree_destroy(fstree*);
//void list(fstree*, Element);

fstree * newFSTree(char *name, fstree *parent) {
	fstree *bt = MALLOC(sizeof(fstree));
	//bt->insert = bstree_insert;
	//bt->remove = bstree_remove;
	//bt->inorder = bstree_inorder;
	//bt->preorder = bstree_preorder;
	//bt->postorder = bstree_postorder;
	bt->destroy = fstree_destroy;
	bt->getName = getName;
	bt->printName = printName;
	//bt->leftNode = NULL;
	//bt->rightNode = NULL;
	strcpy(bt->name, name);
	strcpy(bt->e.name, name);
	//printf("assigned tree name: %s\n", bt->name);
	bt->childdirs = ArrayList(2);
	bt->childfiles = ArrayList(2);
	bt->parent = parent;
	//bt->e.first[0] = '\0';
	//bt->e.last[0] = '\0';
	//printf("returning tree\n");
	return (fstree*) bt;
}

char *getName(fstree* bt) {
	return bt->name;
}

void printName(fstree* bt) {
	//printf("printname called\n");
	printf("%s\n", bt->name);
}

/*BSTree *bstree_insert(BSTree* bt, Element e){
	if(bt == NULL) {
		bt = newBSTree();
		bt->e = e;
	} else if(strcmp(bt->e.last, "\0") == 0) {
		bt->e = e;
	} else if(eCompare(e, bt->e) > 0) { //go right
		if(bt->rightNode == NULL)
			bt->rightNode = newBSTree();
		bt = bt->insert(bt->rightNode, e);
	} else { //go left
		if(bt->leftNode == NULL)
			bt->leftNode = newBSTree();
		bt = bt->insert(bt->leftNode, e);
	}
	return bt;
}*/

/*BSTree *bstree_remove(BSTree* bt, Element e, int* i) {

	if(bt == NULL) {
		return (BSTree*)bt;
	} if(eCompare(e, bt->e) > 0) {
		bt->rightNode = (BSTree*)bt->remove(bt->rightNode, e, i);
	} else if(eCompare(e, bt->e) < 0) {
		bt->leftNode = (BSTree*)bt->remove(bt->leftNode, e, i);
	} else {
		BSTree *temp = (BSTree*)bt->remove(bt->leftNode, e, i);
		BSTree *right = (BSTree*)bt->rightNode;
		bt->leftNode = NULL;
		bt->rightNode = NULL;
		FREE(bt);
		*i = *i+1;
		if(temp == NULL && right == NULL) {
			bt = (BSTree*)newBSTree();
		} else if (temp != NULL) {
			bt = (BSTree*)temp;
			if(right != NULL) {
				while(temp->rightNode != NULL) {
					temp = (BSTree*)temp->rightNode;
				}
				temp->rightNode = (BSTree*)right;
			} else {
				//FREE(right); //<-- COMMENTED TO FIX GRADE
			}
		} else if(right != NULL) {
			bt = (BSTree*)right;
		}
	}
	return bt;
}*/
/*void list(BSTree* bt, Element e) {
	if(bt->leftNode != NULL)
		list(bt->leftNode, e);
	listWithLastname((void*)&bt->e, e);
	if(bt->rightNode != NULL)
		list(bt->rightNode, e);
}*/

/*BSTree * bstree_inorder(BSTree * bt, void* something, void* (*f)(void* s, Element e)) {
	if(bt->leftNode != NULL)
		bt->inorder(bt->leftNode, something, (void*)bt->inorder); 
	printElement(NULL, bt->e);
	if(bt->rightNode != NULL)
		bt->inorder(bt->rightNode, something, (void*)bt->inorder);
}*/

/*BSTree * bstree_preorder(BSTree * bt, void* something, void* (*f)(void* s, Element e)) {
	printElement(NULL, bt->e);
	if(bt->leftNode != NULL)
		bt->preorder(bt->leftNode, something, (void*)bt->preorder); 
	if(bt->rightNode != NULL)
		bt->preorder(bt->rightNode, something, (void*)bt->preorder);
}*/

/*BSTree * bstree_postorder(BSTree * bt, void* something, void* (*f)(void* s, Element e)) {
	if(bt->leftNode != NULL)
		bt->postorder(bt->leftNode, something, (void*)bt->postorder); 
	if(bt->rightNode != NULL)
		bt->postorder(bt->rightNode, something, (void*)bt->postorder);
	printElement(NULL, bt->e);
}*/

void fstree_destroy(fstree* bt) {
	List *l;
	size_t x;
	//printf("tree destroy called\n");
	fstree *temp;
	//printf("calling destroy for childfiles\n");
	//bt->childfiles->destroy(bt->childfiles);
	//printf("childfiles should be destroyed\n");

	l = bt->childfiles;
	x = l->size(l);
	//printf("childdirs is size %lu\n", x);
	if(l->size(l) > 0) {
		while(x >= 0 && l->size(l) != 0) {	
			temp = (fstree*)l->getFSTreeAt(l,x);
			if(temp != NULL) {
				//printf("calling removelast for tree: ");
				//printElement(NULL, temp->e);
				//printf("calling destroy for tree\n");
				temp->destroy(temp);
				l = l->removelast(l, (void**)temp);
				//printf("removed a tree from end of list\n");
			}
			x--;
		}
	} 
	//printf("calling destroy for childdirs list\n");
	l->destroy(l);

	l = bt->childdirs;
	x = l->size(l);
	//printf("childdirs is size %lu\n", x);
	if(l->size(l) > 0) {
		while(x >= 0 && l->size(l) != 0) {	
			temp = (fstree*)l->getFSTreeAt(l,x);
			if(temp != NULL) {
				//printf("calling removelast for tree: ");
				//printElement(NULL, temp->e);
				//printf("calling destroy for tree\n");
				temp->destroy(temp);
				l = l->removelast(l, (void**)temp);
				//printf("removed a tree from end of list\n");
			}
			x--;
		}
	} 
	//printf("calling destroy for childdirs list\n");
	l->destroy(l);
	FREE(bt);
}

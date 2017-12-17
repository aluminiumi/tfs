#ifndef ___Element_DEFINITION_HEADER_
#define ___Element_DEFINITION_HEADER_

#include<string.h>
#include<stdio.h>

/************************************
*******
******* Definition Of Element, 
******* Change this file's definition of Element
******* To Change Any Data Structure Which Takes
******* Elements As Content. 
*******
*************************************/

struct _first_last_name{
   char name[21];
   char first[21];
   char last[21];
};

typedef struct _first_last_name Element;


/*eCompare: Element x Element -> {-1, 0, 1}
* This function is not defined here.  
* Compare two Elements. 
* return -1 if the left Element is less than
* return 1 if the left Element is larger than
* return 0 if they are equal or are a match.  
*/ 
int eCompare(Element, Element);

/* printElement
* This is a fold-ready function. 
* a is not used, can be anything, 
* returns a. 
*/
static void * printElement(void *a, Element e){
  //printf("printElement called\n");
  if(strcmp(e.name, "\0") != 0)
  	printf("%s\n", e.name);
  else
	printf("empty\n");
  return a;
}

/* listWithLastname
* This is a fold-ready function. 
* a is a pointer to an Element for comparison.  
* If the last names match, print the first name of e.
* returns a. 
*/
static void * listWithLastname(void * a, Element e){
  if(strcmp(((Element*)a)->last, e.last) == 0)
    printf("  %s\n", ((Element*)a)->first);
    return a;
}


#endif

#include "element.h"

/*eCompare: Element x Element -> {-1, 0, 1}
* This function is not defined here.  
* Compare two Elements. 
* return -1 if the left Element is less than
* return 1 if the left Element is larger than
* return 0 if they are equal or are a match.  
*/ 
int eCompare(Element left, Element right) {
	if(strcmp(left.last, right.last) == 0) {
		if(strcmp(left.first, "*") == 0)
			return 0;
		return strcmp(left.first, right.first);
	} else
		return strcmp(left.last, right.last);
}


#include "filesystem.h"
#include "trackedmemory.h"

int main(int argc, char ** argv){
	char command[100] = "";
	char path[MAX_PATH_LENGTH];
	int temp;
	Element e;
	fsys *myfs;
	
	myfs = newFileSystem();
	
	while(scanf("%s", command) > 0){
		char argument[1001] = "";
		//ls
		if(strcmp(command, "ls") == 0){
			//ls and ls <path>
			//scanf("%1001s", argument);
			fgets(argument, 1001, stdin);
			fflush(stdin);
			while(strncmp(argument+strlen(argument)-1, "\r", 1) == 0
			|| strncmp(argument+strlen(argument)-1, "\n", 1) == 0)
				strcpy(argument+strlen(argument)-1, "\0");
			myfs->fs_ls(myfs, argument);
		} else if(strcmp(command, "pwd") == 0) {
			myfs->fs_pwd(myfs);
		} else if(strcmp(command, "mkdir") == 0) {
			//mkdir <path>
			//strcpy(argument, "");
			scanf("%1001s", argument);
			//printf("%s\n", argument);
			myfs->fs_mkdir(myfs, argument);
		} else if(strcmp(command, "touch") == 0) {
			//touch <path>
			scanf("%1001s", argument);
			//printf("%s\n", argument);
			myfs->fs_touch(myfs, argument);
		} else if(strcmp(command, "cd") == 0) {
			//cd <path>
			scanf("%1001s", argument);
			myfs->fs_cd(myfs, argument);
		} /*else if(strcmp(command, "rm -f") == 0) {
			//rm -f <path>
			scanf("%1001s", argument);
			myfs->fs_rmf(myfs, argument);
		} */ else if(strcmp(command, "rm") == 0) {
			//rm <path>
			fgets(argument, 1001, stdin);
			fflush(stdin);
			//scanf("%1001s", argument);
			if(strncmp(argument, " -f ", 4) == 0) {
				//printf("sending to -f\n");
				strcpy(argument, argument+4);
				while(strncmp(argument+strlen(argument)-1, "\r", 1) == 0
				|| strncmp(argument+strlen(argument)-1, "\n", 1) == 0)
					strcpy(argument+strlen(argument)-1, "\0");
				//printf("sending argument \'%s\'\n", argument);
				myfs->fs_rmf(myfs, argument);
			} else {
				strcpy(argument, argument+1);
				while(strncmp(argument+strlen(argument)-1, "\r", 1) == 0
				|| strncmp(argument+strlen(argument)-1, "\n", 1) == 0)
					strcpy(argument+strlen(argument)-1, "\0");
				//printf("sending argument \'%s\'\n", argument);
				myfs->fs_rm(myfs, argument);
			}
		} else if(strcmp(command, "find") == 0) {
			//find <local name>
			scanf("%1001s", argument);
			myfs->fs_find(myfs, argument);
		} 
	}
	myfs->destroyFileSystem(myfs);
	//printMemoryUsage();
	assertNoMemoryLeaks();
	return 0;
}

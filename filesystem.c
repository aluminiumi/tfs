#include "filesystem.h"

  //struct _file_system;
  //typedef struct _file_system fsys;
  
fstree *getTreeAtPath(fsys*, char*);
fstree *quietGetTreeAtPath(fsys* fs, char *path);
fstree *getFileTreeAtPath(fsys*, char*);
char *getPathOfTree(fsys*, fstree*, char*);
char *sanitizePath(char*, char*);
int safeToCreate(fstree*, char*);
fstree *follow(fstree*, char*);
fstree *quietfollow(fstree*, char*);
fstree *getLastValidTreeInPath(fstree*, char*);
void prepend(char*, const char*);

fsys* newFileSystem() {
	//printf("creating new fs\n");
	fstree *mytree = newFSTree("/", NULL);
	//printf("tree structure created\n");
	fsys *myfs = MALLOC(sizeof(fsys));
	myfs->tree = mytree;
	myfs->tree->fs = myfs;
	myfs->fs_pwd = fs_pwd;
	myfs->destroyFileSystem = destroyFileSystem;
	myfs->fs_ls = fs_ls;
	myfs->fs_mkdir = fs_mkdir;
	myfs->fs_touch = fs_touch;
	myfs->fs_cd = fs_cd;
	myfs->fs_rm = fs_rm;
	myfs->fs_rmf = fs_rmf;
	myfs->fs_find = fs_find;
	//printf("assigning cwd\n");
	strcpy(myfs->cwd, "/");
	//printf("returning new fs\n");
	return myfs;
}

char *sanitizePath(char *cwd, char *path) {
	if(path == NULL || strcmp(path, "\0") == 0 || strcmp(path, "\n") == 0)
		return cwd;
	if(strncmp(path, " ", 1) == 0)
		strcpy(path, path+1);
	if(strncmp(path+strlen(path)-1, "\n", 1) == 0)
		strcpy(path+strlen(path)-1, "\0");
	//printf("sanitize: got path \"%s\"\n", path);
	if(strncmp(path, "/", 1) == 0) {
		//printf("follow: got an absolute path\n");
	} else {
		char temp[1001];
		strcpy(temp, cwd);
		if(strlen(cwd) > 1)
			strcat(temp, "/");
		//printf("temp is %s\n", temp);
		strcat(temp, path); 
		strcpy(path, temp);
		//printf("sanitize: formed absolute path: %s\n", path);
	}
	return path;
}

/*
 3. SafeToCreate(NODE* n, PATH P): boolean
          • Starting from n for relative paths, checks to see that the prefix of
            PATH P that already exists is a directory and that it is safe to create
            the missing remainder of the path.
          • Error Case A: a name along the path is to a file and not a directory.
          • Error Output A: Path Error:       Cannot create sub-directory content, ’<Absolute PA
TH To
          • Error Case B: the path given indicates a file or directory that already
            exists.
          • Error Output B: Path Error: ’<Absolute PATH>’ already exists and cannot be created.
*/
void truncateToPath(char *dest, char *path) {
	//path = sanitizePath(ft->fs->cwd, path);
	size_t x;
	x = 1;
	//printf("truncateToPath: received dest: %s\n", dest);
	//printf("truncateToPath: received path: %s\n", path);
	while(strncmp(path+strlen(path)-x, "/", 1) != 0) {
		x++; //becomes length of token to chop off from end
	}
	//printf("truncateToPath: chopping %lu bytes\n", x);
	strncpy(dest, path, strlen(path)-x);
	strcpy(dest+strlen(path)-x, "\0");
	if(strncmp(dest, "/", 1) != 0)
		prepend(path, "/");
	//printf("truncateToPath: created %s\n", dest);
}

void truncateToFilename(char *dest, char *path) {
	//path = sanitizePath(ft->fs->cwd, path);
	size_t x;
	x = 1;
	while(strncmp(path+strlen(path)-x, "/", 1) != 0) {
		x++; //becomes length of token to chop off from end
	}
	strncpy(dest, path+strlen(path)-x+1, x);
	strcpy(dest+x, "\0");
	//printf("truncateToFilename: created %s\n", dest);
}

int isAncestorOf(fstree *cwd, fstree *dest) {
	while(cwd->parent != NULL) {
		if(cwd == dest)
			return 1;
		cwd = cwd->parent;
	}
	return 0;
}

int safeToCreate(fstree *ft, char *path) {
	path = sanitizePath(ft->fs->cwd, path);
	char realpath[1001] = "";
	char filename[21] = "";
	//realpath[0] = '\0';
	//truncate last element off of path
	truncateToPath(realpath, path);
	truncateToFilename(filename, path);
	//printf("safetocreate: checking path %s\n", realpath);
	//getTreeAtPath(path) to make sure it's sane
	if(strlen(realpath) == 0 || quietGetTreeAtPath(ft->fs, realpath) != NULL) {
		//printf("safecheck: realpath empty or got a tree\n");
		List *cd = quietGetTreeAtPath(ft->fs, realpath)->childdirs;
		List *cd2 = quietGetTreeAtPath(ft->fs, realpath)->childfiles;
		if(!cd->alreadyExists(cd, filename) && !cd2->alreadyExists(cd2, filename)) {	
			return 1;
		} else {	
			char abspath[1001] = "";
			strcpy(abspath, realpath);
			strcat(abspath, "/");
			strcat(abspath, filename);
			strcat(abspath, "\0");
			printf("Path Error: '%s' already exists and cannot be created.\n", abspath);
			return 0;
		}
	} else if(quietGetTreeAtPath(ft->fs, realpath) == NULL) {
		//printf("safecheck: realpath (%s) was null, checking if file\n", realpath);
		if(getFileTreeAtPath(ft->fs, realpath) != NULL) {
			return 0;
		} else {
			//printf("not a file\n");
			return 1;
			//printf("Path Error: directory ’%s’ does not exist.\n", abspath);
			//printf("follow: returning null; token was not found in list\n");
		}
	}
	return 0;
}

//mimics follow, but for create functions
fstree *getLastValidTreeInPath(fstree *ft, char *path) {
	if(strncmp(path, "/", 1) == 0)
		if(strlen(path) == 1)
			return ft->fs->tree;	
	char *token;
	char temppath[1001] = "";
	char newpath[1001] = "";
	fstree *rettree;
	strcpy(temppath, path);
	token = strtok(temppath, "/");
	size_t x;
	List *l = ft->childdirs;
	if(token == NULL) {
		return ft;
	}
	else if(strcmp(token, ".") == 0) {
		strcpy(newpath, path+strlen(token)+1);
		return getLastValidTreeInPath(ft, newpath);
	} else if(strcmp(token, "..") == 0) {	
		strcpy(newpath, path+strlen(token)+1);
		if(ft->parent != NULL) { //not root directory {
			return getLastValidTreeInPath(ft->parent, newpath);
		} else {
			return getLastValidTreeInPath(ft, newpath); //root directory, return self
		}
	} else {
		for(x=0; x<l->size(l); x++) {
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				if(strlen(path) <= strlen(token)+1) {
					rettree = l->getFSTreeAt(l,x);
					return rettree;
				}
				else {
					strcpy(newpath, path+strlen(token)+1);
					return getLastValidTreeInPath(l->getFSTreeAt(l,x), newpath);
				}
			}
		}
		return ft; //token was not found in list for this dir
	}
}
/*
2. Follow(NODE * n, PATH P): NODE *

        • If P is absolute, set n to point to the root
        • Walk the path from n and return the pointer to the directory or file
          indicated by the path
        • If an error occurs, return NULL;
        • Error Case: a directory along prefix of path does not exist.


                                       3
^L          • Error Output: Path Error: directory ’<Absolute PATH To Missing Directory>’ does no
t e
*/
fstree *follow(fstree *ft, char *path) {
	//printf("follow: received path: %s\n", path);
	if(strncmp(path, "/", 1) == 0)
		if(strlen(path) == 1)
			return ft->fs->tree;	
	char *token;
	char temppath[1001] = "";
	char newpath[1001] = "";
	fstree *rettree;
	strcpy(temppath, path);
	token = strtok(temppath, "/");
	//printf("follow: token is %s\n", token);
	size_t x;
	List *l = ft->childdirs;
	if(token == NULL) {
		return ft;
	}
	else if(strcmp(token, ".") == 0) {
		strcpy(newpath, path+strlen(token)+1);
		return follow(ft, newpath);
	} else if(strcmp(token, "..") == 0) {	
		strcpy(newpath, path+strlen(token)+1);
		if(ft->parent != NULL) { //not root directory {
			return follow(ft->parent, newpath);
		} else {
			return follow(ft, newpath); //root directory, return self
		}
	} else {
		//printf("follow: traversing list of size %lu\n", l->size(l));
		for(x=0; x<l->size(l); x++) {
			//printf("follow: x is %lu\n", x);
			//printf("follow: checking against tree: %s\n", l->getFSTreeAt(l,x)->name);
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				//printf("follow: match on %s in %s\n", l->getFSTreeAt(l,x)->name, ft->name);
				//printf("follow: path was %s\n", path);
				//printf("follow: token is %s\n", token);
				//printf("follow: strlen of token+1 is %lu\n", strlen(token)+1);
				//printf("follow: strlen of path is %lu\n", strlen(path));
				if(strlen(path) <= strlen(token)+1) {
				//newpath = backuppath+strlen(token)+2;
				//printf("follow: newpath is %s\n", newpath);
				//if(strcmp(newpath, "\0") == 0) {
					//printf("follow: final node found\n");
					rettree = l->getFSTreeAt(l,x);
					//if(rettree == NULL)
					//	printf("follow: returning null tree\n");
					//else
					//	printf("follow: returning good tree: %s\n", rettree->name);
					return rettree;
				}
				else {
					strcpy(newpath, path+strlen(token)+1);
					return follow(l->getFSTreeAt(l,x), newpath);
				}
			}
		}
		l = ft->childfiles;
		int isFile = 0;
		for(x=0; x<l->size(l); x++) {
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				isFile = 1;	
			}
		}
		char abspath[1001] = "";
		getPathOfTree(ft->fs, ft, abspath);
		strcat(abspath, token);
		strcat(abspath, "\0");
		if(isFile) {
			//printf("Path Error: Cannot create sub-directory content, '%s' is a file.\n", abspath);
		} else {
			printf("Path Error: directory '%s' does not exist.\n", abspath);
			//printf("follow: returning null; token was not found in list\n");
		}
		return NULL; //token was not found in list for this dir
	}
}


fstree *quietfollow(fstree *ft, char *path) {
	//printf("follow: received path: %s\n", path);
	if(strncmp(path, "/", 1) == 0)
		if(strlen(path) == 1)
			return ft->fs->tree;	
	char *token;
	char temppath[1001] = "";
	char newpath[1001] = "";
	fstree *rettree;
	strcpy(temppath, path);
	token = strtok(temppath, "/");
	size_t x;
	List *l = ft->childdirs;
	if(token == NULL) {
		return ft;
	}
	else if(strcmp(token, ".") == 0) {
		strcpy(newpath, path+strlen(token)+1);
		return quietfollow(ft, newpath);
	} else if(strcmp(token, "..") == 0) {	
		strcpy(newpath, path+strlen(token)+1);
		if(ft->parent != NULL) { //not root directory {
			return quietfollow(ft->parent, newpath);
		} else {
			return quietfollow(ft, newpath); //root directory, return self
		}
	} else {
		for(x=0; x<l->size(l); x++) {
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				if(strlen(path) <= strlen(token)+1) {
					rettree = l->getFSTreeAt(l,x);
					return rettree;
				}
				else {
					strcpy(newpath, path+strlen(token)+1);
					return quietfollow(l->getFSTreeAt(l,x), newpath);
				}
			}
		}
		return NULL; //token was not found in list for this dir
	}
}


void prepend(char* s, const char* t)
{
    size_t len = strlen(t);
    size_t i;

    memmove(s + len, s, strlen(s) + 1);

    for (i = 0; i < len; ++i)
    {
        s[i] = t[i];
    }
}

/*
  1. GetPWD(NODE* n): PATH

      (a) returns the absolute path to the node n.
      (b) walk the parent pointers back to the root node to figure out the path
          (in reverse).
*/
char *getPathOfTree(fsys *fs, fstree *ft, char *path) {
	//char path[1001] = "";
	size_t index = 0;
	while(strcmp(ft->name, "/") != 0) {
		prepend(path, ft->name);
		ft = ft->parent;
		prepend(path, "/");
	}
	if(strncmp(path, "/", 1) != 0)
		prepend(path, "/");
	//printf("getpath: returning '%s'\n", path);
	return path;
	//ft->name
}

fstree *filefollow(fstree *ft, char *path) {
	//printf("filefollow: received path: %s\n", path);
	if(strncmp(path, "/", 1) == 0)
		if(strlen(path) == 1)
			return ft->fs->tree;	
	char *token;
	char temppath[1001] = "";
	char newpath[1001] = "";
	fstree *rettree;
	strcpy(temppath, path);
	token = strtok(temppath, "/");
	//printf("filefollow: token is %s\n", token);
	size_t x;
	List *l = ft->childdirs;
	if(token == NULL) {
		return ft;
	}
	else if(strcmp(token, ".") == 0) {
		strcpy(newpath, path+strlen(token)+1);
		return filefollow(ft, newpath);
	} else if(strcmp(token, "..") == 0) {	
		strcpy(newpath, path+strlen(token)+1);
		if(ft->parent != NULL) { //not root directory {
			return filefollow(ft->parent, newpath);
		} else {
			return filefollow(ft, newpath); //root directory, return self
		}
	} else {
		//printf("filefollow: traversing list of size %lu\n", l->size(l));
		for(x=0; x<l->size(l); x++) {
			//printf("filefollow: x is %lu\n", x);
			//printf("filefollow: checking against tree: %s\n", l->getFSTreeAt(l,x)->name);
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				//printf("filefollow: match on %s in %s\n", l->getFSTreeAt(l,x)->name, ft->name);
				//printf("filefollow: path was %s\n", path);
				//printf("filefollow: token is %s\n", token);
				//printf("filefollow: strlen of token+1 is %lu\n", strlen(token)+1);
				//printf("filefollow: strlen of path is %lu\n", strlen(path));
				if(strlen(path) <= strlen(token)+1) {
				//if(strcmp(newpath, "\0") == 0) {
					//printf("follow: final node found\n");
					rettree = l->getFSTreeAt(l,x);
					/*if(rettree == NULL)
						printf("filefollow: returning null tree\n");
					else
						printf("filefollow: returning good tree: %s\n", rettree->name);
					*/
					return rettree;
				}
				else {
					//printf("filefollow: calling follow again\n");
					strcpy(newpath, path+strlen(token)+1);
					return filefollow(l->getFSTreeAt(l,x), newpath);
				}
			}
		}
		//printf("filefollow: checking if %s is a file\n", token);
		l = ft->childfiles;
		int isFile = 0;
		for(x=0; x<l->size(l); x++) {
			if(strcmp(l->getFSTreeAt(l, x)->name, token) == 0) {
				isFile = 1;	
			}
		}
		char abspath[1001] = "";
		getPathOfTree(ft->fs, ft, abspath);
		strcat(abspath, token);
		strcat(abspath, "\0");
		if(isFile) {
			printf("Path Error: Cannot create sub-directory content. '%s' is a file.\n", abspath);
			return ft;
		} else {
			//printf("Path Error: directory '%s' does not exist.\n", abspath);
			//printf("follow: returning null; token was not found in list\n");
		}
		return NULL; //token was not found in list for this dir
	}
}

fstree *getFileTreeAtPath(fsys* fs, char* path) {
	path = sanitizePath(fs->cwd, path);
	fstree *foundtree = (fstree*)filefollow(fs->tree, path);
	return (fstree*)foundtree;
}

fstree *quietGetTreeAtPath(fsys* fs, char *path) {
	path = sanitizePath(fs->cwd, path);
	fstree *foundtree = (fstree*)quietfollow(fs->tree, path);
	return (fstree*)foundtree;
}

fstree *getTreeAtPath(fsys* fs, char* path) {
	//expect a root path	
	path = sanitizePath(fs->cwd, path);
	//printf("getTree: calling follow\n");
	//return (fstree*)follow(fs->tree, path);
	fstree *foundtree = (fstree*)follow(fs->tree, path);
	//if (foundtree == NULL)
	//	printf("getTree: received null tree from follow\n");
	//else
	//	printf("%s\n", foundtree->name);
	//printf("getTree: returning tree\n");
	return (fstree*)foundtree;
}

void destroyFileSystem(fsys* fs) {
	//printf("fs destroy called\n");
	fs->tree->destroy(fs->tree);
	FREE(fs);
}

void fs_ls(fsys* fs, char* path) {
	path = sanitizePath(fs->cwd, path);
	char newpath[1001] = "";
	List *cd;
	if(getTreeAtPath(fs, path) != NULL) {
		getPathOfTree(fs, getTreeAtPath(fs,path), newpath);
		//printf("new path is %s\n", newpath);
		size_t x;

		printf("Listing For \'%s\':\n", newpath);
		
		cd = getTreeAtPath(fs, path)->childdirs;
		for(x=0; x<cd->size(cd); x++)
			printf("D %s\n", cd->getFSTreeAt(cd,x)->name);
		//cd->printall(cd);
		
		cd = getTreeAtPath(fs, path)->childfiles;
		for(x=0; x<cd->size(cd); x++)
			printf("F %s\n", cd->getFSTreeAt(cd,x)->name);
		//cd->printall(cd);
	}  //TODO: else if
	//Error Output: List Error: Cannot perform list operation. ’<Absolute PATH>’ is a file
	else {
		printf("List Error: Cannot perform list operation.\n");
		//printf("ls: Invalid path.\n");
	}
	
}

void fs_pwd(fsys* fs) {
	printf("%s\n", fs->cwd);
}

fs_mkdir_core(fsys* fs, char *parentpath, char* filename) {
	//printf("core: got parent \'%s\' and filename \'%s\'\n", parentpath, filename);
	List *cd = getTreeAtPath(fs, parentpath)->childdirs;
	cd->prepend(cd, newFSTree(filename, getTreeAtPath(fs, parentpath)));
	cd->getFSTreeAt(cd, 0)->fs = fs;
	cd = cd->sort(cd);
}

void fs_mkdir(fsys* fs, char* path) {
	if(safeToCreate(fs->tree, path)) {
		//printf("safe to create\n");
		List *cd, *cd2;
		char realpath[1001] = "";
		truncateToPath(realpath, path);
		//printf("mkdirout: truncate gave us realpath \'%s\'\n", realpath);
		//printf("mkdirout: full path was \'%s\'\n", path);
		char filename[21] = "";
		truncateToFilename(filename, path);
		if(quietGetTreeAtPath(fs, realpath) == NULL) { //need to prepopulate dir structure
			//printf("mkdirif: need to prepopulate\n");
			char existingpath[1001] = "";
			getPathOfTree(fs, getLastValidTreeInPath(fs->tree, realpath), existingpath);
			//tokenize diff between existingpath and realpath
			char tempex[1001] = "";
			char remainder[1001] = "";
			char *token;
			//printf("mkdirif: existingpath is \'%s\'\n", existingpath);
			//printf("mkdirif: realpath is \'%s\'\n", realpath);
			strcpy(tempex, existingpath);
			strcpy(remainder, path+strlen(existingpath));
			//printf("mkdirif: created remainder: \'%s\'\n", remainder);
			token = strtok(remainder, "/");
			int i;
			char *result[21]={NULL};
   			for(i=0;  token!=NULL; token=strtok(NULL, "/")) { //create tokens
				result[i]=token;
				//printf("token %d is %s\n", i, result[i]);
				i++;
			}
			result[i]=NULL;
			int j;
			for(j=0; j<i; j++) { //create each dir, from tokens
				token = result[j];
				fs_mkdir_core(fs, tempex, token);
				if(strncmp(tempex, "/", 1 == 0))
					strcat(tempex, "/");
				else if(strlen(tempex) > 1)
					strcat(tempex, "/");
				strcat(tempex, token);
				strcat(tempex, "\0");
				//fs_find(fs, "*");
			}
		} else { 
			fs_mkdir_core(fs, realpath, filename);
		}
	} else
		printf("Make Dir Error: Cannot create directory.\n");
}

void fs_touch(fsys* fs, char* path) {
	if(safeToCreate(fs->tree, path)) {
		//printf("touch: is safe to create\n");
		List *cd, *cd2;
		char realpath[1001] = "";
		truncateToPath(realpath, path);
		char filename[21] = "";
		truncateToFilename(filename, path);
		//printf("touch: checking if need to populate\n");
		if(quietGetTreeAtPath(fs, realpath) == NULL) { //need to prepopulate dir structure
			fs_mkdir(fs, realpath);
		}
		//printf("touch: done populating\n");
		cd = quietGetTreeAtPath(fs, realpath)->childfiles;
		//cd2 = getTreeAtPath(fs, realpath)->childdirs;	
		cd->prepend(cd, newFSTree(filename, quietGetTreeAtPath(fs, realpath)));
		cd->getFSTreeAt(cd, 0)->fs = fs;
		cd = cd->sort(cd);
	} else {
		//printf("touch: not safe\n");
		printf("Touch Error: Cannot create file.\n");
	}
}

void fs_cd(fsys* fs, char* path) {
	//getTreeAtPath(fs, path);
	if(getTreeAtPath(fs, path) != NULL) {
		char pathnew[1001] = "";
		//printf("cd: path is good\n");
		getPathOfTree(fs, getTreeAtPath(fs, path), pathnew);
		//printf("cd: setting cwd to %s\n", pathnew);
		strcpy(fs->cwd, pathnew);
	} else {
		printf("Change Dir Error: Cannot change working directory.\n");
		//printf("cd: path is bad (null tree returned)\n");
	}
}

void fs_rm(fsys* fs, char* path) {
	//printf("fs_rm running\n");	
	path = sanitizePath(fs->cwd, path);
	//printf("rm: got path %s\n", path);
	fstree *dest = getTreeAtPath(fs, path);
	char filename[21] = "";
	truncateToFilename(filename, path);
	char realpath[1001] = "";
	truncateToPath(realpath, path);
	if(dest == NULL) { //not a directory, or doesn't exist
		dest = getTreeAtPath(fs, realpath);
		List *l = dest->childfiles;
		if(!l->alreadyExists(l, filename)) {
			//printf("rm: file does not exist\n");
			printf("Remove Error: Cannot remove file or directory.");
		} else { //exists as a file
			fstree *temptree;
			size_t index = l->getIndexOf(l, filename);
			//printf("rm: index is %lu\n", index);
			temptree = l->getFSTreeAt(l, index);
			temptree->destroy(temptree);
			l = l->remove(l, index, (void**)temptree);
		}
	} else { //is a directory
		//printf("rm: is a dir, checking if empty\n");
		//printf("rm: dest %s has %lu files and %lu directories\n", dest->name, dest->childdirs->size, dest->childfiles->size);
		if(dest->childdirs->size(dest->childdirs) == 0 && dest->childfiles->size(dest->childfiles) == 0) {
			//printf("rm: is empty\n");
			if(dest->parent == NULL) {
				//printf("rm: cannot remove /\n");
				printf("Remove Error: Cannot remove file or directory.");
			} else {
				dest = dest->parent;
				List *l = dest->childdirs;
				fstree *temptree;
				size_t index = l->getIndexOf(l, filename);
				temptree = l->getFSTreeAt(l, index);
				temptree->destroy(temptree);
				l = l->remove(l, index, (void**)temptree);
			}
			
		} else {	
			char abspath[1001] = "";
			strcpy(abspath, realpath);
			strcat(abspath, "/");
			strcat(abspath, filename);
			strcat(abspath, "\0");
			printf("Remove Error: directory \'%s\' is not empty.\n", abspath);
			//printf("rm: directory is not empty\n");
		}
	}
	//fs->tree->destroy(fs->tree);
}

void fs_rmf(fsys* fs, char* path) {
	//printf("fs_rmf running\n");
	path = sanitizePath(fs->cwd, path);
	//printf("rm: got path %s\n", path);
	fstree *dest = getTreeAtPath(fs, path);
	char filename[21] = "";
	truncateToFilename(filename, path);
	if(dest == NULL) { //not a directory, or doesn't exist
		char realpath[1001] = "";
		truncateToPath(realpath, path);
		dest = getTreeAtPath(fs, realpath);
		List *l = dest->childfiles;
		if(!l->alreadyExists(l, filename)) {
			//printf("rm: file does not exist\n");
			printf("Remove Error: Cannot remove file or directory.");
		} else { //exists as a file
			fstree *temptree;
			size_t index = l->getIndexOf(l, filename);
			//printf("rm: index is %lu\n", index);
			temptree = l->getFSTreeAt(l, index);
			temptree->destroy(temptree);
			l = l->remove(l, index, (void**)temptree);
		}
	} else { //is a directory
		if(dest->parent == NULL) {
			//printf("rm: cannot remove /\n");
			printf("Remove Error: Cannot remove file or directory.");
		} else if (isAncestorOf(getTreeAtPath(fs, fs->cwd), dest)) {
			//printf("rm: cannot remove ancestor directory\n");
			printf("Remove Error: Cannot remove file or directory.");
		} else {
			dest = dest->parent;
			List *l = dest->childdirs;
			fstree *temptree;
			size_t index = l->getIndexOf(l, filename);
			if(index+1 == 0)
				printf("rm: got bad index\n");
			else {
				//printf("index was %lu\n", index);
				temptree = l->getFSTreeAt(l, index);
				temptree->destroy(temptree);
				l = l->remove(l, index, (void**)temptree);
				//l = l->remove(l, index);
			}
		}
	}
	//fs->tree->destroy(fs->tree);
}

void findhelp(fstree *tree, char *target) {
	char output[1001] = "";
	getPathOfTree(tree->fs, tree, output);
	if (strstr(tree->name, target) != NULL || strcmp(target, "*") == 0) {
		printf("D %s\n", output);
	}
	size_t x;
	List *l = tree->childfiles;
	for(x=0; x<l->size(l); x++) {
		if (strstr(l->getFSTreeAt(l,x)->name, target) != NULL || strcmp(target, "*") == 0) {
			strcpy(output, "\0");
			getPathOfTree(tree->fs, l->getFSTreeAt(l,x), output);
			printf("F %s\n", output);
		}
	}
	l = tree->childdirs;
	for(x=0; x<l->size(l); x++)
		findhelp(l->getFSTreeAt(l, x), target);
	//if (strstr(tree->name, target) != NULL || strcmp(target, "*") == 0) {
	//	printf("D %s\n", output);
	//}
}

void fs_find(fsys* fs, char* target) {
	//printf("fs_find running, got %s\n", target);
	printf("Searching For '%s':\n", target);
	findhelp(fs->tree, target);
}



#ifndef ___fs_H_
#define ___fs_H_

#include "stdheader.h"
#include "tree.h"

#define MAX_LNAME_LENGTH 21
#define MAX_PATH_LENGTH 1001

  //typedef struct _file_system fsys;
  struct _file_system{
	void (*fs_pwd)(fsys*);
        void (*destroyFileSystem)(fsys*);
	void (*fs_ls)(fsys*, char*);
	void (*fs_find)(fsys*, char*);
	void (*fs_rmf)(fsys*, char*);
	void (*fs_rm)(fsys*, char*);
	void (*fs_cd)(fsys*, char*);
	void (*fs_touch)(fsys*, char*);
	void (*fs_mkdir)(fsys*, char*);
	fstree *tree;
	char cwd[MAX_PATH_LENGTH];
  };
  
  //file system functions
  fsys* newFileSystem();
  void destroyFileSystem(fsys*);
  void fs_ls(fsys*, char*);
  void fs_pwd(fsys*);
  void fs_mkdir(fsys*, char*);
  void fs_touch(fsys*, char*);
  void fs_cd(fsys*, char*);
  void fs_rm(fsys*, char*);
  void fs_rmf(fsys*, char*);
  void fs_find(fsys*, char*);
 

#endif

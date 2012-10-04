
#pragma once

/*
 * return the index of argv that have opt. -1 for not found.
 */
int opt_check_for(const std::string opt,int argc , const char * argv[]);
void opt_remove(int & argc,const char * argv[], int index);

/*
	return the dir that hold our main database.
 */
fs::path hm_getdbdir(void);
fs::path hm_getexecdir(void);
bool hm_hasroom(const std::string &roomid);

// if success , not return
int os_exec(const fs::path &exe,int argc,const char * argv[]);
int os_runexe(const fs::path exe,int argc,const char * argv[]);
std::string os_runtostring(fs::path exe,int argc,const char * argv[]) throw(int);

int bring_editor(fs::path filename);

int hm_main_caller(MAINFUNC mainfunc, const char * arg1,const char * arg2,...);

std::string hm_uuidgen();

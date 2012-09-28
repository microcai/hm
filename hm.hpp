#pragma once

/*
 * return the index of argv that have opt. -1 for not found.
 */
int opt_check_for(const std::string opt,int argc , const char * argv[]);

/*
	return the dir that hold our main database.
 */
fs::path hm_getdbdir(void);

void expand_roomids(std::list<std::string> &result,const std::string roomid);

int bring_editor(fs::path filename);

typedef int (*MAINFUNC)(int argc, const char * argv[]);

int main_book(int argc , const char * argv[]);

int main_client(int argc , const char * argv[]);

int main_init(int argc , const char * argv[]);

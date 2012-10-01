
#pragma once

/*
 * return the index of argv that have opt. -1 for not found.
 */
int opt_check_for(const std::string opt,int argc , const char * argv[]);

/*
	return the dir that hold our main database.
 */
fs::path hm_getdbdir(void);
fs::path hm_getexecdir(void);

// if success , not return
int os_exec(fs::path &exe,int argc,const char * argv[]);

void expand_roomids(std::list<std::string> &result,const std::string roomid);

int bring_editor(fs::path filename);


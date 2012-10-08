
#pragma once

/*
 * return the index of argv that have opt. -1 for not found.
 */
int opt_check_for(const std::string opt,int argc , const char * argv[]);
void opt_remove(int & argc,const char * argv[], int index);
void opt_remove(int & argc,const char * argv[], const std::string opt);

/*
	return the dir that hold our main database.
 */
const fs::path hm_getdbdir(void);
const fs::path hm_getexecdir(void);
const fs::path hm_getwwwroot(void);

bool hm_hasroom(const std::string &roomid);

// if success , not return
int os_exec(const fs::path &exe,int argc,const char * argv[], const std::vector<std::string> & env = std::vector<std::string>() );
int os_runexe(const fs::path exe,int argc,const char * argv[]);
const fs::path os_exe_self();

std::vector<std::string> getenvall();

int bring_editor(fs::path filename);

int hm_main_caller(MAINFUNC mainfunc, const char * arg1,const char * arg2,...);

std::string hm_uuidgen();


void httpd_output_response( int status , std::string contenttype = std::string("text/plain"),uintmax_t contentlength = 0);

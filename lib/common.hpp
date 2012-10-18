
#pragma once



/*
	return the dir that hold our main database.
 */
const fs::path hm_getdbdir(void);
const fs::path hm_getexecdir(void);
const fs::path hm_getwwwroot(void);

bool hm_hasroom(const std::string &roomid);

/**
 * fork and return the stdio as fd
 *
 * for child, return pid = null , fd = -1
 */
std::tuple<pid_t,int> hmfork();
// if success , not return
int os_exec(const fs::path &exe,int argc,const char * argv[], const std::map<std::string,std::string> & env = std::map<std::string,std::string>() );
int os_runexe(const fs::path exe,int argc,const char * argv[]);
const fs::path os_exe_self();

std::map<std::string,std::string> getenvall();

int bring_editor(fs::path filename);

int hm_main_caller(int (*mainfunc)(int argc,const char * argv[]), const char * arg1,const char * arg2,...);

std::string hm_uuidgen();


void httpd_output_response( int status , std::string contenttype = std::string("text/plain"),uintmax_t contentlength = 0);


enum arg_type{
	/**
	 * X is [0-9]
	 * Z is [0-9a-zA-Z]
	 */
	arg_type_date_unknow,
	arg_type_date_offset, /** X */
	arg_type_roomid, /** XXXX */
	arg_type_date,  /** XXXXXXXX */
	arg_type_date_sql,/** XXXX-XX-XX */
	arg_type_date_period, /** XXXX-XX-XX,XXXX-XX-XX */ // passed by javascript
	arg_type_telephone, /** XXXXXXXXXXX or XXXXXXXX */
	arg_type_uuid, /** ZZZZZZZZ-ZZZZ-ZZZZ-ZZZZ-ZZZZZZZZZZZZ */
};



/** 猜测参数类型 **/
arg_type check_arg_type(const std::string argstr);

bool match_key(const std::string & line,const std::string & key);

void  walkdir(const fs::path & dir , std::function<void( const fs::path & item )> cb);
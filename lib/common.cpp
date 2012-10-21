
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <crypto++/md5.h>
#include <crypto++/hex.h>

#include "pch.hpp"
#include "hm.hpp"

const fs::path hm_getdbdir(void)
{
	// current dir, but if $HMDIR exist ,then point to $HMDIR
	char * _hmdir = std::getenv("HMDIR");
	if(_hmdir){
		fs::path hmdir = _hmdir;
		return fs::complete(hmdir); // absolute path
	}else{ // current path
		return fs::current_path();
	}
}

const fs::path hm_getexecdir(void)
{
	// current dir, but if $HMDIR exist ,then point to $HMDIR
	char * _hmexecdir = std::getenv("HMEXEC");
	if(_hmexecdir){
		fs::path hmexecdir = _hmexecdir;
		return fs::complete(hmexecdir); // absolute path
	}else{ // build path
		return fs::path("/usr/libexec/hm");
	}
}

const fs::path hm_getwwwroot(void)
{
	char * wwwroot = std::getenv("HMWWWROOT");
	if(wwwroot){
		return fs::complete(wwwroot);
	}

	return hm_getdbdir() / "www";
}

bool hm_hasroom(const std::string &roomid)
{
	fs::path roomdir =  hm_getdbdir() / "rooms" / roomid;

	return fs::exists(roomdir);
}


/**
 * fork and return the stdio as fd
 *
 * for child, return pid = null , fd = -1
 */
std::tuple<pid_t,int> hmfork()
{
	pid_t pid =  fork();
	int fd[2];

	if(socketpair(AF_UNIX,SOCK_STREAM,0,fd)<0)
		return std::make_tuple((pid_t)-1,-1);

	if(pid == 0){
		dup2(fd[1],0);
		dup2(fd[1],1);
		close(fd[1]);
		close(fd[0]);
		return std::make_tuple((pid_t)pid,-1);
	}
	close(fd[1]);
	return std::make_tuple(pid,fd[0]);
}

int os_exec(const fs::path &exe,int argc,const char * argv[], const std::map<std::string,std::string> & env)
{
	int ret;
	char* exe_argv[argc+1];

	for(int i=0;i<argc;i++){
		exe_argv[i]=strdup(argv[i]);
	}

	exe_argv[argc]=NULL;

	if(env.empty()){
		ret = ::execv(exe.c_str(),exe_argv);
		ret = ::execvp(exe.c_str(),exe_argv);
	}else{

		char* exe_env[env.size()+1];
		int exe_env_p = 0;//exe_env[0];

		for( auto p : env ){
			exe_env[exe_env_p] = strdup((p.first + "=" + p.second).c_str());
			exe_env_p++;
		}
		
		exe_env[exe_env_p]=nullptr;

		ret = ::execve(exe.c_str(),exe_argv,exe_env);
		ret = ::execvpe(exe.c_str(),exe_argv,exe_env);

		for(int i=0;i<env.size();i++){
			free(exe_env[i]);
		}
	}

	for(int i=0;i<argc;i++){
		free(exe_argv[i]);
	}

	return ret;
}

int os_runexe(const fs::path exe,int argc,const char * argv[])
{
	int status;

	pid_t pid = fork();

	if(pid==0){
		os_exec(exe,argc,argv);
		_exit(127);
	}else if(pid > 0){
		waitpid(pid,&status,0);
	}else{
		std::cerr << "无法创建进程" << std::endl;
	}
	return status;
}

std::map<std::string,std::string> getenvall()
{
	std::map<std::string,std::string> envs;

	auto penv = ::__environ;

	while(*penv){
		std::string thisenv = *penv;

		std::string envkey =  thisenv.substr(0,thisenv.find('='));
		std::string envval =  thisenv.substr(thisenv.find('=')+1,std::string::npos);

		envs.insert(std::make_pair(envkey,envval));
		penv++;
	}

	return envs;
}

const fs::path os_exe_self()
{
	// return the exe itself
	fs::path exe = fs::read_symlink("/proc/self/exe");

	// if has (deleted)
	std::string filename = exe.filename().string();

	if(filename.length() > 10){
		if(filename.substr(filename.length() - 10,10)==" (deleted)"){
			std::string realname = filename.substr(0,filename.length() - 10);
			return exe.parent_path() / realname ;
		}
	}
	return exe;
}

int bring_editor(fs::path filename)
{
	// fork and execv EDITOR. if no EDITOR, vim

	// first, find EDITOR
	const char * _editor = std::getenv("EDITOR");

	if(!_editor){ // no EDITOR env, fallback to vim
		_editor = "vim";
	}

	const char *argv[2]={
		_editor,
		filename.c_str(),
	};

	return os_runexe(_editor,2,argv);
}

int hm_main_caller(MAINFUNC mainfunc,const char * arg1,const char * arg2,...)
{
	std::vector<const char*> argv;

	typedef const char * lpcstr;

	lpcstr p;
	va_list va;

	argv.push_back(arg1);
	argv.push_back(arg2);

	if(arg2){
		va_start(va,arg2);
		while(p = va_arg(va,lpcstr))
		{
			argv.push_back(p);
		}
		va_end(va);
	}
	// argv expan to char **
	const char * _argv[argv.size()];
	for(int i=0;i < argv.size();i++)
		_argv[i] = argv[i];

	return mainfunc(argv.size(),_argv);
}

/*
 * generationt new UUID
 */
std::string hm_uuidgen()
{
	std::string output;
	hmrunner uuidgen(main_shell);
	uuidgen.main("shell","uuidgen",NULL);
	uuidgen >> output;
	return output.substr(0,36);
}

void httpd_output_response(int status,const std::map<std::string,std::string> otherheader)
{
	static std::map<int,std::string> httpstatus = {
		{200,"OK"},
		{301,"Moved Permanently"},
		{302,"Found"},
		{401,"Unauthorized"},
		{404,"Not Found"},
	};

	BOOST_ASSERT(!httpstatus[status].empty());

	std::cout << "HTTP/1.1 " << status << " " <<  httpstatus[status] << "\r\n";

	for( const auto & headline : otherheader){
		std::cout << headline.first << ": " << headline.second << "\r\n";
	}
	std::cout << "\r\n";
	std::cout.flush();
}

void httpd_output_response(
	int status /*=200*/,
	const std::string contenttype,
	uintmax_t contentlength,
	const std::map<std::string,std::string> otherheader /*=empty*/ )
{

	std::map<std::string,std::string> header;

	if(contentlength >0){
 		header.insert(std::make_pair("Content-Length",itoa(contentlength)));
		//std::cout << "Content-Length: " << contentlength << "\r\n";
	}else{
		header.insert(std::make_pair("Connection","close"));
	}
	
	header.insert(std::make_pair("Content-Type",contenttype));
	
	for( const auto & headline : otherheader){
		header.insert(headline);	
	}

	httpd_output_response(status,header);
}

/** 猜测参数类型 **/
arg_type check_arg_type(const std::string argstr)
{
	std::map<enum arg_type,boost::regex> checker={
		{arg_type_roomid,boost::regex("[1-9][0-9][0-9][0-9]")},
		{arg_type_date,boost::regex("[1-2](0|9)[0-9][0-9][1-9][0-9][0-9][0-9]")},
		{arg_type_date_sql,boost::regex("[1-2](0|9)[0-9][0-9]-[0-1][0-9]-[0-3][0-9]")},
		{arg_type_date_period,boost::regex("[1-2](0|9)[0-9][0-9]-[0-1][0-9]-[0-9][0-9],[1-2](0|9)[0-9][0-9]-[0-1][0-9]-[0-3][0-9]")},
		{arg_type_uuid,boost::regex("[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
									"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
									"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
									"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
									"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]")},
		{arg_type_date_offset,boost::regex("([0-9]|-[0-9])")},
	};

	for (auto & regexer : checker) {
		if(boost::regex_match(argstr,regexer.second))
			return regexer.first;
	}
	return arg_type_date_unknow;
}

bool match_key(const std::string & line,const std::string & _key)
{
	std::string key = _key + "=";
	return strncasecmp(line.c_str(),key.c_str(),key.length())==0;
}

void  walkdir(const fs::path & dir , std::function<void( const fs::path & item )> cb)
{
	for(fs::directory_iterator diritend,dirit(dir);dirit!=diritend ; dirit++){
		cb(dirit->path());
	}
}

std::string md5(const uint8_t* message,uint length)
{
	CryptoPP::Weak1::MD5 hash;
	CryptoPP::HexEncoder encoder(nullptr,false /*uppercase=false*/ );

	byte digest[ CryptoPP::Weak1::MD5::DIGESTSIZE ];

	hash.CalculateDigest( digest, message, length);

	std::string output;
	encoder.Attach( new CryptoPP::StringSink( output ));
	encoder.Put( digest, sizeof(digest) );
	encoder.MessageEnd();
	return output;
}

std::string md5(const char* message,uint length)
{
	return md5(reinterpret_cast<const uint8_t*>(message),length);
}

std::string md5(const std::string message)
{
	return md5(message.c_str(),message.length());
}

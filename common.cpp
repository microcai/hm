
#include "pch.hpp"
#include "hm.hpp"

int opt_check_for(const std::string opt,int argc , const char * argv[])
{
	for(int i =0;i < argc ; i++){
		if(opt == argv[i])
		{
			return i;
		}
	}
	return -1;
}

fs::path hm_getdbdir(void)
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

fs::path hm_getexecdir(void)
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

bool hm_hasroom(const std::string &roomid)
{
	fs::path roomdir =  hm_getdbdir() / "rooms" / roomid;

	return fs::exists(roomdir);
}

int os_exec(const fs::path &exe,int argc,const char * argv[])
{
	char* exe_argv[argc+1];

	for(int i=0;i<argc;i++){
		exe_argv[i]=strdup(argv[i]);
	}

	exe_argv[argc]=NULL;

	int ret = ::execv(exe.c_str(),exe_argv);
	ret = ::execvp(exe.c_str(),exe_argv);

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

int hm_main_caller(MAINFUNC mainfunc, const char * arg1,const char * arg2,...)
{
	std::vector<const char*> argv;

	const char * p;
	va_list va;

	argv.push_back(arg1);
	argv.push_back(arg2);

	if(!arg2){
		va_start(va,arg2);
		while(p = va_arg(va,const char *))
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
	hmrunner uuidgen(main_shell);
	uuidgen.main("shell","uuidgen",NULL);

	std::string output;

	char line[800];

	output = fgets(line,sizeof(line),uuidgen);

	uuidgen.wait();

	return output.substr(0,36);
}

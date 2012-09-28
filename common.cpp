
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

int bring_editor(fs::path filename)
{
	// fork and execv EDITOR. if no EDITOR, vim

	int status;

	// first, find EDITOR

	const char * _editor = std::getenv("EDITOR");

	if(!_editor){ // no EDITOR env, fallback to vim
		_editor = "vim";
	}

	pid_t pid =fork();
	if(pid==0){
		execlp( _editor,_editor , filename.c_str(), NULL 	);
		return 127;
	}else if(pid > 0){
		waitpid(pid,&status,0);
	}else{
		std::cerr << "无法创建进程" << std::endl;
	}

	return status;
}

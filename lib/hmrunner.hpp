#pragma once
#include <sys/socket.h>
/*
 * run hm command and take the output as input
 */
class hmrunner{
public:
	hmrunner(MAINFUNC func){
			mainfunc = func;
			pipe=NULL;
			m_pwd = fs::current_path();
			pid = -1;
		};

	~hmrunner(){
			wait();
			std::fclose(pipe);
		};

	// return FILE *, must be called by main()
	operator FILE*(){
		if(pipe)
			return pipe;
		else{
			throw "call main before get pipe";
		}
	};

	// fork and run main, if fork failed , return -1
	template<typename... Args>
	int  main(const char * arg1,Args... args){
		int fds[2];
		//::pipe(fds);

		socketpair(AF_UNIX,SOCK_STREAM,0,fds);

		pid=fork();

		if(pid==0){
			close(fds[0]);
			dup2(fds[1],0);
			dup2(fds[1],1);
			dup2(fds[1],2);
			close(fds[1]);

			chdir(this->m_pwd.c_str());

			int ret = hm_main_caller(mainfunc,arg1,args...);
			exit(ret);
		}else if(pid < 0){
			close(fds[1]);
			close(fds[0]);
			return -1;
		}else{
			close(fds[1]);
			this->pipe = fdopen(fds[0],"r+");
		}
	}

	// wait and get result
	// note: will deadlock if you didn't read !
	int wait(){
		int status;

		if(pid ==-1)
			return -1;

		::waitpid(pid,&status,0);
		pid = -1;
		return status;
	};

	const fs::path & pwd(const fs::path & pwd){
		this->m_pwd = pwd;
		return m_pwd;
	}

private:
	MAINFUNC mainfunc;
	FILE * pipe;
	pid_t pid;
	fs::path m_pwd;
};

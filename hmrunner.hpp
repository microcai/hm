#pragma once
/*
 * run hm command and take the output as input
 */
class hmrunner{
public:
	hmrunner(MAINFUNC func):
		mainfunc(func){
			pipe=NULL;
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
	int  main(const char * arg1,const char * arg2,Args... args){
		int fds[2];
		::pipe(fds);

		pid=fork();

		if(pid==0){
			close(fds[0]);
			dup2(fds[1],1);
			dup2(fds[1],2);

			int ret = hm_main_caller(mainfunc,arg1,arg2,args...);
			exit(ret);
		}else if(pid < 0){
			close(fds[1]);
			close(fds[0]);
			return -1;
		}else{
			close(fds[1]);
			this->pipe = fdopen(fds[0],"r");
		}
	}

	// wait and get result
	// note: will deadlock if you didn't read !
	int wait(){
		int status;
		::waitpid(pid,&status,0);
		return status;
	};

private:
	MAINFUNC mainfunc;
	FILE * pipe;
	pid_t pid;
};

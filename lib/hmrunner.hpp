#pragma once
#include <string.h>
#include <tuple>

/*
 * run hm command and connect it's stdio
 */
class hmrunner{
public:
	hmrunner(MAINFUNC func){
			mainfunc = func;
			pipe=NULL;
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

	/*READ ONE LINE*/
	hmrunner & operator >>(std::string &_line){
		_line.clear();
		
		char line[80]={0};
		do{
			memset(line,0,sizeof(line));
			fgets(line,sizeof(line),*this);
			_line += line;			
		}while(line[78]!=0 && line[79]!='\n');		
		return *this;
	};

	// fork and run main, if fork failed , return -1
	template<typename... Args>
	int  main(const char * arg1,Args... args){
		int fds[2];
		//::pipe(fds);

		socketpair(AF_UNIX,SOCK_STREAM,0,fds);

		if( std::get<0>(callatfork) ){
			std::get<1>(callatfork)();
		}
		pid=fork();

		if(pid==0){
			if( std::get<0>(callatfork) ){
				std::get<3>(callatfork)();
			}
			close(fds[0]);
			dup2(fds[1],0);
			dup2(fds[1],1);
			close(fds[1]);

			exit(hm_main_caller(mainfunc,arg1,args...));			
		}else if(pid < 0){
			close(fds[1]);
			close(fds[0]);
			return -1;
		}else{
			
			if( std::get<0>(callatfork) ){
				std::get<2>(callatfork)();
			}
			
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

	typedef std::function<void()> atfork_t;

	static void dumy(){}

	void atfork(atfork_t child = hmrunner::dumy,atfork_t parent = hmrunner::dumy,atfork_t prepare = hmrunner::dumy){
		callatfork = std::make_tuple(true,prepare,parent,child);
	}

private:
	std::tuple<bool,atfork_t,atfork_t,atfork_t> callatfork;
	
	MAINFUNC mainfunc;
	FILE * pipe;
	pid_t pid;
};

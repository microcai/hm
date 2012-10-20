#pragma once
#include <unistd.h>
#include <sys/socket.h>
#include <string>
#include <tuple>
#include <cstring>

#include "../sub.hpp"
/*
 * run hm command and connect it's stdio
 */
class hmrunner{
public:
	hmrunner(MAINFUNC func){
			mainfunc = func;
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
			std::memset(line,0,sizeof(line));
			if(std::fgets(line,sizeof(line),pipe))
				_line+=line;
		}while(line[78]!=0 && line[79]!='\n');
		return *this;
	};

	/* return the child stdout as string*/
	operator std::string(){
		std::string ret;
		char buffer[32];
		int s;
		while(!std::feof(pipe) && (s = fread(buffer,1,sizeof(buffer),pipe))>0){
			ret.append(buffer,s);
		}
		return ret;
	};

	// fork and run main, if fork failed , return -1
	template<typename... Args>
	int  main(const char * arg1,Args... args){
		int fds[2];

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
	void atfork(atfork_t child = []{},atfork_t parent = []{},atfork_t prepare = []{}){
		callatfork = std::make_tuple(true,prepare,parent,child);
	}

private:
	std::tuple<bool,atfork_t,atfork_t,atfork_t> callatfork;

	MAINFUNC mainfunc=nullptr;
	FILE * pipe=nullptr;
	pid_t pid=-1;
};

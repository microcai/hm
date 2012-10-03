
#pragma once

typedef int (*MAINFUNC)(int argc, const char * argv[]);

//typedef boost::function<int(int argc, const char * argv[])> MAINFUNC;

int main_book(int argc , const char * argv[]);

int main_client(int argc , const char * argv[]);

template<typename... Args>
inline int main_client(const char * arg1,Args... args){
	int hm_main_caller(MAINFUNC mainfunc, const char * arg1,const char * arg2,...);
	hm_main_caller(main_client,arg1,args...);
};

int main_init(int argc , const char * argv[]);

int main_status(int argc , const char * argv[]);

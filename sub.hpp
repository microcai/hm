
#pragma once

typedef int (*MAINFUNC)(int argc,const char * argv[]);

//typedef boost::function<int(int argc, const char * argv[])> MAINFUNC;

int main_book(int argc, const char * argv[]);

int main_client(int argc, const char * argv[]);

int main_init(int argc, const char * argv[]);

int main_status(int argc, const char * argv[]);

int main_shell(int argc, const char * argv[]);

int main_gc(int argc, const char * argv[]);

int main_httpd(int argc, const char * argv[]);

int main_httpfile(int argc, const char * argv[]);

int main_cgi(int argc, const char * argv[]);
// kate: indent-mode cstyle; indent-width 4; replace-tabs off; tab-width 4;

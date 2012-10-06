#include "pch.hpp"
#include "hm.hpp"


int main_httpfile(int argc , const char * argv[])
{
	/**
	 * hm httpfile FILE, then return the file. used by hm httpd only
	 *
	 * if run with nginx or apache, use them. only hm cgi is then called by nginx/apache
	 **/

	//fs::wwwroot =  argv[1];

	fs::path wwwroot = hm_getwwwroot();

	// return http-response body

	/**
	 * HTTP/1.1 200 OK
	 * content-type:
	 * content-length:
	 *
	 * --content--
	 **/




	std::cout << "hm httpfile " << std::endl;


	std::cout << getenv("REQUEST_METHOD") << std::endl;// REQUEST_METHOD


}

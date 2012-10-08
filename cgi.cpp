
#include "pch.hpp"
#include "hm.hpp"

int main_cgi(int argc , const char * argv[])
{
	httpd_output_response(200,"application/x-javascript");

	// hm cgi , or hm-cgi , is called by httpd va /cgi/hm-cgi/path_info?query_string

	//最重要的是解析这个哦！ PATH_INFO
	const char  * PATH_INFO = getenv("PATH_INFO");
	const char  * QUERY_STRING = getenv("QUERY_STRING");

	std::cout << "cgi test , url is " << PATH_INFO << std::endl;

	std::cout << "query string is " << QUERY_STRING << std::endl;
	return 0;
}


#include "pch.hpp"
#include "hm.hpp"

int main_cgi(int argc , const char * argv[])
{
	httpd_output_response(200,"application/x-javascript");

	// hm cgi , or hm-cgi , is called by httpd va /cgi/hm-cgi/path_info?query_string

	//最重要的是解析这个哦！ PATH_INFO
	std::string PATH_INFO = getenv("PATH_INFO");
	std::string QUERY_STRING = getenv("QUERY_STRING");

	//根据 path_info 进行选择吧！
	if(PATH_INFO=="status"){ // 列出指定日期的房间的状态。




	}


	std::cout << "cgi test , url is " << PATH_INFO << std::endl;
	std::cout << "query string is " << QUERY_STRING << std::endl;
	return 0;
}

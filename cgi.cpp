
#include "pch.hpp"
#include "hm.hpp"

int main_cgi(int argc , const char * argv[])
{
	// hm cgi , or hm-cgi , is called by httpd va /cgi/hm-cgi/path_info?query_string

	//最重要的是解析这个哦！ PATH_INFO
	std::string PATH_INFO = getenv("PATH_INFO");
	std::string QUERY_STRING;

	if(getenv("QUERY_STRING"))
		QUERY_STRING = getenv("QUERY_STRING");


	std::cerr << "cgi test , url is " << PATH_INFO << std::endl;
	std::cerr << "query string is " << QUERY_STRING << std::endl;

	//根据 path_info 进行选择吧！
	if(PATH_INFO=="/status"){ // 列出指定日期的房间的状态。
		httpd_output_response(200,"text/json");

		std::cerr << "output www/book/test.json" << std::endl;

		hm_main_caller(main_shell,"shell","cat","www/book/test.json",NULL);
	}

	httpd_output_response(200);

	return 0;
}

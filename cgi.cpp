
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


	std::cerr << "cgi: url is " << PATH_INFO << std::endl;
	std::cerr << "cgi: query string is " << QUERY_STRING << std::endl;

	//根据 path_info 进行选择吧！
	if(PATH_INFO=="/status"){

		httpd_output_response(200);

 		std::cout << "[\n";
		auto ret = hm_main_caller(main_status,"status","--json",QUERY_STRING.c_str(),NULL);
		std::cout << "\n]\n";
		return ret;
	}else if(PATH_INFO=="/book"){ // book  JSON

	}else if(PATH_INFO=="/clientlist"){ // clientlist  JSON

		/**
		 * call hm client list, for each UUID , call client UUID --json
		 * */
		
		
		
	}else if(PATH_INFO=="/today"){ // return today

		httpd_output_response(200);
		std::cout << boost::gregorian::to_sql_string( boost::gregorian::day_clock::local_day());
	}else
		httpd_output_response(404);
	return 0;
}

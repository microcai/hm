
#include "pch.hpp"
#include "hm.hpp"

int main_cgi(int argc , const char * argv[])
{
	httpd_output_response(200,"application/x-javascript");

	// hm cgi , or hm-cgi , is called by httpd va /cgi/hm-cgi?
	//boost::date_time::

	//最重要的是解析这个哦！ PATH_INFO

	std::cout << "cgi test , url is " << getenv("REQUEST_URI") << std::endl;
	return 0;
}

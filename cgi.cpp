
#include "pch.hpp"
#include "hm.hpp"

static int cgi_today()
{
	httpd_output_response(200,"application/json");
	std::cout << boost::gregorian::to_sql_string( boost::gregorian::day_clock::local_day());
	return EXIT_SUCCESS;
}

static int cgi_status(const std::string &PATH_INFO,const std::string &QUERY_STRING)
{
	httpd_output_response(200);

	std::cout << "[\n";
	auto ret = hm_main_caller(main_status,"status","--json",QUERY_STRING.c_str(),NULL);
	std::cout << "\n]\n";
	return ret;
}

static int cgi_clientlist()
{
	/**
	* call hm client list, for each UUID , call client UUID --json
	* */
	bool isfist=true;
	
	std::string line;
	hmrunner clientlist(main_client);
	clientlist.main("client","list",NULL);
	std::cout << "[\n";
	while(!feof(clientlist)){//收集输出
		clientlist >> line;
		line = line.substr(0,36); // 去掉末尾的 \n
		//调用 hm client UUID
		if(check_arg_type(line)==arg_type_uuid){
			if(!isfist){
				std::cout <<"," << std::endl;
			}
			isfist=false;
			hm_main_caller(main_client,"client",line.c_str(),"--json",NULL);			
		}
	}
	std::cout << "\n]\n";
	return EXIT_SUCCESS;
}

static int cgi_clientautocomp(const std::string &PATH_INFO,const std::string &QUERY_STRING)
{
	/**
	* call to cgi /clientlist to get client JSON data. and then
	* parse the JSON data acorrding to QUERY_STRING and return
	* single array back to browser as jquery expected
	**/
	std::string term;
	
	if(QUERY_STRING.find("term=")!=std::string::npos){
		term = QUERY_STRING.substr(5);
	}
	
	hmrunner clientlist(main_cgi);

	clientlist.atfork(
		[](){
			setenv("PATH_INFO","/clientlist",1);
			unsetenv("QUERY_STRING");
		}
	);

	clientlist.main("cgi",static_cast<const char *>(nullptr),nullptr);

	std::stringstream jsonstream;

	while(!feof(clientlist)){
		std::string line ;
		clientlist >> line;
		jsonstream << line;// +=line;
		//jsonstring << line;
	}
	clientlist.wait();
	//子进程退出了，要获得的json也获得了。	
	boostpt::ptree clientlists;
	boostjs::read_json(jsonstream,clientlists);

	httpd_output_response(200,"application/json");
	std::cout << "[\n";
	// 依据已经获得的
	for(auto &client : clientlists){
		std::string name = client.second.get<std::string>("name");
		if(name.find(term)!=std::string::npos){
			// TODO: compare with term
			std::cout << "\t" << "\"";
			std::cout << name;
			std::cout << "\"" << ",\n";			
		}
	}
	std::cout << "\"\"\n";	
	std::cout << "]\n";
	return EXIT_FAILURE;
}

// hm cgi , or hm-cgi , is called by httpd va /cgi/hm-cgi/path_info?query_string
int main_cgi(int,const char **)
{
	// description , main and usage
	typedef std::tuple<const std::string,const std::function<int()>,const std::string> cgiworker;
	
	std::string PATH_INFO(getenv("PATH_INFO"));
	std::string QUERY_STRING(getenv("QUERY_STRING")?getenv("QUERY_STRING"):"");

	std::map<std::string,cgiworker>	cgimapper ={
		{"/today",std::make_tuple("return the date of today",cgi_today,"")},
		{"/clientlist",std::make_tuple("return list of clients",cgi_clientlist,"")},
		{"/status",std::make_tuple("return status of rooms at given date",
								   std::bind(cgi_status,std::cref(PATH_INFO),std::cref(QUERY_STRING)),
								   "?[date],[date]")
		},
		{"/clientautocomp",std::make_tuple("return list of clients",
										   std::bind(cgi_clientautocomp,std::cref(PATH_INFO),std::cref(QUERY_STRING)),
										   "")
		},
	};

	std::cerr << "cgi: url is " << PATH_INFO << std::endl;
	std::cerr << "cgi: query string is " << QUERY_STRING << std::endl;

	//根据 path_info 进行选择吧！
	if(cgimapper.find(PATH_INFO)!=cgimapper.end()){
		return std::get<1>(cgimapper[PATH_INFO])();
	}else if(PATH_INFO=="/"){ // output the index page! this page will tell you how to use our JSON API!
		httpd_output_response(200,"text/html");

		std::cout << "<html>\n<meta charset=\"utf-8\">\n";
		std::cout << "<title>hm-cgi JSON 一览表</title>" ;
		std::cout << "<body>\n" ;

		std::cout << "<div>\n" ;
		std::cout << "\t<p>这里是 hm-cgi 支持的 JSON 调用的列表</p>\n" ;
		std::cout << "</div>\n" ;
		std::cout << "<div>\n" ;
		std::cout << "<lu>\n" ;

		for(auto & it: cgimapper){
			/**
			 * format page like this
			 *
			 * <var>url </var> : usage
			 *
			 * <p>description</p>
			 **/
			std::cout << "<li><a href=\"" << "/cgi-bin/hm-cgi" <<  it.first << "\">" << "<var>" <<  it.first << "</var></a>";
			std::cout << "<var>" << std::get<2>(it.second) << "</var>";
			std::cout << "<p>" << std::get<0>(it.second) << "</p>";
			std::cout << "</li>\n";
		}
		std::cout << "</lu>\n";
		std::cout << "</div>\n";
		std::cout << "</body>\n";
		std::cout << "</html>\n";
		
	}else{
		httpd_output_response(404);
	}
	return 0;
}

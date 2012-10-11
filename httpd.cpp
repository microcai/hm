#include "pch.hpp"
#include "hm.hpp"

/**
 * get http header as std::map.
 **/
static std::map<std::string,std::string> httpd_get_request()
{
	bool request_end = false;
	char maxline[1500]={0};

	std::map<std::string,std::string> header;

	std::cin.getline(maxline,sizeof(maxline));

	std::vector<std::string> tokens;

	boost::algorithm::split(tokens,maxline,boost::is_any_of(" \t\n\r"));

	header.insert(std::make_pair("type",tokens[0]));

	header.insert(std::make_pair("url",tokens[1]));

	header.insert(std::make_pair("ver",tokens[2]));

	// 处理余下的
	do{
		tokens.clear();

		memset(maxline,0,sizeof(maxline));
		//maxline
		std::cin.getline(maxline,sizeof(maxline));
		/*remove terminating \r\n */
		switch(* std::string(maxline).rbegin()){
			case '\r':
			case '\n':
				maxline[std::string(maxline).length()-1]=0;
		}

		if( std::string(maxline).length()){
			int pos = std::string(maxline).find(": ");

			std::string key =  std::string(maxline).substr(0,pos);
			std::string val =  std::string(maxline).substr(pos+2,-1);

			header.insert(std::make_pair(key,val));
		}else{
			request_end = true;
		}
	}while(!request_end);
	return header;
}

int main_httpd(int argc , const char * argv[])
{
	/*
	 * hm http - start http daemon, basically , it's bashttpd
	 *
	 * it's xinet'ed interface, or it can used with fastcgi
	 *
	 * but if you run with --listen , then it's real httpd
	 *
	 * internal http will fork() and re-exec itself to serve the request, so it's like xinet/fastcgi
	 */

	bool listen =  opt_check_for("--listen", argc,argv) >=0;
	if(listen){

		int port = 4000;

		int port_arg =  opt_check_for("--port",argc,argv);
		if(port_arg>=0){
			port = atoi(argv[port_arg+1]);
		}

		std::cout << "running service @" << port << std::endl;

		asio::io_service iosev;
		asio::ip::tcp::acceptor acceptor(iosev,asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port));

		// accept sigchild
		::signal(SIGCHLD,[](int signal_number){
					int status;
					pid_t pid = waitpid(0,&status,0);
				});

		//socket();
		for(;;){
			// socket对象
			asio::ip::tcp::socket socket(iosev);
			// 等待直到客户端连接进来
			acceptor.accept(socket);
			// 显示连接进来的客户端

			int nativesocket = socket.native_handle();
			//nativesocket

			// fork 后处理
			pid_t pid = fork();

			//pid_t pid = 0;//fork();

			if(pid == 0 ){
				dup2(nativesocket,0);
				dup2(nativesocket,1);
				goto processrequest;
			}

			// 显示连接进来的客户端
			
			std::cout <<  "forked child " << pid <<   " to accept client: " << socket.remote_endpoint().address() << std::endl;
		}

	}

	/**
	 * process http request here. use stdin/stdout as socket :)
	 **/
processrequest:

	// read http request
	auto httpheader = httpd_get_request();

	// append envs
	std::map<std::string,std::string> child_env = getenvall();

	child_env.insert(std::make_pair("REQUEST_METHOD",httpheader["type"]));
	child_env.insert(std::make_pair("REQUEST_URI",httpheader["url"]));

	std::string pathinfo = httpheader["url"].substr(0,httpheader["url"].find('?'));

	// 处理 http request:
	// URL 匹配
	if(boost::regex_match(httpheader["url"],boost::regex("/(cgi|cgi-bin)/.*"))){

		if(!boost::regex_match(httpheader["url"],boost::regex("/(cgi|cgi-bin)/hm-cgi/.*"))){
			// not hm-cgi, bad
			goto cgi_not_found;
		}

		static const char *child_argv[3]={
			"hm",
			"cgi",
		};

		size_t pathinfostart;

		// 添加 PATH_INFO， 这个是非常非常重要的哦。
		if( ( (pathinfostart =  pathinfo.find_first_not_of("/cgi-bin/hm-cgi"))!= std::string::npos ) ||
			( (pathinfostart =  pathinfo.find_first_not_of("/cgi/hm-cgi"))!= std::string::npos ) 	)
		{
			child_env.insert(std::make_pair("PATH_INFO",pathinfo.substr(pathinfostart-1)));

			if(httpheader["url"].find('?')!=std::string::npos){
				child_env.insert(
					std::make_pair("QUERY_STRING",httpheader["url"].substr(httpheader["url"].find('?')+1))
				);
			}
		}

		os_exec(os_exe_self(),2,child_argv,child_env);

cgi_not_found:
		httpd_output_response(404);
		std::cout << "cgi script not found" << std::endl;
		std::cout.flush();
		exit(127);

	}else{ // run hm httpfile

		//reexec();
		const char *child_argv[3]={
			"hm",
			"httpfile",
			pathinfo.c_str()
		};

		/** only GET supported **/
		if(httpheader["type"]=="GET"){
			os_exec(os_exe_self(),3,child_argv,child_env);
		}
	}

	if(httpheader["Connection"] == "keep-alive")
		goto processrequest;
	return EXIT_SUCCESS;
}

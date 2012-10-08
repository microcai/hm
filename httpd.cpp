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

	header.insert(std::pair<std::string,std::string>("type",tokens[0]));

	header.insert(std::pair<std::string,std::string>("url",tokens[1]));

	header.insert(std::pair<std::string,std::string>("ver",tokens[2]));

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

			header.insert(std::pair<std::string,std::string>(key,val));
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
					pid_t pid = wait(&status);
					std::cout << "child " << pid <<" exited" << std::endl;
				});

		//socket();
		for(;;){
			// socket对象
			asio::ip::tcp::socket socket(iosev);
			// 等待直到客户端连接进来
			acceptor.accept(socket);
			// 显示连接进来的客户端

			// 显示连接进来的客户端
			std::cout << "accept client: " << socket.remote_endpoint().address() << std::endl;

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

		}

	}

	/**
	 * process http request here. use stdin/stdout as socket :)
	 **/
processrequest:

	// read http request
	auto httpheader = httpd_get_request();

	// append envs
	std::vector<std::string> child_env = getenvall();

	child_env.push_back(std::string("REQUEST_METHOD=") + httpheader["type"]);
	child_env.push_back(std::string("REQUEST_URI=") + httpheader["url"]);

	// 处理 http request:

	// URL 匹配
	if(boost::regex_match(httpheader["url"],boost::regex(".*/(cgi|cgi-bin)/.*"))){

		const char *child_argv[3]={
			"hm",
			"cgi",
		};

		//child_env.push_back(std::string("REQUEST_METHOD=")+ httpheader["type"] );

		os_exec(os_exe_self(),2,child_argv,child_env);

		httpd_output_response(404);
		std::cout << "cgi script not found" << std::endl;
		std::cout.flush();
		exit(127);

	}else{ // run hm httpfile

		if(*httpheader["url"].rbegin() == '/')
			httpheader["url"].append("index.html");
		//reexec();
		const char *child_argv[3]={
			"hm",
			"httpfile",
			httpheader["url"].c_str()
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

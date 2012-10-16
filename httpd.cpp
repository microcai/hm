#include "pch.hpp"
#include "hm.hpp"

/**
 * get http header as std::map.
 **/
static std::map<std::string,std::string> httpd_get_request()
{
	char maxline[1500]={0};
	std::vector<std::string> tokens;

	std::cin.getline(maxline,sizeof(maxline));
	boost::algorithm::split(tokens,maxline,boost::is_any_of(" \t\n\r"));

	std::map<std::string,std::string> header={
		{"type",tokens[0]},
		{"url",tokens[1]},
		{"ver",tokens[2]},
	};

	// 处理余下的
	while(true){
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
			header.insert(
				std::make_pair(
					std::string(maxline).substr(0,pos),
					std::string(maxline).substr(pos+2,-1)
				)
			);
		}else{
			break;
		}
	}
	return header;
}

static void httpd_signal_SIGCHLD_hander(int signal_number)
{
	int status;
	pid_t pid = waitpid(0,&status,0);
}

static void httpd_signal_reexec_hander(int signal_number,int native_handle)
{
	//try to reexec :)
	char fdstr[20]={0};
	snprintf(fdstr,sizeof(fdstr),"%d",native_handle);
	const char * argv[]={
		"hm",
		"httpd",
		"--listen",
		"--sockfd",
		fdstr,
	};

	std::cerr << "re-exec myself, hint Ctrl-C again with-in 2s to stop" << std::endl;

	os_exec(os_exe_self(),5,argv);

	std::cerr << "re-exec failed" << std::endl;
	exit(EXIT_FAILURE);
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
		asio::io_service iosev;
		int port = 4000;
		int native_handle = -1;

		if(opt_check_for("--port",argc,argv)>=0){
			port = atoi(argv[opt_check_for("--port",argc,argv)+1]);
		}

		if(opt_check_for("--sockfd",argc,argv)>=0){
			native_handle = atoi(argv[opt_check_for("--sockfd",argc,argv)+1]);
		}
		if(native_handle>0){
 			std::cout << "running service by passed socket " << native_handle << std::endl;
		}else{
			std::cout << "running service @" << port << std::endl;
		}

		auto acceptor_builder =  [&iosev,&port,&native_handle](){
			if(native_handle>0)
				return asio::ip::tcp::acceptor(iosev,asio::ip::tcp::v6(),native_handle);
			else
				return asio::ip::tcp::acceptor(iosev,asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port));
		};

		asio::ip::tcp::acceptor acceptor =  acceptor_builder();

		// accept SIGCHLD,SIGHUP
		hm_signal(SIGCHLD, httpd_signal_SIGCHLD_hander);
		hm_sigmask(SIG_UNBLOCK,SIGINT);
		hm_signal(SIGALRM,[&acceptor](int signal_number){
			hm_signal(SIGINT,std::bind(httpd_signal_reexec_hander,std::placeholders::_1,acceptor.native_handle()));
			signal(SIGALRM,NULL);
		});
		alarm(2);

		for(;;){
			// socket对象
			asio::ip::tcp::socket socket(iosev);
			// 等待直到客户端连接进来
			acceptor.accept(socket);

			// fork 后处理
			pid_t pid = fork();
			if(pid == 0){
				std::cout.flush();
				dup2(socket.native_handle(),0);
				dup2(socket.native_handle(),1);
				// 子进程进行处理
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
	auto child_env = getenvall();

	child_env.insert(std::make_pair("REQUEST_METHOD",httpheader["type"]));
	child_env.insert(std::make_pair("REQUEST_URI",httpheader["url"]));

	std::string pathinfo = httpheader["url"];

	if(pathinfo.find("?")!=std::string::npos)
	{
		child_env.insert(
			std::make_pair("QUERY_STRING",pathinfo.substr(pathinfo.find('?')+1))
		);
		pathinfo=pathinfo.substr(0,pathinfo.find("?"));
	}
	// 添加 PATH_INFO， 这个是非常非常重要的哦。
	child_env.insert(std::make_pair("PATH_INFO",pathinfo));

	// 处理 http request:
	if(boost::regex_match(httpheader["url"],boost::regex("/(cgi|cgi-bin)/.*"))){// URL 匹配到 cgi
		if(boost::regex_match(httpheader["url"],boost::regex("/(cgi|cgi-bin)/hm-cgi/.*"))){
			if(pathinfo.find("/hm-cgi/")!=std::string::npos){
				// 依据cgi路径更新PATH_INFO
				child_env["PATH_INFO"] = pathinfo.substr(pathinfo.find("/hm-cgi/")+7);
			}
			static const char *child_argv[2]={"hm","cgi"};
			os_exec(os_exe_self(),2,child_argv,child_env);
		}
		// exec 失败，or cgi 脚本未找到
		httpd_output_response(404);
		std::cout << "cgi script not found" << std::endl;
		exit(127);
	}else{ // run hm httpfile
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

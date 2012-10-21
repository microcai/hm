#include "pch.hpp"

#include "hm.hpp"

/**
 * get one line of input from fd. no buffer.
 */
static std::string	fdgetline(int fd)
{
	char buffer[1];
	std::string ret;
	ret.reserve(100);
	while(read(fd,buffer,1)==1){
		if(buffer[0]=='\n')
			break;
		ret+=buffer[0];
	}
	/*remove terminating \r*/
	if(* ret.rbegin()=='\r'){
		ret.erase(ret.length()-1);
	}
	return ret;
}

/**
 * get http header as std::map.
 *
 * note: can't use std::cin here, cause the cgi script might need the input data.thus these
 * shall not be buffered by std::cin, so we call to fdgetline() instead of std::cin.getline()
 **/
static std::map<std::string,std::string> httpd_get_request()
{
	std::vector<std::string> tokens;
	std::string line;

	line = fdgetline(STDIN_FILENO);
	boost::algorithm::split(tokens,line,boost::is_any_of(" \t\n\r"));

	std::map<std::string,std::string> header={
		{"type",tokens[0]},
		{"url",tokens[1]},
		{"ver",tokens[2]},
	};

	// 处理余下的
	while(true){
		tokens.clear();
		line = fdgetline(STDIN_FILENO);

		if(line.length()){
			int pos = line.find(": ");
			header.insert(
				std::make_pair(
					line.substr(0,pos),
					line.substr(pos+2,-1)
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

static bool http_check_auth(std::map<std::string,std::string> &httpheader)
{
	return httpheader.find("Authorization")==httpheader.end();
// 	return false;
}

static int http_auth(std::map<std::string,std::string> &httpheader)
{
	if(!http_check_auth(httpheader)){
		// 401
		std::string authstring = "Digest realm=\"\",\
		qop=\"auth,auth-int\",\
		nonce=\"dcd98b7102dd2f0e8b11d0f600bfb0c093\",\
		opaque=\"5ccc069c403ebaf9f0171e9517f40e41\"";

		std::string response = "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\n"
			"\"http://www.w3.org/TR/1999/REC-html401-19991224/loose.dtd\">\n"
			"<HTML>\n"
			"  <HEAD>\n"
			"    <TITLE>Error</TITLE>\n"
			"    <META HTTP-EQUIV=\"Content-Type\" CONTENT=\"text/html; charset=ISO-8859-1\">\n"
			"  </HEAD>\n"
			"  <BODY><H1>401 Unauthorized.</H1></BODY>\n"
			"</HTML>\n";

		std::map<std::string,std::string> authheader={
			{"WWW-Authenticate",authstring},
			{"Content-Type", "text/html"},
			{"Content-Length", itoa(response.length())},
		};
		httpd_output_response(401,authheader);


		std::cout << response;
		std::cout.flush();
		return false;
	}
	return true;
}
/**
 * process http request here. use stdin/stdout as socket :)
**/
static 	int httpd_processrequest()
{
	std::map<std::string,std::string> httpheader;
	
	do{
		// read http request
		httpheader = httpd_get_request();
	//处理 HTTP Digest
	}while(!http_auth(httpheader));
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
			if(!httpheader["Content-Length"].empty()){
				child_env.insert(std::make_pair("CONTENT_LENGTH",httpheader["Content-Length"]));
			}
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
			if(httpheader["Connection"] == "keep-alive"){
				if(hm_main_caller(main_httpfile,"httpfile",pathinfo.c_str(),nullptr)!=EXIT_SUCCESS)
					exit(EXIT_FAILURE);
				std::cout.flush();
			}else{
				os_exec(os_exe_self(),3,child_argv,child_env);
			}
		}
	}

	if(httpheader["Connection"] == "keep-alive"){
		const char *child_argv[2]={
			"hm",
			"httpd",
		};
		std::cout.flush();
		return os_exec(os_exe_self(),2,child_argv);
	}
	return EXIT_SUCCESS;
}

static void httpd_genaratenonce()
{
	std::string uuid = hm_uuidgen();
	std::string nonce = md5(reinterpret_cast<const uint8_t*>(uuid.c_str()),uuid.length());
	setenv("nonce",nonce.c_str(),1);
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

	char * nonce = getenv("nonce");
	if(!nonce){
		httpd_genaratenonce();
	}
	
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

		asio::ip::tcp::acceptor acceptor = [&iosev,port,native_handle](){
			if(native_handle>0)
				return asio::ip::tcp::acceptor(iosev,asio::ip::tcp::v6(),native_handle);
			else
				return asio::ip::tcp::acceptor(iosev,asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port));
		}();
		
		std::cout << "running service on " << acceptor.local_endpoint() << std::endl;

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
				return httpd_processrequest();
			}
			// 显示连接进来的客户端
			std::cout <<  "forked child " << pid <<   " to accept client: " << socket.remote_endpoint().address() << std::endl;
		}
	}

	return httpd_processrequest();
}

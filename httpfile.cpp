#include "pch.hpp"
#include "hm.hpp"


int main_httpfile(int argc , const char * argv[])
{
	/**
	 * hm httpfile FILE, then return the file. used by hm httpd only
	 *
	 * if run with nginx or apache, use them. only hm cgi is then called by nginx/apache
	 **/
	fs::path wwwroot = hm_getwwwroot();

	// return http-response body

	/**
	 * HTTP/1.1 200 OK
	 * content-type:
	 * content-length:
	 *
	 * --content--
	 **/

	// check for file
	std::string	url = argv[1];

	std::cerr << "httpfile: url is " << url << std::endl;
	if(getenv("QUERY_STRING"))
		std::cerr << "httpfile: query string is " << getenv("QUERY_STRING") << std::endl;
	
	fs::path fsfile = wwwroot / url;
	
	fsfile.normalize();
	int fsfilefd;

	if( *url.rbegin() == '/'  && fs::exists(fsfile / "index.html")){
		/* 有  index 就来  index 嘛*/
		fsfile /= "index.html";
	}else if(fs::is_directory(fsfile) && *url.rbegin()!= '/'){
		// output  301 move
		httpd_output_response(301,{{"Location",url + "/"}});
		return EXIT_SUCCESS;
	}
	
	if(!fs::exists(fsfile) || access(fsfile.c_str(),R_OK)!=0){
		httpd_output_response(404);
		return EXIT_SUCCESS;
	}else if( !fs::is_directory(fsfile) && (fsfilefd = open(fsfile.c_str(),O_RDONLY|O_NOATIME|O_CLOEXEC))>0){
		/**
		 * dont try to understand the lambda expresion used here :)
		 * just a demo how lambda can changed your mind.
		 */
		httpd_output_response(
			200,
			[&fsfile]()->std::string{
				if(fsfile.extension() == ".css"){
					return std::string("text/css");
				}
				// first , check for file type
				char mimetype[100]={0};
				// hm shell file  -i $file
				hmrunner file(main_shell);
				file.main("!","file","-b","--mime-type",fsfile.c_str(),NULL);
				*strchrnul(std::fgets(mimetype,sizeof(mimetype),file),'\n')=0;
				return std::string(mimetype);
			}(),
			fs::file_size(fsfile)
		);
		//file content now !
		sendfile(STDOUT_FILENO,fsfilefd,NULL,fs::file_size(fsfile));
		close(fsfilefd);
		return EXIT_SUCCESS;
	}else if(fs::is_directory(fsfile) && access(fsfile.c_str(),R_OK)==0){
		httpd_output_response(200,"text/html");

		std::cout << "<html>\n <HEAD>\n  <TITLE>Directory  " << url << "</TITLE>\n </HEAD>\n"
		<< "<body><H1>Directory listing of " << url << "</H1><UL>" << "<LI><A HREF=\"../\">../</A></LI>";
		walkdir(fsfile,[](const fs::path& diritem){
			//output dir content as index page
			std::string filename = diritem.filename().generic_string();
			if(fs::is_directory(diritem)){
				filename+="/";
			}
			std::cout << "<LI><A HREF=\"" << filename
				<< "\">" <<	filename << "</A></LI>";
		});
		std::cout << "</UL></UL>\n<H2>HM internal Simple HTTP Server, "
			<< boost::posix_time::second_clock::local_time()
			<< "</H2>\n<H3>Copyright 2011-2012; Microcai </H3>\n";
		std::cout << "</body></html>";
		return EXIT_FAILURE;
	}
	return EXIT_FAILURE;

}

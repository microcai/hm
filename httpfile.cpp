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
	}else if(fs::is_directory(fsfile) && *url.rbegin() != '/'  ){

		// output  301 move
		httpd_output_response(301);

		std::cout << url << "/"  << "\r\n";

		std::cout << "\r\n" ;
		std::cout.flush();
		return EXIT_SUCCESS;
		
	}
	
	if(!fs::exists(fsfile) || access(fsfile.c_str(),R_OK)!=0){

	}else if( !fs::is_directory(fsfile) && (fsfilefd = open(fsfile.c_str(),O_RDONLY|O_NOATIME|O_CLOEXEC))>0){

		auto mimetype = [&fsfile](){
			if(fsfile.extension() == ".css"){
				return std::string("text/css");
			}
			// first , check for file type
			char mimetype[100]={0};
			// hm shell file  -i $file
			hmrunner file(main_shell);
			file.main("!","file","-b","--mime-type",fsfile.c_str(),NULL);
			std::fgets(mimetype,sizeof(mimetype),file);

			*strchrnul(mimetype,'\n')=0;

			return std::string(mimetype);
		};

		httpd_output_response(200,mimetype(),fs::file_size(fsfile));

		//file content now !
		sendfile(1,fsfilefd,NULL,fs::file_size(fsfile));
		close(fsfilefd);
	}
}

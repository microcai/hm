
#include "pch.hpp"
#include "hm.hpp"

static std::string clienttemplate=
"#nick　是昵称，可重复使用相同昵称,必需填写。\n"
"nick=\n\n"
"#telephone 是电话号码，有多个电话可以填写多个,空格隔开\n"
"telephone=\n\n"
"#国籍，未知可不写\n"
"state=\n\n"
"#真实姓名，原名。\n"
"realname=\n\n"
"#中文名，如果是外国人的话。\n"
"realname_zh=\n\n"
"#身份证号码\n"
"id=\n\n"
"#护照号码\n"
"passport=\n\n"
"#其他证件类型的号码\n"
"otherid=\n\n"
"#一下为备注，备注以16个“-”起始。\n"
"----------------\n\n"
;

static void display_help(void)
{

}

static std::string client_get(const fs::path & clientfile,const std::string key)
{
	std::ifstream istream(clientfile.c_str());

	boost::regex regex(std::string("^") + key + "=[A-Za-z]*[a-zA-Z0-9 ]*[a-zA-Z0-9]*$");

	while(!istream.eof()){
		std::string line;
		istream >> line;

		// parse the line
		if( boost::regex_match(line,regex)){
			return line.substr(key.length()+1);
		}
	}
	throw "no nick defined!";
}

static fs::path newclientnick(fs::path clientdir,std::string nick)
{
	int number = 2;

	fs::path nickfile = clientdir / nick;

	while(fs::exists(nickfile))
	{
		std::stringstream surfix;
		surfix << number++;

		nickfile = clientdir / (nick + surfix.str());// nick.append(surfix.str().c_str());
	}

	return nickfile;
}

int main_client(const char * arg1,const char * arg2,...)
{
	std::vector<const char*> argv;

	const char * p;
	va_list va;

	argv.push_back(arg1);
	argv.push_back(arg2);

	if(!arg2){
		va_start(va,arg2);
		while(p = va_arg(va,(const char *) ))
		{
			argv.push_back(p);
		}
		va_end(va);
	}
	// argv expan to char **
	const char * _argv[argv.size()];
	for(int i=0;i < argv.size();i++)
		_argv[i] = argv[i];

	return main_client(argv.size(),_argv);
}

int main_client(int argc , const char * argv[])
{
	/*
	 * useage, hm client add, then popup vim, just fill in things
	 *
	 * hm client mod nick|clientid|phonenumber , etc , any thing that can refer to a client
	 */

	int argc_start = opt_check_for("client",argc,argv);

	if((argc - argc_start) ==1){
			display_help();
			return EXIT_SUCCESS;
	}

	fs::path hmdir = hm_getdbdir();

	if( std::string("add") ==  argv[argc_start+1] ){
		// hm client add

		// write out tempfile
		char* _tmppath = tempnam(hmdir.c_str(),"tmp_client_");
		fs::path tmppath(_tmppath); free(_tmppath);

		std::fstream tmpfile(tmppath.c_str(),std::ios::out);

		tmpfile << clienttemplate;
		tmpfile.close();

		// popup vim
		int ret = bring_editor(tmppath);

		if(ret){
			fs::remove(tmppath);
			return EXIT_FAILURE;
		}

		std::string nick;

		try{
		// 移动文件到　clients　下。首先获得　nick 。
		nick = client_get(tmppath,"nick");
		fs::path nickfile = newclientnick(hmdir / "clients", nick );
		fs::rename(tmppath, nickfile);
		}catch(...){
			fs::remove(tmppath);
			return EXIT_FAILURE;
		}

		return main_client("client","merge",nick.c_str(),NULL);

	}else if(std::string("merge") ==  argv[argc_start+1]){
		// hm client merge XXXX, automantically merge same nick. will be called after hm client add
		// hm client merge , automantically merge same client.  will be called after hm client mod

		std::cout << "TODO: merge same results" << std::endl;

		return EXIT_SUCCESS;

	}
}


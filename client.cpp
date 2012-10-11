
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

static bool json_output=false;

static std::string client_get(const fs::path & clientfile,const std::string key)
{
	keyvalfile clientinfo(clientfile);

	return clientinfo[key];
}

int main_client_add()
{
	auto hmdir = hm_getdbdir();

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
	fs::path nickfile = hmdir / "clients" / hm_uuidgen();
	fs::rename(tmppath, nickfile);
	}catch(...){
		fs::remove(tmppath);
		return EXIT_FAILURE;
	}

	return main_client("client","merge",nick.c_str(),NULL);
}

int main_client(int argc , const char * argv[])
{
	/*
	 * useage, hm client add, then popup vim, just fill in things
	 *
	 * hm client mod nick|clientid|phonenumber , etc , any thing that can refer to a client
	 */

	json_output = opt_check_for("--json",argc,argv)>0;
	if(json_output)
		opt_remove(argc,argv,"--json");

	int argc_start = opt_check_for("client",argc,argv)+1;

	if((argc - argc_start) ==0){
			display_help();
			return EXIT_SUCCESS;
	}

	fs::path hmdir = hm_getdbdir();

	if( std::string("add") ==  argv[argc_start] ){
		// hm client add
		main_client_add();
	}else if(std::string("merge") ==  argv[argc_start]){
		// hm client merge XXXX, automantically merge same nick. will be called after hm client add
		// hm client merge , automantically merge same client.  will be called after hm client mod

		std::cout << "TODO: merge same results" << std::endl;

		return EXIT_SUCCESS;

	}else if( check_arg_type(argv[argc_start]) == arg_type_uuid ){
		// hm client UUID
		std::string uuid = argv[argc_start];

		// check for UUID existence
		fs::path clientfile = hmdir / "clients" / uuid;

		if(!fs::exists(clientfile))
		{
			std::cerr << " client " << uuid << " not found!" << std::endl;
			return EXIT_FAILURE;
		}

		keyvalfile clientinfo(clientfile);

		if(json_output){// return json about this client
			std::cout << "{ " ;

			std::cout << "\"uuid\" : " << quote(uuid) << " , " ;

			std::cout << "\"name\" : " ;

			if( clientinfo["realname"].length())
			{
				std::cout << quote( clientinfo["realname"] ) << " , " ;
			}else{
				std::cout << quote( clientinfo["nick"] )  << " , " ;
			}

			std::cout << "}, " ;

		}else{//or just stdout

		}
	}else if ( std::string("list") ==  argv[argc_start] ){

		// 遍历 clients 文件夹
		hmdir  / "clients" ;
		
	}else{
		// return UUID as client
		// search for argv[argc_start], lets use grep !

		bool stopfisrt = opt_check_for("--first",argc,argv)!=-1;
		if(stopfisrt)
			opt_remove(argc,argv,"--first");

		bool userselect = opt_check_for("--select",argc,argv)!=-1;
		if(userselect)
			opt_remove(argc,argv,"--select");

		FILE * out = stdout;

		if(opt_check_for("--outfd",argc,argv)>=0)
		{
			int outfd = atoi(argv[ opt_check_for("--outfd",argc,argv) +1]);
			out = fdopen(outfd,"a");

			int optoutfd = opt_check_for("--outfd",argc,argv);
			opt_remove(argc,argv,optoutfd);
			opt_remove(argc,argv,optoutfd);
		}

		hmrunner greper(main_shell);

		greper.pwd(hmdir / "clients"); // let grep work in clients dir

		greper.main("!","grep",argv[argc_start],"-rl",NULL);

		std::vector<std::string> uuids;

		// read grep output
		do{
			char line[800]={0};
			fgets(line,sizeof(line),greper);

			std::string uuid = line;

			if(uuid.length()>36)
				uuids.push_back(uuid);

		}while(!stopfisrt && !feof(greper));

		if(stopfisrt || !userselect){
			for(int i = 0 ; i < uuids.size();i++)
				fputs(uuids[i].c_str(),out);
		}

		if( !stopfisrt && uuids.size() > 1 && userselect)// if more than one line, let user choise if no --fisrt
		{
			// ask questions
			// TODO
		}
		return EXIT_FAILURE;
	}
}


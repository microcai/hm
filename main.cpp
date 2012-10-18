
#include "pch.hpp"
#include "hm.hpp"
#include "sub.hpp"

#define FILLCMD(cmd,helpstr)	{ #cmd, main_##cmd, helpstr },

static struct{
	const std::string cmd;
	MAINFUNC main;
	const char * help;
}cmdlist[]={
	FILLCMD(book, "book the room" )

	FILLCMD(client, "manage the clients" )

	FILLCMD(init, "init database" )

	FILLCMD(status, "room status" )

	FILLCMD(shell, NULL)

	{"!",main_shell,NULL},

	FILLCMD(gc,"scan for outdated stuff")

	FILLCMD(httpd,"run as http daemon")

	FILLCMD(httpfile,NULL)

	FILLCMD(cgi,"cgi script, called by httpd")

};

static size_t _cmdlistsize(void)
{
	return (sizeof(cmdlist)/sizeof(cmdlist[0]));
}
#define cmdlistsize (_cmdlistsize())

static void display_help(const char *self)
{
	for(int i=0; i < cmdlistsize ; i++){
		if(cmdlist[i].help)
			std::cout << self  << "\t" <<  cmdlist[i].cmd << "\t" << cmdlist[i].help << std::endl;
	}
}

int main(int argc, const char *argv[])
{
	// check for --hmdir and remove that
	if(argc >=2 && std::string("--hmdir") == argv[1]){
		if(!fs::exists(argv[2])){
			std::cerr << "hmdir " << argv[2] << " does not exist!" << std::endl;
			return EXIT_FAILURE;
		}
		if(chdir(argv[2])<0){
			std::cerr << "cann\'t chdir to" << argv[2] << std::endl;
			exit(EXIT_FAILURE);
		}
		unsetenv("HMDIR");

		opt_remove(argc,argv,1);
		opt_remove(argc,argv,1);
	}

	for(int i=0; i < cmdlistsize ; i++){
		int opt_offset = 1;
		std::string cmd(argv[0]);
		if(argc >=2)
			cmd = argv[1];

		std::string str;
		

		int m = std::string(argv[0]).find_last_of("-");
		if( m != std::string::npos){
			cmd = std::string(argv[0]+m+1);
//			std::cerr << cmd;
			opt_offset = 0;
			argv[0] = cmd.c_str();
		};

		if( cmd == cmdlist[i].cmd)
			return cmdlist[i].main(argc-opt_offset, argv+opt_offset);
	}

	if(argc<=1){ // no argument
		display_help(argv[0]);
		return EXIT_SUCCESS;
	}

	// else run script located in HMEXEC
	fs::path hmexecdir = hm_getexecdir();

	fs::path script = hmexecdir / argv[1];

	script.normalize();

	if(fs::exists(script)){
		os_exec(script,argc -1, argv +1);
	}

	// exec failed, say , script not found
	std::cerr << "command `" << argv[1] << "\' not found" << std::endl;
    return EXIT_FAILURE;
}

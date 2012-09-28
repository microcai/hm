
#include "pch.hpp"
#include "hm.hpp"


void create_skel()
{

}

#define FILLCMD(cmd,helpstr)	{ #cmd, main_##cmd, helpstr },

static struct{
	const std::string cmd;
	MAINFUNC main;
	const char * help;
}cmdlist[]={
	FILLCMD(book, "reserve the room" )

	FILLCMD(client, "manage the clients" )

	FILLCMD(init, "init database" )
};

static size_t _cmdlistsize(void)
{
	return (sizeof(cmdlist)/sizeof(cmdlist[0]));
}
#define cmdlistsize (_cmdlistsize())

static void display_help(const char *self)
{
	for(int i=0; i < cmdlistsize ; i++){
		std::cout << self  << "\t" <<  cmdlist[i].cmd << "\t" << cmdlist[i].help << std::endl;
	}
}

int main(int argc, const char *argv[])
{
    if(argc<=1){ // no argument
		display_help(argv[0]);
		return EXIT_SUCCESS;
	}else{
		for(int i=0; i < cmdlistsize ; i++){
			if(cmdlist[i].cmd == argv[1])
				return cmdlist[i].main(argc-1, argv+1);
		}
	}

	std::cerr << "command `" << argv[1] << "\' not found" << std::endl;

    return EXIT_FAILURE;
}

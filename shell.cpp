#include "pch.hpp"
#include "hm.hpp"

int main_shell(int argc , const char * argv[])
{
	/*
	 * hm shell or hm !, run command like shell, helper function for excuting external commands, private use only
	 */

	if(argc ==1)
		return EXIT_FAILURE;

	return os_exec(fs::path(argv[1]),argc - 1 , argv + 1 );
}

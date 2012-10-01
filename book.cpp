#include "pch.hpp"
#include "hm.hpp"
#include "templates.hpp"


int main_book(int argc , const char * argv[])
{
	/*
	 * hm book roomid by name (from x to x)|(at x)  [ with checkin checkinname]
	 */
	std::vector<std::string> roomid;
	int argc_start,argc_current;

	argc_current = argc_start = opt_check_for("book",argc,argv);
	argc_current++;

	if(argc_current<argc){// 就好
		expand_roomids_withverify(roomid,argv[argc_current]);
		if(roomid.empty())
		{
			std::cerr << "room: " << argv[argc_current] << " not found!";
			return EXIT_FAILURE;
		}
	}



}

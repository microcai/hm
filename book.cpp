#include "pch.hpp"
#include "hm.hpp"

int main_book(int argc , const char * argv[])
{
	/*
	 * hm book roomid by name (from x to x)|(at x)  [ with checkin checkinname]
	 */
	std::vector<std::string> roomid;
	int argc_start,argc_current;
	std::string UUID;
	boost::gregorian::date bookdate;

	argc_current = argc_start = opt_check_for("book",argc,argv);
	argc_current++;

	if(argc_current<argc){// 就好
		expand_roomids_withverify(roomid,argv[argc_current]);
		if(roomid.empty())
		{
			std::cerr << "room: " << argv[argc_current] << " not found!";
			return EXIT_FAILURE;
		}
		argc_current++;
	}

	if(argc_current<argc){// 就好
		if(std::string("by") != argv[argc_current]){
			std::cerr << "syntax error, see hm help book" << std::endl;
			return EXIT_FAILURE;
		}
		argc_current++;
	}else{
		std::cerr << "syntax error, see hm help book" << std::endl;
		return EXIT_FAILURE;
	}

	if(argc_current<argc){// 就好
		// 这个应该是用户引用了，直接调用 hm client XXX，会返回一个UUID哦！

		hmrunner uuid(main_client);

		uuid.main("client","--first",argv[argc_current],NULL);

		char line[100];

		UUID = std::fgets(line,sizeof(line),uuid);

		uuid.wait();

		std::cerr << "the client UUID is " << UUID << std::endl;

		UUID = UUID.substr(0,36);

		argc_current ++;

	}else{
		std::cerr << "syntax error, see hm help book" << std::endl;
		return EXIT_FAILURE;
	}

	if(argc_current < argc){ // 好, 获得一个 range
		// expect at|from
		if(std::string("at")==argv[argc_current]){
			argc_current ++;

			if(argc_current < argc){
				bookdate = boost::gregorian::from_string(argv[argc_current]);

				argc_current ++;

			}else{
				std::cerr << "syntax error, see hm help book" << std::endl;
				return EXIT_FAILURE;
			}
		}else{
			std::cerr << "syntax error, see hm help book" << std::endl;
			return EXIT_FAILURE;
		}
	}else{
		std::cerr << "syntax error, see hm help book" << std::endl;
		return EXIT_FAILURE;
	}

	std::cout << "client : " << UUID << std::endl;
	std::cout << "room : " << roomid[0].c_str() << std::endl;

	std::cout << "date : " << bookdate << std::endl;
//


	// all info got, write out new plans. first check if avaliavle

	return  EXIT_FAILURE;
}

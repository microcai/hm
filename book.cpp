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

	fs::path hmdir = hm_getdbdir();

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

	// all info got, write out new plans. first check if avaliavle

	for(int i =0 ; i < roomid.size() ; i++){
		fs::path dbfile  = hmdir / "rooms"/ roomid[i] / "schedule" / boost::gregorian::to_sql_string(bookdate);

		if(fs::exists(dbfile)){
			// 错误！ 这是严重的错误！
			std::cerr << "room" << roomid[i] << "已经有预约";
			return EXIT_FAILURE;
		}
	}

	// write db now

	for(int i =0 ; i < roomid.size() ; i++){
		fs::path dbfilename  = hmdir / "rooms"/ roomid[i] / "schedule" / boost::gregorian::to_sql_string(bookdate);
	/*
		#入住客户
		clients=UUID with ， seperated
		#预订者
		booker=UUID
		#(预计)到达时间，时间格式为标准ctime输出格式。本地时间。
		arrival=
		#(预计)离开时间，时间格式为标准ctime输出格式。本地时间。
		leave=
		#操作员
		operator=
		#特殊服务。比如预订晚餐。special由hm的第三方命令提供具体解析。
		special=
	*/

		std::cout << "client : " << UUID << std::endl;
		std::cout << "room : " << roomid[0].c_str() << std::endl;

		std::cout << "date : " << bookdate << std::endl;
		std::cout << "file : "  << dbfilename << std::endl;

		std::fstream dbfile(dbfilename.c_str(),std::ios::out);

		dbfile << "booker=" << UUID << std::endl;
		dbfile << "clients=" << std::endl;
	}


	return  EXIT_FAILURE;
}

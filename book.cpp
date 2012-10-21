#include "pch.hpp"
#include "hm.hpp"

int main_book(int argc , const char * argv[])
{
	/*
	 * hm book roomid by name (from x to x)|(at x)  [ with checkin checkinname]
	 */
	std::string UUID;
	int argc_start,argc_current;
	std::vector<std::string> roomid;
	boost::gregorian::date bookdate1,bookdate2;
	fs::path hmdir = hm_getdbdir();

	argc_current = argc_start = opt_check_for("book",argc,argv);
	argc_current++;

	if(argc_current<argc){// 就好
		roomid = expand_roomids_withverify<std::vector<std::string>>(argv[argc_current]);
		if(roomid.empty())
		{
			std::cerr << "room: " << argv[argc_current] << " not found!" << std::endl;
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
		uuid >> UUID;
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
				bookdate1 = bookdate2 = boost::gregorian::from_string(argv[argc_current]);
				argc_current ++;
			}else{
				std::cerr << "syntax error, see hm help book" << std::endl;
				return EXIT_FAILURE;
			}
		}else if(std::string("from")==argv[argc_current] &&
				std::string("to")==argv[argc_current+2] &&
				check_arg_type(argv[argc_current+1]) == arg_type_date_sql &&
				check_arg_type(argv[argc_current+3]) == arg_type_date_sql )
		{
			argc_current +=4;
			bookdate1 = boost::gregorian::from_string(argv[argc_current]);

			bookdate2 = boost::gregorian::from_string(argv[argc_current]);
			
		}else{
			std::cerr << "syntax error, see hm help book" << std::endl;
			return EXIT_FAILURE;
		}
	}else{
		std::cerr << "syntax error, see hm help book" << std::endl;
		return EXIT_FAILURE;
	}

	// all info got, write out new plans. first check if avaliavle
	for(auto& thisroom: roomid){
		boost::gregorian::date_duration	dr(1);
		for(boost::gregorian::date bookdate=bookdate1; bookdate <= bookdate2 ; bookdate+=dr){
			fs::path dbfilename  = hmdir / "rooms"/ thisroom / "schedule" / boost::gregorian::to_sql_string(bookdate);

			if(fs::exists(dbfilename)){
				// 错误！ 这是严重的错误！
				std::cerr << "room " << thisroom << " 已经有预约";
				std::cout << "room " << thisroom << " 已经有预约";
				return EXIT_FAILURE;
			}
		}
	}

	// write db now
	for(auto& thisroom: roomid){
		boost::gregorian::date_duration	dr(1);
		for(boost::gregorian::date bookdate=bookdate1; bookdate <= bookdate2 ; bookdate+=dr){
			fs::path dbfilename  = hmdir / "rooms"/ thisroom / "schedule" / boost::gregorian::to_sql_string(bookdate);
			
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
			std::cout << "room : " << thisroom << std::endl;

			std::cout << "date : " << bookdate << std::endl;
			std::cout << "file : "  << dbfilename << std::endl;

			std::fstream dbfile(dbfilename.c_str(),std::ios::out);

			dbfile << "booker=" << UUID << std::endl;
			dbfile << "clients=" << std::endl;
		}
	}
	return  EXIT_SUCCESS;
}

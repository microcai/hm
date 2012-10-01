#include "pch.hpp"
#include "hm.hpp"

int display_status(boost::gregorian::date day=boost::gregorian::day_clock::local_day())
{
	// 获得 room 列表。
	std::list<fs::path> rooms;

	fs::path roomdir =  hm_getdbdir() / "rooms";

	for(fs::directory_iterator diritend,dirit(roomdir);dirit!=diritend ; dirit++){
		rooms.push_back(dirit->path());
//		std::cout << dirit->path().filename().string() << std::endl;
	}



	//boost::sort();
	//std::sort();

	// 遍历所有的房间，显示状态
	std::string theday = boost::gregorian::to_sql_string(day);
	std::cout << "checking date : " << day << std::endl;

	// 检查指定日期
	for(std::list<fs::path>::iterator it = rooms.begin(); it != rooms.end(); it++	){

		fs::path planfileS = (*it / "schedule" / theday);
		fs::path planfileH = (*it / "history" / theday);

		if(fs::exists(planfileS)||fs::exists(planfileH)){

		}else{
			// 没该文件，说明客房有空哦
			std::cout << "room " << it->filename() << " is available" << std::endl;
		};
	}

	return EXIT_SUCCESS;
}


int main_status(int argc , const char * argv[])
{
	/*
	 * hm status - list room status, sorted
	 */
	int argc_start = opt_check_for("status",argc,argv);

	if((argc - argc_start) ==1){
		return display_status();
	}else{
		boost::gregorian::date day;
		try{
			day = boost::gregorian::from_string(argv[argc_start+1]);
		}catch(...){ // 不是标准日期啊

			day = boost::gregorian::day_clock::local_day();
			boost::gregorian::date_duration dr(atoi(argv[argc_start+1]));
			day += dr;
		}
		return display_status(day);
	}
}

#include "pch.hpp"
#include "hm.hpp"

static bool roomsort(const fs::path & A , const fs::path & B )
{
	return A.filename().string() < B.filename().string();
}

/**
 * output json format instead of human readable output
 */
static bool json_output=false;
/**
 * 	{
		"roomid": "1101",
		"free":"0",
		"booker":{ "name":"kiki" },
		"special":"$special"
	},
 */
static int display_status(boost::gregorian::date day=boost::gregorian::day_clock::local_day())
{
	// 获得 room 列表。
	std::vector<fs::path> rooms;

	fs::path roomdir =  hm_getdbdir() / "rooms";

	walkdir(roomdir,[&rooms](const fs::path & thisdir){
		rooms.push_back(thisdir);
	});

	std::sort(rooms.begin(),rooms.end(),roomsort);

	// 遍历所有的房间，显示状态
	std::string theday = boost::gregorian::to_sql_string(day);

	if(!json_output)
		std::cout << "checking date : " << day << std::endl;

	// 检查指定日期
	for(auto it = rooms.begin(); it != rooms.end(); it++	){
		if(json_output){
			std::cout << "\t{\n" ;
			std::cout << "\t\t\"roomid\" : " << it->filename() <<  " ,\n";
		}

		fs::path planfileS = (*it / "schedule" / theday);
		fs::path planfileH = (*it / "history" / theday);

		if(fs::exists(planfileS)||fs::exists(planfileH)){if(json_output){

			const fs::path & planfile = fs::exists(planfileS)?planfileS:planfileH;

			std::cout << "\t\t\"free\" : false, \n" ;

			// 输出客户信息 ：）
			keyvalfile roominfo(planfile);

			std::string uuid = roominfo["booker"] ;

			std::cout << "\t\t\"booker\" : ";

			hm_main_caller(main_client,"client","--json", uuid.c_str(),NULL);

			std::cout << "\n";

			std::cout << "\t\t\"special\" : " << quote(roominfo["special"]) << ", \n ";

			std::cout << "\t},\n";

		}}else{
			// 没该文件，说明客房有空哦
			if(json_output){
				std::cout << "\t\t\"free\" : true, \n" ;
				std::cout << "\t\t\"booker\" : { \"uuid\" : \"\" ,  \"name\" : \"\" , } , \n" ;
				std::cout << "\t\t\"special\" : \"\", \n ";
				std::cout << "\t},\n";
			}else
				std::cout << "room " << it->filename() << " is available" << std::endl;
		};
	}

	return EXIT_SUCCESS;
}

static int display_status(boost::gregorian::date_period period)
{
	return display_status(period.begin());
	// a period period, this is called by web page via AJAX, so must support json output
	if(json_output);

		//std::cout << "//";



}

int main_status(int argc , const char * argv[])
{
	/*
	 * hm status - list room status, sorted
	 */
	int argc_start = opt_check_for("status",argc,argv);

	json_output = opt_check_for("--json",argc,argv)>0;
	if(json_output)
		opt_remove(argc,argv,"--json");

	if((argc - argc_start) ==1)
		return display_status();

	switch(check_arg_type(argv[argc_start+1])){
		case arg_type_date:
		case arg_type_date_sql:
			return display_status(boost::gregorian::from_string(argv[argc_start+1]));
		case arg_type_date_offset:
			return display_status(
				boost::gregorian::day_clock::local_day() + boost::gregorian::date_duration(atoi(argv[argc_start+1]))
			);
		case arg_type_date_period:
			/**
			 * construct date_period if the date is not the same
			 */

			// so first, splite it
			std::vector<std::string> dates;
			boost::algorithm::split(dates,argv[argc_start+1],boost::is_any_of(","));

			if(dates[0] == dates[1])
				return  display_status(boost::gregorian::from_string(dates[0]));
			else{
				return display_status(
					boost::gregorian::date_period(
						boost::gregorian::from_string(dates[0]),
						boost::gregorian::from_string(dates[1])
					)
				);
			}
	}
}

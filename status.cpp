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

static std::vector<fs::path> hm_rooms()
{
	// 获得 room 列表。
	std::vector<fs::path> rooms;
	walkdir(hm_getdbdir() / "rooms",[&rooms](const fs::path & thisdir){
		rooms.push_back(thisdir);
	});
	std::sort(rooms.begin(),rooms.end(),[](const fs::path & A , const fs::path & B){return A.filename().string() < B.filename().string();});
	return rooms;
}

static bool has_planfile(const boost::filesystem::path & room , const std::string & theday)
{
	return fs::exists(room / "schedule" / theday) || fs::exists(room / "history" / theday);
}

static fs::path get_planfile(const boost::filesystem::path & room , const std::string & theday)
{
	if(fs::exists(room / "schedule" / theday))
		return room / "schedule" / theday;
	if(fs::exists(room / "history" / theday))
		return room / "history" / theday;	
}

/**
	{
		"roomid": "1101",
		"free":"0",
		"multi":"0",
		"booker":{ "name":"kiki" },
		"special":"$special"
	},
*/
static int display_status(boost::gregorian::date day1,boost::gregorian::date day2)
{
	// 获得 room 列表。
	std::vector<fs::path> rooms = hm_rooms();

	boostpt::ptree	status_rooms_array;
	
	//return display_status(day1);
	// a period period, this is called by web page via AJAX, so must support json output
	if(json_output);

	for(const fs::path & room : rooms){
		boost::gregorian::date_duration	dr(1);
		std::vector<fs::path> planfiles;
		int duration = 0;
		
		for(boost::gregorian::date day=day1; day <= day2 ; day+=dr ){			
			std::string theday = boost::gregorian::to_sql_string(day);
			if(has_planfile(room,theday)){
				planfiles.push_back(get_planfile(room,theday));
			}
			duration ++;
		}

		boostpt::ptree roomstatus;
		roomstatus.put("roomid",room.filename().generic_string());
	
		if(planfiles.empty()){//空闲的房间
			roomstatus.put("free",true);
			roomstatus.put("multi",false);
		}else{//非空闲的，看是不是同一个人住的
			roomstatus.put("free",false);
			//special
			std::map<std::string,std::string> booker;
			for(const fs::path & planfile : planfiles){
				keyvalfile roominfo(planfile);
				booker.insert(std::make_pair(roominfo["booker"],roominfo["special"]));
			}
			if(booker.size() > 1){
				roomstatus.put("multi",true);
			}else{
				roomstatus.put("multi",false);
				roomstatus.put("special",booker.begin()->second);
				//about booker
				std::string uuid = booker.begin()->first;
				hmrunner	clientbooker(main_client);
				clientbooker.main("client","--json",uuid.c_str(),nullptr);
				std::stringstream bookerjsonstream(clientbooker);
				boostpt::ptree bookerjsontree;
				boostjs::read_json(bookerjsonstream,bookerjsontree);
				roomstatus.put_child("booker",bookerjsontree);
			}
		}
		status_rooms_array.push_back(std::make_pair("",roomstatus));
	}
	
	boostpt::ptree status_rooms;
	status_rooms.put_child("rooms",status_rooms_array);
	if(json_output)
		boostjs::write_json(std::cout,status_rooms);
	else{
		// TODO iterate through the ptree and output human readable text
	}
	return 0;
}

static int display_status(boost::gregorian::date day=boost::gregorian::day_clock::local_day())
{
	// 获得 room 列表。
	std::vector<fs::path> rooms = hm_rooms();

	// 遍历所有的房间，显示状态
	std::string theday = boost::gregorian::to_sql_string(day);

	if(!json_output)
		std::cout << "checking date : " << day << std::endl;
	else
		std::cout << "{\"rooms\":[\n";

	// 检查指定日期
	bool isfirst=true;
	for(const fs::path & room : rooms) {
		if(json_output){
			if(!isfirst)
				std::cout << ",\n";
			isfirst = false;
			std::cout << "\t{\n";
			std::cout << "\t\t\"roomid\" : " << room.filename() <<  " ,\n";
		}

		if(has_planfile(room,theday)){if(json_output){

			const fs::path planfile = get_planfile(room,theday);

			std::cout << "\t\t\"free\" : \"false\", \n" ;
			std::cout << "\t\t\"multi\" : \"false\", \n" ;

			// 输出客户信息 ：）
			keyvalfile roominfo(planfile);

			std::string uuid = roominfo["booker"] ;

			std::cout << "\t\t\"booker\" : ";

			hm_main_caller(main_client,"client","--json", uuid.c_str(),NULL);

			std::cout << ",\n";

			std::cout << "\t\t\"special\" : " << quote(roominfo["special"]) << "\n ";

			std::cout << "\t}";

		}}else{
			// 没该文件，说明客房有空哦
			if(json_output){
				std::cout << "\t\t\"multi\" : \"false\", \n" ;
 				std::cout << "\t\t\"free\" : \"true\" \n" ;
				std::cout << "\t}";
			}else
				std::cout << "room " << room.filename() << " is available" << std::endl;
		};
	}
	if(json_output)
		std::cout << "\n]}\n";
	return EXIT_SUCCESS;
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
						boost::gregorian::from_string(dates[0]),
						boost::gregorian::from_string(dates[1])					
				);
			}
	}
}

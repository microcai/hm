#include "pch.hpp"
#include "hm.hpp"

enum arg_type{
	/**
	 * X is [0-9]
	 * Z is [0-9a-zA-Z]
	 */
	arg_type_date_offset, /** X */
	arg_type_roomid, /** XXXX */
	arg_type_date,  /** XXXXXXXX */
	arg_type_date_sql,/** XXXX-XX-XX */
	arg_type_date_period, /** XX-XX-XX,XX-XX-XX */ // passed by javascript
	arg_type_telephone, /** XXXXXXXXXXX or XXXXXXXX */
	arg_type_uuid, /** ZZZZZZZZ-ZZZZ-ZZZZ-ZZZZ-ZZZZZZZZZZZZ */
};

/** 猜测参数类型 **/
static arg_type check_arg_type(const std::string argstr)
{
	std::map<enum arg_type,boost::regex> checker;

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_roomid,
			boost::regex("[1-9][0-9][0-9][0-9]")
		)
	);

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_date,
			boost::regex("[1-2](0|9)[0-9][0-9][1-9][0-9][0-9][0-9]")
		)
	);

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_date_sql,
			boost::regex("[1-2](0|9)[0-9][0-9]-[1-9][0-9]-[0-9][0-9]")
		)
	);

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_date_period,
			boost::regex("[1-2](0|9)[0-9][0-9]-[1-9][0-9]-[0-9][0-9],[1-2](0|9)[0-9][0-9]-[1-9][0-9]-[0-9][0-9]")
		)
	);

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_uuid,
			boost::regex("[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
			"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
			"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
			"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]-"
			"[0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z][0-9a-zA-Z]")
		)
	);

	checker.insert(
		std::pair<enum arg_type,boost::regex>(
			arg_type_date_offset,
			boost::regex("([0-9]|-[0-9])")
		)
	);


	for (auto & regexer : checker) {
		if(boost::regex_match(argstr,regexer.second))
			return regexer.first;
	}

}

static bool roomsort(const fs::path & A , const fs::path & B )
{
	return A.filename().string() < B.filename().string();
}

/**
 * output json format instead of human readable output
 */
static bool json_output=false;

static int display_status(boost::gregorian::date day=boost::gregorian::day_clock::local_day())
{
	// 获得 room 列表。
	std::vector<fs::path> rooms;

	fs::path roomdir =  hm_getdbdir() / "rooms";

	for(fs::directory_iterator diritend,dirit(roomdir);dirit!=diritend ; dirit++){
		rooms.push_back(dirit->path());
	}

	std::sort(rooms.begin(),rooms.end(),roomsort);

	// 遍历所有的房间，显示状态
	std::string theday = boost::gregorian::to_sql_string(day);

	if(json_output){
		std::cout << "[" << std::endl;
	}else
		std::cout << "checking date : " << day << std::endl;

	// 检查指定日期
	for(auto it = rooms.begin(); it != rooms.end(); it++	){

		fs::path planfileS = (*it / "schedule" / theday);
		fs::path planfileH = (*it / "history" / theday);

		if(fs::exists(planfileS)||fs::exists(planfileH)){

		}else{
			// 没该文件，说明客房有空哦
			std::cout << "room " << it->filename() << " is available" << std::endl;
		};
	}

	if(json_output){
		std::cout << "[" << std::endl;
	}else
		std::cout << "checking date : " << day << std::endl;


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

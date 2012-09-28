#include "pch.hpp"
#include "hm.hpp"
#include "templates.hpp"

static std::string skel_room=
"# alias 是房间的别名。可以给房间起个性化的名字。注意，skel的话这里要为空。\n\
alias=\n\
\n\
#equipment 是房间装备，可以写 aircond, bath, hotwater\n\
#skel有这里的装备会变成新创建的房间的默认装备。　\n\
equipment=\n\
\n\
#rent 是日租金，skel这里的设置会成为房间的默认设置。可以使用$和￥符号，hm会自动转化。\n\
rent=\n\
\n\
#床位数\n\
beds=\
";


static std::string config="\
#栋数\n\
building=\
\n\
#栋数\n\
building=\
";

/*
 * this function was called when user type hm init
 */
int main_init(int argc ,const char * argv[])
{
	/*
		run hm init --skel to initialize the skel directory.
		then the user can tweek the skel directory.
		then run hm init number of rooms or hm init list of rooms to init the room db.
	*/
	bool opt_force = !(opt_check_for("-f",argc,argv)==-1);

	fs::path hmdir = hm_getdbdir();

	fs::path skeldir = hmdir / "skel";
	fs::path skel_room_path  = skeldir / "skel_room.txt";

	try{

	if(opt_check_for("--skel",argc,argv)>=0)
	{ // got --skel

		if(!fs::exists(skeldir))
			fs::create_directory(skeldir);
		else if(!opt_force){
			std::cout << "skel exists, add -f to override" << std::endl;
			return EXIT_FAILURE;
		}
		// 创建　结构
		// extract form resourse.

		std::cout << "creating " << skel_room_path << std::endl;

		std::fstream skel_room_file(skel_room_path.c_str(),std::ios::out);
		skel_room_file << skel_room;

		std::cout << "请修改文件:" << skel_room_path << "以配置酒店的默认房间格局" << std::endl;
		std::cout << "修改完成后执行　hm init ROOMID 来创建房间。" << std::endl;
		std::cout << "可以多次执行创建多个房间。" << "ROOMID　可以使用批量匹配符。" << std::endl;

		return EXIT_SUCCESS;
	};

	fs::create_directory(hmdir / "rooms");
	fs::create_directory(hmdir / "clients");
	}catch(fs::filesystem_error & e){
		//std::cerr << e.path1() << std::endl;
		std::cerr << "无法创建" << e.path1() << "，请确认对" << hmdir << "有足够的读写权限" << std::endl;
	}

	// have to say that, hm init roomID.
	int argc_start = opt_check_for("init",argc,argv);

	std::list<std::string> rooms;
	//FIXME: 可能需要过滤掉一些选项
	for(int i = argc_start + 1; i < argc ; i++)
	{
		if(argv[i][0]=='-')
			continue;
		expand_roomids(rooms,argv[i]);
	}

	try{
	// 为每个 room 编号创建数据库
	for(std::list<std::string>::iterator p=rooms.begin(); p != rooms.end();p++)
	{
		fs::path roomdir = hmdir / "rooms" / * p ;
		fs::create_directory(roomdir);

		fs::create_directory(roomdir / "pictures");

		fs::create_directory(roomdir / "history");

		// schedule 目录，过时的文件会被自动移动到　history
		fs::create_directory(roomdir / "schedule");

		// info 文件包含了房间的信息
		fs::copy_file(skel_room_path,roomdir / "info",fs::copy_option::fail_if_exists);

	}
	}catch(fs::filesystem_error & e){

	}

}

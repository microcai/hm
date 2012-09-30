
#include "pch.hpp"
#include "hm.hpp"

void expand_roomids(std::list<std::string> &result,const std::string roomid)
{
	std::list<std::string> tokens;

	// 从　roomid 描述中构建真正的房间列表。不检查合法性（房间是否存在）

	// 1_109 1_109-108 1_102-1_209，逗号隔开。
	boost::split(tokens,roomid,boost::is_any_of(","));

	// 遍历 list 后为　- 操作生成
	for (std::list<std::string>::iterator  p = tokens.begin(); p != tokens.end(); p++)
	{
		//查找　-，无 - 就不用展开
		if(p->find('-')==std::string::npos)
		{
			result.push_back(*p);
		}else{
			//有　- 就展开
			//TODO
		}
	}
}

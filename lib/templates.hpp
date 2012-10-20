#pragma once

template< typename SequenceSequenceT>
inline SequenceSequenceT expand_roomids(const std::string roomid)
{
	SequenceSequenceT result;
	std::list<std::string> tokens;

	// 从　roomid 描述中构建真正的房间列表。不检查合法性（房间是否存在）

	// 1_109 1_109-108 1_102-1_209，逗号隔开。
	boost::split(tokens,roomid,boost::is_any_of(","));

	// 遍历 list 后为　- 操作生成
	for (const auto & p : tokens )
	{
		//查找　-，无 - 就不用展开
		if(p.find('-')==std::string::npos)
		{
			result.push_back(p);
		}else{
			//有　- 就展开
			//TODO
		}
	}
	return result;
}


template< typename SequenceSequenceT>
inline  SequenceSequenceT expand_roomids_withverify(const std::string roomid)
{
	SequenceSequenceT result;
	std::list<std::string> res = expand_roomids<std::list<std::string>>(roomid);

	// exame the result
	for (const auto & p : res )
	{
		//查找　-，无 - 就不用展开
		if( hm_hasroom(p) )
		{
			result.push_back(p);
		}
	}
	return result;
}

template<typename... Args>
inline int main_client(const char * arg1,Args... args){
	return hm_main_caller(main_client,arg1,args...);
};

template<typename strT>
inline strT quote(const strT & str){
	return "\"" + str + "\"";
}

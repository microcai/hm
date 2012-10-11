 #pragma once

template< typename SequenceSequenceT>
inline void expand_roomids(SequenceSequenceT &result,const std::string roomid)
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


template< typename SequenceSequenceT>
inline  void expand_roomids_withverify(SequenceSequenceT &result,const std::string roomid)
{
	std::list<std::string> res;

	expand_roomids(res,roomid);

	// exame the result
	for (std::list<std::string>::iterator  p = res.begin(); p != res.end(); p++)
	{
		//查找　-，无 - 就不用展开
		if( hm_hasroom(*p) )
		{
			result.push_back(*p);
		}
	}
	//hm_hasroom(res);
}

template<typename... Args>
inline int main_client(const char * arg1,Args... args){
	return hm_main_caller(main_client,arg1,args...);
};

template<typename strT>
inline strT quote(const strT & str){
	return "\"" + str + "\"";
}

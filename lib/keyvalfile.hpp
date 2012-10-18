#pragma once
#include <boost/filesystem.hpp>
/**
 */
class keyvalfile{
public:

	keyvalfile(const boost::filesystem::path & keyvalfile):
		file(keyvalfile)
	{
		// reading all contents into memory, one line pre iterator
		std::fstream contentfile(file.string());

		std::string line;

		while(!contentfile.eof()){
			line.clear();
			contentfile >> line;
			contents.push_back(line);
		}
	}

	std::string operator[](const std::string & key){
		for(auto & line : contents){
			if(match_key(line,key)){
				return line.substr(key.length()+1);
			}
		}
		return "";
	}

private:
	const boost::filesystem::path file;
	std::vector<std::string> contents;
};

#include "pch.hpp"
#include "hm.hpp"

int main_gc(int argc , const char * argv[])
{
	/*
	 * hm gc -- called automantically every day.
	 *
	 * gc will move out-dated bookinfo  from schedule to history
	 */

	fs::path hmdir = hm_getdbdir();
	// :) if called by hm , hm will append --quiet
	bool quiet = opt_check_for("--quiet",argc,argv)!=-1;
	// cd into the rooms dir
	chdir( (hmdir / "rooms").c_str() );
	boost::gregorian::date today = boost::gregorian::day_clock::local_day();
	/*
	 * walk through all rooms, one process per-rooms
	 *
	 * 绝佳的并行处理哦！
	 */
	std::map<pid_t,fs::path> worker;

	//上帝啊，lambda太棒了！
	walkdir(fs::current_path(), [&worker,&quiet,&today]( const fs::path & dirit){
		pid_t pid  = fork();

		if(pid==0){

			// as child , we can chdir into subdir and leave parent un-affected
			if(chdir( (dirit / "schedule").c_str())<0)
				exit(EXIT_FAILURE);

			walkdir(fs::current_path(),[&dirit,&quiet,&today](const fs::path & entry){
				auto entrydate = boost::gregorian::from_string(entry.filename().string());
				if(entrydate < today){
					if(!quiet)
						std::cout << "处理 " << dirit.filename() <<  " 日期 " << entrydate << std::endl;
					fs::rename(entry, dirit/ "history" / entry.filename() );
					sync();
				}
			});

			exit(EXIT_SUCCESS);
		}else if(pid >0){
			worker.insert(std::make_pair(pid,dirit));
		}
	});

	int exit_status;

	while(!worker.empty()){ // 只要有一个 child 失败，整体就失败
		int status;
		pid_t exited_child =  wait(&status);
		worker.erase(exited_child);
		if(status != EXIT_SUCCESS)
			exit_status =  EXIT_FAILURE;
	}

	return exit_status;
}

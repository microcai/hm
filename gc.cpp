#include "pch.hpp"
#include "hm.hpp"

int main_gc(int argc , const char * argv[])
{
	/*
	 * hm gc -- called automantically every day.
	 *
	 * gc will move out-dated bookinfo  from schedule to history
	 *
	 *
	 *
	 */

	fs::path hmdir = hm_getdbdir();

	// :) if called by hm , hm will append --quiet
	bool quiet = opt_check_for("--quiet",argc,argv)!=-1;

	// cd into the rooms dir
	chdir( (hmdir / "rooms").c_str() );

	boost::gregorian::date today = boost::gregorian::day_clock::local_day();

	std::string todaystr = boost::gregorian::to_sql_string(today);

	/*
	 * walk through all rooms, one process per-rooms
	 *
	 * 绝佳的并行处理哦！
	 */
	std::map<pid_t,fs::path> worker;

	fs::directory_iterator diritend;

	for(fs::directory_iterator dirit(fs::current_path());dirit!=diritend ; dirit++){
		pid_t pid  = fork();

		if(pid==0){

			// as child , we can chdir into subdir and leave parent un-affected
			if(chdir( (dirit->path() / "schedule").c_str())<0)
				exit(EXIT_FAILURE);

				for(fs::directory_iterator schedule_entry(fs::current_path());schedule_entry!=diritend ; schedule_entry++){
					fs::path entry = schedule_entry->path();

					auto entrydate = boost::gregorian::from_string(entry.filename().string());

					if(entrydate < today){
						if(!quiet)
							std::cout << "处理 " << dirit->path().filename() <<  " 日期 " << entrydate << std::endl;

						fs::rename(entry, dirit->path() / "history" / schedule_entry->path().filename() );

						sync();
					}
				}

			exit(EXIT_SUCCESS);

		}else if(pid >0){
			worker.insert(std::pair<pid_t,fs::path>(pid,dirit->path()));
		}
	}

	int status,exit_status;

	while(worker.size()>0){
		pid_t exited_child =  wait(&status);

		worker.erase(exited_child);

		if(status != EXIT_SUCCESS)
			exit_status =  EXIT_FAILURE;
	}

	return exit_status;
}

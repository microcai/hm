#include "opt.hpp"

int opt_check_for(const std::string opt,int argc , const char * argv[])
{
	for(int i =0;i < argc ; i++){
		if(opt == argv[i])
		{
			return i;
		}
	}
	return -1;
}

void opt_remove(int &argc,const char * argv[], int index)
{
	for(int i = index ; i < (argc-1) ; i++)
		argv[i] = argv[i+1];

	argv[argc-1] = NULL;
	argc --;
}

void opt_remove(int & argc,const char * argv[], const std::string opt)
{
	int index = opt_check_for(opt,argc,argv);
	if(index >=0)
		opt_remove(argc,argv,index);
}
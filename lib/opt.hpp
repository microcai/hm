#pragma once
#include <string>
/*
 * return the index of argv that have opt. -1 for not found.
 */
int opt_check_for(const std::string opt,int argc , const char * argv[]);
void opt_remove(int & argc,const char * argv[], int index);
void opt_remove(int & argc,const char * argv[], const std::string opt);
#include "pch.hpp"
#include "hm.hpp"

static sighandler gsighandler[_NSIG];

static void _sighandler(int signal_number)
{
	gsighandler[signal_number](signal_number);
}

int hm_sigmask(int how,int signal_number)
{
	sigset_t sigset={0};
	sigaddset(&sigset,signal_number);
	return sigprocmask(how,&sigset,NULL);
}

sighandler hm_signal(int signal_number,const sighandler handler)
{
	auto oldhandler = gsighandler[signal_number];
	gsighandler[signal_number]=handler;
	::signal(signal_number,_sighandler);
	return oldhandler;
}	
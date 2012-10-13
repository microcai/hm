#pragma once

typedef boost::function<void(int signal_number)> sighandler;

sighandler hm_signal(int signal_number, const sighandler handler);

int hm_sigmask(int how,int signal_number);


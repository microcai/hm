#pragma once

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <signal.h>

#include <string>
#include <vector>
#include <list>
#include <algorithm>

#include <cstdlib>
#include <iostream>
#include <exception>
#include <fstream>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
namespace asio = boost::asio;

#include <boost/bind.hpp>
#include <boost/concept_check.hpp>
#include <boost/date_time.hpp>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/function.hpp>
#include <boost/range/algorithm.hpp>

#include <boost/regex.hpp>

#define HM_VERSION	"0.1"


#ifndef __HM_PCH__
#define __HM_PCH__

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <sys/prctl.h>

#include <unistd.h>
#include <fcntl.h>

#include <cstdlib>
#include <cstdio>
#include <cstdarg>
#include <cstddef>
#include <csignal>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <algorithm>


#include <iostream>
#include <exception>
#include <fstream>
#include <functional>

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>
namespace asio = boost::asio;

#include <boost/assert.hpp>
#include <boost/date_time.hpp>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/property_tree/ptree.hpp>
namespace boostpt = boost::property_tree;
#include <boost/property_tree/json_parser.hpp>
namespace boostjs = boost::property_tree::json_parser;
#include <boost/range/algorithm.hpp>

#include <boost/regex.hpp>
#include <boost/smart_ptr.hpp>
#include <boost/tuple/tuple.hpp>

#define HM_VERSION	"0.1"

#endif
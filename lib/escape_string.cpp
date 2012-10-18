#include <string>
#include <cstring>
#include "escape_string.h"
// http://www.ietf.org/rfc/rfc2396.txt
// section 2.3
static const char unreserved_chars[] =
		// when determining if a url needs encoding
		// % should be ok
		"%+"
		// reserved
		";?:@=&,$/"
		// unreserved (special characters) ' excluded,
		// since some buggy trackers fail with those
		"-_!.~*()"
		// unreserved (alphanumerics)
		"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz"
		"0123456789";

static const char hex_chars[] = "0123456789abcdef";

std::string escape_string(std::string str)
{
	std::string ret;
	for (auto c : str)
	{
		if (std::strchr(unreserved_chars, c) && c != 0)
		{
				ret += c;
		}
		else
		{
				ret += '%';
				ret += hex_chars[((unsigned char)c) >> 4];
				ret += hex_chars[((unsigned char)c) & 15];
		}
	}
	return ret;
}
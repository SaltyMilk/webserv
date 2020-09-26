#include "webserv.h"

struct tm getDate()
{
	struct timeval tp;
	gettimeofday(&tp, nullptr);
	struct tm date;
	strptime(std::to_string(tp.tv_sec).c_str(), "%s", &date);
	return date;
}

//IMF dates to string
std::string get_imf_fixdate()
{
	struct tm date = getDate();
	char buff[420];
	strftime(buff, 420, "%a, %d %b %Y %H:%M:%S %Z", &date);
	return (std::string(buff));
}

#include "../../includes/webserv.h"

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

std::string get_last_modified(std::string filename)
{
	struct stat st;
	struct tm date;
	stat(filename.c_str(), &st);

	time_t time = st.st_mtim.tv_sec;
	struct timeval tp;
	gettimeofday(&tp, nullptr);	
	if (time > tp.tv_sec)//if last modified is in the future, we change it to the current time
		time = tp.tv_sec;
	strptime(std::to_string(time).c_str(), "%s", &date);
	char buff[420];
	strftime(buff, 420, "%a, %d %b %Y %H:%M:%S %Z", &date);
	return (buff);
}

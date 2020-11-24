#include "../../includes/webserv.h"

t_headers g_headers[18] =
{
		{ACCEPT_CHARSETS, "ACCEPT-CHARSETS", "Accept-Charsets"},
		{ACCEPT_LANGUAGE, "ACCEPT-LANGUAGE", "Accept-Language"},
		{ALLOW, "ALLOW", "Allow"},
		{AUTHORIZATION, "AUTHORIZATION", "Authorization"},
		{CONTENT_LANGUAGE, "CONTENT-LANGUAGE", "Content-Language"},
		{CONTENT_LENGTH, "CONTENT-LENGTH", "Content-Length"},
		{CONTENT_LOCATION, "CONTENT-LOCATION", "Content-Location"},
		{CONTENT_TYPE, "CONTENT-TYPE", "Content-Type"},
		{DATE, "DATE", "Date"},
		{HOST, "HOST", "Host"},
		{LAST_MODIFIED, "LAST-MODIFIED", "Last-Modified"},
		{LOCATION, "LOCATION", "Location"},
		{REFERER, "REFERER", "Referer"},
		{RETRY_AFTER, "RETRY-AFTER", "Retry-After"},
		{SERVER, "SERVER", "Server"},
		{TRANSFER_ENCODING, "TRANSFER-ENCODING", "Transfer-Encoding"},
		{USER_AGENT, "USER-AGENT", "User-Agent"},
		{WWW_AUTHENTICATE, "WWW-AUTHENTICATE", "WWW-Authenticate"}
};

std::string	format_header(int header, std::string value)
{
	return (std::string(g_headers[header].name + ": ") + value);
}

std::string get_header_field(int header)
{
	return (std::string(g_headers[header].value));
}

int 		get_header_id(std::string header_field)
{
	std::string upcase_hf = std::string(ft_strupcase(const_cast<char *>(header_field.c_str())));
	size_t	i = -1;
	while (++i < 18)
		if (upcase_hf == g_headers[i].value)
			return (g_headers[i].id);
	return (-1);
}


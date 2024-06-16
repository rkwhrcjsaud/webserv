#ifndef HTTP_HPP
# define HTTP_HPP
# include <string>
# include <map>

class Http
{
	public:
		Http();
		Http(const Http& obj);
		~Http();

		Http& operator= (Http& rhs);

		std::map<std::string, std::string> headers;
		std::string body;

		virtual bool parse(std::string s) = 0;
};

#endif
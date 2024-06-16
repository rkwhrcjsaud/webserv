#ifndef HTTP_REQUEST_HPP
# define HTTP_REQUEST_HPP
# include "Http.hpp"

enum EHttpMethod
{
	GET,
	POST,
	DELETE
};

class HttpRequest : public Http
{
	public:
		HttpRequest();
		HttpRequest(const HttpRequest& obj);
		~HttpRequest();

		HttpRequest& operator= (HttpRequest& rhs);

		EHttpMethod method;
		std::string target;
		std::string version;

		virtual bool parse(std::string s);
};

#endif
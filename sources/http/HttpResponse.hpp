#ifndef HTTP_RESPONSE_HPP
# define HTTP_RESPONSE_HPP
# include "Http.hpp"

class HttpResponse : public Http
{
	public:
		HttpResponse();
		HttpResponse(const HttpResponse& obj);
		~HttpResponse();

		HttpResponse& operator= (HttpResponse& rhs);

		std::string version;
		unsigned int statusCode;
		std::string message;

		virtual bool parse(std::string s);
};

#endif
#ifndef OPTION_HANDLER_HEADER
#define OPTION_HANDLER_HEADER

/** GLOBAL	HEADERS	**/
#include <functional>
#include <string>

class Option {
public:
    Option( const std::string &name, std::function< void() > handler );

	const std::string &getName() const;

	void execute();

private:
	std::function<void ()> _handler;
	std::string _name;

};

#endif

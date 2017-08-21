#include "Option.h"

/** GLOBAL	HEADERS	**/

/** LOCAL	HEADERS	**/

/** NOTES **/

Option::Option( const std::string &name, std::function< void() > handler )
	: _name(name), _handler(handler)
{
}

const std::string & Option::getName() const {
	return _name;
}

void Option::execute(){
	_handler();
}


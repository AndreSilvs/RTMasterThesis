#include "MenuStack.h"

/** GLOBAL	HEADERS	**/

/** LOCAL	HEADERS	**/

/** NOTES **/

MenuStack::MenuStack()
{
}

void MenuStack::pushMenu( Menu* menu ) {
    menu->selectOption( 0 );
	_menus.push( menu );
}
void MenuStack::popMenu() {
	if( !isEmpty() ) _menus.pop();
}
void MenuStack::popMenu( int number ) {
	while ( !isEmpty() && number > 0 ) {
		_menus.pop();
		number--;
	}
}
void MenuStack::clear() {
    while ( !isEmpty() ) {
        _menus.pop();
    }
}

bool MenuStack::isEmpty() const {
	return _menus.empty();
}
	
int MenuStack::getSize() const {
	return _menus.size();
}
	
Menu *MenuStack::getTop() {
	return _menus.top();
}

const Menu *MenuStack::getTop() const {
    return _menus.top();
}

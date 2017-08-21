#ifndef MENUSTACK_HEADER
#define MENUSTACK_HEADER

/** GLOBAL	HEADERS	**/
#include <stack>
#include <string>

/** LOCAL	HEADERS	**/
#include "Menu.h"

/** NOTES **/

class MenuStack{
public:
	MenuStack();
	
	void pushMenu( Menu* menu );
	void popMenu();
	void popMenu( int numberPops );

    void clear();
	
	bool isEmpty() const;
	
	int getSize() const;
	
	Menu *getTop();
    const Menu *getTop() const;
	
private:
	std::stack< Menu* > _menus;
};

#endif

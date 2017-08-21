#ifndef MENU_H
#define MENU_H

// GLOBAL HEADERS
#include <string>
#include <vector>

// LOCAL HEADERS
#include "Option.h"

class Menu {
public:
	Menu();
	Menu( const std::string &title );
	~Menu();

    void setTitle( const std::string &title );
	const std::string &getTitle() const;

	void addOption( const Option &option );
	void addOption( const Option &option, int index );
	
	const std::string &getSelectedOptionName() const;
	const std::string &getOptionName( int index ) const;
	
	int getSelectedIndex() const;
	int getNumOptions() const;
	
	void selectNextOption();
	void selectPreviousOption();
	void selectOption( int index );
	void selectOptionByName( std::string name );

	void executeSelectedOption();
	void executeOption( int index );

private:
	std::string _title;
	std::vector< Option > _options;

	int _selectedOption;
};

#endif
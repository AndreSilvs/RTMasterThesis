#include "Menu.h"

// GLOBAL HEADERS

// LOCAL HEADERS

Menu::Menu() :
	_selectedOption(0),
	_title("")
{

}
Menu::Menu( const std::string &title ) :
	_selectedOption( 0 ),
	_title( title )
{
}
Menu::~Menu() {
	
}

void Menu::setTitle( const std::string &title ) {
	_title = title;
}
const std::string &Menu::getTitle() const {
	return _title;
}

void Menu::addOption( const Option &option ) {
	_options.push_back( option );
}
void Menu::addOption( const Option &option, int index ) {
	_options.insert( _options.begin() + index, option );
}
	
const std::string & Menu::getSelectedOptionName() const {
	return _options[_selectedOption].getName();
}
const std::string & Menu::getOptionName( int index ) const {
	return _options[index].getName();
}
	
int Menu::getSelectedIndex() const {
	return _selectedOption;
}
int Menu::getNumOptions() const {
	return (int)_options.size();
}
	
void Menu::selectNextOption() {
	_selectedOption = (_selectedOption + 1) % getNumOptions();
}
void Menu::selectPreviousOption() {
    int totalOptions = getNumOptions();
	_selectedOption = (_selectedOption + totalOptions - 1) % totalOptions;
}
void Menu::selectOption( int index ) {
	_selectedOption = index;
}
void Menu::selectOptionByName( std::string name ) {
	for ( int i = 0, iEnd = (int)_options.size(); i < iEnd; ++i ) {
		if ( _options[ i ].getName() == name ) {
			selectOption( i );
			return;
		}
	}
}
	
void Menu::executeSelectedOption() {
	_options[_selectedOption].execute();
}
void Menu::executeOption( int index ) {
	_options[index].execute();
}


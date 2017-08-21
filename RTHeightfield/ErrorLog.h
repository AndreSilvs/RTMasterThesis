#pragma once

/** GLOBAL	HEADERS	**/
#include <fstream>
#include <string>

class ErrorLog {
public:
	ErrorLog( const std::string &fileName );
	~ErrorLog();

    void setName( const std::string &fileName );
	
    void reportError( const std::string &errorMessage );
	void reportError( int errorId, const std::string &errorMessage );
    void reportWarning( const std::string &errorMessage );
	void reportWarning( int warningId, const std::string &warningMessage );
	
private:

	bool openLog();
	void closeLog();
	
    std::string _fileName;

	std::ofstream _errorLog;
};

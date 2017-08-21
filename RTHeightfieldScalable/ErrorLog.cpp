#include "ErrorLog.h"

/** LOCAL	HEADERS	**/
#include "TimeCalcs.h"

ErrorLog::ErrorLog( const std::string &fileName ) :
	_fileName( fileName )
{
}

ErrorLog::~ErrorLog() {

}

void ErrorLog::setName( const std::string &fileName ) {
    _fileName = fileName;
}

void ErrorLog::reportError( const std::string &errorMessage ) {
    if ( openLog() ) {
        _errorLog << "[" << getCurrentTimeString() << "]" << " [Error] " << errorMessage << std::endl;
        closeLog();
    }
}
	
void ErrorLog::reportError( int errorId, const std::string &errorMessage ) {
	if ( openLog() ) {
		_errorLog << "[" << getCurrentTimeString() << "]" << " [Error: " << errorId << "] " << errorMessage << std::endl;
		closeLog();
	}
}
void ErrorLog::reportWarning( const std::string &warningMessage ) {
    if ( openLog() ) {
        _errorLog << "[" << getCurrentTimeString() << "]" << " [Warning] " << warningMessage << std::endl;
        closeLog();
    }
}

void ErrorLog::reportWarning( int warningId, const std::string &warningMessage ) {
	if ( openLog() ) {
		_errorLog << "[" << getCurrentTimeString() << "]" << " [Warning: " << warningId << "] " << warningMessage << std::endl;
		closeLog();
	}
}

// PRIVATE

bool ErrorLog::openLog() {
	_errorLog.open( _fileName, std::ios::app );
	return _errorLog.is_open();
}

void ErrorLog::closeLog() {
	_errorLog.close();
}
#include "DynamicFloatMap.h"

#include "MathCalcs.h"

DynamicFloatWrapper::DynamicFloatWrapper() :
    _loop( false ), _currentValue( 0.0f ), _minValue( 0.0f ), _maxValue( 1.0f ), _delta( 0.1f )
{
}
DynamicFloatWrapper::DynamicFloatWrapper( float value, float minValue, float maxValue, float delta, bool loop ) :
    _loop( loop ), _currentValue( value ), _minValue( minValue ), _maxValue( maxValue ), _delta( delta )
{
    if ( loop ) {
        _set = &DynamicFloatWrapper::setLoopValue;
    }
    else {
        _set = &DynamicFloatWrapper::setClampValue;
    }
}

float DynamicFloatWrapper::getValue() const {
    return _currentValue;
}
void DynamicFloatWrapper::setValue( float v ) {
    (this->*_set)( v );
}

void DynamicFloatWrapper::addDelta() {
    setValue( _currentValue + _delta );
}
void DynamicFloatWrapper::subDelta() {
    setValue( _currentValue - _delta );
}
bool DynamicFloatWrapper::canLoop() const {
    return _loop;
}

void DynamicFloatWrapper::setClampValue( float v ) {
    _currentValue = mathCalcs::clampToRange( v, _minValue, _maxValue );
}
void DynamicFloatWrapper::setLoopValue( float v ) {
    _currentValue = mathCalcs::loopInRange( v, _minValue, _maxValue );
}




DynamicFloatMap::DynamicFloatMap() {

}
DynamicFloatMap::~DynamicFloatMap() {

}

bool DynamicFloatMap::addData( const std::string& name, const DynamicFloatWrapper& data ) {
    return _data.insert( { name, data } ).second;
}

bool DynamicFloatMap::hasData( const std::string& name ) const {
    return _data.count( name ) > 0;
}

std::vector< std::pair< std::string, DynamicFloatWrapper* > > DynamicFloatMap::getVariablesArray() {
    std::vector< std::pair< std::string, DynamicFloatWrapper* > > vArray;
    for ( auto& p : _data ) {
        vArray.push_back( std::pair<std::string, DynamicFloatWrapper*>{ p.first, &p.second } );
    }
    return vArray;
}

DynamicFloatWrapper* DynamicFloatMap::getData( const std::string& name ) {
    if ( hasData( name ) ) {
        return &_data[ name ];
    }
    return nullptr;
}

DynamicFloatWrapper& DynamicFloatMap::operator[]( const std::string& name ) {
    return _data[ name ];
}
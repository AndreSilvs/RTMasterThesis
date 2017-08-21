#pragma once

#include <map>
#include <vector>
#include <string>

class DynamicFloatWrapper {
public:
    DynamicFloatWrapper();
    DynamicFloatWrapper( float value, float minValue, float maxValue, float delta, bool loop = false );

    float getValue() const;
    void setValue( float v );

    void addDelta();
    void subDelta();

    bool canLoop() const;

private:
    bool _loop;
    float _minValue;
    float _maxValue;
    float _currentValue;
    float _delta;

    void ( DynamicFloatWrapper::*_set )(float);

    void setClampValue( float v );
    void setLoopValue( float v );
};

class DynamicFloatMap {
public:
    DynamicFloatMap();
    ~DynamicFloatMap();

    bool addData( const std::string& name, const DynamicFloatWrapper& data );

    bool hasData( const std::string& name ) const;

    std::vector< std::pair< std::string, DynamicFloatWrapper* > > getVariablesArray();

    DynamicFloatWrapper* getData( const std::string& name );

    DynamicFloatWrapper& operator[]( const std::string& name );

private:
    std::map< std::string, DynamicFloatWrapper > _data;

};
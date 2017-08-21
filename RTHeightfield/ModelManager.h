#pragma once

#include <map>
#include <memory>
#include <string>

#include "Vao.h"

class ModelManager {
public:

    bool addModel( const std::string& name, const VaoInitializer& vInit );

    void renderModel( const std::string& name );

private:
    std::map< std::string, std::unique_ptr< Vao > > _models;

};

#include "ModelManager.h"

bool ModelManager::addModel( const std::string& name, const VaoInitializer& vInit ) {
    if ( _models.find( name ) == _models.end() ) {
        return false;
    }
    
    std::unique_ptr< Vao > model;
    if ( !model->createVao( vInit ) ) {
        return false;
    }

    _models.emplace( std::pair< std::string, std::unique_ptr< Vao> >( name, std::move( model ) ) );

    return true;
}

void ModelManager::renderModel( const std::string& name ) {
    auto m = _models.find( name );
    if (  m != _models.end() ) {
        m->second->render();
    }
}
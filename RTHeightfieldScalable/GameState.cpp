#include "GameState.h"

#include "GameStateManager.h"

GameState::GameState( std::string name, bool transparent, bool cooperative )
	: _name(name), _transparent(transparent), _cooperative(cooperative), _isActive( false )
{
}
	
std::string GameState::getName() {
	return _name;
}

bool GameState::isTransparent() {
	return _transparent;
}
bool GameState::isCooperative() {
	return _cooperative;
}
bool GameState::isActive() {
	return _isActive;
}

void GameState::registerManager(GameStateManager *manager) {
	_manager = manager;
}

void GameState::activate() {
	_isActive = true;
}
void GameState::deactivate() {
	_isActive = false;
}
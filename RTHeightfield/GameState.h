#pragma once

/** GLOBAL	HEADERS	**/
#include <map>
#include <memory>
#include <string>

/** FORWARD DECLARATIONS **/
class GameStateManager;

class GameState {
public:
	GameState( std::string name , bool transparent , bool cooperative );
	
	std::string getName();
	bool isTransparent();
	bool isCooperative();
	bool isActive();

	virtual void handleEvents() = 0;
	virtual void update( int dtime ) = 0;
	virtual void render() = 0;
	
	virtual void enterState() = 0;
	virtual void exitState() = 0;

protected:
	GameStateManager *_manager;

private:
	friend class GameStateManager;
	void registerManager(GameStateManager *manager);
	void activate();
	void deactivate();

	std::string _name;

	bool _isActive;
	bool _transparent;
	bool _cooperative;
};
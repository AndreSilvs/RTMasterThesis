#pragma once

/** GLOBAL	HEADERS	**/
#include <queue>
#include <map>
#include <memory>
#include <string>
#include <vector>

/** LOCAL	HEADERS	**/
#include "GameState.h"

enum GameStateEventType {
	GAME_STATE_PUSH,
	GAME_STATE_POP,
	GAME_STATE_SWITCH,
	GAME_STATE_POP_BACK_TO
};

struct GameStateEvent {
	GameStateEvent( GameStateEventType type, std::string name );

	GameStateEventType eventType;
	std::string stateName;
};

/*
Base class responsible for managing all application states
The deriving class should handle initialization and resource management, and event processing.

- Transparent states - Allow rendering from states below (up to the first non-transparent state)
- Cooperative states - Allow simultaneous updating from states below (up to the first non-cooperative state)
(However only the current state is allowed to do event handling)
*/
class GameStateManager {
public:
	GameStateManager();
	
	virtual bool initialize() = 0;
	virtual void cleanup() = 0;
	virtual void processEvents() = 0;

	std::string getCurrentStateName();
	int getNumStatesInStack();

	void registerState( std::shared_ptr< GameState > state );

	void pushNextState( std::string stateName );
	void switchState( std::string stateName );
	void popCurrentState();
	void popBackToState( std::string stateName );

	void handleEvents();
	void update( int dtime );
	void render();

	void doStateEvents();

	void disableInput();

protected:
	std::map< std::string, std::shared_ptr< GameState > > _states;

	std::queue< GameStateEvent > _stateEvents;
	std::vector< GameState* > _stateStack;
	
	unsigned int _lowestTransparentState;
	unsigned int _lowestCooperativeState;
	
	GameState* getTop();
	//GameState* getState( std::string name );
	
	void pushState( std::shared_ptr< GameState > state );
	void popState();

	int getStatePositionInStack( std::string name );

	void updateTransparency();
	void updateCooperativity();
};

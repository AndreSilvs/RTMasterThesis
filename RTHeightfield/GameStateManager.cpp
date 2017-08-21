#include "GameStateManager.h"

/** LOCAL	HEADERS	**/
#include "NullState.h"

GameStateEvent::GameStateEvent( GameStateEventType type, std::string name ) :
	eventType( type ),
	stateName( name )
{
}

GameStateManager::GameStateManager()
	: _lowestTransparentState(0), _lowestCooperativeState(0)
{
	registerState( std::shared_ptr< GameState >( new NullState() ) );
	_stateStack.push_back( _states.find( NullState::stateName() )->second.get() );
	_stateStack[ 0 ]->activate();
}

std::string GameStateManager::getCurrentStateName() {
	return _stateStack.back()->getName();
}
int GameStateManager::getNumStatesInStack() {
	return (int)_stateStack.size();
}

void GameStateManager::registerState( std::shared_ptr< GameState > state ) {
	_states.insert( std::pair< std::string, std::shared_ptr< GameState > >( state->getName(), state ));
	state->registerManager( this );
}

void GameStateManager::pushNextState( std::string stateName ) {
	_stateEvents.push( GameStateEvent( GAME_STATE_PUSH, stateName ) );
}
void GameStateManager::switchState( std::string stateName ) {
	_stateEvents.push( GameStateEvent( GAME_STATE_SWITCH, stateName ) );
}
void GameStateManager::popCurrentState() {
	_stateEvents.push( GameStateEvent( GAME_STATE_POP, "" ) );
}
void GameStateManager::popBackToState( std::string stateName ) {
	_stateEvents.push( GameStateEvent( GAME_STATE_POP_BACK_TO, stateName ) );
}

void GameStateManager::handleEvents() {
	getTop()->handleEvents();
}
void GameStateManager::update( int dtime ) {
	for ( unsigned int i = _lowestCooperativeState ; i < _stateStack.size(); ++i ) {
		_stateStack[i]->update( dtime );
	}
}
void GameStateManager::render() {
	for ( unsigned int i = _lowestTransparentState ; i < _stateStack.size(); ++i ) {
		_stateStack[i]->render();
	}
}

void GameStateManager::doStateEvents() {
	while ( !_stateEvents.empty() ) {
		GameStateEventType eventType = _stateEvents.front().eventType;
		if ( eventType == GAME_STATE_PUSH ) {
			auto it = _states.find( _stateEvents.front().stateName );
			if ( it == _states.end() ) {
				return;
			}

			pushState( it->second );
		}
		else if ( eventType == GAME_STATE_SWITCH ) {
			auto it = _states.find( _stateEvents.front().stateName );
			if ( it == _states.end() ) {
				return;
			}

			popState();
			pushState( it->second );
		}
		else if ( eventType == GAME_STATE_POP ) {
			popState();
		}
		else if ( eventType == GAME_STATE_POP_BACK_TO ) {
			int position = getStatePositionInStack( _stateEvents.front().stateName );
			if ( position != -1 ) {
				for ( int i = (int)_stateStack.size()-1; i > position; --i ) {
					popState();
				}
			}
		}
		_stateEvents.pop();
	}
}

void GameStateManager::disableInput() {

}

// Private methods
GameState* GameStateManager::getTop() {
	return _stateStack[_stateStack.size()-1];
}

int GameStateManager::getStatePositionInStack( std::string name ) {
	for ( int i = (int)_stateStack.size() - 1; i >= 0; --i ) {
		if ( _stateStack[i]->getName() == name ) {
			return (int)i;
		}
	}

	return -1;
}

void GameStateManager::pushState( std::shared_ptr< GameState > state ) {
	getTop()->deactivate();
	_stateStack.push_back( state.get() );
	getTop()->activate();

	getTop()->enterState();
		
	updateTransparency();
	updateCooperativity();
}
void GameStateManager::popState() {
	if ( _stateStack.size() > 1 ) {
		getTop()->exitState();

		getTop()->deactivate();
		_stateStack.pop_back();
		//getTop()->enterState();
		
		getTop()->enterState();
		getTop()->activate();

		updateTransparency();
		updateCooperativity();
	}
}

void GameStateManager::updateTransparency() {
	for ( int i = (int)_stateStack.size() - 1; i >= 0; --i ) {
		_lowestTransparentState = i;
		if ( !_stateStack[i]->isTransparent() ) {
			break;
		}
	}
}
void GameStateManager::updateCooperativity() {
	for ( int i = (int)_stateStack.size() - 1; i >= 0; --i ) {
		_lowestCooperativeState = i;
		if ( !_stateStack[i]->isCooperative() ) {
			break;
		}
	}
}
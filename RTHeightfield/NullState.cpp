#include "NullState.h"

std::string NullState::stateName() {
	return "_";
}

NullState::NullState()
	: GameState(stateName(), false, false)
{
}

NullState::~NullState()
{
}
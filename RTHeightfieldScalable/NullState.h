#pragma once

/** GLOBAL	HEADERS	**/
#include <string>

/** LOCAL	HEADERS	**/
#include "GameState.h"

class NullState : public GameState {
public:
	static std::string stateName();

	NullState();
	~NullState();

	void handleEvents() {}
	void update( int dtime ) {}
	void render() {}

	void enterState() {}
	void exitState() {}
};


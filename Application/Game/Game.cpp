#include "Game.h"

#include "Level/MainLevel.h"

Game::Game()
{
	AddLevel(new MainLevel());
}

Game::~Game()
{
}

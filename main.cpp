#include <iostream>

#include <windows.h>
#include <stdlib.h>
#include <time.h>

#include "Game.h"
#include "GameConfig.h"
#include "NoteManager.h"
#include "Judgment.h"
#include "TrackConfig.h"

#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int main(int argc, char* argv[])
{
    Game game;
    game.Run();
    return 0;
}
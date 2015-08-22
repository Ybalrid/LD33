#include "pch.hpp"
#include <iostream>


#ifdef _WIN32
#define mainFunc() int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,LPSTR lpCmdLine, int nCmdShow)
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#else
#define mainFunc() int main(void) 
#endif

#include "Game.h"

mainFunc()
{
	Game* g(new Game());
	g->run();
	delete g;
	return EXIT_SUCCESS;
}
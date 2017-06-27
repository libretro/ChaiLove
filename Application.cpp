#include <stdint.h>
#include <string>
#include <SDL.h>
#include <SDL_gfxPrimitives.h>

#include "Application.h"

#include "chaigame/chaigame.h"

Application* Application::m_instance = NULL;

bool Application::isRunning() {
	return m_instance != NULL;
}
void Application::destroy() {
	m_instance = NULL;
}

Application* Application::getInstance() {
	if (!m_instance) {
		m_instance = new Application;
	}
	return m_instance;
}

void Application::quit(void) {
	// Tell SDL to quit.
	SDL_Quit();
}

bool Application::load() {
	// Initialize SDL.
	if (SDL_Init(SDL_INIT_EVERYTHING) == -1) {
		return false;
	}

	// Build the Screen.
	screen = SDL_SetVideoMode(640, 480, 32, SDL_SWSURFACE | SDL_SRCALPHA | SDL_RESIZABLE);
	if (screen == NULL) {
		SDL_Quit();
		return false;
	}

	// Enable video buffering.
	videoBuffer = (unsigned int *)screen->pixels;

	// Fix alpha blending.
	if (SDL_SetAlpha(screen, SDL_SRCALPHA, 0) == -1) {
		// Error: Do nothing.
	}

	// Disable the mouse cursor from showing up.
	SDL_ShowCursor(SDL_DISABLE);

	// Initalize the chaigame subsystems.
	keyboard.load();

	// ChaiScript.
	#ifndef __DISABLE_CHAISCRIPT__
	// Load main.chai.
	chai.eval_file("main.chai");

	// Find the game functions.
	chaiload = chai.eval<std::function<void ()> >("load");
	chaiupdate = chai.eval<std::function<void (Uint32)> >("update");
	chaidraw = chai.eval<std::function<void ()> >("draw");

	// Register the Graphics module.
	chai.add(chaiscript::fun(chaigame::graphics::rectangle), "rectangle");

	// Register the Keyboard module.
	chai.add(chaiscript::fun(&chaigame::keyboard::update), "update");
	chai.add(chaiscript::fun(&chaigame::keyboard::isDown), "isDown");
	chai.add_global(chaiscript::var(std::ref(keyboard)), "keyboard");

	// Initialize the game.
	chaiload();
	#endif

	// Set up the game timer.
	tick = SDL_GetTicks();

	return true;
}

bool Application::update() {
	bool quit = false;

	// Update all the input.
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
			case SDL_MOUSEBUTTONDOWN:
				quit = true;
				if ( event.button.button == SDL_BUTTON_LEFT ) {
				} else if ( event.button.button == SDL_BUTTON_RIGHT ) {
					/* Switch to prev graphics */
					//curprim--;
				}
				break;
			case SDL_KEYDOWN:

				#ifndef __DISABLE_CHAISCRIPT__
				//x += chai.eval<int>("multiply(5, 20);");
				//x += 100;
				#endif
			default:
				break;
		}
	}

	keyboard.update();

	// Retrieve the new game time.
	Uint32 current = SDL_GetTicks();

	// Update the game.
	#ifndef __DISABLE_CHAISCRIPT__
	chaiupdate(current - tick);
	#endif

	// Update the timer.
	tick = current;

	return quit;
}

/**
 * Render the application.
 */
void Application::draw(){
	// Clear the screen
	Uint32 color = SDL_MapRGBA(screen->format, 0, 0, 0, 255);
	SDL_FillRect(screen, NULL, color);

	// Test drawing a rectangle.
	static int x = 10;
	static int y = 10;

	if (keyboard.isDown("up")) {
		y -= 6;
	}
	if (keyboard.isDown("down")) {
		y += 6;
	}
	if (keyboard.isDown("left")) {
		x -= 6;
	}
	if (keyboard.isDown("right")) {
		x += 6;
	}
	chaigame::graphics::rectangle(x, y, 100, 100, 0, 255, 255, 255);

	// Render the game.
	#ifndef __DISABLE_CHAISCRIPT__
	chaidraw();
	#endif

	// Update the screen.
	SDL_UpdateRect(screen, 0, 0, 0, 0);
	SDL_Flip(screen);
}
#include <SDL2/SDL.h>
#include <iostream>
#include <vector>

#ifdef HANDHELD
const bool HandheldMode = true;
#else
const bool HandheldMode = false;
#endif

#pragma region ControllerButtons

struct ControllerButtons
{
	bool A = false;
	bool B = false;
	bool X = false;
	bool Y = false;

	bool Start = false;
	bool Select = false;

	bool DPad_Up = false;
	bool DPad_Down = false;
	bool DPad_Left = false;
	bool DPad_Right = false;

	bool L1 = false;
	bool L2 = false;
	bool L3 = false;
	bool R1 = false;
	bool R2 = false;
	bool R3 = false;

	void Clear()
	{
		A = B = X = Y = false;
		Start = Select = false;
		DPad_Up = DPad_Down = DPad_Left = DPad_Right = false;
		L1 = L2 = L3 = false;
		R1 = R2 = R3 = false;
	}

	void SetState(int buttonCode, bool state)
	{
		switch (buttonCode)
		{
			case SDL_CONTROLLER_BUTTON_A: A = state; break;
			case SDL_CONTROLLER_BUTTON_B: B = state; break;
			case SDL_CONTROLLER_BUTTON_X: X = state; break;
			case SDL_CONTROLLER_BUTTON_Y: Y = state; break;

			case SDL_CONTROLLER_BUTTON_START: Start = state; break;
			case SDL_CONTROLLER_BUTTON_BACK: Select = state; break;

			case SDL_CONTROLLER_BUTTON_DPAD_UP: DPad_Up = state; break;
			case SDL_CONTROLLER_BUTTON_DPAD_DOWN: DPad_Down = state; break;
			case SDL_CONTROLLER_BUTTON_DPAD_LEFT: DPad_Left = state; break;
			case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: DPad_Right = state; break;

			case SDL_CONTROLLER_BUTTON_LEFTSHOULDER: L1 = state; break;
			case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER: R1 = state; break;

			case SDL_CONTROLLER_BUTTON_LEFTSTICK: L3 = state; break;
			case SDL_CONTROLLER_BUTTON_RIGHTSTICK: R3 = state; break;

			default: break;
		}

	}
};

#pragma endregion

#pragma region Color Handler

std::vector<SDL_Color> colors;

SDL_Color GetColor(int hexCode)
{
	SDL_Color c;

	c.a = 0xff;

	c.b = hexCode & 0xff;
	hexCode = hexCode >> 8;

	c.g = hexCode & 0xff;
	hexCode = hexCode >> 8;

	c.r = hexCode & 0xff;

	return c;
}

void InitColors()
{
	colors.push_back(GetColor(0xf5f5f5));

	colors.push_back(GetColor(0x3a2032));
	colors.push_back(GetColor(0xdfa6c7));
	colors.push_back(GetColor(0xe32d38));
	colors.push_back(GetColor(0xffa226));
	colors.push_back(GetColor(0x89dd47));
	colors.push_back(GetColor(0x1f5347));
	colors.push_back(GetColor(0x3cb1c8));
	colors.push_back(GetColor(0xaec0d4));
	colors.push_back(GetColor(0xb4a56a));
	colors.push_back(GetColor(0xcc3495));
	colors.push_back(GetColor(0x5f8dc9));
	colors.push_back(GetColor(0x00e7ff));
	colors.push_back(GetColor(0xff7d20));
	colors.push_back(GetColor(0xff00ff));
}

#pragma endregion

bool running = false;
bool fullscreen = false;

//Target a 640*480 window / full screen size, ideally on a handheld device.

//The target window / full screen Width.
int windowW = 640;

//The target window / full screen Height.
int windowH = 480;

//The current desktop Width.
int desktopW = 0;

//The current desktop Height.
int desktopH = 0;

//Variables for FPS counting and Update timer.
int frameCount = 0;
int timerFPS = 0;
int lastTime = 0;
int lastFrame = 0;
int fps = 0;

//Pointers for SDL.
SDL_Renderer* renderer = NULL;
SDL_Window* window = NULL;
SDL_GameController* gameController = NULL;

//The game controller's button states.
ControllerButtons cb;

//Initial and current position of the displayed box.
int posX = 20;
int posY = 10;
int size = 100;
int step = 4;

void UpdatePosition()
{
	if(cb.DPad_Up == true) posY = posY - step;
	if(cb.DPad_Down == true) posY = posY + step;
	if(cb.DPad_Left == true) posX = posX - step;
	if(cb.DPad_Right == true) posX = posX + step;

	if(posX < 0) posX = 0;
	if(posY < 0) posY = 0;

	if(posX > (windowW - size)) posX = windowW - size;
	if(posY > (windowH - size)) posY = windowH - size;
}

SDL_Color UpdateColor()
{
	if(cb.A) return colors[1];
	if(cb.B) return colors[2];
	if(cb.X) return colors[3];
	if(cb.Y) return colors[4];
	if(cb.Start) return colors[5];
	if(cb.Select) return colors[6];
	if(cb.L1) return colors[7];
	if(cb.L2) return colors[8];
	if(cb.L3) return colors[9];
	if(cb.R1) return colors[10];
	if(cb.R2) return colors[11];
	if(cb.R3) return colors[12];

	return colors[0];
}

//Shows the window, either in windowed mode or in full screen, depending on the "fullscreen" variable's state.
void FullscreenSwitcher()
{
	if(fullscreen == true)
	{
		SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
	}
	else
	{
		SDL_SetWindowFullscreen(window, 0);
	}
}

void Update()
{
	//This is here just for the F11 key handling.
	FullscreenSwitcher();
}

void Input()
{
	SDL_Event e;

	while(SDL_PollEvent(&e))
	{
		if(e.type == SDL_QUIT)
		{
			running = false;
		}
		else if(e.type == SDL_CONTROLLERBUTTONDOWN)
		{
			std::cout << "Controller button down " << ((int)e.cbutton.button) << std::endl;
			cb.SetState((int)e.cbutton.button, true);
		}
		else if(e.type == SDL_CONTROLLERBUTTONUP)
		{
			std::cout << "Controller button up " << ((int)e.cbutton.button) << std::endl;
			cb.SetState((int)e.cbutton.button, false);
		}
		else
		{
			//std::cout << "Other: " << e.type << std::endl;
		}

		//Quit when Start and Select pressed together.
		if(cb.Start == true && cb.Select == true)
		{
			running = false;
		}
	}

	//Grab the currently pressed keyboard keys.
	const Uint8 *keystates = SDL_GetKeyboardState(NULL);

	//Quit when the Escape key is pressed.
	if(keystates[SDL_SCANCODE_ESCAPE])
	{
		running = false;
	}

	//Switch between windowed and full screen mode when F11 is pressed.
	if(keystates[SDL_SCANCODE_F11])
	{
		fullscreen = !fullscreen;
	}
}

void Draw()
{
	//Set a background color and clear the screen.
	SDL_SetRenderDrawColor(renderer, 0x10, 0x30, 0x90, 0xff);
	SDL_RenderClear(renderer);

	//Update the rectangle's position, based on which direction buttons are being pressed.
	UpdatePosition();

	SDL_Rect rect;
	rect.x = posX;
	rect.y = posY;
	rect.w = 100;
	rect.h = 100;

	//Get the color of the box, based on which controller buttons are pressed.
	SDL_Color color = UpdateColor();

	//Draw the box we can move and color with the controller.
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xff);
	SDL_RenderFillRect(renderer, &rect);

	//All other drawing operations would come here.

	//End the drawing by calculating how much time it took to draw the frame.
	frameCount++;
	int timerFPS = SDL_GetTicks() - lastFrame;

	//Typically the drawing took less than 1/60th of a frame (for 60 FPS)
	//so wait for the rest of the time by releasing the CPU for other processes.
	if(timerFPS < (1000/60))
	{
		SDL_Delay((1000/60) - timerFPS);
	}

	//Update the screen.
	SDL_RenderPresent(renderer);
}


#pragma region Initialize

bool Initialize()
{
	fullscreen = false;

	//Initialize SDL2 in general.
	if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
	{
		std::cout << "Failed at SDL_Init()" << std::endl;
		return false;
	}

	//Create a rendering surface for SDL2, which includes a hidden window.
	if(SDL_CreateWindowAndRenderer(windowW, windowH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN, &window, &renderer) < 0)
	{
		std::cout << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
		return false;
	}

	//Technically only here we can get the display resolution, which is annoying.
	//I may need to destroy and re-create the window for the best size match.

	SDL_DisplayMode dm;

	if (SDL_GetDesktopDisplayMode(0, &dm) < 0)
	{
		std::cout << "Failed at SDL_SDL_GetDesktopDisplayMode()" << std::endl;
		//SDL_Log("SDL_GetDesktopDisplayMode failed: %s", SDL_GetError());
		return false;
	}

	desktopW = dm.w;
	desktopH = dm.h;

	//For now, don't worry about vertical screens. Normalize the screen size for horizontal screens.
	if(dm.w < dm.h)
	{
		desktopW = dm.h;
		desktopH = dm.w;
	}

	// desktopW = 320;
	// desktopH = 240;

	//Detect if it's a fairly standard 320x240 screen.
	if(desktopW <= 320)
	{
		//Re-target the window / full screen resolution to 320x240.
		windowW = 320;
		windowH = 240;

		//Technically we should switch to full screen mode here, as it's likely a handheld device.

		//Close the existing renderer and window, it needs to be re-created with a smaller size.
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);

		//Create a rendering surface for SDL2 for 320x240, which includes a hidden window.
		if(SDL_CreateWindowAndRenderer(windowW, windowH, SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIDDEN, &window, &renderer) < 0)
		{
			std::cout << "Failed at SDL_CreateWindowAndRenderer() at 320x240" << std::endl;
			return false;
		}
	}

	std::cout << "Screen resolution: " << desktopW << " x " << desktopH << std::endl;

	//Initialize the game controller.
	if(SDL_NumJoysticks() > 0)
	{
		//Enumerate through the connected Game Controllers and use the first one as default.
		for(int i=0; i < SDL_NumJoysticks(); i++)
		{
			if(SDL_IsGameController(i) == true)
			{
				gameController = SDL_GameControllerOpen(i);

				if(gameController == NULL)
				{
					std::cout << "Failed at GameControllerOpen()" << std::endl;
					return false;
				}

				std::cout << "Game controller found: " << SDL_GameControllerNameForIndex(i) << std::endl;

				break;
			}
		}		
	}

	if(gameController == NULL)
	{
		std::cout << "Warning: Game Controller not found." << std::endl;
		return false;
	}

	//At this point the basics are all good. Now show the window, switch to full screen if needed.

	if(HandheldMode == true)
	{
		//Automatically switch to full screen in Handheld mode.
		fullscreen = true;

		//Hide the mouse pointer in Handheld mode.
		SDL_ShowCursor(0);
	}

	SDL_SetWindowTitle(window, "SDL2 Template");

	SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "2");

	SDL_ShowWindow(window);

	//Make the window full screen, if that mode has been selected above.
	FullscreenSwitcher();

	InitColors();

	//All good, initialization completed.
	return true;
}

#pragma endregion

//---------------------------------------------------------------------------

void Close()
{
		SDL_GameControllerClose(gameController);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
		SDL_Quit();
}

int main()
{
	if(Initialize() == false)
	{
		//Initialization failed. The corresponding message has been printed to the console.
		Close();

		return 1;
	}

	running = true;
	lastTime = 0;

	cb.Clear();

	//This is the basic Game Loop.
	while(running)
	{
		lastFrame = SDL_GetTicks();

		if(lastFrame >= (lastTime + 1000))
		{
			lastTime = lastFrame;
			fps = frameCount;
			frameCount = 0;
		}

		if(fps > 0)
		{
			//std::cout << "FPS: " << fps << std::endl;
		}

		Update();
		Input();
		Draw();
	}

	Close();

	return 0;
}

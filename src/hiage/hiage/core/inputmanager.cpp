#include "../sdl-includes.h"
#include "inputmanager.h"

using namespace hiage;

InputManager::InputManager() : initialized(false)
{
}

//initialize the input manager
bool InputManager::initialize()
{
	return true;
}

bool InputManager::keyDown(int keycode)
{
	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[keycode])
	{
		return true;
	}

	return false;
}

int InputManager::mouseButtonDown(Vector2<double> * pPos)
{
	Uint8 keys;
	if (pPos)
	{
		int x, y;
		keys = SDL_GetMouseState(&x, &y);
		*pPos = Vector2<double>(x, y);
	}
	else
	{
		keys = SDL_GetMouseState(0, 0);
	}

	for (int i = 0; i <=3; i++)
	{
		if (keys & SDL_BUTTON(i))
		{
			return i;
		}
	}

	return 0;
}

bool InputManager::mouseButtonDown(int button, Vector2<double> * pos)
{
	Uint8 keys;
	if (pos)
	{
		int x, y;
		keys = SDL_GetMouseState(&x, &y);
		*pos = Vector2<double>(x, y);
	}
	else
	{
		keys = SDL_GetMouseState(0, 0);
	}

	if (keys & SDL_BUTTON(button))
	{
		return true;
	}

	return false;
}

Vector2<double> InputManager::getMousePosition()
{
	int x,y;
	Vector2<double> mousePosition;

	SDL_GetMouseState(&x, &y);
	mousePosition.set(x,y);

	return mousePosition;
}

/*
int InputManager::MouseClick()
{

}

bool InputManager::MouseClick(int nButton)
{
	SDL_Event p_Event;

	while (SDL_PollEvent(&p_Event))
	{
		if (p_Event.type == SDL_MOUSEBUTTONDOWN)
		{
			if (nButton == 1)
			{
				if (p_Event.button.button == SDL_BUTTON_LEFT)
				{
					return true;
				}
			}

		}
	}
}*/

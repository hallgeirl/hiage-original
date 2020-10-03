#include "../sdl-includes.h"
#include "inputmanager.hpp"

using namespace hiage;
using namespace std;

std::unordered_map<std::string, int> KeyBindings::keyNameToScancodeMappings;

KeyBindings::KeyBindings()
{
	// Populate key name to scancode mappings
	if (keyNameToScancodeMappings.size() == 0)
	{
		/*
			Generated using regex on all SDL scancodes from header.
			Regex used: ^SDL_SCANCODE_(.+)$
			Replace with: keyNameToScancodeMappings["\L$1"] = \USDL_SCANCODE_$1;
		*/
		keyNameToScancodeMappings["a"] = SDL_SCANCODE_A;
		keyNameToScancodeMappings["b"] = SDL_SCANCODE_B;
		keyNameToScancodeMappings["c"] = SDL_SCANCODE_C;
		keyNameToScancodeMappings["d"] = SDL_SCANCODE_D;
		keyNameToScancodeMappings["e"] = SDL_SCANCODE_E;
		keyNameToScancodeMappings["f"] = SDL_SCANCODE_F;
		keyNameToScancodeMappings["g"] = SDL_SCANCODE_G;
		keyNameToScancodeMappings["h"] = SDL_SCANCODE_H;
		keyNameToScancodeMappings["i"] = SDL_SCANCODE_I;
		keyNameToScancodeMappings["j"] = SDL_SCANCODE_J;
		keyNameToScancodeMappings["k"] = SDL_SCANCODE_K;
		keyNameToScancodeMappings["l"] = SDL_SCANCODE_L;
		keyNameToScancodeMappings["m"] = SDL_SCANCODE_M;
		keyNameToScancodeMappings["n"] = SDL_SCANCODE_N;
		keyNameToScancodeMappings["o"] = SDL_SCANCODE_O;
		keyNameToScancodeMappings["p"] = SDL_SCANCODE_P;
		keyNameToScancodeMappings["q"] = SDL_SCANCODE_Q;
		keyNameToScancodeMappings["r"] = SDL_SCANCODE_R;
		keyNameToScancodeMappings["s"] = SDL_SCANCODE_S;
		keyNameToScancodeMappings["t"] = SDL_SCANCODE_T;
		keyNameToScancodeMappings["u"] = SDL_SCANCODE_U;
		keyNameToScancodeMappings["v"] = SDL_SCANCODE_V;
		keyNameToScancodeMappings["w"] = SDL_SCANCODE_W;
		keyNameToScancodeMappings["x"] = SDL_SCANCODE_X;
		keyNameToScancodeMappings["y"] = SDL_SCANCODE_Y;
		keyNameToScancodeMappings["z"] = SDL_SCANCODE_Z;
		keyNameToScancodeMappings["1"] = SDL_SCANCODE_1;
		keyNameToScancodeMappings["2"] = SDL_SCANCODE_2;
		keyNameToScancodeMappings["3"] = SDL_SCANCODE_3;
		keyNameToScancodeMappings["4"] = SDL_SCANCODE_4;
		keyNameToScancodeMappings["5"] = SDL_SCANCODE_5;
		keyNameToScancodeMappings["6"] = SDL_SCANCODE_6;
		keyNameToScancodeMappings["7"] = SDL_SCANCODE_7;
		keyNameToScancodeMappings["8"] = SDL_SCANCODE_8;
		keyNameToScancodeMappings["9"] = SDL_SCANCODE_9;
		keyNameToScancodeMappings["0"] = SDL_SCANCODE_0;
		keyNameToScancodeMappings["return"] = SDL_SCANCODE_RETURN;
		keyNameToScancodeMappings["escape"] = SDL_SCANCODE_ESCAPE;
		keyNameToScancodeMappings["backspace"] = SDL_SCANCODE_BACKSPACE;
		keyNameToScancodeMappings["tab"] = SDL_SCANCODE_TAB;
		keyNameToScancodeMappings["space"] = SDL_SCANCODE_SPACE;
		keyNameToScancodeMappings["minus"] = SDL_SCANCODE_MINUS;
		keyNameToScancodeMappings["equals"] = SDL_SCANCODE_EQUALS;
		keyNameToScancodeMappings["leftbracket"] = SDL_SCANCODE_LEFTBRACKET;
		keyNameToScancodeMappings["rightbracket"] = SDL_SCANCODE_RIGHTBRACKET;
		keyNameToScancodeMappings["backslash"] = SDL_SCANCODE_BACKSLASH;
		keyNameToScancodeMappings["nonushash"] = SDL_SCANCODE_NONUSHASH;
		keyNameToScancodeMappings["semicolon"] = SDL_SCANCODE_SEMICOLON;
		keyNameToScancodeMappings["apostrophe"] = SDL_SCANCODE_APOSTROPHE;
		keyNameToScancodeMappings["grave "] = SDL_SCANCODE_GRAVE;
		keyNameToScancodeMappings["comma"] = SDL_SCANCODE_COMMA;
		keyNameToScancodeMappings["period"] = SDL_SCANCODE_PERIOD;
		keyNameToScancodeMappings["slash"] = SDL_SCANCODE_SLASH;
		keyNameToScancodeMappings["capslock"] = SDL_SCANCODE_CAPSLOCK;
		keyNameToScancodeMappings["f1"] = SDL_SCANCODE_F1;
		keyNameToScancodeMappings["f2"] = SDL_SCANCODE_F2;
		keyNameToScancodeMappings["f3"] = SDL_SCANCODE_F3;
		keyNameToScancodeMappings["f4"] = SDL_SCANCODE_F4;
		keyNameToScancodeMappings["f5"] = SDL_SCANCODE_F5;
		keyNameToScancodeMappings["f6"] = SDL_SCANCODE_F6;
		keyNameToScancodeMappings["f7"] = SDL_SCANCODE_F7;
		keyNameToScancodeMappings["f8"] = SDL_SCANCODE_F8;
		keyNameToScancodeMappings["f9"] = SDL_SCANCODE_F9;
		keyNameToScancodeMappings["f10"] = SDL_SCANCODE_F10;
		keyNameToScancodeMappings["f11"] = SDL_SCANCODE_F11;
		keyNameToScancodeMappings["f12"] = SDL_SCANCODE_F12;
		keyNameToScancodeMappings["printscreen"] = SDL_SCANCODE_PRINTSCREEN;
		keyNameToScancodeMappings["scrolllock"] = SDL_SCANCODE_SCROLLLOCK;
		keyNameToScancodeMappings["pause"] = SDL_SCANCODE_PAUSE;
		keyNameToScancodeMappings["insert "] = SDL_SCANCODE_INSERT;
		keyNameToScancodeMappings["home"] = SDL_SCANCODE_HOME;
		keyNameToScancodeMappings["pageup"] = SDL_SCANCODE_PAGEUP;
		keyNameToScancodeMappings["delete"] = SDL_SCANCODE_DELETE;
		keyNameToScancodeMappings["end"] = SDL_SCANCODE_END;
		keyNameToScancodeMappings["pagedown"] = SDL_SCANCODE_PAGEDOWN;
		keyNameToScancodeMappings["right"] = SDL_SCANCODE_RIGHT;
		keyNameToScancodeMappings["left"] = SDL_SCANCODE_LEFT;
		keyNameToScancodeMappings["down"] = SDL_SCANCODE_DOWN;
		keyNameToScancodeMappings["up"] = SDL_SCANCODE_UP;
		keyNameToScancodeMappings["numlockclear "] = SDL_SCANCODE_NUMLOCKCLEAR;
		keyNameToScancodeMappings["kp_divide"] = SDL_SCANCODE_KP_DIVIDE;
		keyNameToScancodeMappings["kp_multiply"] = SDL_SCANCODE_KP_MULTIPLY;
		keyNameToScancodeMappings["kp_minus"] = SDL_SCANCODE_KP_MINUS;
		keyNameToScancodeMappings["kp_plus"] = SDL_SCANCODE_KP_PLUS;
		keyNameToScancodeMappings["kp_enter"] = SDL_SCANCODE_KP_ENTER;
		keyNameToScancodeMappings["kp_1"] = SDL_SCANCODE_KP_1;
		keyNameToScancodeMappings["kp_2"] = SDL_SCANCODE_KP_2;
		keyNameToScancodeMappings["kp_3"] = SDL_SCANCODE_KP_3;
		keyNameToScancodeMappings["kp_4"] = SDL_SCANCODE_KP_4;
		keyNameToScancodeMappings["kp_5"] = SDL_SCANCODE_KP_5;
		keyNameToScancodeMappings["kp_6"] = SDL_SCANCODE_KP_6;
		keyNameToScancodeMappings["kp_7"] = SDL_SCANCODE_KP_7;
		keyNameToScancodeMappings["kp_8"] = SDL_SCANCODE_KP_8;
		keyNameToScancodeMappings["kp_9"] = SDL_SCANCODE_KP_9;
		keyNameToScancodeMappings["kp_0"] = SDL_SCANCODE_KP_0;
		keyNameToScancodeMappings["kp_period"] = SDL_SCANCODE_KP_PERIOD;
		keyNameToScancodeMappings["kp_equals"] = SDL_SCANCODE_KP_EQUALS;
		keyNameToScancodeMappings["f13"] = SDL_SCANCODE_F13;
		keyNameToScancodeMappings["f14"] = SDL_SCANCODE_F14;
		keyNameToScancodeMappings["f15"] = SDL_SCANCODE_F15;
		keyNameToScancodeMappings["f16"] = SDL_SCANCODE_F16;
		keyNameToScancodeMappings["f17"] = SDL_SCANCODE_F17;
		keyNameToScancodeMappings["f18"] = SDL_SCANCODE_F18;
		keyNameToScancodeMappings["f19"] = SDL_SCANCODE_F19;
		keyNameToScancodeMappings["f20"] = SDL_SCANCODE_F20;
		keyNameToScancodeMappings["f21"] = SDL_SCANCODE_F21;
		keyNameToScancodeMappings["f22"] = SDL_SCANCODE_F22;
		keyNameToScancodeMappings["f23"] = SDL_SCANCODE_F23;
		keyNameToScancodeMappings["f24"] = SDL_SCANCODE_F24;
		keyNameToScancodeMappings["execute"] = SDL_SCANCODE_EXECUTE;
		keyNameToScancodeMappings["help"] = SDL_SCANCODE_HELP;
		keyNameToScancodeMappings["menu"] = SDL_SCANCODE_MENU;
		keyNameToScancodeMappings["select"] = SDL_SCANCODE_SELECT;
		keyNameToScancodeMappings["stop"] = SDL_SCANCODE_STOP;
		keyNameToScancodeMappings["again   "] = SDL_SCANCODE_AGAIN;
		keyNameToScancodeMappings["undo"] = SDL_SCANCODE_UNDO;
		keyNameToScancodeMappings["cut"] = SDL_SCANCODE_CUT;
		keyNameToScancodeMappings["copy"] = SDL_SCANCODE_COPY;
		keyNameToScancodeMappings["paste"] = SDL_SCANCODE_PASTE;
		keyNameToScancodeMappings["find"] = SDL_SCANCODE_FIND;
		keyNameToScancodeMappings["mute"] = SDL_SCANCODE_MUTE;
		keyNameToScancodeMappings["volumeup"] = SDL_SCANCODE_VOLUMEUP;
		keyNameToScancodeMappings["volumedown"] = SDL_SCANCODE_VOLUMEDOWN;
		keyNameToScancodeMappings["kp_comma"] = SDL_SCANCODE_KP_COMMA;
		keyNameToScancodeMappings["kp_equalsas400"] = SDL_SCANCODE_KP_EQUALSAS400;
		keyNameToScancodeMappings["kp_00"] = SDL_SCANCODE_KP_00;
		keyNameToScancodeMappings["kp_000"] = SDL_SCANCODE_KP_000;
		keyNameToScancodeMappings["thousandsseparator"] = SDL_SCANCODE_THOUSANDSSEPARATOR;
		keyNameToScancodeMappings["decimalseparator"] = SDL_SCANCODE_DECIMALSEPARATOR;
		keyNameToScancodeMappings["currencyunit"] = SDL_SCANCODE_CURRENCYUNIT;
		keyNameToScancodeMappings["currencysubunit"] = SDL_SCANCODE_CURRENCYSUBUNIT;
		keyNameToScancodeMappings["kp_leftparen"] = SDL_SCANCODE_KP_LEFTPAREN;
		keyNameToScancodeMappings["kp_rightparen"] = SDL_SCANCODE_KP_RIGHTPAREN;
		keyNameToScancodeMappings["kp_leftbrace"] = SDL_SCANCODE_KP_LEFTBRACE;
		keyNameToScancodeMappings["kp_rightbrace"] = SDL_SCANCODE_KP_RIGHTBRACE;
		keyNameToScancodeMappings["kp_tab"] = SDL_SCANCODE_KP_TAB;
		keyNameToScancodeMappings["kp_backspace"] = SDL_SCANCODE_KP_BACKSPACE;
		keyNameToScancodeMappings["kp_a"] = SDL_SCANCODE_KP_A;
		keyNameToScancodeMappings["kp_b"] = SDL_SCANCODE_KP_B;
		keyNameToScancodeMappings["kp_c"] = SDL_SCANCODE_KP_C;
		keyNameToScancodeMappings["kp_d"] = SDL_SCANCODE_KP_D;
		keyNameToScancodeMappings["kp_e"] = SDL_SCANCODE_KP_E;
		keyNameToScancodeMappings["kp_f"] = SDL_SCANCODE_KP_F;
		keyNameToScancodeMappings["kp_xor"] = SDL_SCANCODE_KP_XOR;
		keyNameToScancodeMappings["kp_power"] = SDL_SCANCODE_KP_POWER;
		keyNameToScancodeMappings["kp_percent"] = SDL_SCANCODE_KP_PERCENT;
		keyNameToScancodeMappings["kp_less"] = SDL_SCANCODE_KP_LESS;
		keyNameToScancodeMappings["kp_greater"] = SDL_SCANCODE_KP_GREATER;
		keyNameToScancodeMappings["kp_ampersand"] = SDL_SCANCODE_KP_AMPERSAND;
		keyNameToScancodeMappings["kp_dblampersand"] = SDL_SCANCODE_KP_DBLAMPERSAND;
		keyNameToScancodeMappings["kp_verticalbar"] = SDL_SCANCODE_KP_VERTICALBAR;
		keyNameToScancodeMappings["kp_dblverticalbar"] = SDL_SCANCODE_KP_DBLVERTICALBAR;
		keyNameToScancodeMappings["kp_colon"] = SDL_SCANCODE_KP_COLON;
		keyNameToScancodeMappings["kp_hash"] = SDL_SCANCODE_KP_HASH;
		keyNameToScancodeMappings["kp_space"] = SDL_SCANCODE_KP_SPACE;
		keyNameToScancodeMappings["kp_at"] = SDL_SCANCODE_KP_AT;
		keyNameToScancodeMappings["kp_exclam"] = SDL_SCANCODE_KP_EXCLAM;
		keyNameToScancodeMappings["kp_memstore"] = SDL_SCANCODE_KP_MEMSTORE;
		keyNameToScancodeMappings["kp_memrecall"] = SDL_SCANCODE_KP_MEMRECALL;
		keyNameToScancodeMappings["kp_memclear"] = SDL_SCANCODE_KP_MEMCLEAR;
		keyNameToScancodeMappings["kp_memadd"] = SDL_SCANCODE_KP_MEMADD;
		keyNameToScancodeMappings["kp_memsubtract"] = SDL_SCANCODE_KP_MEMSUBTRACT;
		keyNameToScancodeMappings["kp_memmultiply"] = SDL_SCANCODE_KP_MEMMULTIPLY;
		keyNameToScancodeMappings["kp_memdivide"] = SDL_SCANCODE_KP_MEMDIVIDE;
		keyNameToScancodeMappings["kp_plusminus"] = SDL_SCANCODE_KP_PLUSMINUS;
		keyNameToScancodeMappings["kp_clear"] = SDL_SCANCODE_KP_CLEAR;
		keyNameToScancodeMappings["kp_clearentry"] = SDL_SCANCODE_KP_CLEARENTRY;
		keyNameToScancodeMappings["kp_binary"] = SDL_SCANCODE_KP_BINARY;
		keyNameToScancodeMappings["kp_octal"] = SDL_SCANCODE_KP_OCTAL;
		keyNameToScancodeMappings["kp_decimal"] = SDL_SCANCODE_KP_DECIMAL;
		keyNameToScancodeMappings["kp_hexadecimal"] = SDL_SCANCODE_KP_HEXADECIMAL;
		keyNameToScancodeMappings["lctrl"] = SDL_SCANCODE_LCTRL;
		keyNameToScancodeMappings["lshift"] = SDL_SCANCODE_LSHIFT;
		keyNameToScancodeMappings["lalt "] = SDL_SCANCODE_LALT;
		keyNameToScancodeMappings["lgui "] = SDL_SCANCODE_LGUI;
		keyNameToScancodeMappings["rctrl"] = SDL_SCANCODE_RCTRL;
		keyNameToScancodeMappings["rshift"] = SDL_SCANCODE_RSHIFT;
		keyNameToScancodeMappings["ralt"] = SDL_SCANCODE_RALT;
		keyNameToScancodeMappings["rgui "] = SDL_SCANCODE_RGUI;
		keyNameToScancodeMappings["audionext"] = SDL_SCANCODE_AUDIONEXT;
		keyNameToScancodeMappings["audioprev"] = SDL_SCANCODE_AUDIOPREV;
		keyNameToScancodeMappings["audiostop"] = SDL_SCANCODE_AUDIOSTOP;
		keyNameToScancodeMappings["audioplay"] = SDL_SCANCODE_AUDIOPLAY;
		keyNameToScancodeMappings["audiomute"] = SDL_SCANCODE_AUDIOMUTE;
		keyNameToScancodeMappings["mediaselect"] = SDL_SCANCODE_MEDIASELECT;
		keyNameToScancodeMappings["www"] = SDL_SCANCODE_WWW;
		keyNameToScancodeMappings["mail"] = SDL_SCANCODE_MAIL;
		keyNameToScancodeMappings["calculator"] = SDL_SCANCODE_CALCULATOR;
		keyNameToScancodeMappings["computer"] = SDL_SCANCODE_COMPUTER;
		keyNameToScancodeMappings["ac_search"] = SDL_SCANCODE_AC_SEARCH;
		keyNameToScancodeMappings["ac_home"] = SDL_SCANCODE_AC_HOME;
		keyNameToScancodeMappings["ac_back"] = SDL_SCANCODE_AC_BACK;
		keyNameToScancodeMappings["ac_forward"] = SDL_SCANCODE_AC_FORWARD;
		keyNameToScancodeMappings["ac_stop"] = SDL_SCANCODE_AC_STOP;
		keyNameToScancodeMappings["ac_refresh"] = SDL_SCANCODE_AC_REFRESH;
		keyNameToScancodeMappings["ac_bookmarks"] = SDL_SCANCODE_AC_BOOKMARKS;
		keyNameToScancodeMappings["brightnessdown"] = SDL_SCANCODE_BRIGHTNESSDOWN;
		keyNameToScancodeMappings["brightnessup"] = SDL_SCANCODE_BRIGHTNESSUP;
		keyNameToScancodeMappings["displayswitch"] = SDL_SCANCODE_DISPLAYSWITCH;
		keyNameToScancodeMappings["kbdillumtoggle"] = SDL_SCANCODE_KBDILLUMTOGGLE;
		keyNameToScancodeMappings["kbdillumdown"] = SDL_SCANCODE_KBDILLUMDOWN;
		keyNameToScancodeMappings["kbdillumup"] = SDL_SCANCODE_KBDILLUMUP;
		keyNameToScancodeMappings["eject"] = SDL_SCANCODE_EJECT;
		keyNameToScancodeMappings["sleep"] = SDL_SCANCODE_SLEEP;
		keyNameToScancodeMappings["app1"] = SDL_SCANCODE_APP1;
		keyNameToScancodeMappings["app2"] = SDL_SCANCODE_APP2;
		keyNameToScancodeMappings["audiorewind"] = SDL_SCANCODE_AUDIOREWIND;
		keyNameToScancodeMappings["audiofastforward"] = SDL_SCANCODE_AUDIOFASTFORWARD;
	}
}

void KeyBindings::mapKey(const string& keyName, const string& actionName)
{
	if (keyNameToScancodeMappings.contains(keyName))
	{
		auto scanCode = keyNameToScancodeMappings.at(keyName);

		bindings[actionName] = scanCode;
	}
	else
	{
		clog << "WARNING: Key name " + keyName + " was not found." << endl;
	}
}

int KeyBindings::getKeyCodeFromAction(const std::string& actionName) const
{
	if (bindings.contains(actionName))
		return bindings.at(actionName);

	clog << "WARNING: Key action name " + actionName + " was not found." << endl;

	return -1;
}

InputManager::InputManager(const KeyBindings& keyBindings) : initialized(false), keyBindings(keyBindings)
{
}

//initialize the input manager
bool InputManager::initialize()
{
	return true;
}

bool InputManager::keyDown(const std::string& action)
{
	auto keyCode = keyBindings.getKeyCodeFromAction(action);

	const Uint8 *keys = SDL_GetKeyboardState(NULL);

	if (keys[keyCode])
	{
		return true;
	}

	return false;
}

int InputManager::mouseButtonDown(Vector2<double> * pPos)
{
	Uint32 keys;
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
	Uint32 keys;
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

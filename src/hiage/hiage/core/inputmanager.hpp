#pragma once

#include "../core/typedefs.h"
#include "../util/vector2.h"

#include <unordered_map>
#include <string>

namespace hiage
{
	class KeyBindings
	{
	private:
		static std::unordered_map<std::string, int> keyNameToScancodeMappings;  // key name => scancode
		std::unordered_map<std::string, int> bindings;							// scan code => action

	public:
		KeyBindings();
		void mapKey(const std::string& keyName, const std::string& actionName);
		int getKeyCodeFromAction(const std::string& actionName) const;
	};

	//class that handles input (mouse/keyboard/joystick etc.)
	class __IMPORTEXPORT InputManager
	{
	private:
		KeyBindings keyBindings;

	public:
		InputManager(const KeyBindings& keyBindings);

		bool initialize();

		bool keyDown(const std::string& action);

		int mouseButtonDown(Vector2<double> * pos = 0);	//returns 1 for left, 2 for right button etc.
		bool mouseButtonDown(int button, Vector2<double> * pos = 0);

		Vector2<double> getMousePosition();

	private:
		bool initialized;
	};
}

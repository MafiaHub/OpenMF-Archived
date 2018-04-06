#include "base_input_manager.hpp"
#include <algorithm>

void MFInput::InputManager::removeKeyCallback(std::shared_ptr<KeyInputCallback> callback)
{
	mKeyCallbacks.erase(std::remove(mKeyCallbacks.begin(), mKeyCallbacks.end(), callback), mKeyCallbacks.end());
}

void MFInput::InputManager::removeButtonCallback(std::shared_ptr<ButtonInputCallback> callback)
{
	mButtonCallbacks.erase(std::remove(mButtonCallbacks.begin(), mButtonCallbacks.end(), callback), mButtonCallbacks.end());
}

void MFInput::InputManager::removeWindowResizeCallback(std::shared_ptr<WindowResizeCallback> callback)
{
	mWindowResizeCallbacks.erase(std::remove(mWindowResizeCallbacks.begin(), mWindowResizeCallbacks.end(), callback), mWindowResizeCallbacks.end());
}

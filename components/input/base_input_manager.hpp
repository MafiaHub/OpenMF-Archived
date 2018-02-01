#ifndef INPUT_MANAGER_BASE_H
#define INPUT_MANAGER_BASE_H

#include <memory>
#include <vector>

namespace MFInput
{

class KeyInputCallback
{
public:
    virtual void call(bool down, unsigned int keyCode)=0;
};

class ButtonInputCallback
{
public:
    virtual void call(bool down, unsigned int buttonNumber, unsigned int x, unsigned int y)=0;
};

class WindowResizeCallback
{
public:
    virtual void call(unsigned int width, unsigned int height)=0;
};

/**
  Manages the input handling and the game window.
*/

class InputManager
{
public:
    virtual ~InputManager() {};

    virtual void initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y)=0;
    virtual void destroyWindow()=0;
    virtual bool windowClosed()=0;
    virtual void getWindowSize(unsigned int &width, unsigned int &height)=0;
    virtual bool keyPressed(unsigned int keyCode)=0;
    virtual void setMousePosition(unsigned int x, unsigned int y)=0;
    virtual void getMousePosition(unsigned int &x, unsigned int &y)=0;
    virtual void setMouseVisible(bool visible)=0;                         // TODO: maybe rename to setCursorVisible?
    virtual bool mouseButtonPressed(unsigned int button)=0;
    virtual void processEvents()=0;

    void addKeyCallback(std::shared_ptr<KeyInputCallback> callback)               { mKeyCallbacks.push_back(callback);          };
    void addButtonCallback(std::shared_ptr<ButtonInputCallback> callback)         { mButtonCallbacks.push_back(callback);       };
    void addWindowResizeCallback(std::shared_ptr<WindowResizeCallback> callback)  { mWindowResizeCallbacks.push_back(callback); };

protected:
    std::vector<std::shared_ptr<KeyInputCallback>>      mKeyCallbacks;
    std::vector<std::shared_ptr<ButtonInputCallback>>   mButtonCallbacks;
    std::vector<std::shared_ptr<WindowResizeCallback>>  mWindowResizeCallbacks;
};

}

#endif

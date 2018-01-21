#ifndef INPUT_MANAGER_BASE_H
#define INPUT_MANAGER_BASE_H

namespace MFInput
{

/**
  Manages the input handling and the game window.
*/

class InputManager
{
public:
    virtual void initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y)=0;
    virtual void destroyWindow()=0;
    virtual bool windowClosed()=0;
    virtual void getWindowSize(unsigned int &width, unsigned int &height)=0;
    virtual bool keyPressed(unsigned int keyCode)=0;
    virtual void setMousePosition(unsigned int x, unsigned int y)=0;
    virtual void getMousePosition(unsigned int &x, unsigned int &y)=0;
    virtual void setMouseVisible(bool visible)=0;
    virtual bool mouseButtonPressed(unsigned int button)=0;
    virtual void processEvents()=0;
};

}

#endif

#ifndef INPUT_MANAGER_IMPLEMENTATION_H
#define INPUT_MANAGER_IMPLEMENTATION_H

#include <input/base_input_manager.hpp>
#include <utils/logger.hpp>
#include <SDL2/SDL.h>
#include <string>
#include <vector>
#include <sdl_graphics_window.hpp>

#define INPUT_MANAGER_MODULE_STRING "input manager"
#define NUMBER_OF_KEYS 1024
#define NUMBER_OF_MOUSE_BUTTONS 4

namespace MFInput
{

class InputManagerImplementation: public InputManager
{
public:
    InputManagerImplementation();
    virtual void initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y) override;
    virtual void destroyWindow() override;
    virtual bool windowClosed() override;
    virtual void getWindowSize(unsigned int &width, unsigned int &height) override;
    virtual bool keyPressed(unsigned int keyCode) override;
    virtual void setCursorPosition(unsigned int x, unsigned int y) override;
    virtual void getCursorPosition(unsigned int &x, unsigned int &y) override;
    virtual void setCursorVisible(bool visible) override;
    virtual bool mouseButtonPressed(unsigned int button) override;
    virtual void processEvents() override;
    virtual osgViewer::GraphicsWindow *getWindow();

protected:
    SDL_Window *mWindow;
    unsigned int mWindowWidth;
    unsigned int mWindowHeight;
    osg::ref_ptr<SDLUtil::GraphicsWindowSDL2> mOSGWindow;

    bool mClosed;
    bool mKeyboardState[NUMBER_OF_KEYS];
    bool mMouseState[NUMBER_OF_MOUSE_BUTTONS];
};

}

#endif

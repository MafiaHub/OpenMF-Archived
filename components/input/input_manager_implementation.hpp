#ifndef INPUT_MANAGER_IMPLEMENTATION_H
#define INPUT_MANAGER_IMPLEMENTATION_H

#include <input/base_input_manager.hpp>
#include <loggers/console.hpp>
#include <SDL2/SDL.h>
#include <string>
#include <sdl_graphics_window.hpp>

#define INPUT_MANAGER_MODULE_STRING "input manager"
#define NUMBER_OF_KEYS 1024

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
    virtual bool mouseButtonPressed(unsigned int button) override;
    virtual void processEvents() override;

    SDLUtil::GraphicsWindowSDL2 *getWindow() { return mOSGWindow.get(); };

protected:
    SDL_Window *mWindow;
    unsigned int mWindowWidth;
    unsigned int mWindowHeight;
    osg::ref_ptr<SDLUtil::GraphicsWindowSDL2> mOSGWindow;

    bool mClosed;
    bool mKeyboardState[NUMBER_OF_KEYS];
};

InputManagerImplementation::InputManagerImplementation(): InputManager()
{
    mWindow = 0;
    mClosed = false;
    mWindowWidth = 0;
    mWindowHeight = 0;
}

void InputManagerImplementation::getWindowSize(unsigned int &width, unsigned int &height)
{
    width = mWindowWidth;
    height = mWindowHeight;
}

bool InputManagerImplementation::windowClosed()
{
    return mClosed;
}

void InputManagerImplementation::initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
    // taken from https://github.com/OpenMW/openmw/blob/c7f60a6dc87db0f59a064415ba844917a394af78/apps/openmw/engine.cpp#L317

    mWindowWidth = width;
    mWindowHeight = height;

    int pos_x = SDL_WINDOWPOS_CENTERED_DISPLAY(0),
        pos_y = SDL_WINDOWPOS_CENTERED_DISPLAY(0);

    Uint32 flags = SDL_WINDOW_OPENGL|SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE;
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS,0);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 0);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    mWindow = SDL_CreateWindow("OpenMF",pos_x,pos_y,width,height,flags);

    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    SDL_GetWindowPosition(mWindow,&traits->x,&traits->y);
    SDL_GetWindowSize(mWindow,&traits->width,&traits->height);

    traits->windowName = SDL_GetWindowTitle(mWindow);
    traits->windowDecoration = !(SDL_GetWindowFlags(mWindow)&SDL_WINDOW_BORDERLESS);
    traits->screenNum = SDL_GetWindowDisplayIndex(mWindow);
    traits->red = 8;
    traits->green = 8;
    traits->blue = 8;
    traits->alpha = 0;
    traits->depth = 24;
    traits->stencil = 8;
    traits->vsync = false;
    traits->doubleBuffer = true;
    traits->inheritedWindowData = new SDLUtil::GraphicsWindowSDL2::WindowData(mWindow);

    mOSGWindow = new SDLUtil::GraphicsWindowSDL2(traits);
}

void InputManagerImplementation::destroyWindow()
{
}

void InputManagerImplementation::processEvents()
{
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_KEYDOWN:
            {
                if (event.key.repeat)
                    break;

                unsigned int code = event.key.keysym.scancode;

                if (code < NUMBER_OF_KEYS)
                    mKeyboardState[code] = true;

                break;
            }

            case SDL_KEYUP:
            {
                unsigned int code = event.key.keysym.scancode;

                if (code < NUMBER_OF_KEYS)
                    mKeyboardState[code] = false;

                break;
            }

            case SDL_WINDOWEVENT:
                switch (event.window.event)
                {
                    case SDL_WINDOWEVENT_CLOSE:
                        mClosed = true;
                        break;

                    case SDL_WINDOWEVENT_SIZE_CHANGED:
                        mWindowWidth = event.window.data1;
                        mWindowHeight = event.window.data2;
                        break;

                    default: break;
                }
                break;

            case SDL_QUIT:
                mClosed = true;
                break;

            default:
                break;
        }
    }
}

bool InputManagerImplementation::keyPressed(unsigned int keyCode)
{
    return mKeyboardState[keyCode];
}

bool InputManagerImplementation::mouseButtonPressed(unsigned int button)
{
    return false;
}

}

#endif

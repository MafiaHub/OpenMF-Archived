#ifndef INPUT_MANAGER_IMPLEMENTATION_H
#define INPUT_MANAGER_IMPLEMENTATION_H

#include <input/base_input_manager.hpp>
#include <loggers/console.hpp>
#include <SDL2/SDL.h>
#include <string>

#define INPUT_MANAGER_MODULE_STRING "input manager"

namespace MFInput
{

class InputManagerImplementation: public InputManager
{
public:
    InputManagerImplementation();
    virtual void initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y) override;
    virtual void destroyWindow() override;
    virtual bool keyPressed(unsigned int keyCode) override;
    virtual bool mouseButtonPressed(unsigned int button) override;
    virtual void processEvents() override;

protected:
    SDL_Window *mWindow;
    SDL_Surface *mSurface;

    std::string getSDLError() { std::string s(SDL_GetError()); return s; };
};

InputManagerImplementation::InputManagerImplementation(): InputManager()
{
    mWindow = 0;
    mSurface = 0;
}

void InputManagerImplementation::initWindow(unsigned int width, unsigned int height, unsigned int x, unsigned int y)
{
    MFLogger::ConsoleLogger::info("Initializing window.",INPUT_MANAGER_MODULE_STRING);

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        MFLogger::ConsoleLogger::fatal("Could not initialize SDL, error: " + getSDLError() + ".");
        return;
    }

    mWindow = SDL_CreateWindow("OpenMF",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,width,height,SDL_WINDOW_SHOWN);

    if (!mWindow)
    {
        MFLogger::ConsoleLogger::fatal("Could not initialize SDL window, error: " + getSDLError() + ".");
        return;
    }
 
    mSurface = SDL_GetWindowSurface(mWindow);
}

void InputManagerImplementation::destroyWindow()
{
    SDL_DestroyWindow(mWindow);
    SDL_Quit();
}

void InputManagerImplementation::processEvents()
{
}

bool InputManagerImplementation::keyPressed(unsigned int keyCode)
{
    return false;
}

bool InputManagerImplementation::mouseButtonPressed(unsigned int button)
{
    return false;
}

}

#endif

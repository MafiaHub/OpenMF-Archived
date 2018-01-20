#ifndef OPENMW_COMPONENTS_SDLUTIL_SDLGRAPHICSWINDOW_H
#define OPENMW_COMPONENTS_SDLUTIL_SDLGRAPHICSWINDOW_H

/* This file has been forked from OpenMW. */

#include <SDL2/SDL_video.h>
#include <osgViewer/GraphicsWindow>
#include <osg/DeleteHandler>
#include <osg/Version>

namespace SDLUtil
{

class GraphicsWindowSDL2 : public osgViewer::GraphicsWindow
{
    SDL_Window*     mWindow;
    SDL_GLContext   mContext;

    bool            mValid;
    bool            mRealized;
    bool            mOwnsWindow;

    void init();

    virtual ~GraphicsWindowSDL2();

public:
    GraphicsWindowSDL2(osg::GraphicsContext::Traits *traits);

    virtual bool isSameKindAs(const Object* object) const { return dynamic_cast<const GraphicsWindowSDL2*>(object)!=0; }
    virtual const char* libraryName() const { return "osgViewer"; }
    virtual const char* className() const { return "GraphicsWindowSDL2"; }

    virtual bool valid() const { return mValid; }

    /** Realise the GraphicsContext.*/
    virtual bool realizeImplementation();

    /** Return true if the graphics context has been realised and is ready to use.*/
    virtual bool isRealizedImplementation() const { return mRealized; }

    /** Close the graphics context.*/
    virtual void closeImplementation();

    /** Make this graphics context current.*/
    virtual bool makeCurrentImplementation();

    /** Release the graphics context.*/
    virtual bool releaseContextImplementation();

    /** Swap the front and back buffers.*/
    virtual void swapBuffersImplementation();

    /** Set sync-to-vblank. */
    virtual void setSyncToVBlank(bool on);

    /** Set Window decoration.*/
    virtual bool setWindowDecorationImplementation(bool flag);

    /** Raise specified window */
    virtual void raiseWindow();

    /** Set the window's position and size.*/
    virtual bool setWindowRectangleImplementation(int x, int y, int width, int height);

    /** Set the name of the window */
    virtual void setWindowName(const std::string &name);

    /** Set mouse cursor to a specific shape.*/
    virtual void setCursor(MouseCursor cursor);

    /** Get focus.*/
    virtual void grabFocus() {}

    /** Get focus on if the pointer is in this window.*/
    virtual void grabFocusIfPointerInWindow() {}

    /** WindowData is used to pass in the SDL2 window handle attached to the GraphicsContext::Traits structure. */
    struct WindowData : public osg::Referenced
    {
        WindowData(SDL_Window *window) : mWindow(window)
        { }

        SDL_Window *mWindow;
    };
};

GraphicsWindowSDL2::~GraphicsWindowSDL2()
{
    close(true);
}

GraphicsWindowSDL2::GraphicsWindowSDL2(osg::GraphicsContext::Traits *traits)
    : mWindow(0)
    , mContext(0)
    , mValid(false)
    , mRealized(false)
    , mOwnsWindow(false)
{
    _traits = traits;

    init();
    if(valid())
    {
        setState(new osg::State);
        getState()->setGraphicsContext(this);

        if(_traits.valid() && _traits->sharedContext.valid())
        {
            getState()->setContextID(_traits->sharedContext->getState()->getContextID());
            incrementContextIDUsageCount(getState()->getContextID());
        }
        else
        {
            getState()->setContextID(osg::GraphicsContext::createNewContextID());
        }
    }
}


bool GraphicsWindowSDL2::setWindowDecorationImplementation(bool flag)
{
    if(!mWindow) return false;

    SDL_SetWindowBordered(mWindow, flag ? SDL_TRUE : SDL_FALSE);
    return true;
}

bool GraphicsWindowSDL2::setWindowRectangleImplementation(int x, int y, int width, int height)
{
    if(!mWindow) return false;

    SDL_SetWindowPosition(mWindow, x, y);
    SDL_SetWindowSize(mWindow, width, height);
    return true;
}

void GraphicsWindowSDL2::setWindowName(const std::string &name)
{
    if(!mWindow) return;

    SDL_SetWindowTitle(mWindow, name.c_str());
    _traits->windowName = name;
}

void GraphicsWindowSDL2::setCursor(MouseCursor mouseCursor)
{
    _traits->useCursor = false;
}


void GraphicsWindowSDL2::init()
{
    if(mValid) return;

    if(!_traits.valid())
        return;

    // getEventQueue()->setCurrentEventState(osgGA::GUIEventAdapter::getAccumulatedEventState().get());

    WindowData *inheritedWindowData = dynamic_cast<WindowData*>(_traits->inheritedWindowData.get());
    mWindow = inheritedWindowData ? inheritedWindowData->mWindow : NULL;

    mOwnsWindow = (mWindow == 0);
    if(mOwnsWindow)
    {
        OSG_NOTICE<<"Error: No SDL window provided."<<std::endl;
        return;
    }

    // SDL will change the current context when it creates a new one, so we
    // have to get the current one to be able to restore it afterward.
    SDL_Window *oldWin = SDL_GL_GetCurrentWindow();
    SDL_GLContext oldCtx = SDL_GL_GetCurrentContext();

    mContext = SDL_GL_CreateContext(mWindow);
    if(!mContext)
    {
        OSG_NOTICE<< "Error: Unable to create OpenGL graphics context: "<<SDL_GetError() <<std::endl;
        return;
    }

    SDL_GL_SetSwapInterval(_traits->vsync ? 1 : 0);

    SDL_GL_MakeCurrent(oldWin, oldCtx);

    mValid = true;

#if OSG_MIN_VERSION_REQUIRED(3,3,4)
    getEventQueue()->syncWindowRectangleWithGraphicsContext();
#else
    getEventQueue()->syncWindowRectangleWithGraphcisContext();
#endif
}


bool GraphicsWindowSDL2::realizeImplementation()
{
    if(mRealized)
    {
        OSG_NOTICE<< "GraphicsWindowSDL2::realizeImplementation() Already realized" <<std::endl;
        return true;
    }

    if(!mValid) init();
    if(!mValid) return false;

    SDL_ShowWindow(mWindow);

#if OSG_MIN_VERSION_REQUIRED(3,3,4)
    getEventQueue()->syncWindowRectangleWithGraphicsContext();
#else
    getEventQueue()->syncWindowRectangleWithGraphcisContext();
#endif

    mRealized = true;

    return true;
}

bool GraphicsWindowSDL2::makeCurrentImplementation()
{
    if(!mRealized)
    {
        OSG_NOTICE<<"Warning: GraphicsWindow not realized, cannot do makeCurrent."<<std::endl;
        return false;
    }

    return SDL_GL_MakeCurrent(mWindow, mContext)==0;
}

bool GraphicsWindowSDL2::releaseContextImplementation()
{
    if(!mRealized)
    {
        OSG_NOTICE<< "Warning: GraphicsWindow not realized, cannot do releaseContext." <<std::endl;
        return false;
    }

    return SDL_GL_MakeCurrent(NULL, NULL)==0;
}


void GraphicsWindowSDL2::closeImplementation()
{
    // OSG_NOTICE<<"Closing GraphicsWindowSDL2"<<std::endl;

    if(mContext)
        SDL_GL_DeleteContext(mContext);
    mContext = NULL;

    if(mWindow && mOwnsWindow)
        SDL_DestroyWindow(mWindow);
    mWindow = NULL;

    mValid = false;
    mRealized = false;
}

void GraphicsWindowSDL2::swapBuffersImplementation()
{
    if(!mRealized) return;

    //OSG_NOTICE<< "swapBuffersImplementation "<<this<<" "<<OpenThreads::Thread::CurrentThread()<<std::endl;

    SDL_GL_SwapWindow(mWindow);
}

void GraphicsWindowSDL2::setSyncToVBlank(bool on)
{
    SDL_Window *oldWin = SDL_GL_GetCurrentWindow();
    SDL_GLContext oldCtx = SDL_GL_GetCurrentContext();

    SDL_GL_MakeCurrent(mWindow, mContext);

    SDL_GL_SetSwapInterval(on ? 1 : 0);

    SDL_GL_MakeCurrent(oldWin, oldCtx);
}

void GraphicsWindowSDL2::raiseWindow()
{
    SDL_RaiseWindow(mWindow);
}

}

#endif /* OSGGRAPHICSWINDOW_H */

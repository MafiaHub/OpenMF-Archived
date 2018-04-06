#include "ImGuiHandler.hpp"

#include "imgui/imgui.h"

#include <iostream>

#include <osgUtil/GLObjectsVisitor>
#include <osgUtil/SceneView>
#include <osgUtil/UpdateVisitor>

#include <iterator>

// This is the main rendering function that you have to implement and provide to
// ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
// If text or lines are blurry when integrating ImGui in your engine:
// - in your Render function, try translating your projection matrix by
// (0.5f,0.5f) or (0.375f,0.375f)
void
ImGui_RenderDrawLists(ImDrawData* draw_data)
{
    // We are using the OpenGL fixed pipeline to make the example code simpler to
    // read!
    // Setup render state: alpha-blending enabled, no face culling, no depth
    // testing, scissor enabled, vertex/texcoord/color pointers.
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TRANSFORM_BIT);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_SCISSOR_TEST);
    glDisable(GL_LIGHTING);
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glEnable(GL_TEXTURE_2D);
    // glUseProgram(0); // You may want this if using this code in an OpenGL 3+
    // context

    // Handle cases of screen coordinates != from framebuffer coordinates (e.g.
    // retina displays)
    ImGuiIO& io = ImGui::GetIO();
    float    fb_height = io.DisplaySize.y * io.DisplayFramebufferScale.y;
    draw_data->ScaleClipRects(io.DisplayFramebufferScale);

    // Setup orthographic projection matrix
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glOrtho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, -1.0f, +1.0f);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Render command lists
#define OFFSETOF(TYPE, ELEMENT) ((size_t) & (((TYPE*)0)->ELEMENT))
    for (int n = 0; n < draw_data->CmdListsCount; n++) {
        const ImDrawList*    cmd_list = draw_data->CmdLists[n];
        const unsigned char* vtx_buffer =
            (const unsigned char*)&cmd_list->VtxBuffer.front();
        const ImDrawIdx* idx_buffer = &cmd_list->IdxBuffer.front();
        glVertexPointer(2, GL_FLOAT, sizeof(ImDrawVert),
            (void*)(vtx_buffer + OFFSETOF(ImDrawVert, pos)));
        glTexCoordPointer(2, GL_FLOAT, sizeof(ImDrawVert),
            (void*)(vtx_buffer + OFFSETOF(ImDrawVert, uv)));
        glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(ImDrawVert),
            (void*)(vtx_buffer + OFFSETOF(ImDrawVert, col)));

        for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.size(); cmd_i++) {
            const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
            if (pcmd->UserCallback) {
                pcmd->UserCallback(cmd_list, pcmd);
            }
            else {
                glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
                glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w),
                    (int)(pcmd->ClipRect.z - pcmd->ClipRect.x),
                    (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
                glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount,
                    GL_UNSIGNED_SHORT, idx_buffer);
            }
            idx_buffer += pcmd->ElemCount;
        }
    }
#undef OFFSETOF

    // Restore modified state
    //    glDisableClientState(GL_COLOR_ARRAY);
    //    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    //    glDisableClientState(GL_VERTEX_ARRAY);
    glBindTexture(GL_TEXTURE_2D, last_texture);
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glPopAttrib();
}

struct ImGuiNewFrameCallback : public osg::Camera::DrawCallback
{

    ImGuiNewFrameCallback(ImGuiHandler& theHandler)
        : m_handler(theHandler)
    {
    }

    virtual void operator()(osg::RenderInfo& theRenderInfo) const
    {
        m_handler.newFrame(theRenderInfo);
    }

private:
    ImGuiHandler & m_handler;
};

struct ImGuiDrawCallback : public osg::Camera::DrawCallback
{

    ImGuiDrawCallback(ImGuiHandler& theHandler)
        : m_handler(theHandler)
    {
    }

    virtual void operator()(osg::RenderInfo& theRenderInfo) const
    {
        m_handler.render(theRenderInfo);
    }

private:
    ImGuiHandler & m_handler;
};

ImGuiHandler::ImGuiHandler(GuiCallback* theGuicallback)
    : m_callback(theGuicallback)
    , g_Time(0.0f)
    , g_MousePressed{ false }
    , g_MouseWheel(0.0f)
    , g_FontTexture(0)
{
}

//////////////////////////////////////////////////////////////////////////////
// Imporant Note: Dear ImGui expects the control Keys indices not to be	    //
// greater thant 511. It actually uses an array of 512 elements. However,   //
// OSG has indices greater than that. So here I do a conversion for special //
// keys between ImGui and OSG.						    //
//////////////////////////////////////////////////////////////////////////////

/**
* Special keys that are usually greater than 512 in OSGga
**/
enum ConvertedKey : int
{
    ConvertedKey_Tab = 257,
    ConvertedKey_Left,
    ConvertedKey_Right,
    ConvertedKey_Up,
    ConvertedKey_Down,
    ConvertedKey_PageUp,
    ConvertedKey_PageDown,
    ConvertedKey_Home,
    ConvertedKey_End,
    ConvertedKey_Delete,
    ConvertedKey_BackSpace,
    ConvertedKey_Enter,
    ConvertedKey_Escape,
    // Modifiers
    ConvertedKey_LeftControl,
    ConvertedKey_RightControl,
    ConvertedKey_LeftShift,
    ConvertedKey_RightShift,
    ConvertedKey_LeftAlt,
    ConvertedKey_RightAlt,
    ConvertedKey_LeftSuper,
    ConvertedKey_RightSuper
};

/**
* Check for a special key and return the converted code (range [257, 511]) if
* so. Otherwise returns -1
*/
static int
ConvertFromOSGKey(int key)
{
    using KEY = osgGA::GUIEventAdapter::KeySymbol;

    switch (key) {
    default: // Not found
        return -1;
    case KEY::KEY_Tab: return ConvertedKey_Tab;
    case KEY::KEY_Left: return ConvertedKey_Left;
    case KEY::KEY_Right: return ConvertedKey_Right;
    case KEY::KEY_Up: return ConvertedKey_Up;
    case KEY::KEY_Down: return ConvertedKey_Down;
    case KEY::KEY_Page_Up: return ConvertedKey_PageUp;
    case KEY::KEY_Page_Down: return ConvertedKey_PageDown;
    case KEY::KEY_Home: return ConvertedKey_Home;
    case KEY::KEY_End: return ConvertedKey_End;
    case KEY::KEY_Delete: return ConvertedKey_Delete;
    case KEY::KEY_BackSpace: return ConvertedKey_BackSpace;
    case KEY::KEY_Return: return ConvertedKey_Enter;
    case KEY::KEY_Escape: return ConvertedKey_Escape;

    case KEY::KEY_Control_L: return ConvertedKey_LeftControl;
    case KEY::KEY_Control_R: return ConvertedKey_RightControl;
    case KEY::KEY_Shift_L: return ConvertedKey_LeftShift;
    case KEY::KEY_Shift_R: return ConvertedKey_RightShift;
    case KEY::KEY_Alt_L: return ConvertedKey_LeftAlt;
    case KEY::KEY_Alt_R: return ConvertedKey_RightAlt;
    case KEY::KEY_Super_L: return ConvertedKey_LeftSuper;
    case KEY::KEY_Super_R: return ConvertedKey_RightSuper;
    }
    assert(false && "Switch has a default case");
    return -1;
}

void
ImGuiHandler::init()
{
    ImGui::CreateContext();
    ImGui::StyleColorsDark();
    ImGuiIO& io = ImGui::GetIO();

    // Keyboard mapping. ImGui will use those indices to peek into the
    // io.KeyDown[] array.
    io.KeyMap[ImGuiKey_Tab] = ConvertedKey_Tab;
    io.KeyMap[ImGuiKey_LeftArrow] = ConvertedKey_Left;
    io.KeyMap[ImGuiKey_RightArrow] = ConvertedKey_Right;
    io.KeyMap[ImGuiKey_UpArrow] = ConvertedKey_Up;
    io.KeyMap[ImGuiKey_DownArrow] = ConvertedKey_Down;
    io.KeyMap[ImGuiKey_PageUp] = ConvertedKey_PageUp;
    io.KeyMap[ImGuiKey_PageDown] = ConvertedKey_PageDown;
    io.KeyMap[ImGuiKey_Home] = ConvertedKey_Home;
    io.KeyMap[ImGuiKey_End] = ConvertedKey_End;
    io.KeyMap[ImGuiKey_Delete] = ConvertedKey_Delete;
    io.KeyMap[ImGuiKey_Backspace] = ConvertedKey_BackSpace;
    io.KeyMap[ImGuiKey_Enter] = ConvertedKey_Enter;
    io.KeyMap[ImGuiKey_Escape] = ConvertedKey_Escape;
    io.KeyMap[ImGuiKey_A] = osgGA::GUIEventAdapter::KeySymbol::KEY_A;
    io.KeyMap[ImGuiKey_C] = osgGA::GUIEventAdapter::KeySymbol::KEY_C;
    io.KeyMap[ImGuiKey_V] = osgGA::GUIEventAdapter::KeySymbol::KEY_V;
    io.KeyMap[ImGuiKey_X] = osgGA::GUIEventAdapter::KeySymbol::KEY_X;
    io.KeyMap[ImGuiKey_Y] = osgGA::GUIEventAdapter::KeySymbol::KEY_Y;
    io.KeyMap[ImGuiKey_Z] = osgGA::GUIEventAdapter::KeySymbol::KEY_Z;

    // Build texture atlas
    unsigned char* pixels;
    int            width, height;
    io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);

    // Create OpenGL texture
    GLint last_texture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
    glGenTextures(1, &g_FontTexture);
    glBindTexture(GL_TEXTURE_2D, g_FontTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA,
        GL_UNSIGNED_BYTE, pixels);

    // Store our identifier
    io.Fonts->TexID = (void*)(intptr_t)g_FontTexture;

    // Cleanup (don't clear the input data if you want to append new fonts later)
    io.Fonts->ClearInputData();
    io.Fonts->ClearTexData();
    glBindTexture(GL_TEXTURE_2D, last_texture);

    io.RenderDrawListsFn = ImGui_RenderDrawLists;
}

void
ImGuiHandler::setCameraCallbacks(osg::Camera* theCamera)
{

    ImGuiDrawCallback* aPostDrawCallback = new ImGuiDrawCallback(*this);
    theCamera->setPostDrawCallback(aPostDrawCallback);

    ImGuiNewFrameCallback* aPreDrawCallback = new ImGuiNewFrameCallback(*this);
    theCamera->setPreDrawCallback(aPreDrawCallback);
}

void
ImGuiHandler::newFrame(osg::RenderInfo& theRenderInfo)
{
    ImGuiIO& io = ImGui::GetIO();

    osg::Viewport* aViewport = theRenderInfo.getCurrentCamera()->getViewport();
    io.DisplaySize = ImVec2(aViewport->width(), aViewport->height());

    double aCurrentTime =
        theRenderInfo.getView()->getFrameStamp()->getSimulationTime();
    io.DeltaTime =
        g_Time > 0.0 ? (float)(aCurrentTime - g_Time) : (float)(1.0f / 60.0f);
    g_Time = aCurrentTime;

    for (int i = 0; i < 3; i++) {
        io.MouseDown[i] = g_MousePressed[i];
    }

    io.MouseWheel = g_MouseWheel;
    g_MouseWheel = 0.0f;

    ImGui::NewFrame();
}

void
ImGuiHandler::render(osg::RenderInfo& /*theRenderInfo*/)
{

    if (m_callback) {
        (*m_callback)();
    }

    ImGui::Render();
}

bool
ImGuiHandler::handle(const osgGA::GUIEventAdapter& theEventAdapter,
    osgGA::GUIActionAdapter& /*theActionAdapter*/,
    osg::Object* /*theObject*/,
    osg::NodeVisitor* /*theNodeVisitor*/)
{
    static bool isInitialized = false;
    if (!isInitialized) {
        isInitialized = true;
        init();
    }
    const bool wantCapureMouse = ImGui::GetIO().WantCaptureMouse;
    const bool wantCapureKeyboard = ImGui::GetIO().WantCaptureKeyboard;
    ImGuiIO&   io = ImGui::GetIO();

    switch (theEventAdapter.getEventType()) {

    case osgGA::GUIEventAdapter::KEYDOWN: {
        const int c = theEventAdapter.getKey();
        const int special_key = ConvertFromOSGKey(c);
        if (special_key > 0) {
            assert(special_key < 512 && "ImGui KeysDown is an array of 512");
            assert(special_key > 256 &&
                "ASCII stop at 127, but we use the range [257, 511]");

            io.KeysDown[special_key] = true;

            io.KeyCtrl = io.KeysDown[ConvertedKey_LeftControl] ||
                io.KeysDown[ConvertedKey_RightControl];
            io.KeyShift = io.KeysDown[ConvertedKey_LeftShift] ||
                io.KeysDown[ConvertedKey_RightShift];
            io.KeyAlt = io.KeysDown[ConvertedKey_LeftAlt] ||
                io.KeysDown[ConvertedKey_RightAlt];
            io.KeySuper = io.KeysDown[ConvertedKey_LeftSuper] ||
                io.KeysDown[ConvertedKey_RightSuper];
        }
        else if (c > 0 && c < 0x10000) {
            io.AddInputCharacter((unsigned short)c);
        }
        return wantCapureKeyboard;
    }
    case osgGA::GUIEventAdapter::KEYUP: {
        const int c = theEventAdapter.getKey();
        const int special_key = ConvertFromOSGKey(c);
        if (special_key > 0) {
            assert(special_key < 512 && "ImGui KeysMap is an array of 512");
            assert(special_key > 256 &&
                "ASCII stop at 127, but we use the range [257, 511]");

            io.KeysDown[special_key] = false;

            io.KeyCtrl = io.KeysDown[ConvertedKey_LeftControl] ||
                io.KeysDown[ConvertedKey_RightControl];
            io.KeyShift = io.KeysDown[ConvertedKey_LeftShift] ||
                io.KeysDown[ConvertedKey_RightShift];
            io.KeyAlt = io.KeysDown[ConvertedKey_LeftAlt] ||
                io.KeysDown[ConvertedKey_RightAlt];
            io.KeySuper = io.KeysDown[ConvertedKey_LeftSuper] ||
                io.KeysDown[ConvertedKey_RightSuper];
        }
        return wantCapureKeyboard;
    }
    case (osgGA::GUIEventAdapter::PUSH): {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(theEventAdapter.getX(),
            io.DisplaySize.y - theEventAdapter.getY());
        g_MousePressed[0] = true;
        return wantCapureMouse;
    }
    case (osgGA::GUIEventAdapter::DRAG):
    case (osgGA::GUIEventAdapter::MOVE): {
        ImGuiIO& io = ImGui::GetIO();
        io.MousePos = ImVec2(theEventAdapter.getX(),
            io.DisplaySize.y - theEventAdapter.getY());
        return wantCapureMouse;
    }
    case (osgGA::GUIEventAdapter::RELEASE): {
        g_MousePressed[0] = false;
        return wantCapureMouse;
    }
    case (osgGA::GUIEventAdapter::SCROLL): {
        g_MouseWheel = theEventAdapter.getScrollingDeltaY();
        return wantCapureMouse;
    }
    default: {
        return false;
    }
    }

    return false;
}

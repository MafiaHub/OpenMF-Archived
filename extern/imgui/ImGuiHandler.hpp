#ifndef SOLEIL__IMGUIHANDLER_H_
#define SOLEIL__IMGUIHANDLER_H_

#include <iostream>
#include <osg/Camera>
#include <osgViewer/ViewerEventHandlers>

#include <imgui/imgui.h>

class ImGuiHandler : public osgGA::GUIEventHandler
{
public:
    struct GuiCallback : public osg::Referenced
    {
        virtual void operator()()
        {
            // It can also be members
            static bool   show_test_window = true;
            static bool   show_another_window = false;
            static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            static char   text[255] = { 0 };

            {
                static float f = 0.0f;
                ImGui::Text("Hello, world!");
                ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
                ImGui::ColorEdit3("clear color", (float*)&clear_color);
                if (ImGui::Button("Test Window")) show_test_window ^= 1;
                if (ImGui::Button("Another Window")) show_another_window ^= 1;
                ImGui::Text("Application average %.3f ms/frame (%.1f FPS)",
                    1000.0f / ImGui::GetIO().Framerate,
                    ImGui::GetIO().Framerate);
                ImGui::InputText("Hello: ", text, 254);
            }

            // 2. Show another simple window. In most cases you will use an explicit
            // Begin/End pair to name the window.
            if (show_another_window) {
                ImGui::Begin("Another Window", &show_another_window);
                ImGui::Text("Hello from another window!");
                ImGui::End();
            }

            // 3. Show the ImGui test window. Most of the sample code is in
            // ImGui::ShowTestWindow().
            if (show_test_window) {
                ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiCond_FirstUseEver);
                ImGui::ShowTestWindow();
            }

        }
    };

public:
    ImGuiHandler(GuiCallback* theGuicallback);

    void init();

    void newFrame(osg::RenderInfo& theRenderInfo);

    void render(osg::RenderInfo& theRenderInfo);

    void setCameraCallbacks(osg::Camera* theCamera);

    virtual bool handle(const osgGA::GUIEventAdapter& theEventAdapter,
        osgGA::GUIActionAdapter&      theActionAdapter,
        osg::Object* theObject, osg::NodeVisitor* theNodeVisitor);

private:
    osg::ref_ptr<GuiCallback> m_callback;

    double       g_Time;
    bool         g_MousePressed[3];
    float        g_MouseWheel;
    GLuint       g_FontTexture;
};

#endif /* SOLEIL__IMGUIHANDLER_H_ */
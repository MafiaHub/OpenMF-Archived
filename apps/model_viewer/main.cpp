#include <iostream>
#include <fstream>
#include <loggers/console.hpp>
#include <cxxopts.hpp>
#include <renderer/osg_renderer.hpp>
#include <string.h>
#include <stdlib.h>

#define DEFAULT_CAMERA_SPEED 7.0

std::string getCameraString(MFRender::MFRenderer *renderer)
{
    double cam[6];
    renderer->getCameraPositionRotation(cam[0],cam[1],cam[2],cam[3],cam[4],cam[5]);

    std::string camStr;

    bool first = true;

    for (int i = 0; i < 6; ++i)
    {
        if (first)
            first = false;
        else
            camStr += ",";

        camStr += std::to_string(cam[i]);
    }

    return camStr;
}

void parseCameraString(std::string str, double params[6])
{
    char *token;
    char cStr[256];
    char delims[] = ",; ";

    strncpy(cStr,str.c_str(),256);

    token = strtok(cStr,delims);

    for (int i = 0; i < 6; ++i)
    {
        params[i] = atof(token);
        token = strtok(NULL,delims);
    } 
}

int main(int argc, char** argv)
{
    cxxopts::Options options("viewer","3D viewer for Mafia 4DS files.");

    options.add_options()
        ("h,help","Display help and exit.")
        ("i,input","Specify input, mission name by default.",cxxopts::value<std::string>())
        ("4,4ds","Load single 4ds model instead of mission.")
        ("f,fov","Specify camera field of view in degrees.",cxxopts::value<int>())
        ("s,camera-speed","Set camera speed (default is " + std::to_string(DEFAULT_CAMERA_SPEED) +  ").",cxxopts::value<double>())
        ("c,camera-info","Write camera position and rotation in console.")
		("b,base-dir", "Specify base game directory.", cxxopts::value<std::string>())
        ("p,place-camera","Place camera at position X,Y,Z,YAW,PITCH,ROLL.",cxxopts::value<std::string>());

    options.parse_positional({"i"});
    auto arguments = options.parse(argc,argv);

    bool cameraInfo = arguments.count("c") > 0;
    bool cameraPlace = arguments.count("p") > 0;
    bool model = arguments.count("4") > 0;

    double cameraSpeed = DEFAULT_CAMERA_SPEED;

    if (arguments.count("s") > 0)
        cameraSpeed = arguments["s"].as<double>();

    if (arguments.count("h") > 0)
    {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (arguments.count("i") < 1)
    {
        MFLogger::ConsoleLogger::fatal("Expected file.");
        std::cout << options.help() << std::endl;
        return 1;
    }

    int fov = 75;

    if (arguments.count("f") > 0)
        fov = arguments["f"].as<int>();

	if (arguments.count("b") > 0)
		MFFile::FileSystem::getInstance()->addPath(arguments["b"].as<std::string>());

    std::string inputFile = arguments["i"].as<std::string>();

    MFRender::OSGRenderer renderer;

    if (model)
        renderer.loadSingleModel(inputFile);
    else
        renderer.loadMission(inputFile);

    renderer.setCameraParameters(true,fov,0,0.01,100000);
    renderer.setFreeCameraSpeed(cameraSpeed);

    if (cameraPlace)
    {
        double cam[6];
        parseCameraString(arguments["p"].as<std::string>(),cam);
        renderer.setCameraPositionRotation(cam[0],cam[1],cam[2],cam[3],cam[4],cam[5]);
    }

    int infoCounter = 0;

    while (!renderer.done())
    {
        if (cameraInfo)
        {
            if (infoCounter <= 0)
            {
                MFLogger::ConsoleLogger::info("camera: " + getCameraString(&renderer));
                infoCounter = 30;
            }
            infoCounter--;
        }

        renderer.frame();
    }

    return 0;
}

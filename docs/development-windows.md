# Developing on Windows

Considering the fact that workspace setup on Windows is slightly more involving, we've decided to dedicate a whole section to the process.

## Prerequisites

We recommend using Visual Studio 2017 and CMake (with GUI) to simplify the whole setup process. Other toolchains (e.g. MSYS2, MinGW, ...) might not be supported, so be advised.

Intermediate knowledge of CMake build system is required, use of CMake GUI can simplify the process.

## Dependencies

Our project currently relies on dependencies that we need to deal with first.

### OpenSceneGraph

Let's start by fetching OSG's dependencies first.

1. Head over to the [OSG download page](http://www.openscenegraph.org/index.php/download-section/dependencies) and download either the small or a full pre-built dependencies package.
2. Extract it to a convenient location (e.g. `W:\3rdparty`.)

Afterwards, we need to clone/download our fork of OpenSceneGraph.

1. Clone/Download [our fork](https://github.com/OpenMafia/osg) to a convenient location (e.g. `W:\osg`.)
2. Navigate to our newly designated folder and create a build directory inside of it.
3. Use CMake to generate projects.
4. Open the solution inside Visual Studio and compile the project (can take up to 30-60 minutes!)

OR

You can also download our pre-compiled binaries for OpenSceneGraph, built using VS 2017 Win64 with Release configuration.

1. Download our pre-compiled binaries from [dropbox link](https://www.dropbox.com/s/5lxzkm7ip6lixd3/omf_osg_vs2017_win64_rel.zip?dl=0)
2. Extract it to a directory accessible by our build system (e.g. `W:\osg\build`.)

### Bullet3

It's quite straightforward to build our bullet3 dependency.

1. Clone/Download [our fork](https://github.com/OpenMafia/bullet3) to a convenient location (e.g. `W:\bullet3`.)
2. Navigate to our newly designated folder and create a build directory inside of it.
3. Use CMake to generate projects.
4. Open the solution inside Visual Studio and compile the project.

OR

You can also download our pre-compiled binaries for Bullet3, built using VS 2017 Win64.

1. Download our pre-compiled binaries from [dropbox link](https://www.dropbox.com/s/b4m39h93j5cgku1/omf_bullet3_vs2017_win64.zip?dl=0)
2. Extract it to a directory accessible by our build system (e.g. `W:\bullet3\build`.)

### SDL2

It is enough to download development libraries for Visual Studio from the official website.

1. Download [Development Libraries for Visual C++ 32/64-bit](https://www.libsdl.org/download-2.0.php) from the official website.
2. Extract it to a convenient location (e.g. `W:\sdl2`).

You can also follow the instructions from previous dependencies to build your own libraries in a similar fashion.


## Compiling

Once we get all our dependencies compiled, we're finally ready to compile OpenMF.

1. Clone/Download [this repository](https://github.com/OpenMafia/OpenMF/) to a convenient location (e.g. `W:\openmf`.)
2. Navigate to our newly designated folder and create a build directory inside of it.
3. To successfully generate project files, we need to provide the paths to our dependencies (If using command-line CMake, to define a variable, use `-D<NAME>=<VALUE>` format, otherwise rely on GUI prompts.):
   * `OSG_THIRD_PARTY_DIR`: points to our `W:\3rdparty` directory.
   * `OSG_DIR`: points to our `W:\osg` directory.
   * `BULLET_ROOT`: points to our `W:\bullet3\build` directory.
   * `SDL2_ROOT`: points to our `W:\sdl2` directory.
   * You also need to specify your build targets, see `build targets` in [development](https://github.com/OpenMafia/OpenMF/tree/master/docs/development.md):
        - Use them by defining such specific variable. (e.g. `cmake .. -DBUILD_GAME=1 -DBUILD_UTILS=1`)
        - On CMake GUI, use `Add Entry` button to define a variable, call it `BUILD_<NAME>` (e.g. `BUILD_GAME`) and set its boolean value to True.
4. Having done that, we can finally build our project!
5. Refer back to our [development](https://github.com/OpenMafia/OpenMF/tree/master/docs/development.md) for more instructions on how to make use of our newly compiled binaries.
6. We recommend setting up environment variables that point to built binaries of the dependencies, this way, you don't need to copy DLLs each time you set up the workspace.

# Development on Windows

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

### Bullet3

It's quite straightforward to build our bullet3 dependency.

1. Clone/Download [our fork](https://github.com/OpenMafia/bullet3) to a convenient location (e.g. `W:\bullet3`.)
2. Navigate to our newly designated folder and create a build directory inside of it.
3. Use CMake to generate projects.
4. Open the solution inside Visual Studio and compile the project.


## Compiling

Once we get all our dependencies compiled, we're finally ready to compile OpenMF.

1. Clone/Download [this repository](https://github.com/OpenMafia/OpenMF/) to a convenient location (e.g. `W:\openmf`.)
2. Navigate to our newly designated folder and create a build directory inside of it.
3. To successfully generate project files, we need to provide the paths to our dependencies (If using command-line CMake, to define a variable, use `-D<NAME>=<VALUE>` format, otherwise rely on GUI prompts.):
   * `OSG_THIRD_PARTY_DIR`: points to our `W:\3rdparty` directory.
   * `OSG_DIR`: points to our `W:\osg` directory.
   * `BULLET_ROOT`: points to our `W:\bullet3` directory.
4. Having done that, we can finally build our project!
5. Refer back to our [development](https://github.com/OpenMafia/OpenMF/tree/master/docs/development.md) for more instructions on how to make use of our newly compiled binaries.

#include <load_def/loaddef_parser.hpp>

namespace MFFormat
{

bool DataFormatLoadDEF::load(std::ifstream &srcFile)
{
    while (srcFile)
    {
        LoadingScreen loadingScreen = {};
        read(srcFile, &loadingScreen);

        if (srcFile)
            mLoadingScreens.push_back(loadingScreen);
    }

    return true;
}

}

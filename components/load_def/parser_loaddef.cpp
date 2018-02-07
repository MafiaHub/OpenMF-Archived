#include <load_def/parser_loaddef.hpp>

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

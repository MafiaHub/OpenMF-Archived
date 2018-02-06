#ifndef FORMAT_PARSERS_MNU_H
#define FORMAT_PARSERS_MNU_H

#include <base_parser.hpp>

namespace MFFormat
{

/**
    Parser for .mnu files. They store game menus for v1.3 of the game.
*/

class DataFormatMNU: public DataFormat
{
public:

    #pragma pack(push,1)
    typedef struct
    {
        char mMagic[4]; // should be 'Menu'
        uint32_t mUnknown;
        uint32_t mNumControls;
    } Header;

    typedef struct
    {
        uint32_t mUnknown;
        char mType[4];
        MFMath::Vec2 mPos;
        float mScaleX;
        float mScaleY;
        uint32_t mTextId; // from Textdb_xx.def
        uint16_t mTextColor;
        uint16_t mBgColor;
    } Control;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Control> getControls()   { return mControls; }
    inline size_t getNumControls()              { return mControls.size(); }

private:
    std::vector<Control> mControls;
};

/**
    Parser for menu.def file. It stores game menus for pre-1.3 versions of the game.
*/

class DataFormatMenuDEF: public DataFormat
{
public:

    #pragma pack(push,1)
    typedef struct
    {
        uint32_t mUnknown;
        char mType[4];
        MFMath::Vec2 mPos;
        float mScaleX;
        float mScaleY;
        uint32_t mTextId; // from Textdb_xx.def
        uint32_t mTextColor;
        uint32_t mBgColor;
    } Control;
    #pragma pack(pop)

    virtual bool load(std::ifstream &srcFile) override;
    inline std::vector<Control> getControls()   { return mControls; }
    inline size_t getNumControls()              { return mControls.size(); }

private:
    std::vector<Control> mControls;
};

}

#endif

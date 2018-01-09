#ifndef OSG_MASKS_H
#define OSG_MASKS_H

namespace MFRender
{

typedef enum
{
    MASK_GAME = 1,       ///< anything that is to be rendered in-game
    MASK_DEBUG = 2       ///< anything that serves debugging and should not be displayed by default
} NodeMask;

};

#endif

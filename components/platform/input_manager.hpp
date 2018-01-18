#ifndef INPUT_MANAGER_H
#define INPUT_MANAGER_H

namespace MFInput
{

class InputManager
{
public:
    virtual void init() = 0;
    virtual void shutdown() = 0;
    virtual void update() = 0;
    
    virtual bool getKeyPress() = 0;
    virtual bool getKeyDown() = 0;
    virtual bool getKeyUp() = 0;

    virtual MousePos getMousePos() = 0;

protected:
    typedef enum
    {
        KEY_UP,
    } Key;

    typedef struct
    {
        int x, y;
    } MousePos;
};

}

#endif // INPUT_MANAGER_H
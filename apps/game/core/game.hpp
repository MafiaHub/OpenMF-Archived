#ifndef GAME_H
#define GAME_H

namespace MFGame
{

class Game
{
public:
    virtual void init() = 0; 
    virtual void shutdown() = 0;
    virtual void input(double delta) = 0;
    virtual void update(double delta) = 0;
    virtual void frame(double delta) = 0;
};

}

#endif // GAME_H

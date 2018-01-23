#ifndef INCREMENTING_ID_MANAGER
#define INCREMENTING_ID_MANAGER

namespace MFGame
{

class IncrementingIDManager: public IDManager
{
public:
    virtual Id allocate() override;
private:
    Id mNextId;
};

Id IncrementingIDManager::allocate()
{
    if (mNextId == MFGame::NullId)
    {
        MFLogger::ConsoleLogger::fatal("Maximum number of IDs has been reached!", ID_MANAGER_MODULE_STR);
        return NullId;
    }

    return mNextId++;
}

}

#endif // INCREMENTING_ID_MANAGER
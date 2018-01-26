#ifndef BACKING_ID_MANAGER_H
#define BACKING_ID_MANAGER_H

namespace MFGame
{

class BackingIDManager : public IDManager
{
  public:
    virtual uint32_t getSlot(Id ident) override;
    virtual bool isValid(Id ident) override;

    virtual Id allocate() override;
    virtual void deallocate(Id ident) override;

  protected:
    virtual Id *getHandle(Id ident, bool warn = true) override;
    uint16_t extractSlot(Id ident)       { return (uint16_t)(ident >> 16); }
    uint16_t extractGeneration(Id ident) { return (uint16_t) ident; }
    uint32_t constructId(uint16_t slot, uint16_t generation) { return (uint32_t)slot << 16 | generation; }

  private:
    std::vector<Id> mIndices;
    std::vector<Id> mFreeIndices;
};

}

#endif // BACKING_ID_MANAGER_H

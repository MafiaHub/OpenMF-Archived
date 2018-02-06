/*
  These are tests that DO NOT require the original game data.
*/

#include <tests_general.hpp>

#include <utils/math.hpp>
#include <engine/engine.hpp>

bool testMath()
{
    printSubHeader("Math");

    auto testAngle = MFMath::Vec3(0.1,0.2,0.3);
    auto toQuatAndBack = testAngle.toQuat().toEuler();
    const double err = 0.01;

    ass(std::abs(testAngle.x - toQuatAndBack.x) <= err && std::abs(testAngle.y - toQuatAndBack.y) <= err && std::abs(testAngle.z - toQuatAndBack.z) <= err);

    ass(std::abs(MFMath::length(MFMath::Vec3(-1,1,2)) - 2.4494) <= 0.001);

    return getNumErrors() == 0;
}

bool testEngine()
{
    printSubHeader("Engine");

    message("Create engine.");
    MFGame::Engine::EngineSettings settings;
    MFGame::Engine *testEngine = new MFGame::Engine(settings);

    auto entityManager = testEngine->getSpatialEntityManager();
    auto entityFactory = testEngine->getSpatialEntityFactory();

    ass(entityManager->getEntityById(10) == 0);

    message("Create an entity.");
    MFGame::SpatialEntity::Id entity1Id = entityFactory->createTestBallEntity();
    message("New entity id: " + std::to_string(entity1Id));
    ass(entityManager->getEntityById(entity1Id) != 0);
    auto entity1 = entityManager->getEntityById(entity1Id);
    ass(entityManager->getEntityById(MFGame::SpatialEntity::NullId) == 0);

    MFMath::Vec3 initPos = entity1->getPosition();
    message("entity position: " + initPos.str());

    static unsigned int steps = 100;

    message("Step " + std::to_string(steps) + " frames.");

    for (unsigned int i = 0; i < steps; ++i)
        testEngine->step(0.1);

    MFMath::Vec3 endPos = entity1->getPosition();
    message("entity position: " + endPos.str());

    ass(std::abs(initPos.z - endPos.z) > 10.0);   // Did it fall?

    message("Delete engine.");
    delete testEngine;

    return getNumErrors() == 0;
}

#ifdef main
#undef main
#endif // main

int main()
{
    printHeader("OPENMF TEST SUITE");   

    testMath();
    testEngine();

    printHeader("TEST RESULTS");
    message("errors: " + std::to_string(getNumErrors()));

    return getNumErrors() == 0 ? 0 : 1;
}

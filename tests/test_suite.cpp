/*
  These are tests that DO NOT require the original game data.
*/

#include <tests_general.hpp>

#include <utils/math.hpp>
#include <engine/engine.hpp>

int main()
{
    printHeader("OPENMF TEST SUITE");   

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

    printHeader("TEST RESULTS");
    message("errors: " + std::to_string(getNumErrors()));

    return getNumErrors() == 0 ? 0 : 1;
}

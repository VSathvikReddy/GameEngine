#include "ECS/ecs.hpp"
#include "ECS/system.hpp"

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <thread> // Required for std::this_thread::sleep_for

// 1. Define standard data components
struct Position { float x, y; };
struct Velocity { float dx, dy; };

// 2. Define a System utilizing your SSystem base
class MovementSystem : public SSystem<std::vector, Position, Velocity> {
public:
    MovementSystem(ECS& ecs) : SSystem<std::vector, Position, Velocity>(ecs) {}

    void update(ECS& ecs, float dt) {
        // Iterate over the cached entities that match the system's signature
        for (auto const& entity : m_entities) {
            auto& pos = ecs.getComponent<Position>(entity);
            auto const& vel = ecs.getComponent<Velocity>(entity);

            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;
        }
    }
};

int main() {
    std::cout << "Starting ECS Game Loop Stress Test...\n";

    ECS ecs;
    ecs.init<Position,Velocity>();
    auto movementSystem = ecs.createSystem<MovementSystem>();

    // --- SETUP: Spawn Entities ---
    std::cout << "Spawning " << MAX_ENTITIES << " entities...\n";
    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity e = ecs.createEntity();

        ecs.addComponent(e, Position{0.0f, 0.0f});
        ecs.addComponent(e, Velocity{10.0f, 5.0f}); 
    }

    // --- GAME LOOP CONFIGURATION ---
    constexpr int TARGET_FPS = 60;
    constexpr double TARGET_FRAME_TIME_MS = 1000.0 / TARGET_FPS;
    const float dt = 1.0f / TARGET_FPS; // Fixed delta time for systems

    bool isRunning = true;
    int frameCount = 0;
    int maxFramesToTest = 600; // Run for exactly 10 simulated seconds at 60fps

    std::cout << "Entering Game Loop (Targeting " << TARGET_FPS << " FPS)...\n";
    std::cout << "Simulating " << maxFramesToTest << " frames.\n\n";

    auto loopStart = std::chrono::high_resolution_clock::now();

    while (isRunning) {
        auto frameStart = std::chrono::high_resolution_clock::now();
        movementSystem->update(ecs, dt);

        std::chrono::duration<double, std::milli> processingTime = std::chrono::high_resolution_clock::now() - frameStart;


        if (frameCount % 60 == 0) {
            auto const& trackedPos = ecs.getComponent<Position>(0);
            std::cout << "[Simulated Time: " << (frameCount / 60) << "s] "
                      << "Entity 0 is at Position: (" << trackedPos.x << ", " << trackedPos.y << ") "
                      << "| Processed 5000 entities in: " << processingTime.count() << " ms\n";
        }

        // 4. Sleep if the frame processed faster than our target frame time
        if (processingTime.count() < TARGET_FRAME_TIME_MS) {
            std::chrono::duration<double, std::milli> sleepDuration(TARGET_FRAME_TIME_MS - processingTime.count());
            std::this_thread::sleep_for(sleepDuration);
        }


        frameCount++;
        if (frameCount >= maxFramesToTest) {
            isRunning = false;
        }
    }

    std::cout << "\nGame Loop finished successfully.\n";
    std::cout << "Simulated 10 seconds in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - loopStart).count() << " real-world seconds.\n";

    return 0;
}
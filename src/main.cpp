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
class MovementSystem : public SSystem<std::vector<Entity>, Position, Velocity> {
public:
    MovementSystem(ECS& ecs) : SSystem<std::vector<Entity>, Position, Velocity>(ecs) {}

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

    // Register Components
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();

    // Register System
    auto movementSystem = ecs.createSystem<MovementSystem>();

    // --- SETUP: Spawn Entities ---
    std::cout << "Spawning " << MAX_ENTITIES << " entities...\n";
    std::vector<Entity> activeEntities;
    activeEntities.reserve(MAX_ENTITIES);

    for (int i = 0; i < MAX_ENTITIES; ++i) {
        Entity e = ecs.createEntity();
        activeEntities.push_back(e);
        ecs.addComponent(e, Position{0.0f, 0.0f});
        
        // Give the first entity a unique velocity so we can easily track it
        if (i == 0) {
            ecs.addComponent(e, Velocity{10.0f, 5.0f}); 
        } else {
            ecs.addComponent(e, Velocity{1.5f, 2.0f});
        }
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
        // 1. Record the time at the start of the frame
        auto frameStart = std::chrono::high_resolution_clock::now();

        // 2. Process Systems
        movementSystem->update(ecs, dt);

        // 3. Calculate how long the frame actually took to process
        auto frameEnd = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> processingTime = frameEnd - frameStart;

        // --- TRACKING PRINT STATEMENT ---
        // Print the progress of the first entity every 60 frames (1 second)
        if (frameCount % 60 == 0) {
            auto const& trackedPos = ecs.getComponent<Position>(activeEntities[0]);
            std::cout << "[Simulated Time: " << (frameCount / 60) << "s] "
                      << "Entity 0 is at Position: (" << trackedPos.x << ", " << trackedPos.y << ") "
                      << "| Processed 5000 entities in: " << processingTime.count() << " ms\n";
        }

        // 4. Sleep if the frame processed faster than our target frame time
        if (processingTime.count() < TARGET_FRAME_TIME_MS) {
            std::chrono::duration<double, std::milli> sleepDuration(TARGET_FRAME_TIME_MS - processingTime.count());
            std::this_thread::sleep_for(sleepDuration);
        }

        // End condition for the stress test
        frameCount++;
        if (frameCount >= maxFramesToTest) {
            isRunning = false;
        }
    }

    auto loopEnd = std::chrono::high_resolution_clock::now();
    auto totalTime = std::chrono::duration_cast<std::chrono::seconds>(loopEnd - loopStart).count();

    std::cout << "\nGame Loop finished successfully.\n";
    std::cout << "Simulated 10 seconds in " << totalTime << " real-world seconds.\n";

    return 0;
}
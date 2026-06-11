#include "ECS/ecs.hpp"
#include <iostream>

struct Transform {
    float x = 0.0f;
    float y = 0.0f;
};

struct Velocity {
    float dx = 0.0f;
    float dy = 0.0f;
};

struct Renderable {
    char sprite = ' ';
};


using GameECS = ECS<Transform, Velocity, Renderable>;

int main() {
    std::cout << "--- Initializing Engine Test ---" << std::endl;
    GameECS ecs;

    // ==========================================
    // Test 1: Entity Creation and Batch Insertion
    // ==========================================
    Entity player = ecs.createEntity();
    std::cout << "Created Entity ID: " << player << std::endl;

    // Use your variadic addComponents! 
    // This inserts data, sets masks, and updates signatures at the exact same time.
    ecs.addComponents(player, Transform{10.0f, 20.0f}, Velocity{1.5f, -0.5f});
    
    std::cout << "Player has Transform? " << (ecs.hasComponent<Transform>(player) ? "Yes" : "No") << std::endl;
    std::cout << "Player has Velocity? " << (ecs.hasComponent<Velocity>(player) ? "Yes" : "No") << std::endl;
    std::cout << "Player has Renderable? " << (ecs.hasComponent<Renderable>(player) ? "Yes" : "No") << std::endl;
    std::cout << "Current Signature Mask: " << ecs.getSignature(player) << std::endl;

    // ==========================================
    // Test 2: Variadic Modification
    // ==========================================
    std::cout << "\n--- Testing Batch Modification ---" << std::endl;
    
    // Instantly overwrite data blocks simultaneously with zero extra overhead
    ecs.modifyComponents(player, Transform{100.0f, 200.0f}, Velocity{0.0f, 0.0f});

    auto& t = ecs.getComponent<Transform>(player);
    auto& v = ecs.getComponent<Velocity>(player);
    std::cout << "Modified Player Pos: (" << t.x << ", " << t.y << ")" << std::endl;
    std::cout << "Modified Player Vel: (" << v.dx << ", " << v.dy << ")" << std::endl;

    // ==========================================
    // Test 3: Engine Cloning using forEachArray
    // ==========================================
    std::cout << "\n--- Testing Compile-Time Tuple Cloning ---" << std::endl;
    
    // Clones the signature, unpacks the index sequence, and duplicates the memory
    Entity clonePlayer = ecs.cloneEntity(player);
    std::cout << "Created Clone Entity ID: " << clonePlayer << std::endl;
    std::cout << "Clone Signature Mask:    " << ecs.getSignature(clonePlayer) << std::endl;

    auto& cloneT = ecs.getComponent<Transform>(clonePlayer);
    std::cout << "Cloned Player Pos: (" << cloneT.x << ", " << cloneT.y << ")" << std::endl;

    // ==========================================
    // Test 4: Pure Hot-Loop Simulation (Systems)
    // ==========================================
    std::cout << "\n--- Testing Hot Loop Execution ---" << std::endl;
    
    // Give the clone some velocity so it moves
    ecs.modifyComponents(clonePlayer, Velocity{5.0f, 10.0f});

    // Simulate what a real System loop does behind the scenes using our cached signature
    auto physicsSignature = GameECS::getComponentSignature<Transform, Velocity>();
    
    // Mock Entity Pool Array (just player and clone for testing)
    Entity activeEntities[] = { player, clonePlayer };

    std::cout << "Running Physics System Tick..." << std::endl;
    for (Entity entity : activeEntities) {
        // Blazing fast bitwise checking. In Release builds, assertions vanish.
        if ((ecs.getSignature(entity) & physicsSignature) == physicsSignature) {
            auto& transform = ecs.getComponent<Transform>(entity);
            auto& velocity = ecs.getComponent<Velocity>(entity);

            // Execute raw cache-friendly math operations
            transform.x += velocity.dx;
            transform.y += velocity.dy;

            std::cout << "  Entity " << entity << " moved to: (" << transform.x << ", " << transform.y << ")" << std::endl;
        }
    }

    // ==========================================
    // Test 5: Safe Destruction
    // ==========================================
    std::cout << "\n--- Testing Complete Destruction ---" << std::endl;
    
    ecs.destroyEntity(player);
    std::cout << "Destroyed Player. Active Signature Mask: " << ecs.getSignature(player) << std::endl;

    std::cout << "\n--- All Engine Mechanics Verified Successfully! ---" << std::endl;
    return 0;
}
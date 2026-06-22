#include "ECS/ecs.hpp"

#include <iostream>
#include <chrono>
#include <random>
#include <vector>
#include <cassert>
#include <iomanip>
#include <string>

// =========================================================================
// TEST COMPONENTS
// =========================================================================
struct Transform { float x, y; };
struct Velocity { float dx, dy; };
struct Health { int hp; };
struct RenderLayer { int z_index; };

// High-precision benchmarking timer
struct ScopeTimer {
    std::string name;
    std::chrono::high_resolution_clock::time_point start;
    ScopeTimer(std::string t_name) : name(t_name), start(std::chrono::high_resolution_clock::now()) {}
    ~ScopeTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms = end - start;
        std::cout << "  [Timer] " << std::left << std::setw(50) << name << " : " 
                  << std::fixed << std::setprecision(4) << ms.count() << " ms\n";
    }
};

int main() {
    std::cout << "=========================================================\n";
    std::cout << "          EXHAUSTIVE ECS CORE VALIDATION SUITE           \n";
    std::cout << "=========================================================\n\n";

    ECS ecs;

    // =========================================================================
    // PHASE 1: INITIALIZATION & REGISTRATION
    // =========================================================================
    std::cout << "--- PHASE 1: REGISTRATION & METADATA ---\n";
    {
        ScopeTimer t("Engine Initialization");
        ecs.init<Transform, Velocity, Health, RenderLayer>();
    }

    // Validate Signature bit assignments
    Signature sigA = ecs.getSignature<Transform>();
    Signature sigB = ecs.getSignature<Velocity>();
    assert(sigA != sigB && "Component types generated identical signatures!");
    assert((ecs.getSignature<Transform, Velocity>()) == (sigA | sigB) && "Variadic signature generation failed!");
    std::cout << "  [Assert] Signatures and Type Metadata: PASSED\n";

    // =========================================================================
    // PHASE 2: MASS CREATION & MEMORY ALLOCATION
    // =========================================================================
    std::cout << "\n--- PHASE 2: ENTITY LIFECYCLE & SPARSITY ---\n";
    {
        ScopeTimer t("Spawning " + std::to_string(MAX_ENTITIES) + " Mixed Entities");
        for (Entity i = 0; i < MAX_ENTITIES; ++i) {
            Entity e = ecs.createEntity();
            
            // Baseline component
            ecs.addComponent(e, Transform{static_cast<float>(i), static_cast<float>(i)});
            
            // Interleaved components to test view filtering
            if (i % 2 == 0) ecs.addComponent(e, Velocity{1.0f, 1.0f});
            if (i % 3 == 0) ecs.addComponent(e, Health{100});
            if (i % 4 == 0) ecs.addComponent(e, RenderLayer{static_cast<int>(MAX_ENTITIES - i)});
        }
    }

    // Validate hasComponent & hasSignature functionality
    assert(ecs.hasComponent<Transform>(0) == true);
    assert(ecs.hasComponent<Velocity>(1) == false); // Odd entity
    assert((ecs.hasSignature<Transform, Health>(3)) == true); // 3 % 3 == 0
    std::cout << "  [Assert] Sparse Bitmask Queries: PASSED\n";

    // =========================================================================
    // PHASE 3: DATA MUTATION & CONST CORRECTNESS
    // =========================================================================
    std::cout << "\n--- PHASE 3: DATA INTEGRITY & MODIFICATION ---\n";
    {
        ScopeTimer t("Targeted Data Modification");
        ecs.modifyData<Health>(3, Health{999});
        
        auto& hp = ecs.getComponent<Health>(3);
        assert(hp.hp == 999 && "modifyData() failed to overwrite existing component value!");
    }

    {
        ScopeTimer t("Const Routing Verification");
        auto const& read_only_trans = ecs.getComponent<const Transform>(5);
        assert(read_only_trans.x == 5.0f && "Data mismatch on const retrieval!");

        const ECS& const_ecs = ecs;
        auto const& strictly_const = const_ecs.getComponent<const Transform>(5);
        assert(strictly_const.y == 5.0f && "Const ECS reference retrieval failed!");
    }
    std::cout << "  [Assert] Value Modification & Const Memory Boundaries: PASSED\n";

    // =========================================================================
    // PHASE 4: CYCLE SORTING & MEMORY ALIGNMENT
    // =========================================================================
    std::cout << "\n--- PHASE 4: CYCLE SORTING STABILITY ---\n";
    {
        ScopeTimer t("Executing In-Place Cycle Sort (RenderLayer)");
        ecs.sort<RenderLayer>([](const RenderLayer& a, const RenderLayer& b) {
            return a.z_index < b.z_index;
        });
    }

    {
        ScopeTimer t("Validating Sort Continuity");
        int previous_z = -1;
        bool is_sorted = true;
        ecs.getView<RenderLayer>().each([&](Entity e, auto& r) {
            if (r.z_index < previous_z) is_sorted = false;
            previous_z = r.z_index;
        });
        assert(is_sorted == true && "Cycle sort corrupted array continuity or sparse mappings!");
    }
    std::cout << "  [Assert] In-Place Array Shifting and Sparse Rebasing: PASSED\n";

    // =========================================================================
    // PHASE 5: COMPLEX VIEW ITERATION & FILTERING
    // =========================================================================
    std::cout << "\n--- PHASE 5: VIEW ALGORITHMS & PERFORMANCE ---\n";
    
    // Expected math: 
    // MAX_ENTITIES = 5000. 
    // Divisible by 2 (Velocity) = 2500
    // Divisible by 3 (Health) = 1667
    // Divisible by 6 (Both) = 834
    
    size_t each_count = 0;
    {
        ScopeTimer t("View::each() Filter Execution (Transform, Velocity, Health)");
        ecs.getView<Transform, Velocity, Health>().each([&](Entity e, auto& t, auto& v, auto& h) {
            t.x += v.dx;
            each_count++;
        });
    }
    assert(each_count == 834 && "View::each() signature filter logic failed!");

    size_t iter_count = 0;
    {
        ScopeTimer t("View::Iterator Ranged-For Execution (Transform, Velocity, Health)");
        auto view = ecs.getView<Transform, Velocity, Health>();
        for (auto [e, t, v, h] : view) {
            t.y += v.dy;
            iter_count++;
        }
    }
    assert(iter_count == 834 && "Iterator ++ or skipInvalid() logic failed!");
    std::cout << "  [Assert] Iterator Skip Logic & Tuple Dereferencing: PASSED\n";

    // =========================================================================
    // PHASE 6: DESTRUCTIVE FRAGMENTATION & RECYCLING
    // =========================================================================
    std::cout << "\n--- PHASE 6: LIFECYCLE DESTRUCTION & RE-SYNC ---\n";
    
    {
        ScopeTimer t("Targeted Component Removal (Swap and Pop)");
        ecs.removeComponent<Velocity>(6); // Entity 6 has Velocity (6 % 2 == 0)
        assert(ecs.hasComponent<Velocity>(6) == false && "Component removal failed to clear bitmask!");
        assert(ecs.hasComponent<Transform>(6) == true && "Component removal accidentally wiped other data!");
    }

    {
        ScopeTimer t("Mass Interleaved Entity Destruction");
        // Destroy half the entities, creating massive holes in the sparse arrays
        for (Entity i = 1; i < MAX_ENTITIES; i += 2) {
            ecs.destroyEntity(i);
        }
    }

    {
        ScopeTimer t("Post-Destruction View Integrity Check");
        size_t surviving = 0;
        ecs.getView<Transform>().each([&](Entity e, auto& t) { surviving++; });
        assert(surviving == 2500 && "Sparse/Dense array desync during Entity destruction!");
        
        // Ensure values weren't scrambled during the massive swap-and-pop event
        auto& t_val = ecs.getComponent<Transform>(0);
        assert(t_val.x == 1.0f && "Data corruption detected during array swap operations!");
    }

    {
        ScopeTimer t("Entity ID Recycling Alignment");
        Entity recycled = ecs.createEntity();
        ecs.addComponent(recycled, Transform{99.0f, 99.0f});
        assert(recycled % 2 != 0 && "Entity Manager failed to pop from available stack correctly!");
    }
    std::cout << "  [Assert] Array Shrinking, Value Swapping, and ID Recycling: PASSED\n";

    std::cout << "\n=========================================================\n";
    std::cout << "    ALL 15 CRITICAL ASSERTIONS PASSED. SYSTEM IS STABLE. \n";
    std::cout << "=========================================================\n";

    return 0;
}
// #include "ECS/ecs.hpp"

// #include <iostream>
// #include <chrono>
// #include <random>
// #include <vector>
// #include <cassert>
// #include <iomanip>
// #include <string>

// // =========================================================================
// // TEST COMPONENTS
// // =========================================================================
// struct Transform { float x, y; };
// struct Velocity { float dx, dy; };
// struct Health { int hp; };
// struct RenderLayer { int z_index; };
// struct ColliderPP { float width, height; };

// // High-precision benchmarking timer
// struct ScopeTimer {
//     std::string name;
//     std::chrono::high_resolution_clock::time_point start;
//     ScopeTimer(std::string t_name) : name(t_name), start(std::chrono::high_resolution_clock::now()) {}
//     ~ScopeTimer() {
//         auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double, std::milli> ms = end - start;
//         std::cout << "  [Timer] " << std::left << std::setw(60) << name << " : " 
//                   << std::fixed << std::setprecision(4) << ms.count() << " ms\n";
//     }
// };

// int main() {
//     std::cout << "==================================================================\n";
//     std::cout << "             THE ULTIMATE ECS CORE VALIDATION SUITE               \n";
//     std::cout << "==================================================================\n\n";

//     ECS ecs;

//     // Use 4000 to leave exactly 1000 open slots in the MAX_ENTITIES (5000) pool for dynamic testing
//     constexpr int SPAWN_COUNT = 4000; 

//     // =========================================================================
//     // PHASE 1: INITIALIZATION & METADATA REGISTRATION
//     // =========================================================================
//     std::cout << "--- PHASE 1: REGISTRATION & METADATA ---\n";
//     {
//         ScopeTimer t("Engine Initialization & Type Mapping");
//         ecs.init<Transform, Velocity, Health, RenderLayer, ColliderPP>();
//     }

//     Signature sigTrans = ecs.getSignature<Transform>();
//     Signature sigVel = ecs.getSignature<Velocity>();
//     assert(sigTrans != sigVel && "CRITICAL: Component types generated identical signatures!");
//     assert((ecs.getSignature<Transform, Velocity>()) == (sigTrans | sigVel) && "CRITICAL: Variadic signature generation failed!");
//     std::cout << "  [Assert] Signatures and Type Metadata: PASSED\n";

//     // =========================================================================
//     // PHASE 2: MASS ALLOCATION & SPARSITY GENERATION
//     // =========================================================================
//     std::cout << "\n--- PHASE 2: MASS ENTITY LIFECYCLE & SPARSITY ---\n";
//     {
//         ScopeTimer t("Spawning " + std::to_string(SPAWN_COUNT) + " Mixed-Signature Entities");
//         for (Entity i = 0; i < SPAWN_COUNT; ++i) {
//             Entity e = ecs.createEntity();
            
//             ecs.addComponent(e, Transform{static_cast<float>(i), static_cast<float>(i)});
            
//             if (i % 2 == 0) ecs.addComponent(e, Velocity{1.0f, 1.0f});
//             if (i % 3 == 0) ecs.addComponent(e, Health{100});
//             if (i % 4 == 0) ecs.addComponent(e, RenderLayer{static_cast<int>(SPAWN_COUNT - i)});
//             if (i % 5 == 0) ecs.addComponent(e, ColliderPP{10.0f, 10.0f});
//         }
//     }

//     assert(ecs.hasComponent<Transform>(0) == true);
//     assert(ecs.hasComponent<Velocity>(1) == false); 
//     assert((ecs.hasSignature<Transform, Health, ColliderPP>(15)) == true); 
//     std::cout << "  [Assert] Sparse Bitmask Matrix Queries: PASSED\n";

//     // =========================================================================
//     // PHASE 3: CONST CORRECTNESS & MEMORY BOUNDARIES
//     // =========================================================================
//     std::cout << "\n--- PHASE 3: DATA INTEGRITY & CONST ROUTING ---\n";
//     {
//         ScopeTimer t("Targeted Data Modification (O(1) Sparse Lookup)");
//         ecs.modifyData<Health>(3, Health{999});
//         auto& hp = ecs.getComponent<Health>(3);
//         assert(hp.hp == 999 && "CRITICAL: modifyData() failed to overwrite existing component!");
//     }

//     {
//         ScopeTimer t("Const Template Routing Verification");
//         auto const& read_only_trans = ecs.getComponent<const Transform>(5);
//         assert(read_only_trans.x == 5.0f && "CRITICAL: Data mismatch on const retrieval!");

//         const ECS& const_ecs = ecs;
//         auto const& strictly_const = const_ecs.getComponent<const Transform>(5);
//         assert(strictly_const.y == 5.0f && "CRITICAL: Const ECS reference retrieval failed!");
//     }
//     std::cout << "  [Assert] Value Modification & Const Reference Routing: PASSED\n";

//     // =========================================================================
//     // PHASE 4: CYCLE SORTING & MEMORY ALIGNMENT
//     // =========================================================================
//     std::cout << "\n--- PHASE 4: IN-PLACE CYCLE SORTING STABILITY ---\n";
//     {
//         ScopeTimer t("Executing High-Speed Cycle Sort (RenderLayer)");
//         ecs.sort<RenderLayer>([](const RenderLayer& a, const RenderLayer& b) {
//             return a.z_index < b.z_index;
//         });
//     }

//     {
//         ScopeTimer t("Validating Sort Continuity Across Dense Array");
//         int previous_z = -1;
//         bool is_sorted = true;
//         ecs.getView<RenderLayer>().each([&](Entity e, auto& r) {
//             if (r.z_index < previous_z) is_sorted = false;
//             previous_z = r.z_index;
//         });
//         assert(is_sorted == true && "CRITICAL: Cycle sort corrupted array continuity!");
//     }
//     std::cout << "  [Assert] In-Place Array Shifting and Sparse Rebasing: PASSED\n";

//     // =========================================================================
//     // PHASE 5: CONTINUOUS SORTED ARRAY SHIFTING
//     // =========================================================================
//     std::cout << "\n--- PHASE 5: DYNAMIC ARRAY BUBBLING & SORTED INSERTS ---\n";
    
//     Entity t1 = ecs.createEntity();
//     Entity t2 = ecs.createEntity();
//     Entity t3 = ecs.createEntity();

//     {
//         ScopeTimer t("Reverse-Order Sorted Insertions (Shift Right)");
//         ecs.addComponent(t1, RenderLayer{30});
//         ecs.addComponent(t2, RenderLayer{20}); 
//         ecs.addComponent(t3, RenderLayer{10}); 
//     }
    
//     assert(ecs.getComponent<RenderLayer>(t1).z_index == 30);
//     assert(ecs.getComponent<RenderLayer>(t3).z_index == 10);

//     {
//         ScopeTimer t("Bi-Directional Array Bubbling (modifyData)");
//         ecs.modifyData(t3, RenderLayer{25}); 
//         assert(ecs.getComponent<RenderLayer>(t3).z_index == 25);

//         ecs.modifyData(t1, RenderLayer{5});  
//         assert(ecs.getComponent<RenderLayer>(t1).z_index == 5);
//     }
    
//     {
//         ScopeTimer t("Sorted Component Removal (Shift Left)");
//         ecs.removeComponent<RenderLayer>(t2); 
//         assert(ecs.hasComponent<RenderLayer>(t2) == false);
//     }
//     std::cout << "  [Assert] Complex Left/Right Memory Bubbling Logic: PASSED\n";

//     // Re-align t1 and t3 for Phase 7 physics integration
//     ecs.addComponent(t1, Transform{0.0f, 0.0f}); ecs.addComponent(t1, Velocity{1.0f, 1.0f}); ecs.addComponent(t1, ColliderPP{1.0f, 1.0f});
//     ecs.addComponent(t3, Transform{0.0f, 0.0f}); ecs.addComponent(t3, Velocity{1.0f, 1.0f}); ecs.addComponent(t3, ColliderPP{1.0f, 1.0f});

//     // =========================================================================
//     // PHASE 6: COMPLEX VIEW ITERATION & FILTERING
//     // =========================================================================
//     std::cout << "\n--- PHASE 6: VIEW ALGORITHMS & PIPELINE SPEED ---\n";
    
//     // Dynamic math computation for view logic
//     const size_t EXPECTED_VIEW_MATCHES = (SPAWN_COUNT / 10) + 2; // %2 AND %5 intersection + t1 + t3
    
//     size_t each_count = 0;
//     {
//         ScopeTimer t("View::each() Execution (Transform, Velocity, ColliderPP)");
//         ecs.getView<Transform, Velocity, ColliderPP>().each([&](Entity e, auto& t, auto& v, auto& c) {
//             t.x += v.dx; // Physics Integration! Entity 0's X is now 1.0f
//             each_count++;
//         });
//     }
//     assert(each_count == EXPECTED_VIEW_MATCHES && "CRITICAL: View::each() signature filter logic failed!");

//     size_t iter_count = 0;
//     {
//         ScopeTimer t("View::Iterator Ranged-For (Transform, Velocity, ColliderPP)");
//         for (auto [e, t, v, c] : ecs.getView<Transform, Velocity, ColliderPP>()) {
//             t.y += v.dy;
//             iter_count++;
//         }
//     }
//     assert(iter_count == EXPECTED_VIEW_MATCHES && "CRITICAL: Iterator ++ or skipInvalid() logic failed!");
//     std::cout << "  [Assert] Iterator Skip Logic & Tuple Dereferencing: PASSED\n";

//     // =========================================================================
//     // PHASE 7: DESTRUCTIVE FRAGMENTATION & RE-SYNC
//     // =========================================================================
//     std::cout << "\n--- PHASE 7: LIFECYCLE DESTRUCTION & SPARSE RE-SYNC ---\n";
    
//     {
//         ScopeTimer t("Targeted Component Removal (Swap and Pop Fallback)");
//         ecs.removeComponent<Velocity>(6); 
//         assert(ecs.hasComponent<Velocity>(6) == false && "CRITICAL: Component removal failed!");
//         assert(ecs.hasComponent<Transform>(6) == true && "CRITICAL: Component removal wiped parallel data!");
//     }

//     {
//         ScopeTimer t("Mass Interleaved Entity Destruction (50% Annihilation)");
//         for (Entity i = 1; i < SPAWN_COUNT; i += 2) {
//             ecs.destroyEntity(i); // Annihilates all odd entities
//         }
//     }

//     {
//         ScopeTimer t("Post-Destruction View Integrity Check");
//         size_t surviving = 0;
//         ecs.getView<Transform>().each([&](Entity e, auto& t) { surviving++; });
        
//         const size_t EXPECTED_SURVIVORS = (SPAWN_COUNT / 2) + 2; // Even entities + t1 + t3
//         assert(surviving == EXPECTED_SURVIVORS && "CRITICAL: Sparse array desync during Entity destruction!");
        
//         auto& t_val = ecs.getComponent<Transform>(0);
//         assert(t_val.x == 1.0f && "CRITICAL: Data corruption detected during dense array swap operations!");
//     }
//     std::cout << "  [Assert] Array Shrinking, Value Swapping, and Integrity: PASSED\n";

//     // =========================================================================
//     // PHASE 8: ID RECYCLING & STACK POINTER ALIGNMENT
//     // =========================================================================
//     std::cout << "\n--- PHASE 8: ENTITY ID RECYCLING & STACK ALIGNMENT ---\n";
//     {
//         ScopeTimer t("Entity ID Stack Popping Alignment");
//         Entity recycled = ecs.createEntity();
//         ecs.addComponent(recycled, Transform{99.0f, 99.0f});
        
//         // We deleted all odd numbers. The LIFO stack MUST give us an odd number back.
//         assert(recycled % 2 != 0 && "CRITICAL: Entity Manager failed to pop from available ID stack!");
//     }
//     std::cout << "  [Assert] Stack LIFO Integrity: PASSED\n";

//     std::cout << "\n==================================================================\n";
//     std::cout << "     ALL ASSERTIONS PASSED. ENGINE CORE IS MATHEMATICALLY SOUND.  \n";
//     std::cout << "==================================================================\n";

//     return 0;
// }

#include "Geometry/rectangle.hpp"
#include <iostream>
Rect<double> temp(){
    return Rect<double>(5.0,5.0,5.0,5.0);
}

int main(){
    Rect<double>& obj = temp().scaleSize(1.0);

    double dummy[1000] = {0};
    
    std::cout<<obj.getCenter().x<<'\n';
}
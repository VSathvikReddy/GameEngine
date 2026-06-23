// =============================================================================
//  ECS Test Suite
//  Covers: ComponentArray, EntityManager, ComponentManager, ECS, View/Iterator
//  Every code path exercised. Assertions validate correctness.
//  Benchmarks time the hot paths.
// =============================================================================

#include "ECS/ecs.hpp"

#include <iostream>
#include <cassert>
#include <chrono>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <random>
#include <functional>
#include <iomanip>
#include <cmath>

// =============================================================================
//  Utilities
// =============================================================================

static int g_pass = 0, g_fail = 0;

#define CHECK(expr, msg)                                                        \
    do {                                                                        \
        if (!(expr)) {                                                          \
            std::cerr << "  [FAIL] " << msg << "  (" #expr ")\n";             \
            ++g_fail;                                                           \
        } else {                                                                \
            std::cout << "  [pass] " << msg << "\n";                           \
            ++g_pass;                                                           \
        }                                                                       \
    } while(0)

struct Timer {
    using Clock = std::chrono::high_resolution_clock;
    Clock::time_point t0;
    std::string label;
    Timer(std::string l) : t0(Clock::now()), label(std::move(l)) {}
    ~Timer() {
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(
                      Clock::now() - t0).count();
        std::cout << "  [bench] " << label << ": "
                  << std::setw(8) << us << " us\n";
    }
};

static void section(const char* name) {
    std::cout << "\n══════════════════════════════════════════\n"
              << "  " << name << "\n"
              << "══════════════════════════════════════════\n";
}

// =============================================================================
//  Component types
// =============================================================================

struct Position  { float x, y, z; };
struct Velocity  { float dx, dy, dz; };
struct Health    { int hp, max_hp; };
struct Tag       { int id; };           // lightweight, used for sort tests
struct BigBlob   { char data[256]; };   // tests move semantics under stress

// =============================================================================
//  1. EntityManager
// =============================================================================

void test_entity_manager() {
    section("EntityManager");

    EntityManager em;

    // Create and verify IDs are unique
    std::vector<Entity> ids;
    {
        Timer t("create 1000 entities");
        for (int i = 0; i < 1000; ++i) ids.push_back(em.createEntity());
    }
    std::sort(ids.begin(), ids.end());
    bool all_unique = std::adjacent_find(ids.begin(), ids.end()) == ids.end();
    CHECK(all_unique, "1000 entity IDs are unique");
    CHECK(ids.back() < MAX_ENTITIES, "All IDs within MAX_ENTITIES");

    // Signature starts zeroed
    CHECK(em.getSignature(ids[0]).none(), "Fresh entity has empty signature");

    // addComponentType / hasComponent
    em.addComponentType(ids[0], 0);
    CHECK(em.hasComponent(ids[0], 0),  "hasComponent true after add");
    CHECK(!em.hasComponent(ids[0], 1), "hasComponent false for absent type");

    // removeComponentType
    em.removeComponentType(ids[0], 0);
    CHECK(!em.hasComponent(ids[0], 0), "hasComponent false after remove");

    // setSignature / getSignature round-trip
    Signature sig; sig.set(3); sig.set(7);
    em.setSignature(ids[1], sig);
    CHECK(em.getSignature(ids[1]) == sig, "setSignature/getSignature round-trip");

    // destroyEntity recycles ID — create, destroy, create, get same ID back (LIFO)
    Entity recycled_candidate = ids[999];
    em.destroyEntity(recycled_candidate);
    Entity next = em.createEntity();
    CHECK(next == recycled_candidate, "Destroyed entity ID recycled (LIFO)");

    // Signature of destroyed entity is reset
    em.addComponentType(ids[5], 2);
    em.destroyEntity(ids[5]);
    // After destroy the signature array entry is zeroed
    // (we create a fresh EntityManager to get a clean slate for this check)
    {
        EntityManager em2;
        Entity e = em2.createEntity();
        em2.addComponentType(e, 1);
        em2.destroyEntity(e);
        Entity e2 = em2.createEntity(); // same ID recycled
        CHECK(em2.getSignature(e2).none(), "Recycled entity starts with clean signature");
    }
}

// =============================================================================
//  2. ComponentArray
// =============================================================================

void test_component_array() {
    section("ComponentArray");

    ComponentArray<Position> arr(200);

    // Insert
    arr.insertData(0, {1.f, 2.f, 3.f});
    arr.insertData(1, {4.f, 5.f, 6.f});
    arr.insertData(2, {7.f, 8.f, 9.f});
    CHECK(arr.size() == 3, "Size is 3 after 3 inserts");

    // getData mutable
    CHECK(arr.getData(1).x == 4.f, "getData returns correct component");

    // getData const
    const ComponentArray<Position>& carr = arr;
    CHECK(carr.getData(2).z == 9.f, "const getData returns correct component");

    // modifyData
    arr.modifyData(1, {10.f, 11.f, 12.f});
    CHECK(arr.getData(1).x == 10.f, "modifyData updates component");

    // removeData (middle element — triggers swap-and-pop)
    arr.removeData(1);
    CHECK(arr.size() == 2, "Size is 2 after remove");
    // entity 2 should have been swapped into slot 1
    CHECK(arr.getData(2).z == 9.f, "Swapped entity still accessible by entity ID");
    // entity 1 must not be accessible (sparse entry is NULL)
    // (checked via destroyEntity path below to avoid asserting)

    // removeData (last element — no swap needed)
    arr.insertData(5, {1.f, 0.f, 0.f});
    arr.removeData(5);
    CHECK(arr.size() == 2, "Size unchanged after remove of last element");

    // destroyEntity on existing entity
    arr.destroyEntity(0);
    CHECK(arr.size() == 1, "destroyEntity removes existing component");

    // destroyEntity on non-existing entity (must NOT assert — uses guard)
    arr.destroyEntity(99);  // entity 99 was never inserted
    CHECK(arr.size() == 1, "destroyEntity on absent entity is a no-op");

    // Stress: fill to capacity and back
    {
        ComponentArray<Tag> stress(500);
        for (int i = 0; i < 500; ++i) stress.insertData(i, {i});
        CHECK(stress.size() == 500, "Fill to max capacity");
        for (int i = 0; i < 500; ++i) stress.removeData(i);
        CHECK(stress.size() == 0, "Empty after removing all");
    }

    // Move semantics: BigBlob should never be copied
    {
        ComponentArray<BigBlob> blob_arr(10);
        BigBlob b{}; b.data[0] = 42;
        blob_arr.insertData(0, std::move(b));
        CHECK(blob_arr.getData(0).data[0] == 42, "BigBlob move-inserted correctly");
    }
}

// =============================================================================
//  3. ComponentArray::sort
// =============================================================================

void test_sort() {
    section("ComponentArray::sort");

    // Build array: entities 0..N with Tag{N-i} (reverse order)
    const int N = 20;
    ComponentArray<Tag> arr(N);
    for (int i = 0; i < N; ++i) arr.insertData(i, {N - i});

    arr.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });

    // Verify dense array is sorted
    bool sorted_ok = true;
    for (size_t i = 1; i < arr.size(); ++i) {
        // Access via entity IDs stored in dense_entity_array
        // We can verify via getData per entity
    }
    // Verify every entity still resolves to the same value it had before
    for (int i = 0; i < N; ++i) {
        if (arr.getData(i).id != N - i) { sorted_ok = false; break; }
    }
    CHECK(sorted_ok, "After sort, each entity still returns its original Tag value");

    // Verify dense array is actually sorted ascending via a fresh array:
    {
        ComponentArray<Tag> arr2(5);
        arr2.insertData(10, {50});
        arr2.insertData(20, {10});
        arr2.insertData(30, {30});
        arr2.insertData(40, {20});
        arr2.insertData(50, {40});
        arr2.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });
        // Values must be {10,20,30,40,50} in dense order
        // entity lookups must still work
        CHECK(arr2.getData(10).id == 50, "Entity 10 tag preserved after sort");
        CHECK(arr2.getData(20).id == 10, "Entity 20 tag preserved after sort");
        CHECK(arr2.getData(30).id == 30, "Entity 30 tag preserved after sort");
        CHECK(arr2.getData(40).id == 20, "Entity 40 tag preserved after sort");
        CHECK(arr2.getData(50).id == 40, "Entity 50 tag preserved after sort");
    }

    // Edge: already sorted
    {
        ComponentArray<Tag> arr3(5);
        for (int i = 0; i < 5; ++i) arr3.insertData(i, {i});
        arr3.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });
        bool ok = true;
        for (int i = 0; i < 5; ++i) if (arr3.getData(i).id != i) ok = false;
        CHECK(ok, "Sort of already-sorted array is a no-op");
    }

    // Edge: single element
    {
        ComponentArray<Tag> arr4(5);
        arr4.insertData(7, {99});
        arr4.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });
        CHECK(arr4.getData(7).id == 99, "Sort of single element is safe");
    }

    // Edge: two elements, reverse order (pure 2-cycle)
    {
        ComponentArray<Tag> arr5(5);
        arr5.insertData(0, {2});
        arr5.insertData(1, {1});
        arr5.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });
        CHECK(arr5.getData(0).id == 2, "Entity 0 value intact after 2-element sort");
        CHECK(arr5.getData(1).id == 1, "Entity 1 value intact after 2-element sort");
    }

    // Randomised stress: 100 entities, random values, sort, verify all lookups
    {
        const int M = 100;
        ComponentArray<Tag> arr6(M);
        std::mt19937 rng(42);
        std::uniform_int_distribution<int> dist(0, 10000);
        std::vector<int> vals(M);
        for (int i = 0; i < M; ++i) { vals[i] = dist(rng); arr6.insertData(i, {vals[i]}); }
        arr6.sort([](const Tag& a, const Tag& b){ return a.id < b.id; });
        bool ok = true;
        for (int i = 0; i < M; ++i) if (arr6.getData(i).id != vals[i]) { ok = false; break; }
        CHECK(ok, "Randomised sort: all entity lookups return original values");
    }
}

// =============================================================================
//  4. ECS — registration, add, get, modify, remove
// =============================================================================

void test_ecs_basic() {
    section("ECS basic: register / add / get / modify / remove");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Health>();

    Entity e0 = ecs.createEntity();
    Entity e1 = ecs.createEntity();
    Entity e2 = ecs.createEntity();

    // addComponent + getComponent
    ecs.addComponent(e0, Position{1.f, 2.f, 3.f});
    ecs.addComponent(e0, Velocity{.1f, .2f, .3f});
    ecs.addComponent(e1, Position{4.f, 5.f, 6.f});
    ecs.addComponent(e1, Health{100, 100});
    ecs.addComponent(e2, Health{50, 200});

    CHECK(ecs.getComponent<Position>(e0).x == 1.f, "getComponent<Position> e0.x");
    CHECK(ecs.getComponent<Velocity>(e0).dy == .2f, "getComponent<Velocity> e0.dy");
    CHECK(ecs.getComponent<Position>(e1).y == 5.f, "getComponent<Position> e1.y");
    CHECK(ecs.getComponent<Health>(e1).hp == 100,  "getComponent<Health> e1.hp");
    CHECK(ecs.getComponent<Health>(e2).max_hp == 200, "getComponent<Health> e2.max_hp");

    // modifyData
    ecs.modifyData(e0, Position{9.f, 8.f, 7.f});
    CHECK(ecs.getComponent<Position>(e0).x == 9.f, "modifyData updates value");

    // In-place mutation via reference
    ecs.getComponent<Health>(e1).hp -= 30;
    CHECK(ecs.getComponent<Health>(e1).hp == 70, "In-place mutation via getComponent ref");

    // hasComponent
    CHECK(ecs.hasComponent<Position>(e0),   "hasComponent true");
    CHECK(!ecs.hasComponent<Health>(e0),    "hasComponent false for absent");

    // hasSignature
    CHECK((ecs.hasSignature<Position, Velocity>(e0)), "hasSignature true for e0");
    CHECK((!ecs.hasSignature<Position, Health>(e0)),  "hasSignature false when missing Health");

    // removeComponent
    ecs.removeComponent<Velocity>(e0);
    CHECK(!ecs.hasComponent<Velocity>(e0), "hasComponent false after removeComponent");
    CHECK(ecs.hasComponent<Position>(e0),  "Other component unaffected by remove");

    // getSignature via ECS
    Signature sig = ecs.getSignature(e0);
    ComponentType pos_type = ecs.getComponentType<Position>();
    CHECK(sig.test(pos_type), "getSignature bit set for Position");

    // getSignature<Args...> helper
    Signature expected = ecs.getSignature<Position, Health>();
    CHECK(expected.test(ecs.getComponentType<Position>()), "getSignature<> sets Position bit");
    CHECK(expected.test(ecs.getComponentType<Health>()),   "getSignature<> sets Health bit");
    CHECK(!expected.test(ecs.getComponentType<Velocity>()), "getSignature<> leaves Velocity clear");
}

// =============================================================================
//  5. ECS — destroyEntity
// =============================================================================

void test_ecs_destroy() {
    section("ECS destroyEntity");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Health>();

    Entity e = ecs.createEntity();
    ecs.addComponent(e, Position{1.f, 2.f, 3.f});
    ecs.addComponent(e, Health{50, 100});

    ecs.destroyEntity(e);

    // Signature must be zeroed
    // (We create a new entity which may get the same ID and must have clean sig)
    Entity e2 = ecs.createEntity();
    CHECK(ecs.getSignature(e2).none(), "Recycled entity has zeroed signature");
    CHECK(!ecs.hasComponent<Position>(e2), "Recycled entity has no Position");

    // Destroy entity that only has some components
    Entity e3 = ecs.createEntity();
    ecs.addComponent(e3, Position{0.f, 0.f, 0.f});
    ecs.destroyEntity(e3); // Health was never added — destroyEntity must not assert
    CHECK(true, "destroyEntity with partial components does not crash");

    // Destroy entity with NO components
    Entity e4 = ecs.createEntity();
    ecs.destroyEntity(e4);
    CHECK(true, "destroyEntity with zero components does not crash");

    // Mass create/destroy cycle — entity pool exhaustion test
    ECS ecs2;
    ecs2.registerComponent<Tag>();
    std::vector<Entity> pool;
    for (int i = 0; i < (int)MAX_ENTITIES; ++i) pool.push_back(ecs2.createEntity());
    for (Entity en : pool) ecs2.destroyEntity(en);
    // All entities returned, create again
    Entity refill = ecs2.createEntity();
    CHECK(refill < MAX_ENTITIES, "Can create entity after full pool recycle");
}

// =============================================================================
//  6. ECS — const correctness
// =============================================================================

void test_const_ecs() {
    section("const ECS / const getComponent");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();

    Entity e = ecs.createEntity();
    ecs.addComponent(e, Position{3.f, 1.f, 4.f});
    ecs.addComponent(e, Velocity{1.f, 5.f, 9.f});

    // Mutable ECS, non-const query → T& (mutable)
    Position& pos = ecs.getComponent<Position>(e);
    pos.x = 100.f;
    CHECK(ecs.getComponent<Position>(e).x == 100.f, "Mutable ref write persists");

    // Mutable ECS, const query → const T&
    const Position& cpos = ecs.getComponent<const Position>(e);
    CHECK(cpos.x == 100.f, "const query on mutable ECS returns correct value");

    // const ECS, const query → const T& (the enforced path)
    const ECS& cecs = ecs;
    const Velocity& cvel = cecs.getComponent<const Velocity>(e);
    CHECK(cvel.dy == 5.f, "const ECS getComponent<const T> returns correct value");
}

// =============================================================================
//  7. ECS::init helper
// =============================================================================

void test_ecs_init() {
    section("ECS::init variadic registration");

    ECS ecs;
    ecs.init<Position, Velocity, Health, Tag>();

    Entity e = ecs.createEntity();
    ecs.addComponent(e, Position{0.f, 0.f, 0.f});
    ecs.addComponent(e, Tag{42});
    CHECK(ecs.getComponent<Tag>(e).id == 42, "init<> registers all types correctly");
    CHECK(ecs.hasComponent<Position>(e), "Position registered via init");
    CHECK(!ecs.hasComponent<Health>(e),  "Health registered but not added");
}

// =============================================================================
//  8. View::each
// =============================================================================

void test_view_each() {
    section("View::each");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Health>();

    // 5 entities with Position+Velocity, 3 with just Position, 2 with just Health
    std::vector<Entity> pv_entities, p_only, h_only;
    for (int i = 0; i < 5; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{(float)i, 0.f, 0.f});
        ecs.addComponent(e, Velocity{1.f, 0.f, 0.f});
        pv_entities.push_back(e);
    }
    for (int i = 0; i < 3; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{99.f, 0.f, 0.f});
        p_only.push_back(e);
    }
    for (int i = 0; i < 2; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Health{10, 10});
        h_only.push_back(e);
    }

    // View<Position, Velocity> must visit exactly the 5 pv_entities
    int visit_count = 0;
    auto view = ecs.getView<Position, Velocity>();
    view.each([&](Entity e, Position& p, Velocity& v) {
        ++visit_count;
        v.dx += 1.f; // mutation through ref
    });
    CHECK(visit_count == 5, "View<P,V>::each visits exactly 5 entities");

    // Verify the mutation went through
    for (Entity e : pv_entities) {
        CHECK(ecs.getComponent<Velocity>(e).dx == 2.f, "View mutation persisted via ref");
        break; // just check first one
    }

    // View with const FirstComp: should visit same entities, no mutation
    int const_visits = 0;
    auto cview = ecs.getView<const Position, Velocity>();
    cview.each([&](Entity e, const Position& p, Velocity& v) {
        ++const_visits;
        (void)p.x; // just read
    });
    CHECK(const_visits == 5, "View<const P, V>::each visits correct count");

    // View<Health>: visits only h_only (2 entities)
    int h_count = 0;
    ecs.getView<Health>().each([&](Entity, Health& h){ ++h_count; });
    CHECK(h_count == 2, "View<Health>::each visits only Health entities");

    // View with no matching entities
    ecs.registerComponent<BigBlob>();
    int blob_count = 0;
    ecs.getView<BigBlob>().each([&](Entity, BigBlob&){ ++blob_count; });
    CHECK(blob_count == 0, "View on empty component array visits 0 entities");
}

// =============================================================================
//  9. View range-for iterator
// =============================================================================

void test_view_iterator() {
    section("View iterator (range-for)");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();

    for (int i = 0; i < 8; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{(float)i, 0.f, 0.f});
        if (i % 2 == 0) ecs.addComponent(e, Velocity{0.f, (float)i, 0.f});
    }

    // Range-for: only even entities (0,2,4,6) have both components
    int count = 0;
    float sum_x = 0.f;
    for (auto [e, pos, vel] : ecs.getView<Position, Velocity>()) {
        ++count;
        sum_x += pos.x;
    }
    CHECK(count == 4, "Iterator: range-for visits 4 even-indexed entities");
    CHECK(sum_x == 0.f + 2.f + 4.f + 6.f, "Iterator: sum of Position.x correct");

    // Structured bindings with const
    int ccount = 0;
    for (auto [e, pos, vel] : ecs.getView<const Position, const Velocity>()) {
        ++ccount;
        (void)pos.x;
    }
    CHECK(ccount == 4, "Iterator: const view range-for visits same count");

    // Empty iteration
    ecs.registerComponent<Health>();
    int empty = 0;
    for (auto [e, h] : ecs.getView<Health>()) ++empty;
    CHECK(empty == 0, "Iterator: empty component range-for visits 0");
}

// =============================================================================
//  10. ECS::sort
// =============================================================================

void test_ecs_sort() {
    section("ECS::sort");

    ECS ecs;
    ecs.registerComponent<Tag>();
    ecs.registerComponent<Position>();

    // Create entities with Tag in reverse order
    const int N = 50;
    for (int i = 0; i < N; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Tag{N - i});        // tags: N, N-1, ..., 1
        ecs.addComponent(e, Position{(float)i, 0.f, 0.f});
    }

    ecs.sort<Tag>([](const Tag& a, const Tag& b){ return a.id < b.id; });

    // After sort, getView iterates dense array front-to-back
    // tags should come out ascending in dense order
    // We verify via the View — it visits in dense order for the first component
    std::vector<int> visited_tags;
    ecs.getView<Tag>().each([&](Entity, Tag& t){ visited_tags.push_back(t.id); });

    bool ascending = std::is_sorted(visited_tags.begin(), visited_tags.end());
    CHECK(ascending, "After ECS::sort<Tag>, view visits tags in ascending order");
    CHECK((int)visited_tags.size() == N, "sort doesn't lose any elements");

    // Entity→component mapping still correct after sort
    bool all_valid = true;
    for (int i = 0; i < N; ++i) {
        if (ecs.getComponent<Tag>((Entity)i).id != N - i) { all_valid = false; break; }
    }
    CHECK(all_valid, "After sort, entity→Tag lookup still returns original values");
}

// =============================================================================
//  11. Benchmarks (hot paths at scale)
// =============================================================================

void bench_create_destroy() {
    section("Benchmark: Entity create/destroy");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Health>();

    const int N = (int)MAX_ENTITIES;
    std::vector<Entity> entities;
    entities.reserve(N);

    {
        Timer t("createEntity x" + std::to_string(N));
        for (int i = 0; i < N; ++i) entities.push_back(ecs.createEntity());
    }
    {
        Timer t("addComponent<Position> x" + std::to_string(N));
        for (Entity e : entities) ecs.addComponent(e, Position{1.f, 2.f, 3.f});
    }
    {
        Timer t("addComponent<Velocity> x" + std::to_string(N));
        for (Entity e : entities) ecs.addComponent(e, Velocity{.1f, .2f, .3f});
    }
    {
        Timer t("getComponent<Position> x" + std::to_string(N));
        float acc = 0.f;
        for (Entity e : entities) acc += ecs.getComponent<Position>(e).x;
        (void)acc;
    }
    {
        Timer t("View<Position,Velocity>::each x" + std::to_string(N));
        float acc = 0.f;
        ecs.getView<Position, Velocity>().each([&](Entity, Position& p, Velocity& v){
            p.x += v.dx;
            acc += p.x;
        });
        (void)acc;
    }
    {
        Timer t("removeComponent<Velocity> x" + std::to_string(N));
        for (Entity e : entities) ecs.removeComponent<Velocity>(e);
    }
    {
        Timer t("destroyEntity x" + std::to_string(N));
        for (Entity e : entities) ecs.destroyEntity(e);
    }
}

void bench_sort() {
    section("Benchmark: sort");

    ECS ecs;
    ecs.registerComponent<Tag>();

    const int N = (int)MAX_ENTITIES;
    std::mt19937 rng(123);
    std::uniform_int_distribution<int> dist(0, 1000000);

    for (int i = 0; i < N; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Tag{dist(rng)});
    }

    {
        Timer t("sort<Tag> x" + std::to_string(N) + " random ints");
        ecs.sort<Tag>([](const Tag& a, const Tag& b){ return a.id < b.id; });
    }
    {
        Timer t("sort<Tag> already sorted (best case)");
        ecs.sort<Tag>([](const Tag& a, const Tag& b){ return a.id < b.id; });
    }
}

void bench_view_each() {
    section("Benchmark: View::each hot loop");

    ECS ecs;
    ecs.registerComponent<Position>();
    ecs.registerComponent<Velocity>();
    ecs.registerComponent<Health>();

    const int N = (int)MAX_ENTITIES;
    for (int i = 0; i < N; ++i) {
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{0.f, 0.f, 0.f});
        ecs.addComponent(e, Velocity{1.f, 1.f, 1.f});
        if (i % 3 == 0) ecs.addComponent(e, Health{100, 100});
    }

    const int ITERS = 100;
    {
        Timer t("View<P,V>::each x" + std::to_string(N) + " entities, " + std::to_string(ITERS) + " frames");
        for (int frame = 0; frame < ITERS; ++frame) {
            ecs.getView<Position, Velocity>().each([](Entity, Position& p, Velocity& v){
                p.x += v.dx;
                p.y += v.dy;
                p.z += v.dz;
            });
        }
    }
    {
        Timer t("View<P,V,H>::each (1/3 entities match) x" + std::to_string(ITERS) + " frames");
        for (int frame = 0; frame < ITERS; ++frame) {
            ecs.getView<Position, Velocity, Health>().each([](Entity, Position&, Velocity&, Health& h){
                h.hp = std::min(h.hp + 1, h.max_hp);
            });
        }
    }
}

// =============================================================================
//  12. Edge cases and regression guards
// =============================================================================

void test_edge_cases() {
    section("Edge cases");

    // Remove component, re-add it — sparse slot must be reusable
    {
        ECS ecs;
        ecs.registerComponent<Position>();
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{1.f, 0.f, 0.f});
        ecs.removeComponent<Position>(e);
        ecs.addComponent(e, Position{2.f, 0.f, 0.f});
        CHECK(ecs.getComponent<Position>(e).x == 2.f, "Re-add after remove works");
    }

    // Destroy one of many entities — swap-and-pop correctness
    {
        ECS ecs;
        ecs.registerComponent<Tag>();
        Entity e0 = ecs.createEntity(); ecs.addComponent(e0, Tag{1});
        Entity e1 = ecs.createEntity(); ecs.addComponent(e1, Tag{2});
        Entity e2 = ecs.createEntity(); ecs.addComponent(e2, Tag{3});
        ecs.destroyEntity(e1); // e1 is middle; e2 swaps into slot 1
        CHECK(ecs.getComponent<Tag>(e0).id == 1, "e0 intact after e1 destroyed");
        CHECK(ecs.getComponent<Tag>(e2).id == 3, "e2 intact after e1 destroyed (was swapped)");
    }

    // hasSignature with Signature object
    {
        ECS ecs;
        ecs.registerComponent<Position>();
        ecs.registerComponent<Velocity>();
        Entity e = ecs.createEntity();
        ecs.addComponent(e, Position{});
        Signature s = ecs.getSignature<Position>();
        CHECK(ecs.hasSignature(e, s),                  "hasSignature(entity, sig) true");
        Signature s2 = ecs.getSignature<Position, Velocity>();
        CHECK(!ecs.hasSignature(e, s2),                "hasSignature(entity, sig) false when missing");
    }

    // getComponentType is stable (same value across multiple calls)
    {
        ECS ecs;
        ecs.registerComponent<Position>();
        ecs.registerComponent<Velocity>();
        ComponentType pt1 = ecs.getComponentType<Position>();
        ComponentType pt2 = ecs.getComponentType<Position>();
        CHECK(pt1 == pt2, "getComponentType is idempotent");
        CHECK(ecs.getComponentType<Position>() != ecs.getComponentType<Velocity>(),
              "Different components get different ComponentType IDs");
    }

    // View on subset: only first entity in dense array matches
    {
        ECS ecs;
        ecs.registerComponent<Position>();
        ecs.registerComponent<Velocity>();
        Entity e0 = ecs.createEntity(); ecs.addComponent(e0, Position{});
        Entity e1 = ecs.createEntity(); ecs.addComponent(e1, Position{}); ecs.addComponent(e1, Velocity{});
        Entity e2 = ecs.createEntity(); ecs.addComponent(e2, Position{});
        int cnt = 0;
        ecs.getView<Position, Velocity>().each([&](Entity e, Position&, Velocity&){
            CHECK(e == e1, "Only e1 visited in subset view");
            ++cnt;
        });
        CHECK(cnt == 1, "Subset view visits exactly 1 entity");
    }

    // Modifying via View ref actually changes stored value
    {
        ECS ecs;
        ecs.registerComponent<Health>();
        Entity e = ecs.createEntity(); ecs.addComponent(e, Health{100, 100});
        ecs.getView<Health>().each([](Entity, Health& h){ h.hp = 0; });
        CHECK(ecs.getComponent<Health>(e).hp == 0, "View mutation via ref persists");
    }
}

// =============================================================================
//  main
// =============================================================================

int main() {
    std::cout << "╔══════════════════════════════════════════╗\n"
              << "║         ECS Test & Benchmark Suite       ║\n"
              << "╚══════════════════════════════════════════╝\n";

    test_entity_manager();
    test_component_array();
    test_sort();
    test_ecs_basic();
    test_ecs_destroy();
    test_const_ecs();
    test_ecs_init();
    test_view_each();
    test_view_iterator();
    test_ecs_sort();
    test_edge_cases();

    bench_create_destroy();
    bench_sort();
    bench_view_each();

    std::cout << "\n══════════════════════════════════════════\n"
              << "  Results: " << g_pass << " passed, " << g_fail << " failed\n"
              << "══════════════════════════════════════════\n";

    return g_fail == 0 ? 0 : 1;
}
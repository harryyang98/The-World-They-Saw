//
// Created by Gary on 1/19/2021.
//

#include "MoveableWall.hpp"

#include "PhysicsObject.hpp"

ECS::Entity MoveableWall::createMoveableWall(vec2 location, vec2 size, float rotation,
                                             COLLISION_HANDLER overlap,
                                             COLLISION_HANDLER hit){
    auto entity = ECS::Entity();

    std::string key = "wall";
    ShadedMesh& resource = cache_resource(key);
    if (resource.mesh.vertices.empty())
    {
        resource = ShadedMesh();
        resource.mesh.vertices.emplace_back(ColoredVertex{vec3 {-0.5, 0.5, -0.02}, vec3{0.0,0.0,0.0}});
        resource.mesh.vertices.emplace_back(ColoredVertex{vec3{0.5, 0.5, -0.02}, vec3{0.0,0.0,0.0}});
        resource.mesh.vertices.emplace_back(ColoredVertex{vec3{0.5, -0.5, -0.02}, vec3{0.0,0.0,0.0}});
        resource.mesh.vertices.emplace_back(ColoredVertex{vec3{-0.5, -0.5, -0.02}, vec3{0.0,0.0,0.0}});

        resource.mesh.vertex_indices = std::vector<uint16_t>({0, 2, 1, 0, 3, 2});

        RenderSystem::createColoredMesh(resource, "salmon");
    }

    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ECS::registry<ShadedMeshRef>.emplace(entity, resource);

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = location;
    motion.angle = rotation;
    motion.velocity = { 0.f, 0.f };
    motion.scale = size;
    motion.zValue = ZValuesMap["Wall"];

    PhysicsObject physicsObject;
    physicsObject.object_type = MOVEABLEWALL;
    physicsObject.fixed = false;
    physicsObject.mass = 30;
    physicsObject.attach(Overlap,overlap);
    physicsObject.attach(Hit, hit);
    ECS::registry<PhysicsObject>.insert(entity, physicsObject);
    //motion.box = size;
    // Create and (empty) Salmon component to be able to refer to all turtles
    ECS::registry<MoveableWall>.emplace(entity);
    resource.texture.color = {1,1,1};
    return entity;
}

ECS::Entity MoveableWall::createCustomMoveableWall(vec2 location, vec2 scale, std::vector<ColoredVertex> vertexes,
                                                   vec2 world_velocity,
                                             float rotation,
                                             COLLISION_HANDLER overlap,
                                             COLLISION_HANDLER hit) {
    assert(vertexes.size() >= 3);
    auto entity = ECS::Entity();
    vec2 sum = vec2{};
    for(int index = 1; index < vertexes.size(); index++){
        sum += vec2{vertexes[index].position.x, vertexes[index].position.y};
    }
    sum /= (float)vertexes.size();
    for (auto& vertex: vertexes){
        vertex.position.x -= sum.x;
        vertex.position.y -= sum.y;
    }

    Global_Meshes::meshes.push_back(ShadedMesh{});
    ShadedMesh& resource = Global_Meshes::meshes.back();
    resource.mesh.vertices.emplace_back(vertexes[0]);
    resource.mesh.vertices.emplace_back(vertexes[1]);
    resource.mesh.vertex_indices = std::vector<uint16_t>{};
    for(int index = 2; index < vertexes.size(); index++) {
        resource.mesh.vertices.emplace_back(vertexes[index]);
        resource.mesh.vertex_indices.emplace_back(0);
        resource.mesh.vertex_indices.emplace_back(index);
        resource.mesh.vertex_indices.emplace_back(index-1);
    }


    printf("%d\n", resource.mesh.vertices.size());
    for(int x : resource.mesh.vertex_indices){
        printf("%d,", x);
    }
    printf("\n");
    RenderSystem::createColoredMesh(resource, "salmon");


    // Store a reference to the potentially re-used mesh object (the value is stored in the resource cache)
    ShadedMeshRef r = ShadedMeshRef{Global_Meshes::meshes.back()};
    ECS::registry<ShadedMeshRef>.insert(entity, r);

    // Setting initial motion values
    Motion& motion = ECS::registry<Motion>.emplace(entity);
    motion.position = location;
    motion.angle = rotation;
    motion.preserve_world_velocity = world_velocity;
    motion.scale = scale;
    motion.zValue = ZValuesMap["Wall"];

    PhysicsObject&  physicsObject = ECS::registry<PhysicsObject>.emplace(entity);
    physicsObject.object_type = MOVEABLEWALL;
    physicsObject.vertex.clear();
    physicsObject.faces.clear();


    vec3 pos0 = vertexes[0].position;
    physicsObject.vertex.emplace_back(PhysicsVertex{vertexes[0].position});
    for(int index = 1; index < vertexes.size(); index++){
        vec3 pos = vertexes[index].position;
        physicsObject.vertex.emplace_back(PhysicsVertex{pos});
        physicsObject.faces.emplace_back(std::pair<int,int>{index-1, index});
    }
    physicsObject.faces.emplace_back(std::pair<int,int>{vertexes.size() - 1, 0});
    sum *= motion.scale;
    float ca = cos(motion.angle);
    float sa = sin(motion.angle);
    vec2 v = { sum.x * ca - sum.y * sa,  sum.x * sa + sum.y * ca };

    motion.position += v;

    physicsObject.fixed = false;
    physicsObject.mass = 30;
//    physicsObject.attach(Overlap,overlap);
// Has bug doing this
//    physicsObject.attach(Hit, LevelLoader::wall_hit);
    physicsObject.attach(Hit, PhysicsObject::handle_collision);
    //motion.box = size;
    // Create and (empty) Salmon component to be able to refer to all turtles
    ECS::registry<MoveableWall>.emplace(entity);

    return entity;
}
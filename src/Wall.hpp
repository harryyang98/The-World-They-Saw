//
// Created by Gary on 1/19/2021.
//

#pragma once


#include "common.hpp"
#include "tiny_ecs.hpp"

class Wall {
public:
    static ECS::Entity createWall(vec2 location, vec2 size, float rotation);
};


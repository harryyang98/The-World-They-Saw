#pragma once

#include "common.hpp"
#include "tiny_ecs.hpp"
#include <levelLoader.hpp>

enum class ButtonType {
	DEFAULT_BUTTON, START, LEVEL_SELECT, QUIT
};

struct Button {
	static ECS::Entity createButton(ButtonType buttonType, vec2 position, std::function<void(ECS::Entity&, const  ECS::Entity&)> overlap);

	ButtonType buttonType = ButtonType::DEFAULT_BUTTON;

	static std::map<ButtonType, std::string> buttonNames;
};
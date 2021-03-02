#include "EnemyAi.hpp"
#include "Enemy.hpp"
#include "soldier.hpp"
#include "debug.hpp"

void EnemyAISystem::step(float elapsed_ms, vec2 window_size_in_game_units) 
{
	if (!ECS::registry<Enemy>.components.empty())
	{
		for (auto& enemy: ECS::registry<Enemy>.entities)
		{
			EnemyAISystem::makeDecision(enemy, elapsed_ms);
		}
	}
}

void EnemyAISystem::makeDecision(ECS::Entity enemy_entity, float elapsed_ms)
{
	// std::cout << "isEnemyExists\n";
	auto& enemy_motion = ECS::registry<Motion>.get(enemy_entity);
	auto& enemy = ECS::registry<Enemy>.get(enemy_entity);
	// std::cout << "makeDecision: " << &soldier_motion << "\n";
	if (EnemyAISystem::isSoldierExists())
	{
		ECS::Entity& soldier = ECS::registry<Soldier>.entities[0];
		auto& soldierMotion = ECS::registry<Motion>.get(soldier);

		Enemy::state aState = enemy.enemyState;
		if (EnemyAISystem::isSoldierExistsInRange(enemy_motion, soldierMotion, 300) && aState == Enemy::WALK_FORWARD) {
			enemy.enemyState = Enemy::WALK_BACKWARD;
			EnemyAISystem::walkBackwardAndShoot(enemy_motion, soldierMotion);

		}
		else if (EnemyAISystem::isSoldierExistsInRange(enemy_motion, soldierMotion, 400) && aState == Enemy::WALK_BACKWARD) {
			enemy.enemyState = Enemy::WALK_BACKWARD;
			EnemyAISystem::walkBackwardAndShoot(enemy_motion, soldierMotion);
		}
		else
		{
			enemy.enemyState = Enemy::WALK_FORWARD;
			EnemyAISystem::walkRandom(enemy_motion);
		}
	}
	else
	{
		enemy.enemyState = Enemy::IDLE;
		EnemyAISystem::idle(enemy_motion);
	}
}

bool EnemyAISystem::isSoldierExists()
{
	return !ECS::registry<Soldier>.components.empty();
}

bool EnemyAISystem::isSoldierExistsInRange(Motion& enemyMotion, Motion& soldierMotion, float range)
{
	return sqrt(pow(soldierMotion.position.x - enemyMotion.position.x, 2) + pow(soldierMotion.position.y - enemyMotion.position.y, 2)) < range;
}

void EnemyAISystem:: idle(Motion& enemyMotion)
{
	enemyMotion.velocity = vec2{ 0.f, 0.f };
}

void EnemyAISystem:: walkBackwardAndShoot(Motion& enemyMotion, Motion& soldierMotion)
{
	// std::cout << "walkBackwardAndShoot: " << &soldierMotion << "\n";
	vec2 soldierPos = soldierMotion.position;
	vec2 enemyPos = enemyMotion.position;


	vec2 posDiff = vec2{ soldierPos.x - enemyPos.x, soldierPos.y - enemyPos.y };
	float distance = sqrt(pow(enemyPos.x - soldierPos.x, 2)) + sqrt(pow(enemyPos.y - soldierPos.y, 2));
	vec2 normalized = vec2{ posDiff.x / distance, posDiff.y / distance };

	enemyMotion.velocity = vec2{ normalized.x * -100.f, normalized.y * -100.f };
	std::cout << enemyMotion.velocity.x<< " ," << enemyMotion.velocity.y << "\n";

	// soldierMotion.velocity = vec2{ -100.f, 0 };

	// std::cout << "backward: " << soldierMotion.velocity.x;

	auto dir = soldierPos - enemyPos;
	float rad = atan2(dir.y, dir.x);
	enemyMotion.angle = rad;

	// std::cout << "backward: " << soldierMotion.velocity.x << ", " << soldierMotion.velocity.y << "\n";
}

void EnemyAISystem::walkRandom(Motion& enemyMotion)
{

}
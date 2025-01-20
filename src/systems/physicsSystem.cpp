#include "systems/physicsSystem.hpp"

#include "components.hpp"
#include "components/inventory.hpp"
#include "game.hpp"
#include "managers/entityManager.hpp"
#include "managers/systemManager.hpp"
#include "opengl/texture.hpp"
#include "scene.hpp"
#include "scenes/chunk.hpp"
#include "scenes/level.hpp"
#include "systems/UISystem.hpp"
#include "third_party/Eigen/Core"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <cstddef>
#include <string>

#ifdef IMGUI
#include "imgui.h"

#include <format>
#endif

// The physicsSystem is in charge of the collision and mouvements
PhysicsSystem::PhysicsSystem() noexcept : mGame(Game::getInstance()) {}

// Performance benchmark:
// Before enabling checks (98f1275078cad77b0b7a3145b4e57c6f098bd078): 2560632.097561ns avg (391 FPS)
// After enabling checks  (1b5262e0b971bb3b8308703605b070f7b8d31608): 2757778.578947ns avg (363 FPS)
// Diff:                                                               197146.481386ns avg ( 28 FPS)

// Whole prog perf
// GCC                                                                2917566.270833ns avg (343 FPS)
// Parralel collision detection std::execution::par_unseq             2887647.807018ns avg (346 FPS)
// Aww soo many frances lost

void PhysicsSystem::update(Scene* scene, const float delta) {
	constexpr const static float G = 1200.0f;
	constexpr const static float jumpForce = 600.0f;

	if (!mGame->getSystemManager()->getUISystem()->empty()) {
		return;
	}

	const auto blocks = scene->view<Components::collision, Components::block>();
	for (const auto entity : scene->view<Components::position, Components::velocity>()) {
		bool onGround = false;

		if (scene->get<Components::velocity>(entity).mVelocity.y() < 1.0f) {
			// Look cache for bellow block
			if (!mCache.lastAbove.contains(entity) ||
			    !scene->contains<Components::block>(mCache.lastAbove[entity]) ||
			    !(onGround = collidingBellow(scene, entity, mCache.lastAbove[entity]))) {
				for (const auto& block : blocks) {
					if (collidingBellow(scene, entity, block)) {
						onGround = true;
						mCache.lastAbove[entity] = block;

						break;
					}
				}
			}
		}

		auto& velocity = scene->get<Components::velocity>(entity).mVelocity;
		if (onGround) {
			// We can jump IF the entity is a misc entity with the jump flag, and the up key is pressed, and
			// we are on the ground
			if (entity == mGame->getPlayerID() && scene->getSignal(SDL_SCANCODE_SPACE)) {
				velocity.y() = jumpForce;
			} else {
				velocity.y() = 0.0f;
			}
		} else {
			velocity.y() -= G * delta;
		}

		if (scene->contains<Components::animated_texture>(entity)) {
			auto& texture = scene->get<Components::animated_texture>(entity);
			if (!onGround) {
				texture.mSelect = 2 * 8 + (velocity.y() > 0);
			} else {
				static float step = 0;
				if (nearZero(velocity.x())) {
					texture.mSelect = 0;
					step = 0;
				} else {
					step += delta * 10;
					if (step >= 4.0f) {
						step = 0.0f;
					}

					texture.mSelect = 8 + static_cast<int>(step);
				}
			}

			if (!nearZero(velocity.x())) {
				// Flip if going left
				texture.mFlip = velocity.x() < 0;
			}
		}

		scene->get<Components::position>(entity).mPosition += velocity * delta;
		velocity.x() *= 0.7;
	}

	itemPhysics(scene);
}

void PhysicsSystem::collide(Scene* scene) {
	// Populate cache once
	const auto leftChunk = (mGame->getLevel()->getPosition() - 1) * Chunk::CHUNK_WIDTH;

	// Reset cache
	for (auto& row : mCache.chunk) {
		for (auto& block : row) {
			block = 0;
		}
	}

	for (const auto& block : scene->view<Components::collision, Components::block>()) {
		const auto pos = scene->get<Components::block>(block).mPosition;
		mCache.chunk[pos.x() - leftChunk][pos.y()] = block;

		if (AABBxAABB(scene, mGame->getPlayerID(), block)) {
			pushBack(scene, mGame->getPlayerID(), block);
		}
	}

	// Get a list of all the entities we need to check
	const auto entities = scene->view<Components::collision, Components::position>();
	for (const auto& entity : entities) {
		const auto pos = scene->get<Components::position>(entity).mPosition;
		const auto cell = mCache.chunk[(pos.x() / Components::block::BLOCK_SIZE) - leftChunk]
					      [pos.y() / Components::block::BLOCK_SIZE];

		if (cell) {
			if (AABBxAABB(scene, entity, cell)) {
				pushBack(scene, entity, cell);
			}
		}
	}

	// Debug editor
#if defined(IMGUI) && defined(DEBUG)
	static bool editor = false;
	ImGui::Begin("Developer menu");
	ImGui::Checkbox("Collision box editor", &editor);
	ImGui::End();

	if (editor) {
		scene->getSignal("collisionEditor"_u) = true;

		ImGui::Begin("Collision editor");

		for (const auto& entity : entities) {
			if (ImGui::TreeNode(std::format("Entity {}", entity).data())) {
				if (scene->contains<Components::position>(entity)) {
					ImGui::SliderFloat2(std::format("Position for entity {}", entity).data(),
							    scene->get<Components::position>(entity).mPosition.data(),
							    -10000.0f, 10000.0f);
				}

				if (scene->contains<Components::block>(entity)) {
					ImGui::SliderInt2(std::format("Position for entity {}", entity).data(),
							  scene->get<Components::block>(entity).mPosition.data(), -10,
							  10);
				}

				ImGui::SliderFloat2(std::format("Offset for entity {}", entity).data(),
						    scene->get<Components::collision>(entity).mOffset.data(), -500,
						    500);

				ImGui::SliderFloat2(std::format("Size for entity {}", entity).data(),
						    scene->get<Components::collision>(entity).mSize.data(), 0.0f,
						    1000.0f);

				ImGui::TreePop();
			}
		}

		ImGui::End();
	}
#endif
}

bool PhysicsSystem::AABBxAABB(const Scene* scene, const EntityID entityID, const EntityID blockID) const {
	using namespace Components;

	const Eigen::Vector2f minA = scene->get<position>(entityID).mPosition + scene->get<collision>(entityID).mOffset;
	const Eigen::Vector2f maxA = minA + scene->get<collision>(entityID).mSize;

	const auto& blockCollision = scene->get<collision>(blockID);
	Eigen::Vector2f minB = scene->get<block>(blockID).mPosition.template cast<float>() * block::BLOCK_SIZE;
	minB += blockCollision.mOffset;

	const Eigen::Vector2f maxB = minB + blockCollision.mSize;

	// If one of these four are true, it means the cubes are not intersecting
	const bool notIntercecting = maxA.x() <= minB.x()     // Amax to the left of Bmin
				     || maxA.y() <= minB.y()  // Amax to the bottom of Bmin
				     || maxB.x() <= minA.x()  // Bmax to the left of Amax
				     || maxB.y() <= minA.y(); // Bmax to the bottom of Amin

	// So return the inverse of not intersecting
	return !notIntercecting;
}

bool PhysicsSystem::collidingBellow(const class Scene* scene, const EntityID entityID, const EntityID blockID) const {
	using namespace Components;

	const Eigen::Vector2f minEntity =
		scene->get<position>(entityID).mPosition + scene->get<collision>(entityID).mOffset;

	// They are definetly not touching the ground when having a upwards velocity
	const Eigen::Vector2f maxEntity = minEntity + scene->get<collision>(entityID).mSize;

	const auto& blockCollision = scene->get<collision>(blockID);
	const Eigen::Vector2f minBlock =
		scene->get<block>(blockID).mPosition.template cast<float>() * block::BLOCK_SIZE +
		blockCollision.mOffset;

	const Eigen::Vector2f maxBlock = minBlock + blockCollision.mSize;

	// on a x level
	const bool notIntercecting = maxEntity.x() - 6 <= minBlock.x()	  // entity to the left of b
				     || maxBlock.x() - 6 <= minEntity.x() // b to the left of main
				     || maxEntity.y() <= minBlock.y();

	if (notIntercecting) {
		return false;
	}

	// Remove dot five to be more precise
	if ((minEntity.y() - 0.5f) < maxBlock.y()) {
		return true;
	}

	return false;
}

// TODO: Read
// https://gamedev.stackexchange.com/questions/38891/making-an-efficient-collision-detection-system/38893#38893
// TODO: Read
// https://gamedev.stackexchange.com/questions/38613/how-do-i-detect-collision-between-movie-clips/38635#38635
// Corners:
// https://gamedev.stackexchange.com/questions/17502/how-to-deal-with-corner-collisions-in-2d?noredirect=1&lq=1
// https://gamedev.stackexchange.com/questions/29371/how-do-i-prevent-my-platformers-character-from-clipping-on-wall-tiles?noredirect=1&lq=1
/*
 * Here we first calculate the positions of the squares
 * Then we calculate the center of the cubes from the positions
 * After that we calculate the distance between the two center of cubes
 * We then SDL_assert that the objects are collising, if not that means `AABBxAABB` has problems
 * Right after we get the depth of the X axis and Y axis that are overlapping
 * Thus we can determine which side we are colliding on by running `std::min(X, Y)`
 * From this info, we will be able to calculate the ammount we need to push back
 * Which has two scenarios:
 * 1. One object is static, so find the one that isn't calculate neg or pos, and push
 * 2. Both aren't static, thus push back both by half the overlap
 * (If the objects are both stationary, pass)
 */
void PhysicsSystem::pushBack(class Scene* scene, const EntityID entity, EntityID block) {
	/*
	 * Thx stack https://gamedev.stackexchange.com/questions/18302/2d-platformer-collisions
	 * See
	 * https://github.com/MonoGame/MonoGame.Samples/blob/3.8.2/Platformer2D/Platformer2D.Core/Game/Player.cs
	 * https://github.com/MonoGame/MonoGame.Samples/blob/3.8.2/Platformer2D/Platformer2D.Core/Game/RectangleExtensions.cs#L30
	 */

	// This is the position of our entity
	const Eigen::Vector2f leftEntity =
		scene->get<Components::position>(entity).mPosition + scene->get<Components::collision>(entity).mOffset;
	const Eigen::Vector2f centerEntity = leftEntity + scene->get<Components::collision>(entity).mSize / 2;

	// And the position of the block
	const auto& blockCollision = scene->get<Components::collision>(block);
	const auto& blockTexture = scene->get<Components::texture>(block);
	const auto& blockPosition = scene->get<Components::block>(block).mPosition;

	const Eigen::Vector2f leftBlock =
		Eigen::Vector2f(static_cast<float>(blockPosition.x()) * blockTexture.mTexture->getWidth(),
				static_cast<float>(blockPosition.y()) * blockTexture.mTexture->getHeight()) +
		blockCollision.mOffset;
	const Eigen::Vector2f centerB = leftBlock + scene->get<Components::collision>(block).mSize / 2;

	const Eigen::Vector2f distance = centerEntity - centerB;
	const Eigen::Vector2f minDistance =
		(scene->get<Components::collision>(entity).mSize + blockCollision.mSize) / 2;

	SDL_assert(!(SDL_abs(distance.x()) > minDistance.x() || SDL_abs(distance.y()) > minDistance.y()) &&
		   "The objects are not colliding?");

	// Calculate the collision depth
	const float depthX = (distance.x() > 0 ? minDistance.x() : -minDistance.x()) - distance.x();
	const float depthY = (distance.y() > 0 ? minDistance.y() : -minDistance.y()) - distance.y();

	const Eigen::Vector2f depth = Eigen::Vector2f(depthX, depthY);

	if (SDL_abs(depth.x()) <= SDL_abs(depth.y())) {
		scene->get<Components::position>(entity).mPosition.x() += depth.x();
	} else {
		scene->get<Components::position>(entity).mPosition.y() += depth.y();
	}
}

void PhysicsSystem::itemPhysics(class Scene* scene) {
	const auto players = scene->view<Components::position, Components::inventory>();
	for (const auto item : scene->view<Components::position, Components::item>()) {
		// Soo test the blocks with items, only if it's close
		// Picking the stuff up
		for (const auto entity : players) {
			if ((scene->get<Components::position>(item).mPosition -
			     scene->get<Components::position>(entity).mPosition)
				    .squaredNorm() < PICK_UP_RANGE_SQ) {
				if (scene->get<Components::inventory>(entity).mInventory->tryPick(scene, item)) {
					scene->erase(item);

					break;
				}
			}
		}
	}
}

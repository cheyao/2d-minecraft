#include "actors/cube.hpp"

#include "components/meshComponent.hpp"
#include "game.hpp"
#include "third_party/glad/glad.h"
#include <functional>

Cube::Cube(class Game* owner) : Actor(owner) {
	const std::vector<Vertex> verticesCube = {
		{{-1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 0 FLD
		{{+1.0f, -1.0f, +1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 1 FRD
		{{-1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 2 FLU
		{{+1.0f, +1.0f, +1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 3 FRU
		{{-1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 4 BLD
		{{+1.0f, -1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 5 BRD
		{{-1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}, // 6 BLU
		{{+1.0f, +1.0f, -1.0f}, {0.0f, 0.0f, 0.0f}, {0.0f, 0.0f}}  // 7 BRU
	};
	const std::vector<GLuint> indicesCube = {2, 7, 6, 2, 3, 7,	// Top
											 0, 4, 5, 0, 5, 1,	// Bottom
											 0, 2, 6, 0, 6, 4,	// Left
											 1, 7, 3, 1, 5, 7,	// Right
											 0, 3, 2, 0, 1, 3,	// Front
											 4, 6, 7, 4, 7, 5}; // Back

	MeshComponent* const cube = new MeshComponent(this, verticesCube, indicesCube, {}, 200);
	cube->setVert("cube.vert");
	cube->setFrag("cube.frag");
	cube->setDrawFunc(std::bind(glDrawElementsInstanced, std::placeholders::_1,
								std::placeholders::_2, std::placeholders::_3, std::placeholders::_4,
								5));

	setScale(0.5);
}
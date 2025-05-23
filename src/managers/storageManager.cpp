#include "managers/storageManager.hpp"

#include "game.hpp"
#include "scenes/level.hpp"
#include "third_party/rapidjson/document.h"
#include "third_party/rapidjson/error/en.h"
#include "third_party/rapidjson/rapidjson.h"
#include "third_party/rapidjson/writer.h"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <cstddef>
#include <memory>
#include <string>

/*
 * A World shall consist of 3 fields:
 * 1. "version"
 * 2. "name"
 * 3. "data"
 */

StorageManager::StorageManager() : mGame(Game::getInstance()) {}

StorageManager::~StorageManager() { SDL_Log("Storage Manager destroyed"); }

void StorageManager::save() {
	SDL_Log("Saving state");

	SDL_Storage* storage = SDL_OpenUserStorage("cyao", "2d-minecraft", 0);
	if (storage == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to open user storage: %s\033[0m",
				SDL_GetError());
		ERROR_BOX("Failed to open storage: Not saving");

		return;
	}

	std::size_t i = 0;
	while (!SDL_StorageReady(storage)) {
		SDL_Delay(10);
		++i;
		if (i >= 100) {
			SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to open user storage: %s\033[0m",
					SDL_GetError());
			ERROR_BOX("Failed to open storage: Abandoning saving operation");

			return;
		}
	}

	saveState(storage);

	SDL_CloseStorage(storage);

	SDL_Log("\033[32mSuccessfully saved state\033[0m");
}

bool StorageManager::restore() {
	SDL_Log("Restoring state");

	SDL_Storage* storage = SDL_OpenUserStorage("cyao", "2d-minecraft", 0);
	if (storage == nullptr) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to open user storage: %s\033[0m",
				SDL_GetError());
		ERROR_BOX("Failed to open storage");

		return 1;
	}

	std::size_t times = 0;
	while (!SDL_StorageReady(storage)) {
		SDL_Delay(10);

		++times;
		if (times >= 100) {
			SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to open user storage: %s\033[0m",
					SDL_GetError());
			ERROR_BOX("Failed to open storage");

			return 1;
		}
	}

	if (restoreState(storage) != 0) {
		return 1;
	}

	SDL_CloseStorage(storage);

	SDL_Log("\033[32mSuccessfully loaded state\033[0m");

	return 0;
}

// TODO: Binary json https://json.nlohmann.me/features/binary_formats/
bool StorageManager::restoreState(SDL_Storage* storage) {
	if (!SDL_GetStoragePathInfo(storage, "worlds.json", nullptr)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mNo save index file found! Loading new state\033[0m");

		return 1;
	}

	SDL_PathInfo info;
	SDL_GetStoragePathInfo(storage, "worlds.json", &info);

	std::unique_ptr<char[]> buffer(new char[info.size + 1]);
	SDL_ReadStorageFile(storage, "worlds.json", buffer.get(), info.size);
	buffer[info.size] = 0; // Ensure null terminasion

	rapidjson::Document worlds;
	if (worlds.Parse(buffer.get()).HasParseError()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to parse json (offset %u): %s\033[0m",
				(unsigned)worlds.GetErrorOffset(), rapidjson::GetParseError_En(worlds.GetParseError()));
		ERROR_BOX("Failed to load save file");

		return 1;
	}

	if (worlds["version"].GetUint64() != LATEST_WORLD_VERSION) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
				"\033[31mERROR! The save file version is not for this game engine version\033[0m");
		ERROR_BOX("ERROR! The save file version is not for this game engine version");

		return 1;
	}

	if (worlds["worlds"].Empty()) {
		SDL_Log("\033[31mWorlds empty! Returning\033[0m");

		return 1;
	}

	for (rapidjson::SizeType i = 0; i < worlds["worlds"].Size(); ++i) {
		SDL_Log("Found world %s", worlds["worlds"][i].GetString());
	}

	return loadWorld(storage, worlds["worlds"][0].GetString());
}

bool StorageManager::loadWorld(struct SDL_Storage* storage, const std::string& world) {
	if (!SDL_GetStoragePathInfo(storage, (world + ".json").data(), nullptr)) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mNo save file %s found! Loading new state\033[0m",
				world.data());

		return 1;
	}

	SDL_PathInfo info;
	SDL_GetStoragePathInfo(storage, (world + ".json").data(), &info);

	std::unique_ptr<char[]> buffer(new char[info.size + 1]);
	SDL_ReadStorageFile(storage, (world + ".json").data(), buffer.get(), info.size);
	buffer[info.size] = 0; // Ensure null terminasion

	rapidjson::Document level;
	if (level.Parse(buffer.get()).HasParseError()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to parse json (offset %u): %s\033[0m",
				(unsigned)level.GetErrorOffset(), rapidjson::GetParseError_En(level.GetParseError()));
		ERROR_BOX("Failed to load save file");

		return 1;
	}

	if (level["version"].GetUint64() != LATEST_LEVEL_VERSION) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
				"\033[31mERROR! The save file version is not for this game engine version\033[0m");
		ERROR_BOX("ERROR! The save file version is not for this game engine version");

		return 1;
	}

	SDL_assert(level["name"] == "Level"); // TODO: More option

	mGame->getLevel()->load(level["data"]);

	return 0;
}

void StorageManager::saveState(SDL_Storage* storage) {
	rapidjson::Document worlds;
	bool oldWorlds = false;

	// If there is already level data
	if (SDL_GetStoragePathInfo(storage, "worlds.json", nullptr)) {
		oldWorlds = true;

		SDL_PathInfo info;
		SDL_GetStoragePathInfo(storage, "worlds.json", &info);

		std::unique_ptr<char[]> buffer(new char[info.size + 1]);
		SDL_ReadStorageFile(storage, "worlds.json", buffer.get(), info.size);
		buffer[info.size] = 0; // Ensure null terminasion

		if (worlds.Parse(buffer.get()).HasParseError()) {
			SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to parse json (offset %u): %s\033[0m",
					(unsigned)worlds.GetErrorOffset(),
					rapidjson::GetParseError_En(worlds.GetParseError()));
			ERROR_BOX("Failed to load old save file, ignoring");
		}
	} else {
		worlds.SetObject();
	}

	// FIXME: Non-hard writted world name, ask for user input
	constexpr const static char* worldName = "world";

	// TODO: Save some more world info
	// TODO: Self-recovery incase of corrupted world save
	worlds.AddMember("version", rapidjson::Value().SetUint64(LATEST_WORLD_VERSION).Move(), worlds.GetAllocator());

	if (!worlds.HasMember("worlds")) {
		worlds.AddMember("worlds", rapidjson::Value(rapidjson::kArrayType).Move(), worlds.GetAllocator());
	}

	if (std::find(worlds["worlds"].Begin(), worlds["worlds"].End(), worldName) == worlds["worlds"].End()) {
		worlds["worlds"].PushBack(rapidjson::Value(worldName, worlds.GetAllocator()).Move(),
					  worlds.GetAllocator());
	}

	if (oldWorlds && !SDL_RenameStoragePath(storage, "worlds.json", "worlds.json.old")) {
		SDL_Log("\033[31mFailed to rename world.json to worlds.json.old %s, ignoring...\033[0m",
			SDL_GetError());
	}

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	worlds.Accept(writer);

	SDL_WriteStorageFile(storage, "worlds.json", sb.GetString(), sb.GetSize());

	saveWorld(storage, worldName);
}

void StorageManager::saveWorld(struct SDL_Storage* storage, const std::string& world) {
	if (SDL_GetStoragePathInfo(storage, (world + ".json").data(), nullptr)) {
		if (!SDL_RenameStoragePath(storage, (world + ".json").data(), (world + ".json.old").data())) {
			SDL_Log("\033[31mFailed to rename world.json to world.json.old %s, ignoring...\033[0m",
				SDL_GetError());
		}
	}

	rapidjson::Document level;
	level.SetObject();

	level.AddMember("version", rapidjson::Value().SetUint64(LATEST_LEVEL_VERSION).Move(), level.GetAllocator());
	level.AddMember("name", rapidjson::Value(mGame->getLevel()->getName().data(), level.GetAllocator()).Move(),
			level.GetAllocator()); // TODO: More options

	level.AddMember("data", rapidjson::Value(rapidjson::kObjectType).Move(), level.GetAllocator());
	mGame->getLevel()->save(level["data"], level.GetAllocator());

	rapidjson::StringBuffer sb;
	rapidjson::Writer<rapidjson::StringBuffer> writer(sb);
	level.Accept(writer);

	SDL_WriteStorageFile(storage, (world + ".json").data(), sb.GetString(), sb.GetSize());
}

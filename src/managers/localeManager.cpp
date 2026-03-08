#include "managers/localeManager.hpp"

#include "third_party/rapidjson/document.h"
#include "third_party/rapidjson/error/en.h"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>
#include <version>

LocaleManager::LocaleManager() : mLocaleDir(getBasePath() + "assets/strings/"), mLocaleDataS(nullptr, SDL_free) {
	int c = 0;
	SDL_Locale** const loc = SDL_GetPreferredLocales(&c);
	std::string locList = "";
	for (int i = 0; i < c; ++i) {
		locList += loc[i]->language;

		if (loc[i]->language != nullptr) {
			locList += "-";
			locList += loc[i]->country;
		}

		locList += " ";
	}

	SDL_Log("LocaleManager.cpp: Found locales %s", locList.data());

	for (int i = 0; i < c; ++i) {
		if (loc[i]->language == nullptr) {
			continue;
		}

		mLocale = loc[i]->language;
		if (loc[i]->country != nullptr) {
			mLocale.append("-");
			mLocale.append(loc[i]->country);
		}

		SDL_Log("LocaleManager.cpp: Tring to load locale %s", mLocale.data());

		if (loadLocale()) {
			SDL_Log("\033[32mLocaleManager.cpp: Successfully loaded system locale %s\033[0m", mLocale.data());

			SDL_free(loc);

			return;
		}
	}

	SDL_free(loc);

	mLocale = "en";
	SDL_Log("\033[31mLocaleManager.cpp: Failed to find valid system locale! Falling back to en.\033[0m");
	if (!loadLocale()) {
		SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "LocaleManager.cpp: Failed to load fallback locale en");
	}
}

std::u32string LocaleManager::U8toU32(const std::string_view& u8) const {
	std::u32string out;

	int word_size = 1;
	for (std::size_t i = 0; i < u8.size(); i += word_size) {
		std::uint32_t tmp = static_cast<std::uint32_t>(u8[i]) & 0xff;

		if (tmp < 0x80UL) {
			word_size = 1;
			out.push_back(u8[i]);
		} else if (tmp < 0xe0UL) {
			word_size = 2;
			out.push_back(((u8[i] & 0x1f) << 6) | (u8[i + 1] & 0x3f));
		} else if (tmp < 0xf0UL) {
			word_size = 3;
			out.push_back(((u8[i] & 0xf) << 12) | ((u8[i + 1] & 0x3f) << 6) | (u8[i + 2] & 0x3f));
		} else if (tmp < 0xf8UL) {
			word_size = 4;
			out.push_back(((u8[i] & 0x7) << 18) | ((u8[i + 1] & 0x3f) << 12) | ((u8[i + 2] & 0x3f) << 6) |
				      (u8[i + 3] & 0x3f));
		} else {
			out.push_back(0x000025A1); // https://www.fileformat.info/info/unicode/char/25a1/index.htm
		}
	}

	return out;
}

std::u32string LocaleManager::get(const std::string_view& id) const {
	if (id[0] == '!') {
		return U8toU32(&id[1]);
	}

	if (!mLocaleData.HasMember(id.data())) {
		SDL_Log("\x1B[31mLocaleManager.cpp: Error! Unknown id %s\033[0m", id.data());

		return U"NAN";
	}

	// Maybe https://wiki.libsdl.org/SDL3/SDL_iconv
	return U8toU32(mLocaleData[id.data()].GetString());
}

bool LocaleManager::loadLocale() {
	const std::string requestedLocale = mLocale;
	SDL_Log("LocaleManager.cpp: Loading %s", requestedLocale.data());

	std::string main = requestedLocale;

#ifdef __cpp_lib_string_contains
	if (requestedLocale.contains('-')) {
#else
	if (requestedLocale.find('-') != std::string::npos) {
#endif
		const std::size_t pos = requestedLocale.find('-');
		main = requestedLocale.substr(0, pos);
	}

	std::vector<std::string> candidates = {requestedLocale};
	if (main != requestedLocale) {
		candidates.emplace_back(main);
	}
	if (main != "en" && requestedLocale != "en") {
		candidates.emplace_back("en");
	}

	for (const auto& candidate : candidates) {
		std::unique_ptr<char[], std::function<void(char*)>> localeDataS(
			static_cast<char*>(loadFile((mLocaleDir + candidate + ".json").data(), nullptr)), SDL_free);
		if (!localeDataS) {
			continue;
		}

		rapidjson::Document localeData;
		if (localeData.ParseInsitu(localeDataS.get()).HasParseError()) {
			SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO, "\033[31mFailed to parse json %s (offset %u): %s\033[0m",
					(mLocaleDir + candidate + ".json").data(), (unsigned)localeData.GetErrorOffset(),
					rapidjson::GetParseError_En(localeData.GetParseError()));
			ERROR_BOX("Failed to load save file");
			ERROR_BOX("Failed to read locale, reinstall assets");

			return false;
		}

		mLocale = candidate;
		mLocaleData.Swap(localeData);
		mLocaleDataS = std::move(localeDataS);

		SDL_Log("LocaleManager.cpp: Found locale %s version %d", mLocale.data(), mLocaleData["version"].GetInt());
		SDL_Log("LocaleManager.cpp: Successfully loaded locale %s", mLocale.data());

		return true;
	}

	SDL_LogCritical(SDL_LOG_CATEGORY_VIDEO,
			"LocaleManager.cpp: Failed to find/read locale sources for %s, fallback %s and en: %s\n",
			requestedLocale.data(), main.data(), SDL_GetError());
	return false;
}

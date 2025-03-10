#pragma once

#include "third_party/rapidjson/document.h"

#include <string>
#include <string_view>

class LocaleManager {
      public:
	LocaleManager();
	LocaleManager(LocaleManager&&) = delete;
	LocaleManager(const LocaleManager&) = delete;
	LocaleManager& operator=(LocaleManager&&) = delete;
	LocaleManager& operator=(const LocaleManager&) = delete;
	~LocaleManager() = default;

	void changeLocale(const std::string_view& locale) {
		mLocale = locale;
		loadLocale();
	}
	std::u32string get(const std::string_view& id) const;

      private:
	std::u32string U8toU32(const std::string_view& u8) const;
	void loadLocale();

	const std::string mLocaleDir;
	std::string mLocale;

	rapidjson::Document mLocaleData;
	std::unique_ptr<char[], std::function<void(char*)>> mLocaleDataS;
};

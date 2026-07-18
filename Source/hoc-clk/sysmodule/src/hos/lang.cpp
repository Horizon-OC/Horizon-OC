/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "lang.hpp"

#include <cstdio>
#include <minIni.h>
#include <string>
#include <unordered_map>

#include "../file/file_utils.hpp"

namespace lang {
    namespace {

        constexpr const char *UltrahandConfigPath = "sdmc:/config/ultrahand/config.ini";
        constexpr const char *LangDir = "sdmc:" FILE_CONFIG_DIR "/lang/";

        std::string gLoadedLang;
        std::unordered_map<std::string, std::string> gCache;

        void NormalizeNewlines(std::string &s) {
            size_t n = 0;
            while ((n = s.find("\\n", n)) != std::string::npos) {
                s.replace(n, 2, "\n");
                n += 1;
            }
        }

        // Lightweight Ultrahand-compatible flat JSON string map parser.
        void ParseJsonContent(const std::string &content, std::unordered_map<std::string, std::string> &result) {
            size_t pos = 0;

            while ((pos = content.find('"', pos)) != std::string::npos) {
                const size_t keyStart = pos + 1;
                const size_t keyEnd = content.find('"', keyStart);
                if (keyEnd == std::string::npos) {
                    break;
                }

                const size_t colonPos = content.find(':', keyEnd);
                if (colonPos == std::string::npos) {
                    break;
                }

                const size_t valueStart = content.find('"', colonPos);
                const size_t valueEnd = content.find('"', valueStart + 1);
                if (valueStart == std::string::npos || valueEnd == std::string::npos) {
                    break;
                }

                std::string key = content.substr(keyStart, keyEnd - keyStart);
                std::string value = content.substr(valueStart + 1, valueEnd - valueStart - 1);

                NormalizeNewlines(key);
                NormalizeNewlines(value);

                result[std::move(key)] = std::move(value);
                pos = valueEnd + 1;
            }
        }

        bool ReadFileContent(const std::string &filePath, std::string &content) {
            FILE *file = fopen(filePath.c_str(), "r");
            if (!file) {
                return false;
            }

            char buffer[256];
            while (fgets(buffer, sizeof(buffer), file) != nullptr) {
                content += buffer;
            }
            fclose(file);
            return true;
        }

        bool LoadLangFile(const std::string &langCode) {
            const std::string path = std::string(LangDir) + langCode + ".json";
            std::string content;
            if (!ReadFileContent(path, content)) {
                return false;
            }

            gCache.clear();
            ParseJsonContent(content, gCache);
            gLoadedLang = langCode;
            return true;
        }

        std::string GetDefaultLang() {
            char buf[32] = {};
            ini_gets("ultrahand", "default_lang", "en", buf, sizeof(buf), UltrahandConfigPath);
            if (buf[0] == '\0') {
                return "en";
            }
            return buf;
        }

        void EnsureLoaded() {
            const std::string langCode = GetDefaultLang();
            if (langCode == gLoadedLang && !gCache.empty()) {
                return;
            }

            if (!LoadLangFile(langCode) && langCode != "en") {
                LoadLangFile("en");
            }
        }

    }  // namespace

    std::string Translate(const std::string &text) {
        if (text.empty()) {
            return text;
        }

        EnsureLoaded();

        const auto it = gCache.find(text);
        if (it != gCache.end() && !it->second.empty()) {
            return it->second;
        }

        return text;
    }

}  // namespace lang

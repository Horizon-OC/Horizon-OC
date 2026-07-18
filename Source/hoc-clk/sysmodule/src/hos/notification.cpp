/*
 * Copyright (c) ppkantorski (bord2death)
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
#include "notification.h"

namespace notification {
    void writeNotification(const std::string &message) {
        static const char *flagPath = "sdmc:/config/ultrahand/flags/NOTIFICATIONS.flag";

        FILE *flagFile = fopen(flagPath, "r");
        if (!flagFile) {
            return;
        }
        fclose(flagFile);

        const std::string translated = lang::Translate(message);

        std::string filename = "hoc-" + std::to_string(std::time(nullptr)) + ".notify";
        std::string fullPath = "sdmc:/config/ultrahand/notifications/" + filename;

        FILE *file = fopen(fullPath.c_str(), "w");
        if (file) {
            fprintf(file, "{\n");
            fprintf(file, "  \"text\": \"%s\",\n", translated.c_str());
            fprintf(file, "  \"fontSize\": 28\n");
            fprintf(file, "}\n");
            fclose(file);
        }
    }
}  // namespace notification

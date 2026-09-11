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

#include <crc32.h>

namespace util {
    uint32_t Crc32(const uint8_t *data, size_t length) {
        uint32_t crc = 0xFFFFFFFF;

        for (size_t i = 0; i < length; i++) {
            crc ^= data[i];
            for (int j = 0; j < 8; j++) {
                crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
            }
        }
        return ~crc;
    }

    uint32_t ChecksumFile(const char *filename) {
        FILE *file = fopen(filename, "rb");
        if (!file) {
            perror("[crc32] Error opening file");
            return 0;
        }

        uint8_t buffer[1024];
        uint32_t crc = 0xFFFFFFFF;
        size_t bytes_read;

        while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
            for (size_t i = 0; i < bytes_read; i++) {
                crc ^= buffer[i];
                for (int j = 0; j < 8; j++) {
                    crc = (crc >> 1) ^ (0xEDB88320 & -(crc & 1));
                }
            }
        }

        fclose(file);
        return ~crc;
    }
}  // namespace util

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

/* --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include <atomic>
#include <cstdarg>
#include <hocclk.h>
#include <string>
#include <switch.h>
#include <time.h>
#include <vector>

#define FILE_CONFIG_DIR "/config/" CONFIG_DIR
#define FILE_CONTEXT_CSV_PATH FILE_CONFIG_DIR "/context.csv"
#define FILE_LOG_FLAG_PATH FILE_CONFIG_DIR "/log.flag"
#define FILE_UART_FLAG_PATH FILE_CONFIG_DIR "/uart.flag"
#define FILE_LOG_FILE_PATH FILE_CONFIG_DIR "/log.txt"
#define FILE_SETTINGS_PATH FILE_CONFIG_DIR "/settings.ini"
#define FILE_KIP_DIR FILE_CONFIG_DIR "/kip"
#define FILE_KIP_CONFIG_PATH FILE_KIP_DIR "/current.ini"
#define FILE_PROFILES_DIR FILE_CONFIG_DIR "/profiles"
#define FILE_LEGACY_CONFIG_PATH FILE_CONFIG_DIR "/config.ini"

namespace fileUtils {

    void Exit();
    Result Initialize();
    bool IsInitialized();
    bool IsLogEnabled();
    bool IsUartEnabled();
    void InitializeAsync();
    void LogLine(const char *format, ...);
    void WriteContextToCsv(const HocClkContext *context);

}  // namespace fileUtils

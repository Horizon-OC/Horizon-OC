/*
 * Copyright (c) 2019 m4xw <m4x@m4xw.net>
 * Copyright (c) 2019 Atmosphere-NX
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
 */

/* See https://github.com/lulle2007200/emuMMC/blob/internal-emummc/source/ */

#include "oc_common.hpp"

#if HOC_IRAM_LOG
#include "fatal_handler_bin.h"
#endif

namespace ams::ldr::hoc {

    #define ATMOSPHERE_REBOOT_TO_FATAL_MAGIC 0x32454641
    #define ATMOSPHERE_IRAM_PAYLOAD_BASE 0x40010000
    #define ATMOSPHERE_FATAL_ERROR_ADDR 0x4003E000

    _Alignas(4096) u8 working_buf[4096];

    void SmcRebootToIramPayload() {
        SecmonArgs args;
        args.X[0] = 0xC3000401;
        args.X[1] = 65001;
        args.X[2] = 0;
        args.X[3] = 2;
        svcCallSecureMonitor(&args);
    }

    Result SmcCopyToIram(uintptr_t dest, const void *src, u32 size) {
        SecmonArgs args;
        args.X[0] = 0xF0000201;
        args.X[1] = (u64)src;
        args.X[2] = (u64)dest;
        args.X[3] = size;
        args.X[4] = 1;
        svcCallSecureMonitor(&args);
        Result rc = 0;
        if (args.X[0] != 0) {
            rc = (26u | ((u32)args.X[0] << 9u));
        }
        return rc;
    }

    Result SmcCopyFromIram(void *dest, uintptr_t src, u32 size) {
        SecmonArgs args;
        args.X[0] = 0xF0000201;
        args.X[1] = (u64)dest;
        args.X[2] = (u64)src;
        args.X[3] = size;
        args.X[4] = 0;
        svcCallSecureMonitor(&args);
        Result rc = 0;
        if (args.X[0] != 0) {
            rc = (26u | ((u32)args.X[0] << 9u));
        }
        return rc;
    }

    #if HOC_UART_LOG
    /* Remove this? */
    void UartLog(const char *fmt, ...) {
        char line[256];
        constexpr size_t PrefixLen = 13;   /* "[HOC] " */
        std::memcpy(line, "[Horizon OC] ", PrefixLen);

        va_list args;
        va_start(args, fmt);
        int n = vsnprintf(line + PrefixLen, sizeof(line) - PrefixLen - 1, fmt, args);
        va_end(args);
        if (n < 0) {
            return;
        }

        size_t body = static_cast<size_t>(n);
        if (body > sizeof(line) - PrefixLen - 2) { /* vsnprintf returns the untruncated length */
            body = sizeof(line) - PrefixLen - 2;
        }
        size_t len = PrefixLen + body;
        line[len++] = '\n';

        svc::OutputDebugString(line, len);
    }
    #endif

    struct log_ctx_t {
        u32 magic;
        u32 sz;
        u32 start;
        u32 end;
        char buf[];
    };

    #define IRAM_LOG_CTX_ADDR 0x4003C000
    #define IRAM_LOG_MAX_SZ 4096

    #if HOC_IRAM_LOG
    void Log(const char *data, ...) {
        static const u32 max_log_sz = sizeof(working_buf) - sizeof(log_ctx_t);
        static bool initDone = false;
        log_ctx_t *log_ctx = (log_ctx_t*)working_buf;

        R_DISCARD(SmcCopyFromIram(working_buf, IRAM_LOG_CTX_ADDR, sizeof(working_buf)));

        if (!initDone) {
            initDone = true;
            log_ctx->buf[0] = '\0';
            log_ctx->magic = 0xaabbccdd;
            log_ctx->start = 0;
            log_ctx->end   = 0;
        }

        va_list args;
        va_start(args, data);
        s32 res = vsnprintf(log_ctx->buf + log_ctx->end, max_log_sz - log_ctx->end, data, args);
        va_end(args);

        if (res < 0 || res >= (static_cast<s32>(max_log_sz - log_ctx->end))) {
            R_DISCARD(SmcCopyToIram(IRAM_LOG_CTX_ADDR, working_buf, sizeof(working_buf)));
            return;
        }

        log_ctx->end += res;
        R_DISCARD(SmcCopyToIram(IRAM_LOG_CTX_ADDR, working_buf, sizeof(working_buf)));
    }
    #endif

    void ViewLog() {
        #if HOC_IRAM_LOG
        if (spl::GetSocType() == spl::SocType_Mariko) {
            return;
        }

        constexpr size_t PageSize = 4096;
        for (size_t ofs = 0; ofs < fatal_handler_bin_size; ofs += PageSize) {
            memcpy(&working_buf, fatal_handler_bin + ofs, std::min(fatal_handler_bin_size - ofs, PageSize));
            R_DISCARD(SmcCopyToIram(ATMOSPHERE_IRAM_PAYLOAD_BASE + ofs, &working_buf, std::min(fatal_handler_bin_size - ofs, PageSize)));
        }

        SmcRebootToIramPayload();

        while(true) { }
        #endif
    }

    namespace {
        constexpr size_t SdLogBufferSize = 0x1000;
        char sdLogBuffer[SdLogBufferSize];
        size_t workingLength = 0;
    }

    void SdLog(const char *data, ...) {
        if (workingLength >= SdLogBufferSize - 1) {
            return;
        }

        va_list args;
        va_start(args, data);
        s32 res = vsnprintf(sdLogBuffer + workingLength, SdLogBufferSize - workingLength, data, args);
        va_end(args);

        if (res < 0) {
            return;
        }

        workingLength = std::min(workingLength + static_cast<size_t>(res), SdLogBufferSize - 1);
    }

    void SaveLog() {
        static bool saved = false;
        if (workingLength == 0 || saved) {
            return;
        }

        if (!cfg::IsSdCardInitialized()) {
            return;
        }

        if (R_FAILED(fs::MountSdCard("sdmc"))) {
            return;
        }

        ON_SCOPE_EXIT { fs::Unmount("sdmc"); };

        constexpr const char *Path = "sdmc:/config/horizon-oc/ldr_log.txt";

        R_DISCARD(fs::DeleteFile(Path));
        if (R_FAILED(fs::CreateFile(Path, workingLength))) {
            return;
        }

        fs::FileHandle file;
        if (R_FAILED(fs::OpenFile(std::addressof(file), Path, fs::OpenMode_Write))) {
            return;
        }
        ON_SCOPE_EXIT { fs::CloseFile(file); };

        if (R_FAILED(fs::WriteFile(file, 0, sdLogBuffer, workingLength, fs::WriteOption::Flush))) {
            return;
        }
        saved = true;
    }

}

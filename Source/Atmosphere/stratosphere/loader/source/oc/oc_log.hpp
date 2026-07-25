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

#pragma once

namespace ams::ldr::hoc {

    Result SmcCopyToIram(uintptr_t dest, const void *src, u32 size);
    void Log(const char *data, ...);
    void ViewLog();

    /* Emit a formatted line over the kernel debug UART via svcOutputDebugString.
       No-op unless the running kernel enables debug logging (audit/debug mesosphere). */
    void UartLog(const char *fmt, ...) __attribute__((format(printf, 1, 2)));

}

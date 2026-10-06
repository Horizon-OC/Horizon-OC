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

#include <cstring>

#include "../file/errors.hpp"
#include "../file/file_utils.hpp"
#include "process_management.hpp"


namespace hos {

    namespace {
        constexpr u64 Qlaunch = 0x0100000000001000ULL;
        constexpr u32 IsQlaunch = 0x20f;
        Service pdmqryClone;
    }  // namespace

    void InitializeProcessManagement() {
        Result rc = 0;

        rc = pmdmntInitialize();
        ASSERT_RESULT_OK(rc, "pmdmntInitialize");

        rc = pminfoInitialize();
        ASSERT_RESULT_OK(rc, "pminfoInitialize");

        rc = pdmqryInitialize();
        ASSERT_RESULT_OK(rc, "pdmqryInitialize");

        Service *pdmqrySrv = pdmqryGetServiceSession();
        serviceClone(pdmqrySrv, &pdmqryClone);
        serviceClose(pdmqrySrv);
        memcpy(pdmqrySrv, &pdmqryClone, sizeof(Service));
    }

    void WaitForQLaunch() {
        Result rc = 0;
        u64 pid = 0;
        do {
            rc = pmdmntGetProcessId(&pid, Qlaunch);
            svcSleepThread(50 * 1000000ULL);  // 50ms
        } while (R_FAILED(rc));
    }

    // Ty to Masa for this function!
    Result IsApplicationOutOfFocus(bool *outOfFocus) {
        static s32 last_total_entries = 0;
        static bool isOutOfFocus = false;
        s32 total_entries = 0;
        s32 start_entry_index = 0;
        s32 end_entry_index = 0;
        u64 TIDnow;
        u64 PIDnow;

        Result rc = pmdmntGetApplicationProcessId(&PIDnow);
        if (R_FAILED(rc))
            return rc;
        rc = pmdmntGetProgramId(&TIDnow, PIDnow);
        if (R_FAILED(rc))
            return rc;

        rc = pdmqryGetAvailablePlayEventRange(&total_entries, &start_entry_index, &end_entry_index);
        if (R_FAILED(rc))
            return rc;
        if (total_entries == last_total_entries) {
            *outOfFocus = isOutOfFocus;
            return 0;
        }
        last_total_entries = total_entries;

        PdmPlayEvent events[16];
        s32 out = 0;
        s32 start_entry = end_entry_index - 15;
        if (start_entry < 0)
            start_entry = 0;
        rc = pdmqryQueryPlayEvent(start_entry, events, sizeof(events) / sizeof(events[0]), &out);
        if (R_FAILED(rc))
            return rc;
        if (out == 0)
            return 1;

        int itr = -1;
        for (int i = out - 1; i >= 0; i--) {
            if (events[i].play_event_type != PdmPlayEventType_Applet)
                continue;
            if (events[i].event_data.applet.applet_id != AppletId_application)
                continue;
            union {
                struct {
                    uint32_t part[2];
                } parts;
                uint64_t full;
            } TID;
            TID.parts.part[0] = events[i].event_data.applet.program_id[1];
            TID.parts.part[1] = events[i].event_data.applet.program_id[0];

            if (TID.full != (TIDnow & ~0xFFF))
                continue;
            else {
                itr = i;
                break;
            }
        }
        if (itr == -1)
            return 1;

        bool isOut = events[itr].event_data.applet.event_type == PdmAppletEventType_OutOfFocus ||
                     events[itr].event_data.applet.event_type == PdmAppletEventType_OutOfFocus4;
        *outOfFocus = isOut;
        isOutOfFocus = isOut;
        return 0;
    }

    u64 GetCurrentApplicationId() {
        Result rc = 0;
        u64 pid = 0;
        u64 tid = 0;
        rc = pmdmntGetApplicationProcessId(&pid);

        if (rc == IsQlaunch) {
            return Qlaunch;
        }

        ASSERT_RESULT_OK(rc, "pmdmntGetApplicationProcessId");

        rc = pminfoGetProgramId(&tid, pid);

        if (rc == IsQlaunch) {
            return Qlaunch;
        }

        ASSERT_RESULT_OK(rc, "pminfoGetProgramId");

        return tid;
    }

    void ExitProcessManagement() {
        pmdmntExit();
        pminfoExit();
        pdmqryExit();
    }

}  // namespace hos

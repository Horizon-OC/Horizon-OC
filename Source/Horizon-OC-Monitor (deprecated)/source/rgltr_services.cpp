// rgltr_services.cpp (no changes needed here—just compile it once)
#include <switch.h>
#include "rgltr.h"
#include "rgltr_services.h"  // for extern Service g_rgltrSrv, etc.

// Global service handle
Service g_rgltrSrv;

Result rgltrInitialize(void) {
    if (hosversionBefore(8, 0, 0)) {
        return MAKERESULT(Module_Libnx, LibnxError_IncompatSysVer);
    }
    return smGetService(&g_rgltrSrv, "rgltr");
}

void rgltrExit(void) {
    serviceClose(&g_rgltrSrv);
}

Result rgltrOpenSession(RgltrSession* session_out, PowerDomainId module_id) {
    const u32 in = (u32)module_id;
    return serviceDispatchIn(
        &g_rgltrSrv,
        0,
        in,
        .out_num_objects = 1,
        .out_objects     = &session_out->s
    );
}

Result rgltrGetVoltage(RgltrSession* session, u32* out_volt) {
    // Service returns µV (microvolts) in a local u32:
    u32 temp = 0;
    Result rc = serviceDispatchOut(&session->s, 4, temp);
    if (R_SUCCEEDED(rc)) {
        *out_volt = temp;
    }
    return rc;
}

void rgltrCloseSession(RgltrSession* session) {
    serviceClose(&session->s);
}

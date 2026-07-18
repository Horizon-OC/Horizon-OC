// rgltr_services.h
// ========
// Minimal header declarations for rgltr‐related functionality.
// Any file that wants to call rgltrOpenSession(), rgltrGetVoltage(), etc. should
// simply do `#include "infonx.h"` (NOT infonx.cpp).

#pragma once

#include <switch.h>  // for Service, Result, hosversionBefore(), smGetService(), serviceClose(), etc.
#include "rgltr.h"   // for RgltrSession, PowerDomainId, etc.

// Global service handle for "rgltr". Defined in infonx.cpp.
extern Service g_rgltrSrv;

// Open/close the "rgltr" service. You must call rgltrInitialize() (once) before using
// rgltrOpenSession() & friends.  Call rgltrExit() when your app is shutting down.
Result rgltrInitialize(void);
void   rgltrExit(void);

// Open a regulator session for the given PowerDomainId (e.g. CPU, GPU, DRAM).
// On success, (*session_out).s will contain a valid Service handle.
Result rgltrOpenSession(RgltrSession* session_out, PowerDomainId module_id);

// Query the current voltage (in microvolts, µV) from a previously opened session.
// Writes the result into *out_volt.
Result rgltrGetVoltage(RgltrSession* session, u32* out_volt);

// Close a previously opened regulator session.
void   rgltrCloseSession(RgltrSession* session);

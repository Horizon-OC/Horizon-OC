/**
 * @file switch.hpp
 * @brief Libnx C++ bindings (AUTO-GENERATED, do not edit).
 * @copyright libnx Authors
 */
#include "switch.hpp"

namespace nx {

const char *CppBindingsVersion() {
    return "libnx C++ bindings v1.0.0 (libnx feebd026)";
}

namespace account {

Result Initialize(AccountServiceType service_type) { return ::accountInitialize(service_type); }
void Exit(void) { ::accountExit(); }
Service* GetServiceSession(void) { return ::accountGetServiceSession(); }
Result GetUserCount(s32* user_count) { return ::accountGetUserCount(user_count); }
Result ListAllUsers(AccountUid* uids, s32 max_uids, s32 *actual_total) { return ::accountListAllUsers(uids, max_uids, actual_total); }
Result GetLastOpenedUser(AccountUid *uid) { return ::accountGetLastOpenedUser(uid); }
Result GetProfile(AccountProfile* out, AccountUid uid) { return ::accountGetProfile(out, uid); }
Result IsUserRegistrationRequestPermitted(bool *out) { return ::accountIsUserRegistrationRequestPermitted(out); }
Result TrySelectUserWithoutInteraction(AccountUid *uid, bool is_network_service_account_required) { return ::accountTrySelectUserWithoutInteraction(uid, is_network_service_account_required); }
void ProfileClose(AccountProfile* profile) { ::accountProfileClose(profile); }
Result ProfileGet(AccountProfile* profile, AccountUserData* userdata, AccountProfileBase* profilebase) { return ::accountProfileGet(profile, userdata, profilebase); }
Result ProfileGetImageSize(AccountProfile* profile, u32* image_size) { return ::accountProfileGetImageSize(profile, image_size); }
Result ProfileLoadImage(AccountProfile* profile, void* buf, size_t len, u32* image_size) { return ::accountProfileLoadImage(profile, buf, len, image_size); }
Result GetPreselectedUser(AccountUid *uid) { return ::accountGetPreselectedUser(uid); }
bool UidIsValid(const AccountUid *Uid) { return ::accountUidIsValid(Uid); }

} // namespace account

namespace aes128 {

void ContextCreate(Aes128Context *out, const void *key, bool is_encryptor) { ::aes128ContextCreate(out, key, is_encryptor); }
void EncryptBlock(const Aes128Context *ctx, void *dst, const void *src) { ::aes128EncryptBlock(ctx, dst, src); }
void DecryptBlock(const Aes128Context *ctx, void *dst, const void *src) { ::aes128DecryptBlock(ctx, dst, src); }
void CbcContextCreate(Aes128CbcContext *out, const void *key, const void *iv, bool is_encryptor) { ::aes128CbcContextCreate(out, key, iv, is_encryptor); }
void CbcContextResetIv(Aes128CbcContext *ctx, const void *iv) { ::aes128CbcContextResetIv(ctx, iv); }
size_t CbcEncrypt(Aes128CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes128CbcEncrypt(ctx, dst, src, size); }
size_t CbcDecrypt(Aes128CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes128CbcDecrypt(ctx, dst, src, size); }
void CtrContextCreate(Aes128CtrContext *out, const void *key, const void *ctr) { ::aes128CtrContextCreate(out, key, ctr); }
void CtrContextResetCtr(Aes128CtrContext *ctx, const void *ctr) { ::aes128CtrContextResetCtr(ctx, ctr); }
void CtrCrypt(Aes128CtrContext *ctx, void *dst, const void *src, size_t size) { ::aes128CtrCrypt(ctx, dst, src, size); }
void XtsContextCreate(Aes128XtsContext *out, const void *key0, const void *key1, bool is_encryptor) { ::aes128XtsContextCreate(out, key0, key1, is_encryptor); }
void XtsContextResetTweak(Aes128XtsContext *ctx, const void *tweak) { ::aes128XtsContextResetTweak(ctx, tweak); }
void XtsContextResetSector(Aes128XtsContext *ctx, uint64_t sector, bool is_nintendo) { ::aes128XtsContextResetSector(ctx, sector, is_nintendo); }
size_t XtsEncrypt(Aes128XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes128XtsEncrypt(ctx, dst, src, size); }
size_t XtsDecrypt(Aes128XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes128XtsDecrypt(ctx, dst, src, size); }

} // namespace aes128

namespace aes192 {

void ContextCreate(Aes192Context *out, const void *key, bool is_encryptor) { ::aes192ContextCreate(out, key, is_encryptor); }
void EncryptBlock(const Aes192Context *ctx, void *dst, const void *src) { ::aes192EncryptBlock(ctx, dst, src); }
void DecryptBlock(const Aes192Context *ctx, void *dst, const void *src) { ::aes192DecryptBlock(ctx, dst, src); }
void CbcContextCreate(Aes192CbcContext *out, const void *key, const void *iv, bool is_encryptor) { ::aes192CbcContextCreate(out, key, iv, is_encryptor); }
void CbcContextResetIv(Aes192CbcContext *ctx, const void *iv) { ::aes192CbcContextResetIv(ctx, iv); }
size_t CbcEncrypt(Aes192CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes192CbcEncrypt(ctx, dst, src, size); }
size_t CbcDecrypt(Aes192CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes192CbcDecrypt(ctx, dst, src, size); }
void CtrContextCreate(Aes192CtrContext *out, const void *key, const void *ctr) { ::aes192CtrContextCreate(out, key, ctr); }
void CtrContextResetCtr(Aes192CtrContext *ctx, const void *ctr) { ::aes192CtrContextResetCtr(ctx, ctr); }
void CtrCrypt(Aes192CtrContext *ctx, void *dst, const void *src, size_t size) { ::aes192CtrCrypt(ctx, dst, src, size); }
void XtsContextCreate(Aes192XtsContext *out, const void *key0, const void *key1, bool is_encryptor) { ::aes192XtsContextCreate(out, key0, key1, is_encryptor); }
void XtsContextResetTweak(Aes192XtsContext *ctx, const void *tweak) { ::aes192XtsContextResetTweak(ctx, tweak); }
void XtsContextResetSector(Aes192XtsContext *ctx, uint64_t sector, bool is_nintendo) { ::aes192XtsContextResetSector(ctx, sector, is_nintendo); }
size_t XtsEncrypt(Aes192XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes192XtsEncrypt(ctx, dst, src, size); }
size_t XtsDecrypt(Aes192XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes192XtsDecrypt(ctx, dst, src, size); }

} // namespace aes192

namespace aes256 {

void ContextCreate(Aes256Context *out, const void *key, bool is_encryptor) { ::aes256ContextCreate(out, key, is_encryptor); }
void EncryptBlock(const Aes256Context *ctx, void *dst, const void *src) { ::aes256EncryptBlock(ctx, dst, src); }
void DecryptBlock(const Aes256Context *ctx, void *dst, const void *src) { ::aes256DecryptBlock(ctx, dst, src); }
void CbcContextCreate(Aes256CbcContext *out, const void *key, const void *iv, bool is_encryptor) { ::aes256CbcContextCreate(out, key, iv, is_encryptor); }
void CbcContextResetIv(Aes256CbcContext *ctx, const void *iv) { ::aes256CbcContextResetIv(ctx, iv); }
size_t CbcEncrypt(Aes256CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes256CbcEncrypt(ctx, dst, src, size); }
size_t CbcDecrypt(Aes256CbcContext *ctx, void *dst, const void *src, size_t size) { return ::aes256CbcDecrypt(ctx, dst, src, size); }
void CtrContextCreate(Aes256CtrContext *out, const void *key, const void *ctr) { ::aes256CtrContextCreate(out, key, ctr); }
void CtrContextResetCtr(Aes256CtrContext *ctx, const void *ctr) { ::aes256CtrContextResetCtr(ctx, ctr); }
void CtrCrypt(Aes256CtrContext *ctx, void *dst, const void *src, size_t size) { ::aes256CtrCrypt(ctx, dst, src, size); }
void XtsContextCreate(Aes256XtsContext *out, const void *key0, const void *key1, bool is_encryptor) { ::aes256XtsContextCreate(out, key0, key1, is_encryptor); }
void XtsContextResetTweak(Aes256XtsContext *ctx, const void *tweak) { ::aes256XtsContextResetTweak(ctx, tweak); }
void XtsContextResetSector(Aes256XtsContext *ctx, uint64_t sector, bool is_nintendo) { ::aes256XtsContextResetSector(ctx, sector, is_nintendo); }
size_t XtsEncrypt(Aes256XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes256XtsEncrypt(ctx, dst, src, size); }
size_t XtsDecrypt(Aes256XtsContext *ctx, void *dst, const void *src, size_t size) { return ::aes256XtsDecrypt(ctx, dst, src, size); }

} // namespace aes256

namespace album {

Result LaShowAlbumFiles(void) { return ::albumLaShowAlbumFiles(); }
Result LaShowAllAlbumFiles(void) { return ::albumLaShowAllAlbumFiles(); }
Result LaShowAllAlbumFilesForHomeMenu(void) { return ::albumLaShowAllAlbumFilesForHomeMenu(); }

} // namespace album

namespace apm {

Result Initialize(void) { return ::apmInitialize(); }
void Exit(void) { ::apmExit(); }
Service* GetServiceSession(void) { return ::apmGetServiceSession(); }
Service* GetServiceSession_Session(void) { return ::apmGetServiceSession_Session(); }
Result GetPerformanceMode(ApmPerformanceMode* out_performanceMode) { return ::apmGetPerformanceMode(out_performanceMode); }
Result SetPerformanceConfiguration(ApmPerformanceMode PerformanceMode, u32 PerformanceConfiguration) { return ::apmSetPerformanceConfiguration(PerformanceMode, PerformanceConfiguration); }
Result GetPerformanceConfiguration(ApmPerformanceMode PerformanceMode, u32 *PerformanceConfiguration) { return ::apmGetPerformanceConfiguration(PerformanceMode, PerformanceConfiguration); }

} // namespace apm

namespace applet {

Result Initialize(void) { return ::appletInitialize(); }
void Exit(void) { ::appletExit(); }
Service* GetServiceSession_Proxy(void) { return ::appletGetServiceSession_Proxy(); }
Service* GetServiceSession_AppletCommonFunctions(void) { return ::appletGetServiceSession_AppletCommonFunctions(); }
Service* GetServiceSession_Functions(void) { return ::appletGetServiceSession_Functions(); }
Service* GetServiceSession_GlobalStateController(void) { return ::appletGetServiceSession_GlobalStateController(); }
Service* GetServiceSession_ApplicationCreator(void) { return ::appletGetServiceSession_ApplicationCreator(); }
Service* GetServiceSession_LibraryAppletSelfAccessor(void) { return ::appletGetServiceSession_LibraryAppletSelfAccessor(); }
Service* GetServiceSession_ProcessWindingController(void) { return ::appletGetServiceSession_ProcessWindingController(); }
Service* GetServiceSession_LibraryAppletCreator(void) { return ::appletGetServiceSession_LibraryAppletCreator(); }
Service* GetServiceSession_CommonStateGetter(void) { return ::appletGetServiceSession_CommonStateGetter(); }
Service* GetServiceSession_SelfController(void) { return ::appletGetServiceSession_SelfController(); }
Service* GetServiceSession_WindowController(void) { return ::appletGetServiceSession_WindowController(); }
Service* GetServiceSession_AudioController(void) { return ::appletGetServiceSession_AudioController(); }
Service* GetServiceSession_DisplayController(void) { return ::appletGetServiceSession_DisplayController(); }
Service* GetServiceSession_DebugFunctions(void) { return ::appletGetServiceSession_DebugFunctions(); }
u64 GetAppletResourceUserId(void) { return ::appletGetAppletResourceUserId(); }
AppletType GetAppletType(void) { return ::appletGetAppletType(); }
void SetThemeColorType(AppletThemeColorType theme) { ::appletSetThemeColorType(theme); }
AppletThemeColorType GetThemeColorType(void) { return ::appletGetThemeColorType(); }
Result GetCradleStatus(u8 *status) { return ::appletGetCradleStatus(status); }
Result GetBootMode(PmBootMode *mode) { return ::appletGetBootMode(mode); }
Result RequestToAcquireSleepLock(void) { return ::appletRequestToAcquireSleepLock(); }
Result ReleaseSleepLock(void) { return ::appletReleaseSleepLock(); }
Result ReleaseSleepLockTransiently(void) { return ::appletReleaseSleepLockTransiently(); }
Result GetWakeupCount(u64 *out) { return ::appletGetWakeupCount(out); }
Result PushToGeneralChannel(AppletStorage *s) { return ::appletPushToGeneralChannel(s); }
Result GetHomeButtonReaderLockAccessor(AppletLockAccessor *a) { return ::appletGetHomeButtonReaderLockAccessor(a); }
Result GetReaderLockAccessorEx(AppletLockAccessor *a, u32 inval) { return ::appletGetReaderLockAccessorEx(a, inval); }
Result GetWriterLockAccessorEx(AppletLockAccessor *a, u32 inval) { return ::appletGetWriterLockAccessorEx(a, inval); }
Result GetCradleFwVersion(u32 *out0, u32 *out1, u32 *out2, u32 *out3) { return ::appletGetCradleFwVersion(out0, out1, out2, out3); }
Result IsVrModeEnabled(bool *out) { return ::appletIsVrModeEnabled(out); }
Result SetVrModeEnabled(bool flag) { return ::appletSetVrModeEnabled(flag); }
Result SetLcdBacklightOffEnabled(bool flag) { return ::appletSetLcdBacklightOffEnabled(flag); }
Result IsInControllerFirmwareUpdateSection(bool *out) { return ::appletIsInControllerFirmwareUpdateSection(out); }
Result SetVrPositionForDebug(s32 x, s32 y, s32 width, s32 height) { return ::appletSetVrPositionForDebug(x, y, width, height); }
Result GetDefaultDisplayResolution(s32 *width, s32 *height) { return ::appletGetDefaultDisplayResolution(width, height); }
Result GetDefaultDisplayResolutionChangeEvent(Event *out_event) { return ::appletGetDefaultDisplayResolutionChangeEvent(out_event); }
Result GetHdcpAuthenticationState(s32 *state) { return ::appletGetHdcpAuthenticationState(state); }
Result GetHdcpAuthenticationStateChangeEvent(Event *out_event) { return ::appletGetHdcpAuthenticationStateChangeEvent(out_event); }
Result SetTvPowerStateMatchingMode(AppletTvPowerStateMatchingMode mode) { return ::appletSetTvPowerStateMatchingMode(mode); }
Result GetApplicationIdByContentActionName(u64 *application_id, const char *name) { return ::appletGetApplicationIdByContentActionName(application_id, name); }
Result SetCpuBoostMode(ApmCpuBoostMode mode) { return ::appletSetCpuBoostMode(mode); }
Result CancelCpuBoostMode(void) { return ::appletCancelCpuBoostMode(); }
Result GetBuiltInDisplayType(s32 *out) { return ::appletGetBuiltInDisplayType(out); }
Result PerformSystemButtonPressingIfInFocus(AppletSystemButtonType type) { return ::appletPerformSystemButtonPressingIfInFocus(type); }
Result SetPerformanceConfigurationChangedNotification(bool flag) { return ::appletSetPerformanceConfigurationChangedNotification(flag); }
Result GetCurrentPerformanceConfiguration(u32 *PerformanceConfiguration) { return ::appletGetCurrentPerformanceConfiguration(PerformanceConfiguration); }
Result OpenMyGpuErrorHandler(AppletGpuErrorHandler *g) { return ::appletOpenMyGpuErrorHandler(g); }
Result GetOperationModeSystemInfo(u32 *info) { return ::appletGetOperationModeSystemInfo(info); }
Result GetSettingsPlatformRegion(SetSysPlatformRegion *out) { return ::appletGetSettingsPlatformRegion(out); }
Result ActivateMigrationService(void) { return ::appletActivateMigrationService(); }
Result DeactivateMigrationService(void) { return ::appletDeactivateMigrationService(); }
Result DisableSleepTillShutdown(void) { return ::appletDisableSleepTillShutdown(); }
Result SuppressDisablingSleepTemporarily(u64 val) { return ::appletSuppressDisablingSleepTemporarily(val); }
Result SetRequestExitToLibraryAppletAtExecuteNextProgramEnabled(void) { return ::appletSetRequestExitToLibraryAppletAtExecuteNextProgramEnabled(); }
void GpuErrorHandlerClose(AppletGpuErrorHandler *g) { ::appletGpuErrorHandlerClose(g); }
Result GpuErrorHandlerGetManualGpuErrorInfoSize(AppletGpuErrorHandler *g, u64 *out) { return ::appletGpuErrorHandlerGetManualGpuErrorInfoSize(g, out); }
Result GpuErrorHandlerGetManualGpuErrorInfo(AppletGpuErrorHandler *g, void* buffer, size_t size, u64 *out) { return ::appletGpuErrorHandlerGetManualGpuErrorInfo(g, buffer, size, out); }
Result GpuErrorHandlerGetManualGpuErrorDetectionSystemEvent(AppletGpuErrorHandler *g, Event *out_event) { return ::appletGpuErrorHandlerGetManualGpuErrorDetectionSystemEvent(g, out_event); }
Result GpuErrorHandlerFinishManualGpuErrorHandling(AppletGpuErrorHandler *g) { return ::appletGpuErrorHandlerFinishManualGpuErrorHandling(g); }
Result LockExit(void) { return ::appletLockExit(); }
Result UnlockExit(void) { return ::appletUnlockExit(); }
Result EnterFatalSection(void) { return ::appletEnterFatalSection(); }
Result LeaveFatalSection(void) { return ::appletLeaveFatalSection(); }
Result SetScreenShotPermission(AppletScreenShotPermission permission) { return ::appletSetScreenShotPermission(permission); }
Result SetRestartMessageEnabled(bool flag) { return ::appletSetRestartMessageEnabled(flag); }
Result SetScreenShotAppletIdentityInfo(AppletIdentityInfo *info) { return ::appletSetScreenShotAppletIdentityInfo(info); }
Result SetControllerFirmwareUpdateSection(bool flag) { return ::appletSetControllerFirmwareUpdateSection(flag); }
Result SetRequiresCaptureButtonShortPressedMessage(bool flag) { return ::appletSetRequiresCaptureButtonShortPressedMessage(flag); }
Result SetAlbumImageOrientation(AlbumImageOrientation orientation) { return ::appletSetAlbumImageOrientation(orientation); }
Result SetDesirableKeyboardLayout(SetKeyboardLayout layout) { return ::appletSetDesirableKeyboardLayout(layout); }
Result CreateManagedDisplayLayer(u64 *out) { return ::appletCreateManagedDisplayLayer(out); }
Result IsSystemBufferSharingEnabled(void) { return ::appletIsSystemBufferSharingEnabled(); }
Result GetSystemSharedLayerHandle(u64 *SharedBufferHandle, u64 *SharedLayerHandle) { return ::appletGetSystemSharedLayerHandle(SharedBufferHandle, SharedLayerHandle); }
Result GetSystemSharedBufferHandle(u64 *SharedBufferHandle) { return ::appletGetSystemSharedBufferHandle(SharedBufferHandle); }
Result CreateManagedDisplaySeparableLayer(u64 *display_layer, u64 *recording_layer) { return ::appletCreateManagedDisplaySeparableLayer(display_layer, recording_layer); }
Result SetManagedDisplayLayerSeparationMode(u32 mode) { return ::appletSetManagedDisplayLayerSeparationMode(mode); }
Result SetHandlesRequestToDisplay(bool flag) { return ::appletSetHandlesRequestToDisplay(flag); }
Result ApproveToDisplay(void) { return ::appletApproveToDisplay(); }
Result OverrideAutoSleepTimeAndDimmingTime(s32 inval0, s32 inval1, s32 inval2, s32 inval3) { return ::appletOverrideAutoSleepTimeAndDimmingTime(inval0, inval1, inval2, inval3); }
Result SetIdleTimeDetectionExtension(AppletIdleTimeDetectionExtension ext) { return ::appletSetIdleTimeDetectionExtension(ext); }
Result GetIdleTimeDetectionExtension(AppletIdleTimeDetectionExtension *ext) { return ::appletGetIdleTimeDetectionExtension(ext); }
Result SetInputDetectionSourceSet(u32 val) { return ::appletSetInputDetectionSourceSet(val); }
Result ReportUserIsActive(void) { return ::appletReportUserIsActive(); }
Result GetCurrentIlluminance(float *fLux) { return ::appletGetCurrentIlluminance(fLux); }
Result IsIlluminanceAvailable(bool *out) { return ::appletIsIlluminanceAvailable(out); }
Result SetAutoSleepDisabled(bool flag) { return ::appletSetAutoSleepDisabled(flag); }
Result IsAutoSleepDisabled(bool *out) { return ::appletIsAutoSleepDisabled(out); }
Result GetCurrentIlluminanceEx(bool *bOverLimit, float *fLux) { return ::appletGetCurrentIlluminanceEx(bOverLimit, fLux); }
Result SetInputDetectionPolicy(AppletInputDetectionPolicy policy) { return ::appletSetInputDetectionPolicy(policy); }
Result SetWirelessPriorityMode(AppletWirelessPriorityMode mode) { return ::appletSetWirelessPriorityMode(mode); }
Result GetProgramTotalActiveTime(u64 *activeTime) { return ::appletGetProgramTotalActiveTime(activeTime); }
Result SetAlbumImageTakenNotificationEnabled(bool flag) { return ::appletSetAlbumImageTakenNotificationEnabled(flag); }
Result SetApplicationAlbumUserData(const void* buffer, size_t size) { return ::appletSetApplicationAlbumUserData(buffer, size); }
Result SaveCurrentScreenshot(AlbumReportOption option) { return ::appletSaveCurrentScreenshot(option); }
Result GetAppletResourceUserIdOfCallerApplet(u64 *out) { return ::appletGetAppletResourceUserIdOfCallerApplet(out); }
Result SetAppletWindowVisibility(bool flag) { return ::appletSetAppletWindowVisibility(flag); }
Result SetAppletGpuTimeSlice(s64 val) { return ::appletSetAppletGpuTimeSlice(val); }
Result SetExpectedMasterVolume(float mainAppletVolume, float libraryAppletVolume) { return ::appletSetExpectedMasterVolume(mainAppletVolume, libraryAppletVolume); }
Result GetExpectedMasterVolume(float *mainAppletVolume, float *libraryAppletVolume) { return ::appletGetExpectedMasterVolume(mainAppletVolume, libraryAppletVolume); }
Result ChangeMainAppletMasterVolume(float volume, u64 unk) { return ::appletChangeMainAppletMasterVolume(volume, unk); }
Result SetTransparentVolumeRate(float val) { return ::appletSetTransparentVolumeRate(val); }
Result UpdateLastForegroundCaptureImage(void) { return ::appletUpdateLastForegroundCaptureImage(); }
Result UpdateCallerAppletCaptureImage(void) { return ::appletUpdateCallerAppletCaptureImage(); }
Result GetLastForegroundCaptureImageEx(void* buffer, size_t size, bool *flag) { return ::appletGetLastForegroundCaptureImageEx(buffer, size, flag); }
Result GetLastApplicationCaptureImageEx(void* buffer, size_t size, bool *flag) { return ::appletGetLastApplicationCaptureImageEx(buffer, size, flag); }
Result GetCallerAppletCaptureImageEx(void* buffer, size_t size, bool *flag) { return ::appletGetCallerAppletCaptureImageEx(buffer, size, flag); }
Result TakeScreenShotOfOwnLayer(bool flag, AppletCaptureSharedBuffer captureBuf) { return ::appletTakeScreenShotOfOwnLayer(flag, captureBuf); }
Result CopyBetweenCaptureBuffers(AppletCaptureSharedBuffer dstCaptureBuf, AppletCaptureSharedBuffer srcCaptureBuf) { return ::appletCopyBetweenCaptureBuffers(dstCaptureBuf, srcCaptureBuf); }
Result ClearCaptureBuffer(bool flag, AppletCaptureSharedBuffer captureBuf, u32 color) { return ::appletClearCaptureBuffer(flag, captureBuf, color); }
Result ClearAppletTransitionBuffer(u32 color) { return ::appletClearAppletTransitionBuffer(color); }
Result AcquireLastApplicationCaptureSharedBuffer(bool *flag, s32 *id) { return ::appletAcquireLastApplicationCaptureSharedBuffer(flag, id); }
Result ReleaseLastApplicationCaptureSharedBuffer(void) { return ::appletReleaseLastApplicationCaptureSharedBuffer(); }
Result AcquireLastForegroundCaptureSharedBuffer(bool *flag, s32 *id) { return ::appletAcquireLastForegroundCaptureSharedBuffer(flag, id); }
Result ReleaseLastForegroundCaptureSharedBuffer(void) { return ::appletReleaseLastForegroundCaptureSharedBuffer(); }
Result AcquireCallerAppletCaptureSharedBuffer(bool *flag, s32 *id) { return ::appletAcquireCallerAppletCaptureSharedBuffer(flag, id); }
Result ReleaseCallerAppletCaptureSharedBuffer(void) { return ::appletReleaseCallerAppletCaptureSharedBuffer(); }
Result TakeScreenShotOfOwnLayerEx(bool flag0, bool immediately, AppletCaptureSharedBuffer captureBuf) { return ::appletTakeScreenShotOfOwnLayerEx(flag0, immediately, captureBuf); }
Result PushContext(AppletStorage *s) { return ::appletPushContext(s); }
Result PopContext(AppletStorage *s) { return ::appletPopContext(s); }
void LockAccessorClose(AppletLockAccessor *a) { ::appletLockAccessorClose(a); }
Result LockAccessorTryLock(AppletLockAccessor *a, bool *flag) { return ::appletLockAccessorTryLock(a, flag); }
Result LockAccessorLock(AppletLockAccessor *a) { return ::appletLockAccessorLock(a); }
Result LockAccessorUnlock(AppletLockAccessor *a) { return ::appletLockAccessorUnlock(a); }
Result CreateLibraryApplet(AppletHolder *h, AppletId id, LibAppletMode mode) { return ::appletCreateLibraryApplet(h, id, mode); }
Result CreateLibraryAppletSelf(AppletHolder *h, AppletId id, LibAppletMode mode) { return ::appletCreateLibraryAppletSelf(h, id, mode); }
Result TerminateAllLibraryApplets(void) { return ::appletTerminateAllLibraryApplets(); }
Result AreAnyLibraryAppletsLeft(bool *out) { return ::appletAreAnyLibraryAppletsLeft(out); }
void HolderClose(AppletHolder *h) { ::appletHolderClose(h); }
bool HolderActive(AppletHolder *h) { return ::appletHolderActive(h); }
Result HolderGetIndirectLayerConsumerHandle(AppletHolder *h, u64 *out) { return ::appletHolderGetIndirectLayerConsumerHandle(h, out); }
Result HolderStart(AppletHolder *h) { return ::appletHolderStart(h); }
Result HolderJump(AppletHolder *h) { return ::appletHolderJump(h); }
Result HolderRequestExit(AppletHolder *h) { return ::appletHolderRequestExit(h); }
Result HolderTerminate(AppletHolder *h) { return ::appletHolderTerminate(h); }
Result HolderRequestExitOrTerminate(AppletHolder *h, u64 timeout) { return ::appletHolderRequestExitOrTerminate(h, timeout); }
void HolderJoin(AppletHolder *h) { ::appletHolderJoin(h); }
Event * HolderGetExitEvent(AppletHolder *h) { return ::appletHolderGetExitEvent(h); }
bool HolderCheckFinished(AppletHolder *h) { return ::appletHolderCheckFinished(h); }
LibAppletExitReason HolderGetExitReason(AppletHolder *h) { return ::appletHolderGetExitReason(h); }
Result HolderSetOutOfFocusApplicationSuspendingEnabled(AppletHolder *h, bool flag) { return ::appletHolderSetOutOfFocusApplicationSuspendingEnabled(h, flag); }
Result HolderPresetLibraryAppletGpuTimeSliceZero(AppletHolder *h) { return ::appletHolderPresetLibraryAppletGpuTimeSliceZero(h); }
Result HolderGetPopInteractiveOutDataEvent(AppletHolder *h, Event **out_event) { return ::appletHolderGetPopInteractiveOutDataEvent(h, out_event); }
bool HolderWaitInteractiveOut(AppletHolder *h) { return ::appletHolderWaitInteractiveOut(h); }
Result HolderPushInData(AppletHolder *h, AppletStorage *s) { return ::appletHolderPushInData(h, s); }
Result HolderPopOutData(AppletHolder *h, AppletStorage *s) { return ::appletHolderPopOutData(h, s); }
Result HolderPushExtraStorage(AppletHolder *h, AppletStorage *s) { return ::appletHolderPushExtraStorage(h, s); }
Result HolderPushInteractiveInData(AppletHolder *h, AppletStorage *s) { return ::appletHolderPushInteractiveInData(h, s); }
Result HolderPopInteractiveOutData(AppletHolder *h, AppletStorage *s) { return ::appletHolderPopInteractiveOutData(h, s); }
Result HolderGetLibraryAppletInfo(AppletHolder *h, LibAppletInfo *info) { return ::appletHolderGetLibraryAppletInfo(h, info); }
Result CreateStorage(AppletStorage *s, s64 size) { return ::appletCreateStorage(s, size); }
Result CreateTransferMemoryStorage(AppletStorage *s, void* buffer, s64 size, bool writable) { return ::appletCreateTransferMemoryStorage(s, buffer, size, writable); }
Result CreateHandleStorage(AppletStorage *s, s64 inval, Handle handle) { return ::appletCreateHandleStorage(s, inval, handle); }
Result CreateHandleStorageTmem(AppletStorage *s, void* buffer, s64 size) { return ::appletCreateHandleStorageTmem(s, buffer, size); }
void StorageClose(AppletStorage *s) { ::appletStorageClose(s); }
void StorageCloseTmem(AppletStorage *s) { ::appletStorageCloseTmem(s); }
Result StorageGetSize(AppletStorage *s, s64 *size) { return ::appletStorageGetSize(s, size); }
Result StorageWrite(AppletStorage *s, s64 offset, const void* buffer, size_t size) { return ::appletStorageWrite(s, offset, buffer, size); }
Result StorageRead(AppletStorage *s, s64 offset, void* buffer, size_t size) { return ::appletStorageRead(s, offset, buffer, size); }
Result StorageGetHandle(AppletStorage *s, s64 *out, Handle *handle) { return ::appletStorageGetHandle(s, out, handle); }
Result StorageMap(AppletStorage *s, void** addr, size_t *size) { return ::appletStorageMap(s, addr, size); }
Result PopLaunchParameter(AppletStorage *s, AppletLaunchParameterKind kind) { return ::appletPopLaunchParameter(s, kind); }
Result RequestLaunchApplication(u64 application_id, AppletStorage* s) { return ::appletRequestLaunchApplication(application_id, s); }
Result RequestLaunchApplicationForQuest(u64 application_id, AppletStorage* s, const AppletApplicationAttributeForQuest *attr) { return ::appletRequestLaunchApplicationForQuest(application_id, s, attr); }
Result GetDesiredLanguage(u64 *LanguageCode) { return ::appletGetDesiredLanguage(LanguageCode); }
Result GetDisplayVersion(char *displayVersion) { return ::appletGetDisplayVersion(displayVersion); }
Result BeginBlockingHomeButtonShortAndLongPressed(s64 val) { return ::appletBeginBlockingHomeButtonShortAndLongPressed(val); }
Result EndBlockingHomeButtonShortAndLongPressed(void) { return ::appletEndBlockingHomeButtonShortAndLongPressed(); }
Result BeginBlockingHomeButton(s64 val) { return ::appletBeginBlockingHomeButton(val); }
Result EndBlockingHomeButton(void) { return ::appletEndBlockingHomeButton(); }
void NotifyRunning(bool *out) { ::appletNotifyRunning(out); }
Result GetPseudoDeviceId(Uuid *out) { return ::appletGetPseudoDeviceId(out); }
Result SetMediaPlaybackState(bool state) { return ::appletSetMediaPlaybackState(state); }
Result IsGamePlayRecordingSupported(bool *flag) { return ::appletIsGamePlayRecordingSupported(flag); }
Result SetGamePlayRecordingState(bool state) { return ::appletSetGamePlayRecordingState(state); }
Result InitializeGamePlayRecording(void) { return ::appletInitializeGamePlayRecording(); }
Result RequestFlushGamePlayingMovieForDebug(void) { return ::appletRequestFlushGamePlayingMovieForDebug(); }
Result RequestToShutdown(void) { return ::appletRequestToShutdown(); }
Result RequestToReboot(void) { return ::appletRequestToReboot(); }
Result RequestToSleep(void) { return ::appletRequestToSleep(); }
Result ExitAndRequestToShowThanksMessage(void) { return ::appletExitAndRequestToShowThanksMessage(); }
Result InitializeApplicationCopyrightFrameBuffer(void) { return ::appletInitializeApplicationCopyrightFrameBuffer(); }
Result SetApplicationCopyrightImage(const void* buffer, size_t size, s32 x, s32 y, s32 width, s32 height, AppletWindowOriginMode mode) { return ::appletSetApplicationCopyrightImage(buffer, size, x, y, width, height, mode); }
Result SetApplicationCopyrightVisibility(bool visible) { return ::appletSetApplicationCopyrightVisibility(visible); }
Result QueryApplicationPlayStatistics(PdmApplicationPlayStatistics *stats, const u64 *application_ids, s32 count, s32 *total_out) { return ::appletQueryApplicationPlayStatistics(stats, application_ids, count, total_out); }
Result QueryApplicationPlayStatisticsByUid(AccountUid uid, PdmApplicationPlayStatistics *stats, const u64 *application_ids, s32 count, s32 *total_out) { return ::appletQueryApplicationPlayStatisticsByUid(uid, stats, application_ids, count, total_out); }
Result ExecuteProgram(s32 programIndex, const void* buffer, size_t size) { return ::appletExecuteProgram(programIndex, buffer, size); }
Result JumpToSubApplicationProgramForDevelopment(u64 application_id, const void* buffer, size_t size) { return ::appletJumpToSubApplicationProgramForDevelopment(application_id, buffer, size); }
Result RestartProgram(const void* buffer, size_t size) { return ::appletRestartProgram(buffer, size); }
Result GetPreviousProgramIndex(s32 *programIndex) { return ::appletGetPreviousProgramIndex(programIndex); }
Result SetDelayTimeToAbortOnGpuError(u64 val) { return ::appletSetDelayTimeToAbortOnGpuError(val); }
Result GetFriendInvitationStorageChannelEvent(Event *out_event) { return ::appletGetFriendInvitationStorageChannelEvent(out_event); }
Result TryPopFromFriendInvitationStorageChannel(AppletStorage *s) { return ::appletTryPopFromFriendInvitationStorageChannel(s); }
Result GetNotificationStorageChannelEvent(Event *out_event) { return ::appletGetNotificationStorageChannelEvent(out_event); }
Result TryPopFromNotificationStorageChannel(AppletStorage *s) { return ::appletTryPopFromNotificationStorageChannel(s); }
Result GetHealthWarningDisappearedSystemEvent(Event *out_event) { return ::appletGetHealthWarningDisappearedSystemEvent(out_event); }
Result SetHdcpAuthenticationActivated(bool flag) { return ::appletSetHdcpAuthenticationActivated(flag); }
Result GetLastApplicationExitReason(s32 *out) { return ::appletGetLastApplicationExitReason(out); }
Result CreateMovieMaker(Service* srv_out, TransferMemory *tmem) { return ::appletCreateMovieMaker(srv_out, tmem); }
Result PrepareForJit(void) { return ::appletPrepareForJit(); }
Result RequestToGetForeground(void) { return ::appletRequestToGetForeground(); }
Result LockForeground(void) { return ::appletLockForeground(); }
Result UnlockForeground(void) { return ::appletUnlockForeground(); }
Result PopFromGeneralChannel(AppletStorage *s) { return ::appletPopFromGeneralChannel(s); }
Result GetPopFromGeneralChannelEvent(Event *out_event) { return ::appletGetPopFromGeneralChannelEvent(out_event); }
Result GetHomeButtonWriterLockAccessor(AppletLockAccessor *a) { return ::appletGetHomeButtonWriterLockAccessor(a); }
Result IsSleepEnabled(bool *out) { return ::appletIsSleepEnabled(out); }
Result PopRequestLaunchApplicationForDebug(AccountUid *uids, s32 count, u64 *application_id, s32 *total_out) { return ::appletPopRequestLaunchApplicationForDebug(uids, count, application_id, total_out); }
Result IsForceTerminateApplicationDisabledForDebug(bool *out) { return ::appletIsForceTerminateApplicationDisabledForDebug(out); }
Result LaunchDevMenu(void) { return ::appletLaunchDevMenu(); }
Result SetLastApplicationExitReason(s32 reason) { return ::appletSetLastApplicationExitReason(reason); }
Result StartSleepSequence(bool flag) { return ::appletStartSleepSequence(flag); }
Result StartShutdownSequence(void) { return ::appletStartShutdownSequence(); }
Result StartRebootSequence(void) { return ::appletStartRebootSequence(); }
Result IsAutoPowerDownRequested(bool *out) { return ::appletIsAutoPowerDownRequested(out); }
Result LoadAndApplyIdlePolicySettings(void) { return ::appletLoadAndApplyIdlePolicySettings(); }
Result NotifyCecSettingsChanged(void) { return ::appletNotifyCecSettingsChanged(); }
Result SetDefaultHomeButtonLongPressTime(s64 val) { return ::appletSetDefaultHomeButtonLongPressTime(val); }
Result UpdateDefaultDisplayResolution(void) { return ::appletUpdateDefaultDisplayResolution(); }
Result ShouldSleepOnBoot(bool *out) { return ::appletShouldSleepOnBoot(out); }
Result GetHdcpAuthenticationFailedEvent(Event *out_event) { return ::appletGetHdcpAuthenticationFailedEvent(out_event); }
Result CreateApplication(AppletApplication *a, u64 application_id) { return ::appletCreateApplication(a, application_id); }
Result PopLaunchRequestedApplication(AppletApplication *a) { return ::appletPopLaunchRequestedApplication(a); }
Result CreateSystemApplication(AppletApplication *a, u64 system_application_id) { return ::appletCreateSystemApplication(a, system_application_id); }
Result PopFloatingApplicationForDevelopment(AppletApplication *a) { return ::appletPopFloatingApplicationForDevelopment(a); }
void ApplicationClose(AppletApplication *a) { ::appletApplicationClose(a); }
bool ApplicationActive(AppletApplication *a) { return ::appletApplicationActive(a); }
Result ApplicationStart(AppletApplication *a) { return ::appletApplicationStart(a); }
Result ApplicationRequestExit(AppletApplication *a) { return ::appletApplicationRequestExit(a); }
Result ApplicationTerminate(AppletApplication *a) { return ::appletApplicationTerminate(a); }
void ApplicationJoin(AppletApplication *a) { ::appletApplicationJoin(a); }
bool ApplicationCheckFinished(AppletApplication *a) { return ::appletApplicationCheckFinished(a); }
AppletApplicationExitReason ApplicationGetExitReason(AppletApplication *a) { return ::appletApplicationGetExitReason(a); }
Result ApplicationRequestForApplicationToGetForeground(AppletApplication *a) { return ::appletApplicationRequestForApplicationToGetForeground(a); }
Result ApplicationTerminateAllLibraryApplets(AppletApplication *a) { return ::appletApplicationTerminateAllLibraryApplets(a); }
Result ApplicationAreAnyLibraryAppletsLeft(AppletApplication *a, bool *out) { return ::appletApplicationAreAnyLibraryAppletsLeft(a, out); }
Result ApplicationRequestExitLibraryAppletOrTerminate(AppletApplication *a, u64 timeout) { return ::appletApplicationRequestExitLibraryAppletOrTerminate(a, timeout); }
Result ApplicationGetApplicationId(AppletApplication *a, u64 *application_id) { return ::appletApplicationGetApplicationId(a, application_id); }
Result ApplicationPushLaunchParameter(AppletApplication *a, AppletLaunchParameterKind kind, AppletStorage* s) { return ::appletApplicationPushLaunchParameter(a, kind, s); }
Result ApplicationGetApplicationControlProperty(AppletApplication *a, NacpStruct *nacp) { return ::appletApplicationGetApplicationControlProperty(a, nacp); }
Result ApplicationGetApplicationLaunchProperty(AppletApplication *a, AppletApplicationLaunchProperty *out) { return ::appletApplicationGetApplicationLaunchProperty(a, out); }
Result ApplicationGetApplicationLaunchRequestInfo(AppletApplication *a, AppletApplicationLaunchRequestInfo *out) { return ::appletApplicationGetApplicationLaunchRequestInfo(a, out); }
Result ApplicationSetUsers(AppletApplication *a, const AccountUid *uids, s32 count, bool flag) { return ::appletApplicationSetUsers(a, uids, count, flag); }
Result ApplicationCheckRightsEnvironmentAvailable(AppletApplication *a, bool *out) { return ::appletApplicationCheckRightsEnvironmentAvailable(a, out); }
Result ApplicationGetNsRightsEnvironmentHandle(AppletApplication *a, u64 *handle) { return ::appletApplicationGetNsRightsEnvironmentHandle(a, handle); }
Result ApplicationGetDesirableUids(AppletApplication *a, AccountUid *uids, s32 count, s32 *total_out) { return ::appletApplicationGetDesirableUids(a, uids, count, total_out); }
Result ApplicationReportApplicationExitTimeout(AppletApplication *a) { return ::appletApplicationReportApplicationExitTimeout(a); }
Result ApplicationSetApplicationAttribute(AppletApplication *a, const AppletApplicationAttribute *attr) { return ::appletApplicationSetApplicationAttribute(a, attr); }
Result ApplicationHasSaveDataAccessPermission(AppletApplication *a, u64 application_id, bool *out) { return ::appletApplicationHasSaveDataAccessPermission(a, application_id, out); }
Result ApplicationPushToFriendInvitationStorageChannel(AppletApplication *a, AccountUid uid, const void* buffer, u64 size) { return ::appletApplicationPushToFriendInvitationStorageChannel(a, uid, buffer, size); }
Result ApplicationPushToNotificationStorageChannel(AppletApplication *a, const void* buffer, u64 size) { return ::appletApplicationPushToNotificationStorageChannel(a, buffer, size); }
Result ApplicationRequestApplicationSoftReset(AppletApplication *a) { return ::appletApplicationRequestApplicationSoftReset(a); }
Result ApplicationRestartApplicationTimer(AppletApplication *a) { return ::appletApplicationRestartApplicationTimer(a); }
Result PopInData(AppletStorage *s) { return ::appletPopInData(s); }
Result PushOutData(AppletStorage *s) { return ::appletPushOutData(s); }
Result PopInteractiveInData(AppletStorage *s) { return ::appletPopInteractiveInData(s); }
Result PushInteractiveOutData(AppletStorage *s) { return ::appletPushInteractiveOutData(s); }
Result GetPopInDataEvent(Event *out_event) { return ::appletGetPopInDataEvent(out_event); }
Result GetPopInteractiveInDataEvent(Event *out_event) { return ::appletGetPopInteractiveInDataEvent(out_event); }
Result GetLibraryAppletInfo(LibAppletInfo *info) { return ::appletGetLibraryAppletInfo(info); }
Result GetMainAppletIdentityInfo(AppletIdentityInfo *info) { return ::appletGetMainAppletIdentityInfo(info); }
Result CanUseApplicationCore(bool *out) { return ::appletCanUseApplicationCore(out); }
Result GetCallerAppletIdentityInfo(AppletIdentityInfo *info) { return ::appletGetCallerAppletIdentityInfo(info); }
Result GetMainAppletApplicationControlProperty(NacpStruct *nacp) { return ::appletGetMainAppletApplicationControlProperty(nacp); }
Result GetMainAppletStorageId(NcmStorageId *storageId) { return ::appletGetMainAppletStorageId(storageId); }
Result GetCallerAppletIdentityInfoStack(AppletIdentityInfo *stack, s32 count, s32 *total_out) { return ::appletGetCallerAppletIdentityInfoStack(stack, count, total_out); }
Result GetNextReturnDestinationAppletIdentityInfo(AppletIdentityInfo *info) { return ::appletGetNextReturnDestinationAppletIdentityInfo(info); }
Result GetDesirableKeyboardLayout(SetKeyboardLayout *layout) { return ::appletGetDesirableKeyboardLayout(layout); }
Result PopExtraStorage(AppletStorage *s) { return ::appletPopExtraStorage(s); }
Result GetPopExtraStorageEvent(Event *out_event) { return ::appletGetPopExtraStorageEvent(out_event); }
Result UnpopInData(AppletStorage *s) { return ::appletUnpopInData(s); }
Result UnpopExtraStorage(AppletStorage *s) { return ::appletUnpopExtraStorage(s); }
Result GetIndirectLayerProducerHandle(u64 *out) { return ::appletGetIndirectLayerProducerHandle(out); }
Result GetMainAppletApplicationDesiredLanguage(u64 *LanguageCode) { return ::appletGetMainAppletApplicationDesiredLanguage(LanguageCode); }
Result GetCurrentApplicationId(u64 *application_id) { return ::appletGetCurrentApplicationId(application_id); }
Result RequestExitToSelf(void) { return ::appletRequestExitToSelf(); }
Result CreateGameMovieTrimmer(Service* srv_out, TransferMemory *tmem) { return ::appletCreateGameMovieTrimmer(srv_out, tmem); }
Result ReserveResourceForMovieOperation(void) { return ::appletReserveResourceForMovieOperation(); }
Result UnreserveResourceForMovieOperation(void) { return ::appletUnreserveResourceForMovieOperation(); }
Result GetMainAppletAvailableUsers(AccountUid *uids, s32 count, bool *flag, s32 *total_out) { return ::appletGetMainAppletAvailableUsers(uids, count, flag, total_out); }
Result SetApplicationMemoryReservation(u64 val) { return ::appletSetApplicationMemoryReservation(val); }
Result ShouldSetGpuTimeSliceManually(bool *out) { return ::appletShouldSetGpuTimeSliceManually(out); }
Result BeginToWatchShortHomeButtonMessage(void) { return ::appletBeginToWatchShortHomeButtonMessage(); }
Result EndToWatchShortHomeButtonMessage(void) { return ::appletEndToWatchShortHomeButtonMessage(); }
Result GetApplicationIdForLogo(u64 *application_id) { return ::appletGetApplicationIdForLogo(application_id); }
Result SetGpuTimeSliceBoost(u64 val) { return ::appletSetGpuTimeSliceBoost(val); }
Result SetAutoSleepTimeAndDimmingTimeEnabled(bool flag) { return ::appletSetAutoSleepTimeAndDimmingTimeEnabled(flag); }
Result TerminateApplicationAndSetReason(Result reason) { return ::appletTerminateApplicationAndSetReason(reason); }
Result SetScreenShotPermissionGlobally(bool flag) { return ::appletSetScreenShotPermissionGlobally(flag); }
Result StartShutdownSequenceForOverlay(void) { return ::appletStartShutdownSequenceForOverlay(); }
Result StartRebootSequenceForOverlay(void) { return ::appletStartRebootSequenceForOverlay(); }
Result SetHealthWarningShowingState(bool flag) { return ::appletSetHealthWarningShowingState(flag); }
Result IsHealthWarningRequired(bool *out) { return ::appletIsHealthWarningRequired(out); }
Result BeginToObserveHidInputForDevelop(void) { return ::appletBeginToObserveHidInputForDevelop(); }
Result ReadThemeStorage(void* buffer, size_t size, u64 offset, u64 *transfer_size) { return ::appletReadThemeStorage(buffer, size, offset, transfer_size); }
Result WriteThemeStorage(const void* buffer, size_t size, u64 offset) { return ::appletWriteThemeStorage(buffer, size, offset); }
Result PushToAppletBoundChannel(AppletStorage *s) { return ::appletPushToAppletBoundChannel(s); }
Result TryPopFromAppletBoundChannel(AppletStorage *s) { return ::appletTryPopFromAppletBoundChannel(s); }
Result GetDisplayLogicalResolution(s32 *width, s32 *height) { return ::appletGetDisplayLogicalResolution(width, height); }
Result SetDisplayMagnification(float x, float y, float width, float height) { return ::appletSetDisplayMagnification(x, y, width, height); }
Result SetHomeButtonDoubleClickEnabled(bool flag) { return ::appletSetHomeButtonDoubleClickEnabled(flag); }
Result GetHomeButtonDoubleClickEnabled(bool *out) { return ::appletGetHomeButtonDoubleClickEnabled(out); }
Result IsHomeButtonShortPressedBlocked(bool *out) { return ::appletIsHomeButtonShortPressedBlocked(out); }
Result IsVrModeCurtainRequired(bool *out) { return ::appletIsVrModeCurtainRequired(out); }
Result SetCpuBoostRequestPriority(s32 priority) { return ::appletSetCpuBoostRequestPriority(priority); }
Result OpenMainApplication(AppletApplication *a) { return ::appletOpenMainApplication(a); }
Result PerformSystemButtonPressing(AppletSystemButtonType type) { return ::appletPerformSystemButtonPressing(type); }
Result InvalidateTransitionLayer(void) { return ::appletInvalidateTransitionLayer(); }
Result RequestLaunchApplicationWithUserAndArgumentForDebug(u64 application_id, const AccountUid *uids, s32 total_uids, bool flag, const void* buffer, size_t size) { return ::appletRequestLaunchApplicationWithUserAndArgumentForDebug(application_id, uids, total_uids, flag, buffer, size); }
Result GetAppletResourceUsageInfo(AppletResourceUsageInfo *info) { return ::appletGetAppletResourceUsageInfo(info); }
Result PushToAppletBoundChannelForDebug(AppletStorage *s, s32 channel) { return ::appletPushToAppletBoundChannelForDebug(s, channel); }
Result TryPopFromAppletBoundChannelForDebug(AppletStorage *s, s32 channel) { return ::appletTryPopFromAppletBoundChannelForDebug(s, channel); }
Result AlarmSettingNotificationEnableAppEventReserve(AppletStorage *s, u64 application_id) { return ::appletAlarmSettingNotificationEnableAppEventReserve(s, application_id); }
Result AlarmSettingNotificationDisableAppEventReserve(void) { return ::appletAlarmSettingNotificationDisableAppEventReserve(); }
Result AlarmSettingNotificationPushAppEventNotify(const void* buffer, u64 size) { return ::appletAlarmSettingNotificationPushAppEventNotify(buffer, size); }
Result FriendInvitationSetApplicationParameter(AppletStorage *s, u64 application_id) { return ::appletFriendInvitationSetApplicationParameter(s, application_id); }
Result FriendInvitationClearApplicationParameter(void) { return ::appletFriendInvitationClearApplicationParameter(); }
Result FriendInvitationPushApplicationParameter(AccountUid uid, const void* buffer, u64 size) { return ::appletFriendInvitationPushApplicationParameter(uid, buffer, size); }
Result CreateGeneralStorageForDebug(u64 id, u64 size) { return ::appletCreateGeneralStorageForDebug(id, size); }
Result ReadGeneralStorageForDebug(void* buffer, size_t size, u64 id, u64 offset, u64 *out_size) { return ::appletReadGeneralStorageForDebug(buffer, size, id, offset, out_size); }
Result WriteGeneralStorageForDebug(const void* buffer, size_t size, u64 id, u64 offset) { return ::appletWriteGeneralStorageForDebug(buffer, size, id, offset); }
Result SetTerminateResult(Result res) { return ::appletSetTerminateResult(res); }
Result GetLaunchStorageInfoForDebug(NcmStorageId *app_storageId, NcmStorageId *update_storageId) { return ::appletGetLaunchStorageInfoForDebug(app_storageId, update_storageId); }
Result GetGpuErrorDetectedSystemEvent(Event *out_event) { return ::appletGetGpuErrorDetectedSystemEvent(out_event); }
Result SetHandlingHomeButtonShortPressedEnabled(bool flag) { return ::appletSetHandlingHomeButtonShortPressedEnabled(flag); }
AppletInfo * GetAppletInfo(void) { return ::appletGetAppletInfo(); }
Event * GetMessageEvent(void) { return ::appletGetMessageEvent(); }
Result GetMessage(u32 *msg) { return ::appletGetMessage(msg); }
bool ProcessMessage(u32 msg) { return ::appletProcessMessage(msg); }
bool MainLoop(void) { return ::appletMainLoop(); }
void Hook(AppletHookCookie* cookie, AppletHookFn callback, void* param) { ::appletHook(cookie, callback, param); }
void Unhook(AppletHookCookie* cookie) { ::appletUnhook(cookie); }
AppletOperationMode GetOperationMode(void) { return ::appletGetOperationMode(); }
ApmPerformanceMode GetPerformanceMode(void) { return ::appletGetPerformanceMode(); }
AppletFocusState GetFocusState(void) { return ::appletGetFocusState(); }
Result SetFocusHandlingMode(AppletFocusHandlingMode mode) { return ::appletSetFocusHandlingMode(mode); }

} // namespace applet

namespace arm {

void DCacheFlush(void* addr, size_t size) { ::armDCacheFlush(addr, size); }
void DCacheClean(void* addr, size_t size) { ::armDCacheClean(addr, size); }
void ICacheInvalidate(void* addr, size_t size) { ::armICacheInvalidate(addr, size); }
void DCacheZero(void* addr, size_t size) { ::armDCacheZero(addr, size); }
u64 GetSystemTick(void) { return ::armGetSystemTick(); }
u64 GetSystemTickFreq(void) { return ::armGetSystemTickFreq(); }
u64 NsToTicks(u64 ns) { return ::armNsToTicks(ns); }
u64 TicksToNs(u64 tick) { return ::armTicksToNs(tick); }
void* GetTls(void) { return ::armGetTls(); }

} // namespace arm

namespace async {

void ValueClose(AsyncValue *a) { ::asyncValueClose(a); }
Result ValueWait(AsyncValue *a, u64 timeout) { return ::asyncValueWait(a, timeout); }
Result ValueGetSize(AsyncValue *a, u64 *size) { return ::asyncValueGetSize(a, size); }
Result ValueGet(AsyncValue *a, void* buffer, size_t size) { return ::asyncValueGet(a, buffer, size); }
Result ValueCancel(AsyncValue *a) { return ::asyncValueCancel(a); }
Result ValueGetErrorContext(AsyncValue *a, ErrorContext *context) { return ::asyncValueGetErrorContext(a, context); }
void ResultClose(AsyncResult *a) { ::asyncResultClose(a); }
Result ResultWait(AsyncResult *a, u64 timeout) { return ::asyncResultWait(a, timeout); }
Result ResultGet(AsyncResult *a) { return ::asyncResultGet(a); }
Result ResultCancel(AsyncResult *a) { return ::asyncResultCancel(a); }
Result ResultGetErrorContext(AsyncResult *a, ErrorContext *context) { return ::asyncResultGetErrorContext(a, context); }

} // namespace async

namespace auda {

Result Initialize(void) { return ::audaInitialize(); }
void Exit(void) { ::audaExit(); }
Service* GetServiceSession(void) { return ::audaGetServiceSession(); }
Result RequestSuspendAudio(u64 pid, u64 delay) { return ::audaRequestSuspendAudio(pid, delay); }
Result RequestResumeAudio(u64 pid, u64 delay) { return ::audaRequestResumeAudio(pid, delay); }
Result GetAudioOutputProcessMasterVolume(u64 pid, float* volume_out) { return ::audaGetAudioOutputProcessMasterVolume(pid, volume_out); }
Result SetAudioOutputProcessMasterVolume(u64 pid, u64 delay, float volume) { return ::audaSetAudioOutputProcessMasterVolume(pid, delay, volume); }
Result GetAudioInputProcessMasterVolume(u64 pid, float* volume_out) { return ::audaGetAudioInputProcessMasterVolume(pid, volume_out); }
Result SetAudioInputProcessMasterVolume(u64 pid, u64 delay, float volume) { return ::audaSetAudioInputProcessMasterVolume(pid, delay, volume); }
Result GetAudioOutputProcessRecordVolume(u64 pid, float* volume_out) { return ::audaGetAudioOutputProcessRecordVolume(pid, volume_out); }
Result SetAudioOutputProcessRecordVolume(u64 pid, u64 delay, float volume) { return ::audaSetAudioOutputProcessRecordVolume(pid, delay, volume); }

} // namespace auda

namespace audctl {

Result Initialize(void) { return ::audctlInitialize(); }
void Exit(void) { ::audctlExit(); }
Service* GetServiceSession(void) { return ::audctlGetServiceSession(); }
Result GetTargetVolume(s32* volume_out, AudioTarget target) { return ::audctlGetTargetVolume(volume_out, target); }
Result SetTargetVolume(AudioTarget target, s32 volume) { return ::audctlSetTargetVolume(target, volume); }
Result GetTargetVolumeMin(s32* volume_out) { return ::audctlGetTargetVolumeMin(volume_out); }
Result GetTargetVolumeMax(s32* volume_out) { return ::audctlGetTargetVolumeMax(volume_out); }
Result IsTargetMute(bool* mute_out, AudioTarget target) { return ::audctlIsTargetMute(mute_out, target); }
Result SetTargetMute(AudioTarget target, bool mute) { return ::audctlSetTargetMute(target, mute); }
Result IsTargetConnected(bool* connected_out, AudioTarget target) { return ::audctlIsTargetConnected(connected_out, target); }
Result SetDefaultTarget(AudioTarget target, u64 fade_in_ns, u64 fade_out_ns) { return ::audctlSetDefaultTarget(target, fade_in_ns, fade_out_ns); }
Result GetDefaultTarget(AudioTarget* target_out) { return ::audctlGetDefaultTarget(target_out); }
Result GetAudioOutputMode(AudioOutputMode* mode_out, AudioTarget target) { return ::audctlGetAudioOutputMode(mode_out, target); }
Result SetAudioOutputMode(AudioTarget target, AudioOutputMode mode) { return ::audctlSetAudioOutputMode(target, mode); }
Result SetForceMutePolicy(AudioForceMutePolicy policy) { return ::audctlSetForceMutePolicy(policy); }
Result GetForceMutePolicy(AudioForceMutePolicy* policy_out) { return ::audctlGetForceMutePolicy(policy_out); }
Result GetOutputModeSetting(AudioOutputMode* mode_out, AudioTarget target) { return ::audctlGetOutputModeSetting(mode_out, target); }
Result SetOutputModeSetting(AudioTarget target, AudioOutputMode mode) { return ::audctlSetOutputModeSetting(target, mode); }
Result SetOutputTarget(AudioTarget target) { return ::audctlSetOutputTarget(target); }
Result SetInputTargetForceEnabled(bool enable) { return ::audctlSetInputTargetForceEnabled(enable); }
Result SetHeadphoneOutputLevelMode(AudioHeadphoneOutputLevelMode mode) { return ::audctlSetHeadphoneOutputLevelMode(mode); }
Result GetHeadphoneOutputLevelMode(AudioHeadphoneOutputLevelMode* mode_out) { return ::audctlGetHeadphoneOutputLevelMode(mode_out); }
Result AcquireAudioVolumeUpdateEventForPlayReport(Event* event_out) { return ::audctlAcquireAudioVolumeUpdateEventForPlayReport(event_out); }
Result AcquireAudioOutputDeviceUpdateEventForPlayReport(Event* event_out) { return ::audctlAcquireAudioOutputDeviceUpdateEventForPlayReport(event_out); }
Result GetAudioOutputTargetForPlayReport(AudioTarget* target_out) { return ::audctlGetAudioOutputTargetForPlayReport(target_out); }
Result NotifyHeadphoneVolumeWarningDisplayedEvent(void) { return ::audctlNotifyHeadphoneVolumeWarningDisplayedEvent(); }
Result SetSystemOutputMasterVolume(float volume) { return ::audctlSetSystemOutputMasterVolume(volume); }
Result GetSystemOutputMasterVolume(float* volume_out) { return ::audctlGetSystemOutputMasterVolume(volume_out); }
Result GetActiveOutputTarget(AudioTarget* target) { return ::audctlGetActiveOutputTarget(target); }

} // namespace audctl

namespace audd {

Result Initialize(void) { return ::auddInitialize(); }
void Exit(void) { ::auddExit(); }
Service* GetServiceSession(void) { return ::auddGetServiceSession(); }
Result RequestSuspendAudioForDebug(u64 pid, u64 delay) { return ::auddRequestSuspendAudioForDebug(pid, delay); }
Result RequestResumeAudioForDebug(u64 pid, u64 delay) { return ::auddRequestResumeAudioForDebug(pid, delay); }

} // namespace audd

namespace auddev {

Result Initialize(void) { return ::auddevInitialize(); }
void Exit(void) { ::auddevExit(); }
Service* GetServiceSession(void) { return ::auddevGetServiceSession(); }
Result ListAudioDeviceName(AudioDeviceName *DeviceNames, s32 max_names, s32 *total_names) { return ::auddevListAudioDeviceName(DeviceNames, max_names, total_names); }
Result SetAudioDeviceOutputVolume(const AudioDeviceName *DeviceName, float volume) { return ::auddevSetAudioDeviceOutputVolume(DeviceName, volume); }
Result GetAudioDeviceOutputVolume(const AudioDeviceName *DeviceName, float *volume) { return ::auddevGetAudioDeviceOutputVolume(DeviceName, volume); }
Result GetActiveAudioDeviceName(AudioDeviceName *DeviceName) { return ::auddevGetActiveAudioDeviceName(DeviceName); }

} // namespace auddev

namespace audin {

Result Initialize(void) { return ::audinInitialize(); }
void Exit(void) { ::audinExit(); }
Service* GetServiceSession(void) { return ::audinGetServiceSession(); }
Service* GetServiceSession_AudioIn(void) { return ::audinGetServiceSession_AudioIn(); }
Result ListAudioIns(char *DeviceNames, s32 count, u32 *DeviceNamesCount) { return ::audinListAudioIns(DeviceNames, count, DeviceNamesCount); }
Result OpenAudioIn(const char *DeviceNameIn, char *DeviceNameOut, u32 SampleRateIn, u32 ChannelCountIn, u32 *SampleRateOut, u32 *ChannelCountOut, PcmFormat *Format, AudioInState *State) { return ::audinOpenAudioIn(DeviceNameIn, DeviceNameOut, SampleRateIn, ChannelCountIn, SampleRateOut, ChannelCountOut, Format, State); }
Result GetAudioInState(AudioInState *State) { return ::audinGetAudioInState(State); }
Result StartAudioIn(void) { return ::audinStartAudioIn(); }
Result StopAudioIn(void) { return ::audinStopAudioIn(); }
Result AppendAudioInBuffer(AudioInBuffer *Buffer) { return ::audinAppendAudioInBuffer(Buffer); }
Result GetReleasedAudioInBuffer(AudioInBuffer **Buffer, u32 *ReleasedBuffersCount) { return ::audinGetReleasedAudioInBuffer(Buffer, ReleasedBuffersCount); }
Result ContainsAudioInBuffer(AudioInBuffer *Buffer, bool *ContainsBuffer) { return ::audinContainsAudioInBuffer(Buffer, ContainsBuffer); }
Result CaptureBuffer(AudioInBuffer *source, AudioInBuffer **released) { return ::audinCaptureBuffer(source, released); }
Result WaitCaptureFinish(AudioInBuffer **released, u32* released_count, u64 timeout) { return ::audinWaitCaptureFinish(released, released_count, timeout); }
u32 GetSampleRate(void) { return ::audinGetSampleRate(); }
u32 GetChannelCount(void) { return ::audinGetChannelCount(); }
PcmFormat GetPcmFormat(void) { return ::audinGetPcmFormat(); }
AudioInState GetDeviceState(void) { return ::audinGetDeviceState(); }

} // namespace audin

namespace audout {

Result Initialize(void) { return ::audoutInitialize(); }
void Exit(void) { ::audoutExit(); }
Service* GetServiceSession(void) { return ::audoutGetServiceSession(); }
Service* GetServiceSession_AudioOut(void) { return ::audoutGetServiceSession_AudioOut(); }
Result ListAudioOuts(char *DeviceNames, s32 count, u32 *DeviceNamesCount) { return ::audoutListAudioOuts(DeviceNames, count, DeviceNamesCount); }
Result OpenAudioOut(const char *DeviceNameIn, char *DeviceNameOut, u32 SampleRateIn, u32 ChannelCountIn, u32 *SampleRateOut, u32 *ChannelCountOut, PcmFormat *Format, AudioOutState *State) { return ::audoutOpenAudioOut(DeviceNameIn, DeviceNameOut, SampleRateIn, ChannelCountIn, SampleRateOut, ChannelCountOut, Format, State); }
Result GetAudioOutState(AudioOutState *State) { return ::audoutGetAudioOutState(State); }
Result StartAudioOut(void) { return ::audoutStartAudioOut(); }
Result StopAudioOut(void) { return ::audoutStopAudioOut(); }
Result AppendAudioOutBuffer(AudioOutBuffer *Buffer) { return ::audoutAppendAudioOutBuffer(Buffer); }
Result GetReleasedAudioOutBuffer(AudioOutBuffer **Buffer, u32 *ReleasedBuffersCount) { return ::audoutGetReleasedAudioOutBuffer(Buffer, ReleasedBuffersCount); }
Result ContainsAudioOutBuffer(AudioOutBuffer *Buffer, bool *ContainsBuffer) { return ::audoutContainsAudioOutBuffer(Buffer, ContainsBuffer); }
Result GetAudioOutBufferCount(u32 *count) { return ::audoutGetAudioOutBufferCount(count); }
Result GetAudioOutPlayedSampleCount(u64 *count) { return ::audoutGetAudioOutPlayedSampleCount(count); }
Result FlushAudioOutBuffers(bool *flushed) { return ::audoutFlushAudioOutBuffers(flushed); }
Result SetAudioOutVolume(float volume) { return ::audoutSetAudioOutVolume(volume); }
Result GetAudioOutVolume(float *volume) { return ::audoutGetAudioOutVolume(volume); }
Result PlayBuffer(AudioOutBuffer *source, AudioOutBuffer **released) { return ::audoutPlayBuffer(source, released); }
Result WaitPlayFinish(AudioOutBuffer **released, u32* released_count, u64 timeout) { return ::audoutWaitPlayFinish(released, released_count, timeout); }
u32 GetSampleRate(void) { return ::audoutGetSampleRate(); }
u32 GetChannelCount(void) { return ::audoutGetChannelCount(); }
PcmFormat GetPcmFormat(void) { return ::audoutGetPcmFormat(); }
AudioOutState GetDeviceState(void) { return ::audoutGetDeviceState(); }

} // namespace audout

namespace audouta {

Result Initialize(void) { return ::audoutaInitialize(); }
void Exit(void) { ::audoutaExit(); }
Service* GetServiceSession(void) { return ::audoutaGetServiceSession(); }
Result RequestSuspend(u64 pid, u64 delay) { return ::audoutaRequestSuspend(pid, delay); }
Result RequestResume(u64 pid, u64 delay) { return ::audoutaRequestResume(pid, delay); }
Result GetProcessMasterVolume(u64 pid, float* volume_out) { return ::audoutaGetProcessMasterVolume(pid, volume_out); }
Result SetProcessMasterVolume(u64 pid, u64 delay, float volume) { return ::audoutaSetProcessMasterVolume(pid, delay, volume); }
Result GetProcessRecordVolume(u64 pid, float* volume_out) { return ::audoutaGetProcessRecordVolume(pid, volume_out); }
Result SetProcessRecordVolume(u64 pid, u64 delay, float volume) { return ::audoutaSetProcessRecordVolume(pid, delay, volume); }

} // namespace audouta

namespace audoutd {

Result Initialize(void) { return ::audoutdInitialize(); }
void Exit(void) { ::audoutdExit(); }
Service* GetServiceSession(void) { return ::audoutdGetServiceSession(); }
Result RequestSuspendForDebug(u64 pid, u64 delay) { return ::audoutdRequestSuspendForDebug(pid, delay); }
Result RequestResumeForDebug(u64 pid, u64 delay) { return ::audoutdRequestResumeForDebug(pid, delay); }

} // namespace audoutd

namespace audrec {

Result Initialize(void) { return ::audrecInitialize(); }
void Exit(void) { ::audrecExit(); }
Service* GetServiceSession(void) { return ::audrecGetServiceSession(); }
Result OpenFinalOutputRecorder(AudrecRecorder* recorder_out, FinalOutputRecorderParameter* param_in, u64 aruid, FinalOutputRecorderParameterInternal* param_out) { return ::audrecOpenFinalOutputRecorder(recorder_out, param_in, aruid, param_out); }
Result RecorderStart(AudrecRecorder* recorder) { return ::audrecRecorderStart(recorder); }
Result RecorderStop(AudrecRecorder* recorder) { return ::audrecRecorderStop(recorder); }
Result RecorderRegisterBufferEvent(AudrecRecorder* recorder, Event* out_event) { return ::audrecRecorderRegisterBufferEvent(recorder, out_event); }
Result RecorderAppendFinalOutputRecorderBuffer(AudrecRecorder* recorder, u64 buffer_client_ptr, FinalOutputRecorderBuffer* param) { return ::audrecRecorderAppendFinalOutputRecorderBuffer(recorder, buffer_client_ptr, param); }
Result RecorderGetReleasedFinalOutputRecorderBuffers(AudrecRecorder* recorder, u64* out_buffers, u64* inout_count, u64* out_released) { return ::audrecRecorderGetReleasedFinalOutputRecorderBuffers(recorder, out_buffers, inout_count, out_released); }
void RecorderClose(AudrecRecorder* recorder) { ::audrecRecorderClose(recorder); }

} // namespace audrec

namespace audren {

u32 GetRevision(void) { return ::audrenGetRevision(); }
int GetMemPoolCount(const AudioRendererConfig* config) { return ::audrenGetMemPoolCount(config); }
size_t GetInputParamSize(const AudioRendererConfig* config) { return ::audrenGetInputParamSize(config); }
size_t GetOutputParamSize(const AudioRendererConfig* config) { return ::audrenGetOutputParamSize(config); }
Result Initialize(const AudioRendererConfig* config) { return ::audrenInitialize(config); }
void Exit(void) { ::audrenExit(); }
Service* GetServiceSession_AudioRenderer(void) { return ::audrenGetServiceSession_AudioRenderer(); }
Event* GetFrameEvent(void) { return ::audrenGetFrameEvent(); }
void WaitFrame(void) { ::audrenWaitFrame(); }
Result GetState(u32* out_state) { return ::audrenGetState(out_state); }
Result RequestUpdateAudioRenderer(const void* in_param_buf, size_t in_param_buf_size, void* out_param_buf, size_t out_param_buf_size, void* perf_buf, size_t perf_buf_size) { return ::audrenRequestUpdateAudioRenderer(in_param_buf, in_param_buf_size, out_param_buf, out_param_buf_size, perf_buf, perf_buf_size); }
Result StartAudioRenderer(void) { return ::audrenStartAudioRenderer(); }
Result StopAudioRenderer(void) { return ::audrenStopAudioRenderer(); }
Result SetAudioRendererRenderingTimeLimit(int percent) { return ::audrenSetAudioRendererRenderingTimeLimit(percent); }

} // namespace audren

namespace audrv {

Result Create(AudioDriver* d, const AudioRendererConfig* config, int num_final_mix_channels) { return ::audrvCreate(d, config, num_final_mix_channels); }
Result Update(AudioDriver* d) { return ::audrvUpdate(d); }
void Close(AudioDriver* d) { ::audrvClose(d); }
int MemPoolAdd(AudioDriver* d, void* buffer, size_t size) { return ::audrvMemPoolAdd(d, buffer, size); }
bool MemPoolRemove(AudioDriver* d, int id) { return ::audrvMemPoolRemove(d, id); }
bool MemPoolAttach(AudioDriver* d, int id) { return ::audrvMemPoolAttach(d, id); }
bool MemPoolDetach(AudioDriver* d, int id) { return ::audrvMemPoolDetach(d, id); }
bool VoiceInit(AudioDriver* d, int id, int num_channels, PcmFormat format, int sample_rate) { return ::audrvVoiceInit(d, id, num_channels, format, sample_rate); }
void VoiceDrop(AudioDriver* d, int id) { ::audrvVoiceDrop(d, id); }
void VoiceStop(AudioDriver* d, int id) { ::audrvVoiceStop(d, id); }
bool VoiceIsPaused(AudioDriver* d, int id) { return ::audrvVoiceIsPaused(d, id); }
bool VoiceIsPlaying(AudioDriver* d, int id) { return ::audrvVoiceIsPlaying(d, id); }
bool VoiceAddWaveBuf(AudioDriver* d, int id, AudioDriverWaveBuf* wavebuf) { return ::audrvVoiceAddWaveBuf(d, id, wavebuf); }
u32 VoiceGetWaveBufSeq(AudioDriver* d, int id) { return ::audrvVoiceGetWaveBufSeq(d, id); }
u32 VoiceGetPlayedSampleCount(AudioDriver* d, int id) { return ::audrvVoiceGetPlayedSampleCount(d, id); }
u32 VoiceGetVoiceDropsCount(AudioDriver* d, int id) { return ::audrvVoiceGetVoiceDropsCount(d, id); }
void VoiceSetBiquadFilter(AudioDriver* d, int id, int biquad_id, float a0, float a1, float a2, float b0, float b1, float b2) { ::audrvVoiceSetBiquadFilter(d, id, biquad_id, a0, a1, a2, b0, b1, b2); }
void VoiceSetExtraParams(AudioDriver* d, int id, const void* params, size_t params_size) { ::audrvVoiceSetExtraParams(d, id, params, params_size); }
void VoiceSetDestinationMix(AudioDriver* d, int id, int mix_id) { ::audrvVoiceSetDestinationMix(d, id, mix_id); }
void VoiceSetMixFactor(AudioDriver* d, int id, float factor, int src_channel_id, int dest_channel_id) { ::audrvVoiceSetMixFactor(d, id, factor, src_channel_id, dest_channel_id); }
void VoiceSetVolume(AudioDriver* d, int id, float volume) { ::audrvVoiceSetVolume(d, id, volume); }
void VoiceSetPitch(AudioDriver* d, int id, float pitch) { ::audrvVoiceSetPitch(d, id, pitch); }
void VoiceSetPriority(AudioDriver* d, int id, int priority) { ::audrvVoiceSetPriority(d, id, priority); }
void VoiceClearBiquadFilter(AudioDriver* d, int id, int biquad_id) { ::audrvVoiceClearBiquadFilter(d, id, biquad_id); }
void VoiceSetPaused(AudioDriver* d, int id, bool paused) { ::audrvVoiceSetPaused(d, id, paused); }
void VoiceStart(AudioDriver* d, int id) { ::audrvVoiceStart(d, id); }
int MixAdd(AudioDriver* d, int sample_rate, int num_channels) { return ::audrvMixAdd(d, sample_rate, num_channels); }
void MixRemove(AudioDriver* d, int id) { ::audrvMixRemove(d, id); }
void MixSetDestinationMix(AudioDriver* d, int id, int mix_id) { ::audrvMixSetDestinationMix(d, id, mix_id); }
void MixSetMixFactor(AudioDriver* d, int id, float factor, int src_channel_id, int dest_channel_id) { ::audrvMixSetMixFactor(d, id, factor, src_channel_id, dest_channel_id); }
void MixSetVolume(AudioDriver* d, int id, float volume) { ::audrvMixSetVolume(d, id, volume); }
int DeviceSinkAdd(AudioDriver* d, const char* device_name, int num_channels, const u8* channel_ids) { return ::audrvDeviceSinkAdd(d, device_name, num_channels, channel_ids); }
void SinkRemove(AudioDriver* d, int id) { ::audrvSinkRemove(d, id); }

} // namespace audrv

namespace avm {

Result Initialize(void) { return ::avmInitialize(); }
void Exit(void) { ::avmExit(); }
Service * GetServiceSession(void) { return ::avmGetServiceSession(); }
Result GetHighestAvailableVersion(u64 id_1, u64 id_2, u32 *version) { return ::avmGetHighestAvailableVersion(id_1, id_2, version); }
Result GetHighestRequiredVersion(u64 id_1, u64 id_2, u32 *version) { return ::avmGetHighestRequiredVersion(id_1, id_2, version); }
Result GetVersionListEntry(u64 application_id, AvmVersionListEntry *entry) { return ::avmGetVersionListEntry(application_id, entry); }
Result GetVersionListImporter(AvmVersionListImporter *out) { return ::avmGetVersionListImporter(out); }
Result GetLaunchRequiredVersion(u64 application_id, u32 *version) { return ::avmGetLaunchRequiredVersion(application_id, version); }
Result UpgradeLaunchRequiredVersion(u64 application_id, u32 version) { return ::avmUpgradeLaunchRequiredVersion(application_id, version); }
Result PushLaunchVersion(u64 application_id, u32 version) { return ::avmPushLaunchVersion(application_id, version); }
Result ListVersionList(AvmVersionListEntry *buffer, size_t count, u32 *out) { return ::avmListVersionList(buffer, count, out); }
Result ListRequiredVersion(AvmRequiredVersionEntry *buffer, size_t count, u32 *out) { return ::avmListRequiredVersion(buffer, count, out); }
void VersionListImporterClose(AvmVersionListImporter *srv) { ::avmVersionListImporterClose(srv); }
Result VersionListImporterSetTimestamp(AvmVersionListImporter *srv, u64 timestamp) { return ::avmVersionListImporterSetTimestamp(srv, timestamp); }
Result VersionListImporterSetData(AvmVersionListImporter *srv, const AvmVersionListEntry *entries, u32 count) { return ::avmVersionListImporterSetData(srv, entries, count); }
Result VersionListImporterFlush(AvmVersionListImporter *srv) { return ::avmVersionListImporterFlush(srv); }

} // namespace avm

namespace barrier {

void Init(Barrier *b, u64 thread_count) { ::barrierInit(b, thread_count); }
void Wait(Barrier *b) { ::barrierWait(b); }

} // namespace barrier

namespace binder {

void Create(Binder* b, s32 id) { ::binderCreate(b, id); }
void Close(Binder* b) { ::binderClose(b); }
Result InitSession(Binder* b, Service* relay) { return ::binderInitSession(b, relay); }
Result TransactParcel(Binder* b, u32 code, void* parcel_data, size_t parcel_data_size, void* parcel_reply, size_t parcel_reply_size, u32 flags) { return ::binderTransactParcel(b, code, parcel_data, parcel_data_size, parcel_reply, parcel_reply_size, flags); }
Result ConvertErrorCode(s32 code) { return ::binderConvertErrorCode(code); }
Result AdjustRefcount(Binder* b, s32 addval, s32 type) { return ::binderAdjustRefcount(b, addval, type); }
Result GetNativeHandle(Binder* b, u32 unk0, Event *event_out) { return ::binderGetNativeHandle(b, unk0, event_out); }
Result IncreaseWeakRef(Binder* b) { return ::binderIncreaseWeakRef(b); }
Result DecreaseWeakRef(Binder* b) { return ::binderDecreaseWeakRef(b); }
Result IncreaseStrongRef(Binder* b) { return ::binderIncreaseStrongRef(b); }
Result DecreaseStrongRef(Binder* b) { return ::binderDecreaseStrongRef(b); }

} // namespace binder

namespace bpc {

Result Initialize(void) { return ::bpcInitialize(); }
void Exit(void) { ::bpcExit(); }
Service* GetServiceSession(void) { return ::bpcGetServiceSession(); }
Result ShutdownSystem(void) { return ::bpcShutdownSystem(); }
Result RebootSystem(void) { return ::bpcRebootSystem(); }
Result GetSleepButtonState(BpcSleepButtonState *out) { return ::bpcGetSleepButtonState(out); }
Result GetPowerButton(bool* out_is_pushed) { return ::bpcGetPowerButton(out_is_pushed); }

} // namespace bpc

namespace bq {

Result RequestBuffer(Binder *b, s32 bufferIdx, BqGraphicBuffer *buf) { return ::bqRequestBuffer(b, bufferIdx, buf); }
Result DequeueBuffer(Binder *b, bool async, u32 width, u32 height, s32 format, u32 usage, s32 *buf, NvMultiFence *fence) { return ::bqDequeueBuffer(b, async, width, height, format, usage, buf, fence); }
Result DetachBuffer(Binder *b, s32 slot) { return ::bqDetachBuffer(b, slot); }
Result QueueBuffer(Binder *b, s32 buf, const BqBufferInput *input, BqBufferOutput *output) { return ::bqQueueBuffer(b, buf, input, output); }
Result CancelBuffer(Binder *b, s32 buf, const NvMultiFence *fence) { return ::bqCancelBuffer(b, buf, fence); }
Result Query(Binder *b, s32 what, s32* value) { return ::bqQuery(b, what, value); }
Result Connect(Binder *b, s32 api, bool producerControlledByApp, BqBufferOutput *output) { return ::bqConnect(b, api, producerControlledByApp, output); }
Result Disconnect(Binder *b, s32 api) { return ::bqDisconnect(b, api); }
Result SetPreallocatedBuffer(Binder *b, s32 buf, const BqGraphicBuffer *input) { return ::bqSetPreallocatedBuffer(b, buf, input); }

} // namespace bq

namespace bt {

Result Initialize(void) { return ::btInitialize(); }
void Exit(void) { ::btExit(); }
Service* GetServiceSession(void) { return ::btGetServiceSession(); }
Result LeClientReadCharacteristic(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, u8 auth_req) { return ::btLeClientReadCharacteristic(connection_handle, is_primary, serv_id, char_id, auth_req); }
Result LeClientReadDescriptor(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const BtdrvGattId *desc_id, u8 auth_req) { return ::btLeClientReadDescriptor(connection_handle, is_primary, serv_id, char_id, desc_id, auth_req); }
Result LeClientWriteCharacteristic(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const void* buffer, size_t size, u8 auth_req, bool with_response) { return ::btLeClientWriteCharacteristic(connection_handle, is_primary, serv_id, char_id, buffer, size, auth_req, with_response); }
Result LeClientWriteDescriptor(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const BtdrvGattId *desc_id, const void* buffer, size_t size, u8 auth_req) { return ::btLeClientWriteDescriptor(connection_handle, is_primary, serv_id, char_id, desc_id, buffer, size, auth_req); }
Result LeClientRegisterNotification(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id) { return ::btLeClientRegisterNotification(connection_handle, is_primary, serv_id, char_id); }
Result LeClientDeregisterNotification(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id) { return ::btLeClientDeregisterNotification(connection_handle, is_primary, serv_id, char_id); }
Result SetLeResponse(u8 server_if, const BtdrvGattAttributeUuid *serv_uuid, const BtdrvGattAttributeUuid *char_uuid, const void* buffer, size_t size) { return ::btSetLeResponse(server_if, serv_uuid, char_uuid, buffer, size); }
Result LeSendIndication(u8 server_if, const BtdrvGattAttributeUuid *serv_uuid, const BtdrvGattAttributeUuid *char_uuid, const void* buffer, size_t size, bool noconfirm) { return ::btLeSendIndication(server_if, serv_uuid, char_uuid, buffer, size, noconfirm); }
Result GetLeEventInfo(void* buffer, size_t size, BtdrvBleEventType *type) { return ::btGetLeEventInfo(buffer, size, type); }
Result RegisterBleEvent(Event* out_event) { return ::btRegisterBleEvent(out_event); }

} // namespace bt

namespace btdev {

Result Initialize(void) { return ::btdevInitialize(); }
void Exit(void) { ::btdevExit(); }
bool GattAttributeUuidIsSame(const BtdrvGattAttributeUuid *a, const BtdrvGattAttributeUuid *b) { return ::btdevGattAttributeUuidIsSame(a, b); }
Result AcquireBleScanEvent(Event* out_event) { return ::btdevAcquireBleScanEvent(out_event); }
Result GetBleScanParameter(u16 parameter_id, BtdrvBleAdvertisePacketParameter *out) { return ::btdevGetBleScanParameter(parameter_id, out); }
Result GetBleScanParameter2(u16 parameter_id, BtdrvGattAttributeUuid *out) { return ::btdevGetBleScanParameter2(parameter_id, out); }
Result StartBleScanGeneral(BtdrvBleAdvertisePacketParameter param) { return ::btdevStartBleScanGeneral(param); }
Result StopBleScanGeneral(void) { return ::btdevStopBleScanGeneral(); }
Result GetBleScanResult(BtdrvBleScanResult *results, u8 count, u8 *total_out) { return ::btdevGetBleScanResult(results, count, total_out); }
Result EnableBleAutoConnection(BtdrvBleAdvertisePacketParameter param) { return ::btdevEnableBleAutoConnection(param); }
Result DisableBleAutoConnection(void) { return ::btdevDisableBleAutoConnection(); }
Result StartBleScanSmartDevice(const BtdrvGattAttributeUuid *uuid) { return ::btdevStartBleScanSmartDevice(uuid); }
Result StopBleScanSmartDevice(void) { return ::btdevStopBleScanSmartDevice(); }
Result AcquireBleConnectionStateChangedEvent(Event* out_event) { return ::btdevAcquireBleConnectionStateChangedEvent(out_event); }
Result ConnectToGattServer(BtdrvAddress addr) { return ::btdevConnectToGattServer(addr); }
Result DisconnectFromGattServer(u32 connection_handle) { return ::btdevDisconnectFromGattServer(connection_handle); }
Result GetBleConnectionInfoList(BtdrvBleConnectionInfo *info, u8 count, u8 *total_out) { return ::btdevGetBleConnectionInfoList(info, count, total_out); }
Result AcquireBleServiceDiscoveryEvent(Event* out_event) { return ::btdevAcquireBleServiceDiscoveryEvent(out_event); }
Result GetGattServices(u32 connection_handle, BtdevGattService *services, u8 count, u8 *total_out) { return ::btdevGetGattServices(connection_handle, services, count, total_out); }
Result GetGattService(u32 connection_handle, const BtdrvGattAttributeUuid *uuid, BtdevGattService *service, bool *flag) { return ::btdevGetGattService(connection_handle, uuid, service, flag); }
Result AcquireBlePairingEvent(Event* out_event) { return ::btdevAcquireBlePairingEvent(out_event); }
Result PairGattServer(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btdevPairGattServer(connection_handle, param); }
Result UnpairGattServer(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btdevUnpairGattServer(connection_handle, param); }
Result UnpairGattServer2(BtdrvAddress addr, BtdrvBleAdvertisePacketParameter param) { return ::btdevUnpairGattServer2(addr, param); }
Result GetPairedGattServerAddress(BtdrvBleAdvertisePacketParameter param, BtdrvAddress *addrs, u8 count, u8 *total_out) { return ::btdevGetPairedGattServerAddress(param, addrs, count, total_out); }
Result AcquireBleMtuConfigEvent(Event* out_event) { return ::btdevAcquireBleMtuConfigEvent(out_event); }
Result ConfigureBleMtu(u32 connection_handle, u16 mtu) { return ::btdevConfigureBleMtu(connection_handle, mtu); }
Result GetBleMtu(u32 connection_handle, u16 *out) { return ::btdevGetBleMtu(connection_handle, out); }
Result AcquireBleGattOperationEvent(Event* out_event) { return ::btdevAcquireBleGattOperationEvent(out_event); }
Result RegisterGattOperationNotification(const BtdrvGattAttributeUuid *uuid) { return ::btdevRegisterGattOperationNotification(uuid); }
Result UnregisterGattOperationNotification(const BtdrvGattAttributeUuid *uuid) { return ::btdevUnregisterGattOperationNotification(uuid); }
Result GetGattOperationResult(BtdrvBleClientGattOperationInfo *out) { return ::btdevGetGattOperationResult(out); }
Result ReadGattCharacteristic(BtdevGattCharacteristic *c) { return ::btdevReadGattCharacteristic(c); }
Result WriteGattCharacteristic(BtdevGattCharacteristic *c) { return ::btdevWriteGattCharacteristic(c); }
Result EnableGattCharacteristicNotification(BtdevGattCharacteristic *c, bool flag) { return ::btdevEnableGattCharacteristicNotification(c, flag); }
Result ReadGattDescriptor(BtdevGattDescriptor *d) { return ::btdevReadGattDescriptor(d); }
Result WriteGattDescriptor(BtdevGattDescriptor *d) { return ::btdevWriteGattDescriptor(d); }
void GattAttributeCreate(BtdevGattAttribute *a, const BtdrvGattAttributeUuid *uuid, u16 handle, u32 connection_handle) { ::btdevGattAttributeCreate(a, uuid, handle, connection_handle); }
u8 GattAttributeGetType(BtdevGattAttribute *a) { return ::btdevGattAttributeGetType(a); }
void GattAttributeGetUuid(BtdevGattAttribute *a, BtdrvGattAttributeUuid *out) { ::btdevGattAttributeGetUuid(a, out); }
u16 GattAttributeGetHandle(BtdevGattAttribute *a) { return ::btdevGattAttributeGetHandle(a); }
u32 GattAttributeGetConnectionHandle(BtdevGattAttribute *a) { return ::btdevGattAttributeGetConnectionHandle(a); }
void GattServiceCreate(BtdevGattService *s, const BtdrvGattAttributeUuid *uuid, u16 handle, u32 connection_handle, u16 instance_id, u16 end_group_handle, bool primary_service) { ::btdevGattServiceCreate(s, uuid, handle, connection_handle, instance_id, end_group_handle, primary_service); }
u16 GattServiceGetInstanceId(BtdevGattService *s) { return ::btdevGattServiceGetInstanceId(s); }
u16 GattServiceGetEndGroupHandle(BtdevGattService *s) { return ::btdevGattServiceGetEndGroupHandle(s); }
u16 GattServiceIsPrimaryService(BtdevGattService *s) { return ::btdevGattServiceIsPrimaryService(s); }
Result GattServiceGetIncludedServices(BtdevGattService *s, BtdevGattService *services, u8 count, u8 *total_out) { return ::btdevGattServiceGetIncludedServices(s, services, count, total_out); }
Result GattServiceGetCharacteristics(BtdevGattService *s, BtdevGattCharacteristic *characteristics, u8 count, u8 *total_out) { return ::btdevGattServiceGetCharacteristics(s, characteristics, count, total_out); }
Result GattServiceGetCharacteristic(BtdevGattService *s, const BtdrvGattAttributeUuid *uuid, BtdevGattCharacteristic *characteristic, bool *flag) { return ::btdevGattServiceGetCharacteristic(s, uuid, characteristic, flag); }
void GattCharacteristicCreate(BtdevGattCharacteristic *c, const BtdrvGattAttributeUuid *uuid, u16 handle, u32 connection_handle, u16 instance_id, u8 properties) { ::btdevGattCharacteristicCreate(c, uuid, handle, connection_handle, instance_id, properties); }
u16 GattCharacteristicGetInstanceId(BtdevGattCharacteristic *c) { return ::btdevGattCharacteristicGetInstanceId(c); }
u8 GattCharacteristicGetProperties(BtdevGattCharacteristic *c) { return ::btdevGattCharacteristicGetProperties(c); }
Result GattCharacteristicGetService(BtdevGattCharacteristic *c, BtdevGattService *service) { return ::btdevGattCharacteristicGetService(c, service); }
Result GattCharacteristicGetDescriptors(BtdevGattCharacteristic *c, BtdevGattDescriptor *descriptors, u8 count, u8 *total_out) { return ::btdevGattCharacteristicGetDescriptors(c, descriptors, count, total_out); }
Result GattCharacteristicGetDescriptor(BtdevGattCharacteristic *c, const BtdrvGattAttributeUuid *uuid, BtdevGattDescriptor *descriptor, bool *flag) { return ::btdevGattCharacteristicGetDescriptor(c, uuid, descriptor, flag); }
void GattCharacteristicSetValue(BtdevGattCharacteristic *c, const void* buffer, size_t size) { ::btdevGattCharacteristicSetValue(c, buffer, size); }
u64 GattCharacteristicGetValue(BtdevGattCharacteristic *c, void* buffer, size_t size) { return ::btdevGattCharacteristicGetValue(c, buffer, size); }
void GattDescriptorCreate(BtdevGattDescriptor *d, const BtdrvGattAttributeUuid *uuid, u16 handle, u32 connection_handle) { ::btdevGattDescriptorCreate(d, uuid, handle, connection_handle); }
Result GattDescriptorGetService(BtdevGattDescriptor *d, BtdevGattService *service) { return ::btdevGattDescriptorGetService(d, service); }
Result GattDescriptorGetCharacteristic(BtdevGattDescriptor *d, BtdevGattCharacteristic *characteristic) { return ::btdevGattDescriptorGetCharacteristic(d, characteristic); }
void GattDescriptorSetValue(BtdevGattDescriptor *d, const void* buffer, size_t size) { ::btdevGattDescriptorSetValue(d, buffer, size); }
u64 GattDescriptorGetValue(BtdevGattDescriptor *d, void* buffer, size_t size) { return ::btdevGattDescriptorGetValue(d, buffer, size); }

} // namespace btdev

namespace btdrv {

Result Initialize(void) { return ::btdrvInitialize(); }
void Exit(void) { ::btdrvExit(); }
Service* GetServiceSession(void) { return ::btdrvGetServiceSession(); }
Result InitializeBluetooth(Event* out_event) { return ::btdrvInitializeBluetooth(out_event); }
Result EnableBluetooth(void) { return ::btdrvEnableBluetooth(); }
Result DisableBluetooth(void) { return ::btdrvDisableBluetooth(); }
Result FinalizeBluetooth(void) { return ::btdrvFinalizeBluetooth(); }
Result LegacyGetAdapterProperties(BtdrvAdapterPropertyOld *properties) { return ::btdrvLegacyGetAdapterProperties(properties); }
Result GetAdapterProperties(BtdrvAdapterPropertySet *properties) { return ::btdrvGetAdapterProperties(properties); }
Result LegacyGetAdapterProperty(BtdrvBluetoothPropertyType type, void* buffer, size_t size) { return ::btdrvLegacyGetAdapterProperty(type, buffer, size); }
Result GetAdapterProperty(BtdrvAdapterPropertyType type, BtdrvAdapterProperty *property) { return ::btdrvGetAdapterProperty(type, property); }
Result LegacySetAdapterProperty(BtdrvBluetoothPropertyType type, const void* buffer, size_t size) { return ::btdrvLegacySetAdapterProperty(type, buffer, size); }
Result SetAdapterProperty(BtdrvAdapterPropertyType type, const BtdrvAdapterProperty *property) { return ::btdrvSetAdapterProperty(type, property); }
Result LegacyStartInquiry(void) { return ::btdrvLegacyStartInquiry(); }
Result StartInquiry(u32 services, s64 duration) { return ::btdrvStartInquiry(services, duration); }
Result StopInquiry(void) { return ::btdrvStopInquiry(); }
Result CreateBond(BtdrvAddress addr, u32 type) { return ::btdrvCreateBond(addr, type); }
Result RemoveBond(BtdrvAddress addr) { return ::btdrvRemoveBond(addr); }
Result CancelBond(BtdrvAddress addr) { return ::btdrvCancelBond(addr); }
Result LegacyRespondToPinRequest(BtdrvAddress addr, bool flag, const BtdrvBluetoothPinCode *pin_code, u8 length) { return ::btdrvLegacyRespondToPinRequest(addr, flag, pin_code, length); }
Result RespondToPinRequest(BtdrvAddress addr, const BtdrvPinCode *pin_code) { return ::btdrvRespondToPinRequest(addr, pin_code); }
Result RespondToSspRequest(BtdrvAddress addr, u32 variant, bool accept, u32 passkey) { return ::btdrvRespondToSspRequest(addr, variant, accept, passkey); }
Result GetEventInfo(void* buffer, size_t size, BtdrvEventType *type) { return ::btdrvGetEventInfo(buffer, size, type); }
Result InitializeHid(Event* out_event) { return ::btdrvInitializeHid(out_event); }
Result OpenHidConnection(BtdrvAddress addr) { return ::btdrvOpenHidConnection(addr); }
Result CloseHidConnection(BtdrvAddress addr) { return ::btdrvCloseHidConnection(addr); }
Result WriteHidData(BtdrvAddress addr, const BtdrvHidReport *buffer) { return ::btdrvWriteHidData(addr, buffer); }
Result WriteHidData2(BtdrvAddress addr, const void* buffer, size_t size) { return ::btdrvWriteHidData2(addr, buffer, size); }
Result SetHidReport(BtdrvAddress addr, BtdrvBluetoothHhReportType type, const BtdrvHidReport *buffer) { return ::btdrvSetHidReport(addr, type, buffer); }
Result GetHidReport(BtdrvAddress addr, u8 report_id, BtdrvBluetoothHhReportType type) { return ::btdrvGetHidReport(addr, report_id, type); }
Result TriggerConnection(BtdrvAddress addr, u16 timeout) { return ::btdrvTriggerConnection(addr, timeout); }
Result AddPairedDeviceInfo(const SetSysBluetoothDevicesSettings *settings) { return ::btdrvAddPairedDeviceInfo(settings); }
Result GetPairedDeviceInfo(BtdrvAddress addr, SetSysBluetoothDevicesSettings *settings) { return ::btdrvGetPairedDeviceInfo(addr, settings); }
Result FinalizeHid(void) { return ::btdrvFinalizeHid(); }
Result GetHidEventInfo(void* buffer, size_t size, BtdrvHidEventType *type) { return ::btdrvGetHidEventInfo(buffer, size, type); }
Result SetTsi(BtdrvAddress addr, u8 tsi) { return ::btdrvSetTsi(addr, tsi); }
Result EnableBurstMode(BtdrvAddress addr, bool flag) { return ::btdrvEnableBurstMode(addr, flag); }
Result SetZeroRetransmission(BtdrvAddress addr, u8 *report_ids, u8 count) { return ::btdrvSetZeroRetransmission(addr, report_ids, count); }
Result EnableMcMode(bool flag) { return ::btdrvEnableMcMode(flag); }
Result EnableLlrScan(void) { return ::btdrvEnableLlrScan(); }
Result DisableLlrScan(void) { return ::btdrvDisableLlrScan(); }
Result EnableRadio(bool flag) { return ::btdrvEnableRadio(flag); }
Result SetVisibility(bool inquiry_scan, bool page_scan) { return ::btdrvSetVisibility(inquiry_scan, page_scan); }
Result EnableTbfcScan(bool flag) { return ::btdrvEnableTbfcScan(flag); }
Result RegisterHidReportEvent(Event* out_event) { return ::btdrvRegisterHidReportEvent(out_event); }
Result GetHidReportEventInfo(void* buffer, size_t size, BtdrvHidEventType *type) { return ::btdrvGetHidReportEventInfo(buffer, size, type); }
void* GetHidReportEventInfoSharedmemAddr(void) { return ::btdrvGetHidReportEventInfoSharedmemAddr(); }
Result GetLatestPlr(BtdrvPlrList *out) { return ::btdrvGetLatestPlr(out); }
Result GetPendingConnections(void) { return ::btdrvGetPendingConnections(); }
Result GetChannelMap(BtdrvChannelMapList *out) { return ::btdrvGetChannelMap(out); }
Result EnableTxPowerBoostSetting(bool flag) { return ::btdrvEnableTxPowerBoostSetting(flag); }
Result IsTxPowerBoostSettingEnabled(bool *out) { return ::btdrvIsTxPowerBoostSettingEnabled(out); }
Result EnableAfhSetting(bool flag) { return ::btdrvEnableAfhSetting(flag); }
Result IsAfhSettingEnabled(bool *out) { return ::btdrvIsAfhSettingEnabled(out); }
Result InitializeBle(Event* out_event) { return ::btdrvInitializeBle(out_event); }
Result EnableBle(void) { return ::btdrvEnableBle(); }
Result DisableBle(void) { return ::btdrvDisableBle(); }
Result FinalizeBle(void) { return ::btdrvFinalizeBle(); }
Result SetBleVisibility(bool discoverable, bool connectable) { return ::btdrvSetBleVisibility(discoverable, connectable); }
Result SetLeConnectionParameter(const BtdrvLeConnectionParams *param) { return ::btdrvSetLeConnectionParameter(param); }
Result SetBleConnectionParameter(BtdrvAddress addr, const BtdrvBleConnectionParameter *param, bool preference) { return ::btdrvSetBleConnectionParameter(addr, param, preference); }
Result SetLeDefaultConnectionParameter(const BtdrvLeConnectionParams *param) { return ::btdrvSetLeDefaultConnectionParameter(param); }
Result SetBleDefaultConnectionParameter(const BtdrvBleConnectionParameter *param) { return ::btdrvSetBleDefaultConnectionParameter(param); }
Result SetBleAdvertiseData(const BtdrvBleAdvertisePacketData *data) { return ::btdrvSetBleAdvertiseData(data); }
Result SetBleAdvertiseParameter(BtdrvAddress addr, u16 min_interval, u16 max_interval) { return ::btdrvSetBleAdvertiseParameter(addr, min_interval, max_interval); }
Result StartBleScan(void) { return ::btdrvStartBleScan(); }
Result StopBleScan(void) { return ::btdrvStopBleScan(); }
Result AddBleScanFilterCondition(const BtdrvBleAdvertiseFilter *filter) { return ::btdrvAddBleScanFilterCondition(filter); }
Result DeleteBleScanFilterCondition(const BtdrvBleAdvertiseFilter *filter) { return ::btdrvDeleteBleScanFilterCondition(filter); }
Result DeleteBleScanFilter(u8 index) { return ::btdrvDeleteBleScanFilter(index); }
Result ClearBleScanFilters(void) { return ::btdrvClearBleScanFilters(); }
Result EnableBleScanFilter(bool flag) { return ::btdrvEnableBleScanFilter(flag); }
Result RegisterGattClient(const BtdrvGattAttributeUuid *uuid) { return ::btdrvRegisterGattClient(uuid); }
Result UnregisterGattClient(u8 client_if) { return ::btdrvUnregisterGattClient(client_if); }
Result UnregisterAllGattClients(void) { return ::btdrvUnregisterAllGattClients(); }
Result ConnectGattServer(u8 client_if, BtdrvAddress addr, bool is_direct, u64 AppletResourceUserId) { return ::btdrvConnectGattServer(client_if, addr, is_direct, AppletResourceUserId); }
Result CancelConnectGattServer(u8 client_if, BtdrvAddress addr, bool is_direct) { return ::btdrvCancelConnectGattServer(client_if, addr, is_direct); }
Result DisconnectGattServer(u32 conn_id) { return ::btdrvDisconnectGattServer(conn_id); }
Result GetGattAttribute(BtdrvAddress addr, u32 conn_id) { return ::btdrvGetGattAttribute(addr, conn_id); }
Result GetGattService(u32 conn_id, const BtdrvGattAttributeUuid *uuid) { return ::btdrvGetGattService(conn_id, uuid); }
Result ConfigureAttMtu(u32 conn_id, u16 mtu) { return ::btdrvConfigureAttMtu(conn_id, mtu); }
Result RegisterGattServer(const BtdrvGattAttributeUuid *uuid) { return ::btdrvRegisterGattServer(uuid); }
Result UnregisterGattServer(u8 server_if) { return ::btdrvUnregisterGattServer(server_if); }
Result ConnectGattClient(u8 server_if, BtdrvAddress addr, bool is_direct) { return ::btdrvConnectGattClient(server_if, addr, is_direct); }
Result DisconnectGattClient(u8 conn_id, BtdrvAddress addr) { return ::btdrvDisconnectGattClient(conn_id, addr); }
Result AddGattService(u8 server_if, const BtdrvGattAttributeUuid *uuid, u8 num_handle, bool is_primary) { return ::btdrvAddGattService(server_if, uuid, num_handle, is_primary); }
Result EnableGattService(u8 server_if, const BtdrvGattAttributeUuid *uuid) { return ::btdrvEnableGattService(server_if, uuid); }
Result AddGattCharacteristic(u8 server_if, const BtdrvGattAttributeUuid *serv_uuid, const BtdrvGattAttributeUuid *char_uuid, u16 permissions, u8 property) { return ::btdrvAddGattCharacteristic(server_if, serv_uuid, char_uuid, permissions, property); }
Result AddGattDescriptor(u8 server_if, const BtdrvGattAttributeUuid *serv_uuid, const BtdrvGattAttributeUuid *desc_uuid, u16 permissions) { return ::btdrvAddGattDescriptor(server_if, serv_uuid, desc_uuid, permissions); }
Result GetBleManagedEventInfo(void* buffer, size_t size, BtdrvBleEventType *type) { return ::btdrvGetBleManagedEventInfo(buffer, size, type); }
Result GetGattFirstCharacteristic(u32 conn_id, const BtdrvGattId *serv_id, bool is_primary, const BtdrvGattAttributeUuid *filter_uuid, u8 *out_property, BtdrvGattId *out_char_id) { return ::btdrvGetGattFirstCharacteristic(conn_id, serv_id, is_primary, filter_uuid, out_property, out_char_id); }
Result GetGattNextCharacteristic(u32 conn_id, const BtdrvGattId *serv_id, bool is_primary, const BtdrvGattId *char_id, const BtdrvGattAttributeUuid *filter_uuid, u8 *out_property, BtdrvGattId *out_char_id) { return ::btdrvGetGattNextCharacteristic(conn_id, serv_id, is_primary, char_id, filter_uuid, out_property, out_char_id); }
Result GetGattFirstDescriptor(u32 conn_id, const BtdrvGattId *serv_id, bool is_primary, const BtdrvGattId *char_id, const BtdrvGattAttributeUuid *filter_uuid, BtdrvGattId *out_desc_id) { return ::btdrvGetGattFirstDescriptor(conn_id, serv_id, is_primary, char_id, filter_uuid, out_desc_id); }
Result GetGattNextDescriptor(u32 conn_id, const BtdrvGattId *serv_id, bool is_primary, const BtdrvGattId *char_id, const BtdrvGattId *desc_id, const BtdrvGattAttributeUuid *filter_uuid, BtdrvGattId *out_desc_id) { return ::btdrvGetGattNextDescriptor(conn_id, serv_id, is_primary, char_id, desc_id, filter_uuid, out_desc_id); }
Result RegisterGattManagedDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvRegisterGattManagedDataPath(uuid); }
Result UnregisterGattManagedDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvUnregisterGattManagedDataPath(uuid); }
Result RegisterGattHidDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvRegisterGattHidDataPath(uuid); }
Result UnregisterGattHidDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvUnregisterGattHidDataPath(uuid); }
Result RegisterGattDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvRegisterGattDataPath(uuid); }
Result UnregisterGattDataPath(const BtdrvGattAttributeUuid *uuid) { return ::btdrvUnregisterGattDataPath(uuid); }
Result ReadGattCharacteristic(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, u8 auth_req) { return ::btdrvReadGattCharacteristic(connection_handle, is_primary, serv_id, char_id, auth_req); }
Result ReadGattDescriptor(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const BtdrvGattId *desc_id, u8 auth_req) { return ::btdrvReadGattDescriptor(connection_handle, is_primary, serv_id, char_id, desc_id, auth_req); }
Result WriteGattCharacteristic(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const void* buffer, size_t size, u8 auth_req, bool with_response) { return ::btdrvWriteGattCharacteristic(connection_handle, is_primary, serv_id, char_id, buffer, size, auth_req, with_response); }
Result WriteGattDescriptor(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id, const BtdrvGattId *desc_id, const void* buffer, size_t size, u8 auth_req) { return ::btdrvWriteGattDescriptor(connection_handle, is_primary, serv_id, char_id, desc_id, buffer, size, auth_req); }
Result RegisterGattNotification(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id) { return ::btdrvRegisterGattNotification(connection_handle, is_primary, serv_id, char_id); }
Result UnregisterGattNotification(u32 connection_handle, bool is_primary, const BtdrvGattId *serv_id, const BtdrvGattId *char_id) { return ::btdrvUnregisterGattNotification(connection_handle, is_primary, serv_id, char_id); }
Result GetLeHidEventInfo(void* buffer, size_t size, BtdrvBleEventType *type) { return ::btdrvGetLeHidEventInfo(buffer, size, type); }
Result RegisterBleHidEvent(Event* out_event) { return ::btdrvRegisterBleHidEvent(out_event); }
Result SetBleScanParameter(u16 scan_interval, u16 scan_window) { return ::btdrvSetBleScanParameter(scan_interval, scan_window); }
Result MoveToSecondaryPiconet(BtdrvAddress addr) { return ::btdrvMoveToSecondaryPiconet(addr); }
Result IsBluetoothEnabled(bool *out) { return ::btdrvIsBluetoothEnabled(out); }
Result AcquireAudioEvent(Event* out_event, bool autoclear) { return ::btdrvAcquireAudioEvent(out_event, autoclear); }
Result GetAudioEventInfo(void* buffer, size_t size, BtdrvAudioEventType *type) { return ::btdrvGetAudioEventInfo(buffer, size, type); }
Result OpenAudioConnection(BtdrvAddress addr) { return ::btdrvOpenAudioConnection(addr); }
Result CloseAudioConnection(BtdrvAddress addr) { return ::btdrvCloseAudioConnection(addr); }
Result OpenAudioOut(BtdrvAddress addr, u32 *audio_handle) { return ::btdrvOpenAudioOut(addr, audio_handle); }
Result CloseAudioOut(u32 audio_handle) { return ::btdrvCloseAudioOut(audio_handle); }
Result StartAudioOut(u32 audio_handle, const BtdrvPcmParameter *pcm_param, s64 in_latency, s64 *out_latency, u64 *out1) { return ::btdrvStartAudioOut(audio_handle, pcm_param, in_latency, out_latency, out1); }
Result StopAudioOut(u32 audio_handle) { return ::btdrvStopAudioOut(audio_handle); }
Result GetAudioOutState(u32 audio_handle, BtdrvAudioOutState *out) { return ::btdrvGetAudioOutState(audio_handle, out); }
Result GetAudioOutFeedingCodec(u32 audio_handle, BtdrvAudioCodec *out) { return ::btdrvGetAudioOutFeedingCodec(audio_handle, out); }
Result GetAudioOutFeedingParameter(u32 audio_handle, BtdrvPcmParameter *out) { return ::btdrvGetAudioOutFeedingParameter(audio_handle, out); }
Result AcquireAudioOutStateChangedEvent(u32 audio_handle, Event* out_event, bool autoclear) { return ::btdrvAcquireAudioOutStateChangedEvent(audio_handle, out_event, autoclear); }
Result AcquireAudioOutBufferAvailableEvent(u32 audio_handle, Event* out_event, bool autoclear) { return ::btdrvAcquireAudioOutBufferAvailableEvent(audio_handle, out_event, autoclear); }
Result SendAudioData(u32 audio_handle, const void* buffer, size_t size, u64 *transferred_size) { return ::btdrvSendAudioData(audio_handle, buffer, size, transferred_size); }
Result AcquireAudioControlInputStateChangedEvent(Event* out_event, bool autoclear) { return ::btdrvAcquireAudioControlInputStateChangedEvent(out_event, autoclear); }
Result GetAudioControlInputState(BtdrvAudioControlButtonState *states, s32 count, s32 *total_out) { return ::btdrvGetAudioControlInputState(states, count, total_out); }
Result AcquireAudioConnectionStateChangedEvent(Event* out_event, bool autoclear) { return ::btdrvAcquireAudioConnectionStateChangedEvent(out_event, autoclear); }
Result GetConnectedAudioDevice(BtdrvAddress *addrs, s32 count, s32 *total_out) { return ::btdrvGetConnectedAudioDevice(addrs, count, total_out); }
Result CloseAudioControlInput(BtdrvAddress addr) { return ::btdrvCloseAudioControlInput(addr); }
Result RegisterAudioControlNotification(BtdrvAddress addr, u32 event_type) { return ::btdrvRegisterAudioControlNotification(addr, event_type); }
Result SendAudioControlPassthroughCommand(BtdrvAddress addr, u32 op_id, u32 state_type) { return ::btdrvSendAudioControlPassthroughCommand(addr, op_id, state_type); }
Result SendAudioControlSetAbsoluteVolumeCommand(BtdrvAddress addr, s32 val) { return ::btdrvSendAudioControlSetAbsoluteVolumeCommand(addr, val); }
Result IsManufacturingMode(bool *out) { return ::btdrvIsManufacturingMode(out); }
Result EmulateBluetoothCrash(BtdrvFatalReason reason) { return ::btdrvEmulateBluetoothCrash(reason); }
Result GetBleChannelMap(BtdrvChannelMapList *out) { return ::btdrvGetBleChannelMap(out); }
void* CircularBufferRead(BtdrvCircularBuffer *c) { return ::btdrvCircularBufferRead(c); }
bool CircularBufferFree(BtdrvCircularBuffer *c) { return ::btdrvCircularBufferFree(c); }

} // namespace btdrv

namespace btm {

Result Initialize(void) { return ::btmInitialize(); }
void Exit(void) { ::btmExit(); }
Service* GetServiceSession(void) { return ::btmGetServiceSession(); }
Result GetState(BtmState *out) { return ::btmGetState(out); }
Result GetHostDeviceProperty(BtmHostDeviceProperty *out) { return ::btmGetHostDeviceProperty(out); }
Result AcquireDeviceConditionEvent(Event* out_event) { return ::btmAcquireDeviceConditionEvent(out_event); }
Result LegacyGetDeviceCondition(BtmDeviceConditionList *out) { return ::btmLegacyGetDeviceCondition(out); }
Result GetDeviceCondition(BtmProfile profile, BtmDeviceCondition *out, size_t count, s32 *total_out) { return ::btmGetDeviceCondition(profile, out, count, total_out); }
Result SetBurstMode(BtdrvAddress addr, bool flag) { return ::btmSetBurstMode(addr, flag); }
Result SetSlotMode(const BtmDeviceSlotModeList *list) { return ::btmSetSlotMode(list); }
Result SetBluetoothMode(BtmBluetoothMode mode) { return ::btmSetBluetoothMode(mode); }
Result SetWlanMode(BtmWlanMode mode) { return ::btmSetWlanMode(mode); }
Result AcquireDeviceInfoEvent(Event* out_event) { return ::btmAcquireDeviceInfoEvent(out_event); }
Result LegacyGetDeviceInfo(BtmDeviceInfoList *out) { return ::btmLegacyGetDeviceInfo(out); }
Result GetDeviceInfo(BtmProfile profile, BtmDeviceInfo *out, size_t count, s32 *total_out) { return ::btmGetDeviceInfo(profile, out, count, total_out); }
Result LegacyAddDeviceInfo(const BtmDeviceInfoLegacy *info) { return ::btmLegacyAddDeviceInfo(info); }
Result AddDeviceInfo(const BtmDeviceInfo *info) { return ::btmAddDeviceInfo(info); }
Result RemoveDeviceInfo(BtdrvAddress addr) { return ::btmRemoveDeviceInfo(addr); }
Result IncreaseDeviceInfoOrder(BtdrvAddress addr) { return ::btmIncreaseDeviceInfoOrder(addr); }
Result LlrNotify(BtdrvAddress addr, s32 unk) { return ::btmLlrNotify(addr, unk); }
Result EnableRadio(void) { return ::btmEnableRadio(); }
Result DisableRadio(void) { return ::btmDisableRadio(); }
Result HidDisconnect(BtdrvAddress addr) { return ::btmHidDisconnect(addr); }
Result HidSetRetransmissionMode(BtdrvAddress addr, const BtmZeroRetransmissionList *list) { return ::btmHidSetRetransmissionMode(addr, list); }
Result AcquireAwakeReqEvent(Event* out_event) { return ::btmAcquireAwakeReqEvent(out_event); }
Result AcquireLlrStateEvent(Event* out_event) { return ::btmAcquireLlrStateEvent(out_event); }
Result IsLlrStarted(bool *out) { return ::btmIsLlrStarted(out); }
Result EnableSlotSaving(bool flag) { return ::btmEnableSlotSaving(flag); }
Result ProtectDeviceInfo(BtdrvAddress addr, bool flag) { return ::btmProtectDeviceInfo(addr, flag); }
Result AcquireBleScanEvent(Event* out_event) { return ::btmAcquireBleScanEvent(out_event); }
Result GetBleScanParameterGeneral(u16 parameter_id, BtdrvBleAdvertisePacketParameter *out) { return ::btmGetBleScanParameterGeneral(parameter_id, out); }
Result GetBleScanParameterSmartDevice(u16 parameter_id, BtdrvGattAttributeUuid *out) { return ::btmGetBleScanParameterSmartDevice(parameter_id, out); }
Result StartBleScanForGeneral(BtdrvBleAdvertisePacketParameter param) { return ::btmStartBleScanForGeneral(param); }
Result StopBleScanForGeneral(void) { return ::btmStopBleScanForGeneral(); }
Result GetBleScanResultsForGeneral(BtdrvBleScanResult *results, u8 count, u8 *total_out) { return ::btmGetBleScanResultsForGeneral(results, count, total_out); }
Result StartBleScanForPaired(BtdrvBleAdvertisePacketParameter param) { return ::btmStartBleScanForPaired(param); }
Result StopBleScanForPaired(void) { return ::btmStopBleScanForPaired(); }
Result StartBleScanForSmartDevice(const BtdrvGattAttributeUuid *uuid) { return ::btmStartBleScanForSmartDevice(uuid); }
Result StopBleScanForSmartDevice(void) { return ::btmStopBleScanForSmartDevice(); }
Result GetBleScanResultsForSmartDevice(BtdrvBleScanResult *results, u8 count, u8 *total_out) { return ::btmGetBleScanResultsForSmartDevice(results, count, total_out); }
Result AcquireBleConnectionEvent(Event* out_event) { return ::btmAcquireBleConnectionEvent(out_event); }
Result BleConnect(BtdrvAddress addr) { return ::btmBleConnect(addr); }
Result BleOverrideConnection(u32 id) { return ::btmBleOverrideConnection(id); }
Result BleDisconnect(u32 connection_handle) { return ::btmBleDisconnect(connection_handle); }
Result BleGetConnectionState(BtdrvBleConnectionInfo *info, u8 count, u8 *total_out) { return ::btmBleGetConnectionState(info, count, total_out); }
Result BleGetGattClientConditionList(BtmGattClientConditionList *list) { return ::btmBleGetGattClientConditionList(list); }
Result AcquireBlePairingEvent(Event* out_event) { return ::btmAcquireBlePairingEvent(out_event); }
Result BlePairDevice(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btmBlePairDevice(connection_handle, param); }
Result BleUnpairDeviceOnBoth(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btmBleUnpairDeviceOnBoth(connection_handle, param); }
Result BleUnPairDevice(BtdrvAddress addr, BtdrvBleAdvertisePacketParameter param) { return ::btmBleUnPairDevice(addr, param); }
Result BleGetPairedAddresses(BtdrvBleAdvertisePacketParameter param, BtdrvAddress *addrs, u8 count, u8 *total_out) { return ::btmBleGetPairedAddresses(param, addrs, count, total_out); }
Result AcquireBleServiceDiscoveryEvent(Event* out_event) { return ::btmAcquireBleServiceDiscoveryEvent(out_event); }
Result GetGattServices(u32 connection_handle, BtmGattService *services, u8 count, u8 *total_out) { return ::btmGetGattServices(connection_handle, services, count, total_out); }
Result GetGattService(u32 connection_handle, const BtdrvGattAttributeUuid *uuid, BtmGattService *service, bool *flag) { return ::btmGetGattService(connection_handle, uuid, service, flag); }
Result GetGattIncludedServices(u32 connection_handle, u16 service_handle, BtmGattService *services, u8 count, u8 *out) { return ::btmGetGattIncludedServices(connection_handle, service_handle, services, count, out); }
Result GetBelongingService(u32 connection_handle, u16 attribute_handle, BtmGattService *service, bool *flag) { return ::btmGetBelongingService(connection_handle, attribute_handle, service, flag); }
Result GetGattCharacteristics(u32 connection_handle, u16 service_handle, BtmGattCharacteristic *characteristics, u8 count, u8 *total_out) { return ::btmGetGattCharacteristics(connection_handle, service_handle, characteristics, count, total_out); }
Result GetGattDescriptors(u32 connection_handle, u16 char_handle, BtmGattDescriptor *descriptors, u8 count, u8 *total_out) { return ::btmGetGattDescriptors(connection_handle, char_handle, descriptors, count, total_out); }
Result AcquireBleMtuConfigEvent(Event* out_event) { return ::btmAcquireBleMtuConfigEvent(out_event); }
Result ConfigureBleMtu(u32 connection_handle, u16 mtu) { return ::btmConfigureBleMtu(connection_handle, mtu); }
Result GetBleMtu(u32 connection_handle, u16 *out) { return ::btmGetBleMtu(connection_handle, out); }
Result RegisterBleGattDataPath(const BtmBleDataPath *path) { return ::btmRegisterBleGattDataPath(path); }
Result UnregisterBleGattDataPath(const BtmBleDataPath *path) { return ::btmUnregisterBleGattDataPath(path); }
Result RegisterAppletResourceUserId(u64 AppletResourceUserId, u32 unk) { return ::btmRegisterAppletResourceUserId(AppletResourceUserId, unk); }
Result UnregisterAppletResourceUserId(u64 AppletResourceUserId) { return ::btmUnregisterAppletResourceUserId(AppletResourceUserId); }
Result SetAppletResourceUserId(u64 AppletResourceUserId) { return ::btmSetAppletResourceUserId(AppletResourceUserId); }
Result GetShortenedDeviceInfo(BtmProfile profile, BtmShortenedDeviceInfo *out, size_t count, s32 *total_out) { return ::btmGetShortenedDeviceInfo(profile, out, count, total_out); }
Result GetShortenedDeviceCondition(BtmProfile profile, BtmShortenedDeviceCondition *out, size_t count, s32 *total_out) { return ::btmGetShortenedDeviceCondition(profile, out, count, total_out); }

} // namespace btm

namespace btmsys {

Result Initialize(void) { return ::btmsysInitialize(); }
void Exit(void) { ::btmsysExit(); }
Result GetServiceSession(Service* srv_out) { return ::btmsysGetServiceSession(srv_out); }
Service* GetServiceSession_IBtmSystemCore(void) { return ::btmsysGetServiceSession_IBtmSystemCore(); }
Result StartGamepadPairing(void) { return ::btmsysStartGamepadPairing(); }
Result CancelGamepadPairing(void) { return ::btmsysCancelGamepadPairing(); }
Result ClearGamepadPairingDatabase(void) { return ::btmsysClearGamepadPairingDatabase(); }
Result GetPairedGamepadCount(u8 *out) { return ::btmsysGetPairedGamepadCount(out); }
Result EnableRadio(void) { return ::btmsysEnableRadio(); }
Result DisableRadio(void) { return ::btmsysDisableRadio(); }
Result GetRadioOnOff(bool *out) { return ::btmsysGetRadioOnOff(out); }
Result AcquireRadioEvent(Event* out_event) { return ::btmsysAcquireRadioEvent(out_event); }
Result AcquireGamepadPairingEvent(Event* out_event) { return ::btmsysAcquireGamepadPairingEvent(out_event); }
Result IsGamepadPairingStarted(bool *out) { return ::btmsysIsGamepadPairingStarted(out); }
Result StartAudioDeviceDiscovery(void) { return ::btmsysStartAudioDeviceDiscovery(); }
Result StopAudioDeviceDiscovery(void) { return ::btmsysStopAudioDeviceDiscovery(); }
Result IsDiscoveryingAudioDevice(bool *out) { return ::btmsysIsDiscoveryingAudioDevice(out); }
Result GetDiscoveredAudioDevice(BtmAudioDevice *out, s32 count, s32 *total_out) { return ::btmsysGetDiscoveredAudioDevice(out, count, total_out); }
Result AcquireAudioDeviceConnectionEvent(Event* out_event) { return ::btmsysAcquireAudioDeviceConnectionEvent(out_event); }
Result ConnectAudioDevice(BtdrvAddress addr) { return ::btmsysConnectAudioDevice(addr); }
Result IsConnectingAudioDevice(bool *out) { return ::btmsysIsConnectingAudioDevice(out); }
Result GetConnectedAudioDevices(BtmAudioDevice *out, s32 count, s32 *total_out) { return ::btmsysGetConnectedAudioDevices(out, count, total_out); }
Result DisconnectAudioDevice(BtdrvAddress addr) { return ::btmsysDisconnectAudioDevice(addr); }
Result AcquirePairedAudioDeviceInfoChangedEvent(Event* out_event) { return ::btmsysAcquirePairedAudioDeviceInfoChangedEvent(out_event); }
Result GetPairedAudioDevices(BtmAudioDevice *out, s32 count, s32 *total_out) { return ::btmsysGetPairedAudioDevices(out, count, total_out); }
Result RemoveAudioDevicePairing(BtdrvAddress addr) { return ::btmsysRemoveAudioDevicePairing(addr); }
Result RequestAudioDeviceConnectionRejection(void) { return ::btmsysRequestAudioDeviceConnectionRejection(); }
Result CancelAudioDeviceConnectionRejection(void) { return ::btmsysCancelAudioDeviceConnectionRejection(); }

} // namespace btmsys

namespace btmu {

Result Initialize(void) { return ::btmuInitialize(); }
void Exit(void) { ::btmuExit(); }
Result GetServiceSession(Service* srv_out) { return ::btmuGetServiceSession(srv_out); }
Service* GetServiceSession_IBtmUserCore(void) { return ::btmuGetServiceSession_IBtmUserCore(); }
Result AcquireBleScanEvent(Event* out_event) { return ::btmuAcquireBleScanEvent(out_event); }
Result GetBleScanFilterParameter(u16 parameter_id, BtdrvBleAdvertisePacketParameter *out) { return ::btmuGetBleScanFilterParameter(parameter_id, out); }
Result GetBleScanFilterParameter2(u16 parameter_id, BtdrvGattAttributeUuid *out) { return ::btmuGetBleScanFilterParameter2(parameter_id, out); }
Result StartBleScanForGeneral(BtdrvBleAdvertisePacketParameter param) { return ::btmuStartBleScanForGeneral(param); }
Result StopBleScanForGeneral(void) { return ::btmuStopBleScanForGeneral(); }
Result GetBleScanResultsForGeneral(BtdrvBleScanResult *results, u8 count, u8 *total_out) { return ::btmuGetBleScanResultsForGeneral(results, count, total_out); }
Result StartBleScanForPaired(BtdrvBleAdvertisePacketParameter param) { return ::btmuStartBleScanForPaired(param); }
Result StopBleScanForPaired(void) { return ::btmuStopBleScanForPaired(); }
Result StartBleScanForSmartDevice(const BtdrvGattAttributeUuid *uuid) { return ::btmuStartBleScanForSmartDevice(uuid); }
Result StopBleScanForSmartDevice(void) { return ::btmuStopBleScanForSmartDevice(); }
Result GetBleScanResultsForSmartDevice(BtdrvBleScanResult *results, u8 count, u8 *total_out) { return ::btmuGetBleScanResultsForSmartDevice(results, count, total_out); }
Result AcquireBleConnectionEvent(Event* out_event) { return ::btmuAcquireBleConnectionEvent(out_event); }
Result BleConnect(BtdrvAddress addr) { return ::btmuBleConnect(addr); }
Result BleDisconnect(u32 connection_handle) { return ::btmuBleDisconnect(connection_handle); }
Result BleGetConnectionState(BtdrvBleConnectionInfo *info, u8 count, u8 *total_out) { return ::btmuBleGetConnectionState(info, count, total_out); }
Result AcquireBlePairingEvent(Event* out_event) { return ::btmuAcquireBlePairingEvent(out_event); }
Result BlePairDevice(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btmuBlePairDevice(connection_handle, param); }
Result BleUnPairDevice(u32 connection_handle, BtdrvBleAdvertisePacketParameter param) { return ::btmuBleUnPairDevice(connection_handle, param); }
Result BleUnPairDevice2(BtdrvAddress addr, BtdrvBleAdvertisePacketParameter param) { return ::btmuBleUnPairDevice2(addr, param); }
Result BleGetPairedDevices(BtdrvBleAdvertisePacketParameter param, BtdrvAddress *addrs, u8 count, u8 *total_out) { return ::btmuBleGetPairedDevices(param, addrs, count, total_out); }
Result AcquireBleServiceDiscoveryEvent(Event* out_event) { return ::btmuAcquireBleServiceDiscoveryEvent(out_event); }
Result GetGattServices(u32 connection_handle, BtmGattService *services, u8 count, u8 *total_out) { return ::btmuGetGattServices(connection_handle, services, count, total_out); }
Result GetGattService(u32 connection_handle, const BtdrvGattAttributeUuid *uuid, BtmGattService *service, bool *flag) { return ::btmuGetGattService(connection_handle, uuid, service, flag); }
Result GetGattIncludedServices(u32 connection_handle, u16 service_handle, BtmGattService *services, u8 count, u8 *out) { return ::btmuGetGattIncludedServices(connection_handle, service_handle, services, count, out); }
Result GetBelongingGattService(u32 connection_handle, u16 attribute_handle, BtmGattService *service, bool *flag) { return ::btmuGetBelongingGattService(connection_handle, attribute_handle, service, flag); }
Result GetGattCharacteristics(u32 connection_handle, u16 service_handle, BtmGattCharacteristic *characteristics, u8 count, u8 *total_out) { return ::btmuGetGattCharacteristics(connection_handle, service_handle, characteristics, count, total_out); }
Result GetGattDescriptors(u32 connection_handle, u16 char_handle, BtmGattDescriptor *descriptors, u8 count, u8 *total_out) { return ::btmuGetGattDescriptors(connection_handle, char_handle, descriptors, count, total_out); }
Result AcquireBleMtuConfigEvent(Event* out_event) { return ::btmuAcquireBleMtuConfigEvent(out_event); }
Result ConfigureBleMtu(u32 connection_handle, u16 mtu) { return ::btmuConfigureBleMtu(connection_handle, mtu); }
Result GetBleMtu(u32 connection_handle, u16 *out) { return ::btmuGetBleMtu(connection_handle, out); }
Result RegisterBleGattDataPath(const BtmBleDataPath *path) { return ::btmuRegisterBleGattDataPath(path); }
Result UnregisterBleGattDataPath(const BtmBleDataPath *path) { return ::btmuUnregisterBleGattDataPath(path); }

} // namespace btmu

namespace capmtp {

Result Initialize(void* mem, size_t size, u32 app_count, u32 max_img, u32 max_vid, const char *other_name) { return ::capmtpInitialize(mem, size, app_count, max_img, max_vid, other_name); }
void Exit(void) { ::capmtpExit(); }
Service* GetRootServiceSession(void) { return ::capmtpGetRootServiceSession(); }
Service* GetServiceSession(void) { return ::capmtpGetServiceSession(); }
Result StartCommandHandler(void) { return ::capmtpStartCommandHandler(); }
Result StopCommandHandler(void) { return ::capmtpStopCommandHandler(); }
bool IsRunning(void) { return ::capmtpIsRunning(); }
Event * GetConnectionEvent(void) { return ::capmtpGetConnectionEvent(); }
bool IsConnected(void) { return ::capmtpIsConnected(); }
Event * GetScanErrorEvent(void) { return ::capmtpGetScanErrorEvent(); }
Result GetScanError(void) { return ::capmtpGetScanError(); }

} // namespace capmtp

namespace caps {

u64 GetShimLibraryVersion(void) { return ::capsGetShimLibraryVersion(); }
CapsAlbumFileDateTime GetDefaultStartDateTime(void) { return ::capsGetDefaultStartDateTime(); }
CapsAlbumFileDateTime GetDefaultEndDateTime(void) { return ::capsGetDefaultEndDateTime(); }
void ConvertApplicationAlbumFileEntryToApplicationAlbumEntry(CapsApplicationAlbumEntry *out, CapsApplicationAlbumFileEntry *in) { ::capsConvertApplicationAlbumFileEntryToApplicationAlbumEntry(out, in); }
void ConvertApplicationAlbumEntryToApplicationAlbumFileEntry(CapsApplicationAlbumFileEntry *out, CapsApplicationAlbumEntry *in) { ::capsConvertApplicationAlbumEntryToApplicationAlbumFileEntry(out, in); }
Result LoadAlbumScreenShotThumbnailImageEx0(u64 *width, u64 *height, CapsScreenShotAttribute *attr, const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsLoadAlbumScreenShotThumbnailImageEx0(width, height, attr, file_id, opts, image, image_size, workbuf, workbuf_size); }

} // namespace caps

namespace capsa {

Result Initialize(void) { return ::capsaInitialize(); }
void Exit(void) { ::capsaExit(); }
Service* GetServiceSession(void) { return ::capsaGetServiceSession(); }
Service* GetServiceSession_Accessor(void) { return ::capsaGetServiceSession_Accessor(); }
Result GetAlbumFileCount(CapsAlbumStorage storage, u64 *count) { return ::capsaGetAlbumFileCount(storage, count); }
Result GetAlbumFileList(CapsAlbumStorage storage, u64 *out, CapsAlbumEntry *entries, u64 count) { return ::capsaGetAlbumFileList(storage, out, entries, count); }
Result LoadAlbumFile(const CapsAlbumFileId *file_id, u64 *out_size, void* filebuf, u64 filebuf_size) { return ::capsaLoadAlbumFile(file_id, out_size, filebuf, filebuf_size); }
Result DeleteAlbumFile(const CapsAlbumFileId *file_id) { return ::capsaDeleteAlbumFile(file_id); }
Result StorageCopyAlbumFile(const CapsAlbumFileId *file_id, CapsAlbumStorage dst_storage) { return ::capsaStorageCopyAlbumFile(file_id, dst_storage); }
Result IsAlbumMounted(CapsAlbumStorage storage, bool *is_mounted) { return ::capsaIsAlbumMounted(storage, is_mounted); }
Result GetAlbumUsage(CapsAlbumStorage storage, CapsAlbumUsage2 *out) { return ::capsaGetAlbumUsage(storage, out); }
Result GetAlbumFileSize(const CapsAlbumFileId *file_id, u64 *size) { return ::capsaGetAlbumFileSize(file_id, size); }
Result LoadAlbumFileThumbnail(const CapsAlbumFileId *file_id, u64 *out_size, void* image, u64 image_size) { return ::capsaLoadAlbumFileThumbnail(file_id, out_size, image, image_size); }
Result LoadAlbumScreenShotImage(u64 *width, u64 *height, const CapsAlbumFileId *file_id, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotImage(width, height, file_id, image, image_size, workbuf, workbuf_size); }
Result LoadAlbumScreenShotThumbnailImage(u64 *width, u64 *height, const CapsAlbumFileId *file_id, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotThumbnailImage(width, height, file_id, image, image_size, workbuf, workbuf_size); }
Result GetAlbumEntryFromApplicationAlbumEntry(CapsAlbumEntry *entry, const CapsApplicationAlbumEntry *application_entry, u64 application_id) { return ::capsaGetAlbumEntryFromApplicationAlbumEntry(entry, application_entry, application_id); }
Result LoadAlbumScreenShotImageEx(u64 *width, u64 *height, const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotImageEx(width, height, file_id, opts, image, image_size, workbuf, workbuf_size); }
Result LoadAlbumScreenShotThumbnailImageEx(u64 *width, u64 *height, const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotThumbnailImageEx(width, height, file_id, opts, image, image_size, workbuf, workbuf_size); }
Result LoadAlbumScreenShotImageEx0(u64 *width, u64 *height, CapsScreenShotAttribute *attr, const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotImageEx0(width, height, attr, file_id, opts, image, image_size, workbuf, workbuf_size); }
Result GetAlbumUsage3(CapsAlbumStorage storage, CapsAlbumUsage3 *out) { return ::capsaGetAlbumUsage3(storage, out); }
Result GetAlbumMountResult(CapsAlbumStorage storage) { return ::capsaGetAlbumMountResult(storage); }
Result GetAlbumUsage16(CapsAlbumStorage storage, u8 flags, CapsAlbumUsage16 *out) { return ::capsaGetAlbumUsage16(storage, flags, out); }
Result GetMinMaxAppletId(bool* success, u64* min, u64* max) { return ::capsaGetMinMaxAppletId(success, min, max); }
Result GetAlbumFileCountEx0(CapsAlbumStorage storage, u8 flags, u64 *count) { return ::capsaGetAlbumFileCountEx0(storage, flags, count); }
Result GetAlbumFileListEx0(CapsAlbumStorage storage, u8 flags, u64 *out, CapsAlbumEntry *entries, u64 count) { return ::capsaGetAlbumFileListEx0(storage, flags, out, entries, count); }
Result GetLastOverlayScreenShotThumbnail(CapsAlbumFileId *file_id, u64 *out_size, void* image, u64 image_size) { return ::capsaGetLastOverlayScreenShotThumbnail(file_id, out_size, image, image_size); }
Result GetLastOverlayMovieThumbnail(CapsAlbumFileId *file_id, u64 *out_size, void* image, u64 image_size) { return ::capsaGetLastOverlayMovieThumbnail(file_id, out_size, image, image_size); }
Result GetAutoSavingStorage(CapsAlbumStorage *storage) { return ::capsaGetAutoSavingStorage(storage); }
Result GetRequiredStorageSpaceSizeToCopyAll(CapsAlbumStorage dst_storage, CapsAlbumStorage src_storage, u64 *out) { return ::capsaGetRequiredStorageSpaceSizeToCopyAll(dst_storage, src_storage, out); }
Result LoadAlbumScreenShotImageEx1(const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, CapsLoadAlbumScreenShotImageOutput *out, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotImageEx1(file_id, opts, out, image, image_size, workbuf, workbuf_size); }
Result LoadAlbumScreenShotThumbnailImageEx1(const CapsAlbumFileId *file_id, const CapsScreenShotDecodeOption *opts, CapsLoadAlbumScreenShotImageOutput *out, void* image, u64 image_size, void* workbuf, u64 workbuf_size) { return ::capsaLoadAlbumScreenShotThumbnailImageEx1(file_id, opts, out, image, image_size, workbuf, workbuf_size); }
Result ForceAlbumUnmounted(CapsAlbumStorage storage) { return ::capsaForceAlbumUnmounted(storage); }
Result ResetAlbumMountStatus(CapsAlbumStorage storage) { return ::capsaResetAlbumMountStatus(storage); }
Result RefreshAlbumCache(CapsAlbumStorage storage) { return ::capsaRefreshAlbumCache(storage); }
Result GetAlbumCache(CapsAlbumStorage storage, CapsAlbumCache *cache) { return ::capsaGetAlbumCache(storage, cache); }
Result GetAlbumCacheEx(CapsAlbumStorage storage, CapsAlbumFileContents contents, CapsAlbumCache *cache) { return ::capsaGetAlbumCacheEx(storage, contents, cache); }
Result GetAlbumEntryFromApplicationAlbumEntryAruid(CapsAlbumEntry *entry, const CapsApplicationAlbumEntry *application_entry) { return ::capsaGetAlbumEntryFromApplicationAlbumEntryAruid(entry, application_entry); }
Result OpenAlbumMovieStream(u64 *stream, const CapsAlbumFileId *file_id) { return ::capsaOpenAlbumMovieStream(stream, file_id); }
Result CloseAlbumMovieStream(u64 stream) { return ::capsaCloseAlbumMovieStream(stream); }
Result GetAlbumMovieStreamSize(u64 stream, u64 *size) { return ::capsaGetAlbumMovieStreamSize(stream, size); }
Result ReadMovieDataFromAlbumMovieReadStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size) { return ::capsaReadMovieDataFromAlbumMovieReadStream(stream, offset, buffer, size, actual_size); }
Result GetAlbumMovieReadStreamBrokenReason(u64 stream) { return ::capsaGetAlbumMovieReadStreamBrokenReason(stream); }
Result GetAlbumMovieReadStreamImageDataSize(u64 stream, u64 *size) { return ::capsaGetAlbumMovieReadStreamImageDataSize(stream, size); }
Result ReadImageDataFromAlbumMovieReadStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size) { return ::capsaReadImageDataFromAlbumMovieReadStream(stream, offset, buffer, size, actual_size); }
Result ReadFileAttributeFromAlbumMovieReadStream(u64 stream, CapsScreenShotAttribute *attr) { return ::capsaReadFileAttributeFromAlbumMovieReadStream(stream, attr); }

} // namespace capsa

namespace capsc {

Result Initialize(void) { return ::capscInitialize(); }
void Exit(void) { ::capscExit(); }
Service* GetServiceSession(void) { return ::capscGetServiceSession(); }
Result NotifyAlbumStorageIsAvailable(CapsAlbumStorage storage) { return ::capscNotifyAlbumStorageIsAvailable(storage); }
Result NotifyAlbumStorageIsUnAvailable(CapsAlbumStorage storage) { return ::capscNotifyAlbumStorageIsUnAvailable(storage); }
Result RegisterAppletResourceUserId(u64 appletResourceUserId, const CapsApplicationId *application_id) { return ::capscRegisterAppletResourceUserId(appletResourceUserId, application_id); }
Result UnregisterAppletResourceUserId(u64 appletResourceUserId, const CapsApplicationId *application_id) { return ::capscUnregisterAppletResourceUserId(appletResourceUserId, application_id); }
Result GetApplicationIdFromAruid(CapsApplicationId *application_id, u64 aruid) { return ::capscGetApplicationIdFromAruid(application_id, aruid); }
Result CheckApplicationIdRegistered(u64 application_id) { return ::capscCheckApplicationIdRegistered(application_id); }
Result GenerateCurrentAlbumFileId(const CapsApplicationId *application_id, CapsAlbumFileContents contents, CapsAlbumFileId *file_id) { return ::capscGenerateCurrentAlbumFileId(application_id, contents, file_id); }
Result GenerateApplicationAlbumEntry(CapsApplicationAlbumEntry *appEntry, const CapsAlbumEntry *entry, u64 application_id) { return ::capscGenerateApplicationAlbumEntry(appEntry, entry, application_id); }
Result SaveAlbumScreenShotFile(const CapsAlbumFileId *file_id, const void* buffer, u64 buffer_size) { return ::capscSaveAlbumScreenShotFile(file_id, buffer, buffer_size); }
Result SaveAlbumScreenShotFileEx(const CapsAlbumFileId *file_id, u64 version, u64 makernote_offset, u64 makernote_size, const void* buffer, u64 buffer_size) { return ::capscSaveAlbumScreenShotFileEx(file_id, version, makernote_offset, makernote_size, buffer, buffer_size); }
Result SetOverlayScreenShotThumbnailData(const CapsAlbumFileId *file_id, const void* image, u64 image_size) { return ::capscSetOverlayScreenShotThumbnailData(file_id, image, image_size); }
Result SetOverlayMovieThumbnailData(const CapsAlbumFileId *file_id, const void* image, u64 image_size) { return ::capscSetOverlayMovieThumbnailData(file_id, image, image_size); }
Result OpenAlbumMovieReadStream(u64 *stream, const CapsAlbumFileId *file_id) { return ::capscOpenAlbumMovieReadStream(stream, file_id); }
Result CloseAlbumMovieStream(u64 stream) { return ::capscCloseAlbumMovieStream(stream); }
Result GetAlbumMovieStreamSize(u64 stream, u64 *size) { return ::capscGetAlbumMovieStreamSize(stream, size); }
Result ReadMovieDataFromAlbumMovieReadStream(u64 stream, u64 offset, void* buffer, size_t size, u64 *actual_size) { return ::capscReadMovieDataFromAlbumMovieReadStream(stream, offset, buffer, size, actual_size); }
Result GetAlbumMovieReadStreamBrokenReason(u64 stream) { return ::capscGetAlbumMovieReadStreamBrokenReason(stream); }
Result GetAlbumMovieReadStreamImageDataSize(u64 stream, u64 *size) { return ::capscGetAlbumMovieReadStreamImageDataSize(stream, size); }
Result ReadImageDataFromAlbumMovieReadStream(u64 stream, u64 offset, void* buffer, size_t size, u64 *actual_size) { return ::capscReadImageDataFromAlbumMovieReadStream(stream, offset, buffer, size, actual_size); }
Result ReadFileAttributeFromAlbumMovieReadStream(u64 stream, CapsScreenShotAttribute *attribute) { return ::capscReadFileAttributeFromAlbumMovieReadStream(stream, attribute); }
Result OpenAlbumMovieWriteStream(u64 *stream, const CapsAlbumFileId *file_id) { return ::capscOpenAlbumMovieWriteStream(stream, file_id); }
Result FinishAlbumMovieWriteStream(u64 stream) { return ::capscFinishAlbumMovieWriteStream(stream); }
Result CommitAlbumMovieWriteStream(u64 stream) { return ::capscCommitAlbumMovieWriteStream(stream); }
Result DiscardAlbumMovieWriteStream(u64 stream) { return ::capscDiscardAlbumMovieWriteStream(stream); }
Result DiscardAlbumMovieWriteStreamNoDelete(u64 stream) { return ::capscDiscardAlbumMovieWriteStreamNoDelete(stream); }
Result CommitAlbumMovieWriteStreamEx(u64 stream, CapsAlbumEntry *entry) { return ::capscCommitAlbumMovieWriteStreamEx(stream, entry); }
Result StartAlbumMovieWriteStreamDataSection(u64 stream) { return ::capscStartAlbumMovieWriteStreamDataSection(stream); }
Result EndAlbumMovieWriteStreamDataSection(u64 stream) { return ::capscEndAlbumMovieWriteStreamDataSection(stream); }
Result StartAlbumMovieWriteStreamMetaSection(u64 stream) { return ::capscStartAlbumMovieWriteStreamMetaSection(stream); }
Result EndAlbumMovieWriteStreamMetaSection(u64 stream) { return ::capscEndAlbumMovieWriteStreamMetaSection(stream); }
Result ReadDataFromAlbumMovieWriteStream(u64 stream, u64 offset, void* buffer, u64 size, u64 *actual_size) { return ::capscReadDataFromAlbumMovieWriteStream(stream, offset, buffer, size, actual_size); }
Result WriteDataToAlbumMovieWriteStream(u64 stream, u64 offset, const void* buffer, u64 size) { return ::capscWriteDataToAlbumMovieWriteStream(stream, offset, buffer, size); }
Result WriteMetaToAlbumMovieWriteStream(u64 stream, u64 offset, const void* buffer, u64 size) { return ::capscWriteMetaToAlbumMovieWriteStream(stream, offset, buffer, size); }
Result GetAlbumMovieWriteStreamBrokenReason(u64 stream) { return ::capscGetAlbumMovieWriteStreamBrokenReason(stream); }
Result GetAlbumMovieWriteStreamDataSize(u64 stream, u64 *size) { return ::capscGetAlbumMovieWriteStreamDataSize(stream, size); }
Result SetAlbumMovieWriteStreamDataSize(u64 stream, u64 size) { return ::capscSetAlbumMovieWriteStreamDataSize(stream, size); }

} // namespace capsc

namespace capsdc {

Result Initialize(void) { return ::capsdcInitialize(); }
void Exit(void) { ::capsdcExit(); }
Service* GetServiceSession(void) { return ::capsdcGetServiceSession(); }
Result DecodeJpeg(u32 width, u32 height, const CapsScreenShotDecodeOption *opts, const void* jpeg, size_t jpeg_size, void* out_image, size_t out_image_size) { return ::capsdcDecodeJpeg(width, height, opts, jpeg, jpeg_size, out_image, out_image_size); }
Result ShrinkJpeg(u32 width, u32 height, const CapsScreenShotDecodeOption *opts, const void* jpeg, size_t jpeg_size, void* out_jpeg, size_t out_jpeg_size, u64 *out_result_size) { return ::capsdcShrinkJpeg(width, height, opts, jpeg, jpeg_size, out_jpeg, out_jpeg_size, out_result_size); }
Result ShrinkJpegEx(u32 scaled_width, u32 scaled_height, u32 jpeg_quality, const CapsScreenShotDecodeOption *opts, const void* jpeg, size_t jpeg_size, void* out_jpeg, size_t out_jpeg_size, u64 *out_result_size) { return ::capsdcShrinkJpegEx(scaled_width, scaled_height, jpeg_quality, opts, jpeg, jpeg_size, out_jpeg, out_jpeg_size, out_result_size); }

} // namespace capsdc

namespace capssc {

Result Initialize(void) { return ::capsscInitialize(); }
void Exit(void) { ::capsscExit(); }
Service* GetServiceSession(void) { return ::capsscGetServiceSession(); }
Result CaptureRawImageWithTimeout(void* buf, size_t size, ViLayerStack layer_stack, u64 width, u64 height, s64 buffer_count, s64 buffer_index, s64 timeout) { return ::capsscCaptureRawImageWithTimeout(buf, size, layer_stack, width, height, buffer_count, buffer_index, timeout); }
Result OpenRawScreenShotReadStream(u64 *out_size, u64 *out_width, u64 *out_height, ViLayerStack layer_stack, s64 timeout) { return ::capsscOpenRawScreenShotReadStream(out_size, out_width, out_height, layer_stack, timeout); }
Result CloseRawScreenShotReadStream(void) { return ::capsscCloseRawScreenShotReadStream(); }
Result ReadRawScreenShotReadStream(u64 *bytes_read, void* buf, size_t size, u64 offset) { return ::capsscReadRawScreenShotReadStream(bytes_read, buf, size, offset); }
Result CaptureJpegScreenShot(u64* out_jpeg_size, void* jpeg_buf, size_t jpeg_buf_size, ViLayerStack layer_stack, s64 timeout) { return ::capsscCaptureJpegScreenShot(out_jpeg_size, jpeg_buf, jpeg_buf_size, layer_stack, timeout); }

} // namespace capssc

namespace capssu {

Result Initialize(void) { return ::capssuInitialize(); }
void Exit(void) { ::capssuExit(); }
Service* GetServiceSession(void) { return ::capssuGetServiceSession(); }
Result SaveScreenShot(const void* buffer, size_t size, AlbumReportOption reportoption, AlbumImageOrientation orientation, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShot(buffer, size, reportoption, orientation, out); }
Result SaveScreenShotWithUserData(const void* buffer, size_t size, AlbumReportOption reportoption, AlbumImageOrientation orientation, const void* userdata, size_t userdata_size, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShotWithUserData(buffer, size, reportoption, orientation, userdata, userdata_size, out); }
Result SaveScreenShotWithUserIds(const void* buffer, size_t size, AlbumReportOption reportoption, AlbumImageOrientation orientation, const AccountUid* uids, size_t uid_count, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShotWithUserIds(buffer, size, reportoption, orientation, uids, uid_count, out); }
Result SaveScreenShotEx0(const void* buffer, size_t size, const CapsScreenShotAttribute *attr, AlbumReportOption reportoption, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShotEx0(buffer, size, attr, reportoption, out); }
Result SaveScreenShotEx1(const void* buffer, size_t size, const CapsScreenShotAttribute *attr, AlbumReportOption reportoption, CapsApplicationData *appdata, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShotEx1(buffer, size, attr, reportoption, appdata, out); }
Result SaveScreenShotEx2(const void* buffer, size_t size, const CapsScreenShotAttribute *attr, AlbumReportOption reportoption, CapsUserIdList *list, CapsApplicationAlbumEntry *out) { return ::capssuSaveScreenShotEx2(buffer, size, attr, reportoption, list, out); }

} // namespace capssu

namespace capsu {

Result Initialize(void) { return ::capsuInitialize(); }
void Exit(void) { ::capsuExit(); }
Service* GetServiceSession(void) { return ::capsuGetServiceSession(); }
Service* GetServiceSession_Accessor(void) { return ::capsuGetServiceSession_Accessor(); }
Result GetAlbumFileListDeprecated1(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, const CapsAlbumFileDateTime *start_datetime, const CapsAlbumFileDateTime *end_datetime, s32 *total_entries) { return ::capsuGetAlbumFileListDeprecated1(entries, count, type, start_datetime, end_datetime, total_entries); }
Result GetAlbumFileListDeprecated2(CapsApplicationAlbumFileEntry *entries, s32 count, CapsContentType type, const CapsAlbumFileDateTime *start_datetime, const CapsAlbumFileDateTime *end_datetime, AccountUid uid, s32 *total_entries) { return ::capsuGetAlbumFileListDeprecated2(entries, count, type, start_datetime, end_datetime, uid, total_entries); }
Result GetAlbumFileList3(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, const CapsAlbumFileDateTime *start_datetime, const CapsAlbumFileDateTime *end_datetime, s32 *total_entries) { return ::capsuGetAlbumFileList3(entries, count, type, start_datetime, end_datetime, total_entries); }
Result GetAlbumFileList4(CapsApplicationAlbumEntry *entries, s32 count, CapsContentType type, const CapsAlbumFileDateTime *start_datetime, const CapsAlbumFileDateTime *end_datetime, AccountUid uid, s32 *total_entries) { return ::capsuGetAlbumFileList4(entries, count, type, start_datetime, end_datetime, uid, total_entries); }
Result DeleteAlbumFile(CapsContentType type, const CapsApplicationAlbumFileEntry *entry) { return ::capsuDeleteAlbumFile(type, entry); }
Result GetAlbumFileSize(const CapsApplicationAlbumFileEntry *entry, u64 *size) { return ::capsuGetAlbumFileSize(entry, size); }
Result LoadAlbumScreenShotImage(s32 *width, s32 *height, CapsScreenShotAttributeForApplication *attr, void* userdata, size_t userdata_maxsize, u32 *userdata_size, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option) { return ::capsuLoadAlbumScreenShotImage(width, height, attr, userdata, userdata_maxsize, userdata_size, image, image_size, workbuf, workbuf_size, entry, option); }
Result LoadAlbumScreenShotThumbnailImage(s32 *width, s32 *height, CapsScreenShotAttributeForApplication *attr, void* userdata, size_t userdata_maxsize, u32 *userdata_size, void* image, size_t image_size, void* workbuf, size_t workbuf_size, const CapsApplicationAlbumFileEntry *entry, const CapsScreenShotDecodeOption *option) { return ::capsuLoadAlbumScreenShotThumbnailImage(width, height, attr, userdata, userdata_maxsize, userdata_size, image, image_size, workbuf, workbuf_size, entry, option); }
Result PrecheckToCreateContents(CapsContentType type, u64 unk) { return ::capsuPrecheckToCreateContents(type, unk); }
Result OpenAlbumMovieStream(u64 *stream, const CapsApplicationAlbumFileEntry *entry) { return ::capsuOpenAlbumMovieStream(stream, entry); }
Result CloseAlbumMovieStream(u64 stream) { return ::capsuCloseAlbumMovieStream(stream); }
Result GetAlbumMovieStreamSize(u64 stream, u64 *size) { return ::capsuGetAlbumMovieStreamSize(stream, size); }
Result ReadAlbumMovieStream(u64 stream, s64 offset, void* buffer, size_t size, u64 *actual_size) { return ::capsuReadAlbumMovieStream(stream, offset, buffer, size, actual_size); }
Result GetAlbumMovieStreamBrokenReason(u64 stream) { return ::capsuGetAlbumMovieStreamBrokenReason(stream); }

} // namespace capsu

namespace clkrst {

Result Initialize(void) { return ::clkrstInitialize(); }
void Exit(void) { ::clkrstExit(); }
Service* GetServiceSession(void) { return ::clkrstGetServiceSession(); }
Result OpenSession(ClkrstSession* session_out, PcvModuleId module_id, u32 unk) { return ::clkrstOpenSession(session_out, module_id, unk); }
void CloseSession(ClkrstSession* session) { ::clkrstCloseSession(session); }
Result SetClockRate(ClkrstSession* session, u32 hz) { return ::clkrstSetClockRate(session, hz); }
Result GetClockRate(ClkrstSession* session, u32 *out_hz) { return ::clkrstGetClockRate(session, out_hz); }
// Result SetMinimumVoltageClockRate(ClkrstSession *session, u32 hz) { return ::clkrstSetMinimumVoltageClockRate(session, hz); }
Result GetPossibleClockRates(ClkrstSession *session, u32 *rates, s32 max_count, PcvClockRatesListType *out_type, s32 *out_count) { return ::clkrstGetPossibleClockRates(session, rates, max_count, out_type, out_count); }
// Result GetDvfsTable(ClkrstSession *session, u32 *out_rate_table, s32 in_rate_count, u32 *out_voltage_table, s32 in_voltage_count, s32 *out_count) { return ::clkrstGetDvfsTable(session, out_rate_table, in_rate_count, out_voltage_table, in_voltage_count, out_count); }

} // namespace clkrst

namespace cmac {

void Aes128ContextCreate(Aes128CmacContext *out, const void *key) { ::cmacAes128ContextCreate(out, key); }
void Aes128ContextUpdate(Aes128CmacContext *ctx, const void *src, size_t size) { ::cmacAes128ContextUpdate(ctx, src, size); }
void Aes128ContextGetMac(Aes128CmacContext *ctx, void *dst) { ::cmacAes128ContextGetMac(ctx, dst); }
void Aes128CalculateMac(void *dst, const void *key, const void *src, size_t size) { ::cmacAes128CalculateMac(dst, key, src, size); }
void Aes192ContextCreate(Aes192CmacContext *out, const void *key) { ::cmacAes192ContextCreate(out, key); }
void Aes192ContextUpdate(Aes192CmacContext *ctx, const void *src, size_t size) { ::cmacAes192ContextUpdate(ctx, src, size); }
void Aes192ContextGetMac(Aes192CmacContext *ctx, void *dst) { ::cmacAes192ContextGetMac(ctx, dst); }
void Aes192CalculateMac(void *dst, const void *key, const void *src, size_t size) { ::cmacAes192CalculateMac(dst, key, src, size); }
void Aes256ContextCreate(Aes256CmacContext *out, const void *key) { ::cmacAes256ContextCreate(out, key); }
void Aes256ContextUpdate(Aes256CmacContext *ctx, const void *src, size_t size) { ::cmacAes256ContextUpdate(ctx, src, size); }
void Aes256ContextGetMac(Aes256CmacContext *ctx, void *dst) { ::cmacAes256ContextGetMac(ctx, dst); }
void Aes256CalculateMac(void *dst, const void *key, const void *src, size_t size) { ::cmacAes256CalculateMac(dst, key, src, size); }

} // namespace cmac

namespace cmif {

void* GetAlignedDataStart(u32* data_words, void* base) { return ::cmifGetAlignedDataStart(data_words, base); }
CmifRequest MakeRequest(void* base, CmifRequestFormat fmt) { return ::cmifMakeRequest(base, fmt); }
void* MakeControlRequest(void* base, u32 request_id, u32 size) { return ::cmifMakeControlRequest(base, request_id, size); }
void MakeCloseRequest(void* base, u32 object_id) { ::cmifMakeCloseRequest(base, object_id); }
void RequestInBuffer(CmifRequest* req, const void* buffer, size_t size, HipcBufferMode mode) { ::cmifRequestInBuffer(req, buffer, size, mode); }
void RequestOutBuffer(CmifRequest* req, void* buffer, size_t size, HipcBufferMode mode) { ::cmifRequestOutBuffer(req, buffer, size, mode); }
void RequestInOutBuffer(CmifRequest* req, void* buffer, size_t size, HipcBufferMode mode) { ::cmifRequestInOutBuffer(req, buffer, size, mode); }
void RequestInPointer(CmifRequest* req, const void* buffer, size_t size) { ::cmifRequestInPointer(req, buffer, size); }
void RequestOutFixedPointer(CmifRequest* req, void* buffer, size_t size) { ::cmifRequestOutFixedPointer(req, buffer, size); }
void RequestOutPointer(CmifRequest* req, void* buffer, size_t size) { ::cmifRequestOutPointer(req, buffer, size); }
void RequestInAutoBuffer(CmifRequest* req, const void* buffer, size_t size, HipcBufferMode mode) { ::cmifRequestInAutoBuffer(req, buffer, size, mode); }
void RequestOutAutoBuffer(CmifRequest* req, void* buffer, size_t size, HipcBufferMode mode) { ::cmifRequestOutAutoBuffer(req, buffer, size, mode); }
void RequestObject(CmifRequest* req, u32 object_id) { ::cmifRequestObject(req, object_id); }
void RequestHandle(CmifRequest* req, Handle handle) { ::cmifRequestHandle(req, handle); }
Result ParseResponse(CmifResponse* res, void* base, bool is_domain, u32 size) { return ::cmifParseResponse(res, base, is_domain, size); }
u32 ResponseGetObject(CmifResponse* res) { return ::cmifResponseGetObject(res); }
Handle ResponseGetCopyHandle(CmifResponse* res) { return ::cmifResponseGetCopyHandle(res); }
Handle ResponseGetMoveHandle(CmifResponse* res) { return ::cmifResponseGetMoveHandle(res); }
Result ConvertCurrentObjectToDomain(Handle h, u32* out_object_id) { return ::cmifConvertCurrentObjectToDomain(h, out_object_id); }
Result CopyFromCurrentDomain(Handle h, u32 object_id, Handle* out_h) { return ::cmifCopyFromCurrentDomain(h, object_id, out_h); }
Result CloneCurrentObject(Handle h, Handle* out_h) { return ::cmifCloneCurrentObject(h, out_h); }
Result QueryPointerBufferSize(Handle h, u16* out_size) { return ::cmifQueryPointerBufferSize(h, out_size); }
Result CloneCurrentObjectEx(Handle h, u32 tag, Handle* out_h) { return ::cmifCloneCurrentObjectEx(h, tag, out_h); }

} // namespace cmif

namespace condvar {

void Init(CondVar* c) { ::condvarInit(c); }
Result WaitTimeout(CondVar* c, Mutex* m, u64 timeout) { return ::condvarWaitTimeout(c, m, timeout); }
Result Wait(CondVar* c, Mutex* m) { return ::condvarWait(c, m); }
Result Wake(CondVar* c, int num) { return ::condvarWake(c, num); }
Result WakeOne(CondVar* c) { return ::condvarWakeOne(c); }
Result WakeAll(CondVar* c) { return ::condvarWakeAll(c); }

} // namespace condvar

namespace console {

void SetFont(PrintConsole* console, ConsoleFont* font) { ::consoleSetFont(console, font); }
void SetWindow(PrintConsole* console, int x, int y, int width, int height) { ::consoleSetWindow(console, x, y, width, height); }
PrintConsole* GetDefault(void) { return ::consoleGetDefault(); }
PrintConsole * Select(PrintConsole* console) { return ::consoleSelect(console); }
PrintConsole* Init(PrintConsole* console) { return ::consoleInit(console); }
void Exit(PrintConsole* console) { ::consoleExit(console); }
void Update(PrintConsole* console) { ::consoleUpdate(console); }
void DebugInit(debugDevice device) { ::consoleDebugInit(device); }
void Clear(void) { ::consoleClear(); }

} // namespace console

namespace crc32 {

u32 CalculateWithSeed(u32 seed, const void *src, size_t size) { return ::crc32CalculateWithSeed(seed, src, size); }
u32 Calculate(const void *src, size_t size) { return ::crc32Calculate(src, size); }

} // namespace crc32

namespace crc32c {

u32 CalculateWithSeed(u32 seed, const void *src, size_t size) { return ::crc32cCalculateWithSeed(seed, src, size); }
u32 Calculate(const void *src, size_t size) { return ::crc32cCalculate(src, size); }

} // namespace crc32c

namespace csrng {

Result Initialize(void) { return ::csrngInitialize(); }
void Exit(void) { ::csrngExit(); }
Service* GetServiceSession(void) { return ::csrngGetServiceSession(); }
Result GetRandomBytes(void *out, size_t out_size) { return ::csrngGetRandomBytes(out, out_size); }

} // namespace csrng

namespace detect {

bool Debugger(void) { return ::detectDebugger(); }
bool Mesosphere(void) { return ::detectMesosphere(); }

} // namespace detect

namespace diag {

[[noreturn]] void AbortWithResult(Result res) { ::diagAbortWithResult(res); }

} // namespace diag

namespace ectxr {

Result Initialize(void) { return ::ectxrInitialize(); }
void Exit(void) { ::ectxrExit(); }
Service* GetServiceSession(void) { return ::ectxrGetServiceSession(); }
Result PullContext(s32 *out0, u32 *out_total_size, u32 *out_size, void *dst, size_t dst_size, u32 descriptor, Result result) { return ::ectxrPullContext(out0, out_total_size, out_size, dst, dst_size, descriptor, result); }

} // namespace ectxr

namespace env {

void Setup(void* ctx, Handle main_thread, LoaderReturnFn saved_lr) { ::envSetup(ctx, main_thread, saved_lr); }
const char* GetLoaderInfo(void) { return ::envGetLoaderInfo(); }
u64 GetLoaderInfoSize(void) { return ::envGetLoaderInfoSize(); }
Handle GetMainThreadHandle(void) { return ::envGetMainThreadHandle(); }
bool IsNso(void) { return ::envIsNso(); }
bool HasHeapOverride(void) { return ::envHasHeapOverride(); }
void* GetHeapOverrideAddr(void) { return ::envGetHeapOverrideAddr(); }
u64 GetHeapOverrideSize(void) { return ::envGetHeapOverrideSize(); }
bool HasArgv(void) { return ::envHasArgv(); }
void* GetArgv(void) { return ::envGetArgv(); }
bool IsSyscallHinted(unsigned svc) { return ::envIsSyscallHinted(svc); }
Handle GetOwnProcessHandle(void) { return ::envGetOwnProcessHandle(); }
LoaderReturnFn GetExitFuncPtr(void) { return ::envGetExitFuncPtr(); }
void SetExitFuncPtr(LoaderReturnFn addr) { ::envSetExitFuncPtr(addr); }
Result SetNextLoad(const char* path, const char* argv) { return ::envSetNextLoad(path, argv); }
bool HasNextLoad(void) { return ::envHasNextLoad(); }
Result GetLastLoadResult(void) { return ::envGetLastLoadResult(); }
bool HasRandomSeed(void) { return ::envHasRandomSeed(); }
void GetRandomSeed(u64 out[2]) { ::envGetRandomSeed(out); }
AccountUid* GetUserIdStorage(void) { return ::envGetUserIdStorage(); }

} // namespace env

namespace error {

ErrorCode CodeCreate(u32 low, u32 desc) { return ::errorCodeCreate(low, desc); }
ErrorCode CodeCreateResult(Result res) { return ::errorCodeCreateResult(res); }
ErrorCode CodeCreateInvalid(void) { return ::errorCodeCreateInvalid(); }
bool CodeIsValid(ErrorCode errorCode) { return ::errorCodeIsValid(errorCode); }
Result ResultShow(Result res, bool jumpFlag, const ErrorContext* ctx) { return ::errorResultShow(res, jumpFlag, ctx); }
Result CodeShow(ErrorCode errorCode, bool jumpFlag, const ErrorContext* ctx) { return ::errorCodeShow(errorCode, jumpFlag, ctx); }
Result ResultBacktraceCreate(ErrorResultBacktrace* backtrace, s32 count, const Result* entries) { return ::errorResultBacktraceCreate(backtrace, count, entries); }
Result ResultBacktraceShow(Result res, const ErrorResultBacktrace* backtrace) { return ::errorResultBacktraceShow(res, backtrace); }
Result EulaShow(SetRegion RegionCode) { return ::errorEulaShow(RegionCode); }
Result SystemUpdateEulaShow(SetRegion RegionCode, const ErrorEulaData* eula) { return ::errorSystemUpdateEulaShow(RegionCode, eula); }
Result CodeRecordShow(ErrorCode errorCode, u64 timestamp) { return ::errorCodeRecordShow(errorCode, timestamp); }
Result ResultRecordShow(Result res, u64 timestamp) { return ::errorResultRecordShow(res, timestamp); }
Result SystemCreate(ErrorSystemConfig* c, const char* dialog_message, const char* fullscreen_message) { return ::errorSystemCreate(c, dialog_message, fullscreen_message); }
Result SystemShow(ErrorSystemConfig* c) { return ::errorSystemShow(c); }
void SystemSetCode(ErrorSystemConfig* c, ErrorCode errorCode) { ::errorSystemSetCode(c, errorCode); }
void SystemSetResult(ErrorSystemConfig* c, Result res) { ::errorSystemSetResult(c, res); }
void SystemSetLanguageCode(ErrorSystemConfig* c, u64 LanguageCode) { ::errorSystemSetLanguageCode(c, LanguageCode); }
void SystemSetContext(ErrorSystemConfig* c, const ErrorContext* ctx) { ::errorSystemSetContext(c, ctx); }
Result ApplicationCreate(ErrorApplicationConfig* c, const char* dialog_message, const char* fullscreen_message) { return ::errorApplicationCreate(c, dialog_message, fullscreen_message); }
Result ApplicationShow(ErrorApplicationConfig* c) { return ::errorApplicationShow(c); }
void ApplicationSetNumber(ErrorApplicationConfig* c, u32 errorNumber) { ::errorApplicationSetNumber(c, errorNumber); }
void ApplicationSetLanguageCode(ErrorApplicationConfig* c, u64 LanguageCode) { ::errorApplicationSetLanguageCode(c, LanguageCode); }

} // namespace error

namespace event {

Result Create(Event* t, bool autoclear) { return ::eventCreate(t, autoclear); }
void LoadRemote(Event* t, Handle handle, bool autoclear) { ::eventLoadRemote(t, handle, autoclear); }
void Close(Event* t) { ::eventClose(t); }
bool Active(Event* t) { return ::eventActive(t); }
Result Wait(Event* t, u64 timeout) { return ::eventWait(t, timeout); }
Result Fire(Event* t) { return ::eventFire(t); }
Result Clear(Event* t) { return ::eventClear(t); }

} // namespace event

namespace fan {

Result Initialize(void) { return ::fanInitialize(); }
void Exit(void) { ::fanExit(); }
Service* GetServiceSession(void) { return ::fanGetServiceSession(); }
Result OpenController(FanController *out, u32 device_code) { return ::fanOpenController(out, device_code); }
void ControllerClose(FanController *controller) { ::fanControllerClose(controller); }
Result ControllerSetRotationSpeedLevel(FanController *controller, float level) { return ::fanControllerSetRotationSpeedLevel(controller, level); }
Result ControllerGetRotationSpeedLevel(FanController *controller, float *level) { return ::fanControllerGetRotationSpeedLevel(controller, level); }

} // namespace fan

namespace fatal {

[[noreturn]] void Throw(Result err) { ::fatalThrow(err); }
void ThrowWithPolicy(Result err, FatalPolicy type) { ::fatalThrowWithPolicy(err, type); }
void ThrowWithContext(Result err, FatalPolicy type, FatalCpuContext *ctx) { ::fatalThrowWithContext(err, type, ctx); }

} // namespace fatal

namespace framebuffer {

Result Create(Framebuffer* fb, NWindow *win, u32 width, u32 height, u32 format, u32 num_fbs) { return ::framebufferCreate(fb, win, width, height, format, num_fbs); }
Result MakeLinear(Framebuffer* fb) { return ::framebufferMakeLinear(fb); }
void Close(Framebuffer* fb) { ::framebufferClose(fb); }
void* Begin(Framebuffer* fb, u32* out_stride) { return ::framebufferBegin(fb, out_stride); }
void End(Framebuffer* fb) { ::framebufferEnd(fb); }

} // namespace framebuffer

namespace friends {

Result LaShowFriendList(AccountUid uid) { return ::friendsLaShowFriendList(uid); }
Result LaShowUserDetailInfo(AccountUid uid, AccountNetworkServiceAccountId id, const FriendsInAppScreenName *first_inAppScreenName, const FriendsInAppScreenName *second_inAppScreenName) { return ::friendsLaShowUserDetailInfo(uid, id, first_inAppScreenName, second_inAppScreenName); }
Result LaStartSendingFriendRequest(AccountUid uid, AccountNetworkServiceAccountId id, const FriendsInAppScreenName *first_inAppScreenName, const FriendsInAppScreenName *second_inAppScreenName) { return ::friendsLaStartSendingFriendRequest(uid, id, first_inAppScreenName, second_inAppScreenName); }
Result LaShowMethodsOfSendingFriendRequest(AccountUid uid) { return ::friendsLaShowMethodsOfSendingFriendRequest(uid); }
Result LaStartFacedFriendRequest(AccountUid uid) { return ::friendsLaStartFacedFriendRequest(uid); }
Result LaShowReceivedFriendRequestList(AccountUid uid) { return ::friendsLaShowReceivedFriendRequestList(uid); }
Result LaShowBlockedUserList(AccountUid uid) { return ::friendsLaShowBlockedUserList(uid); }
Result LaShowMyProfile(AccountUid uid) { return ::friendsLaShowMyProfile(uid); }
Result LaShowMyProfileForHomeMenu(AccountUid uid) { return ::friendsLaShowMyProfileForHomeMenu(uid); }
Result LaStartFriendInvitation(AccountUid uid, s32 id_count, const FriendsFriendInvitationGameModeDescription *desc, const void* userdata, u64 userdata_size) { return ::friendsLaStartFriendInvitation(uid, id_count, desc, userdata, userdata_size); }
Result LaStartSendingFriendInvitation(AccountUid uid, const AccountNetworkServiceAccountId *id_list, s32 id_count, const FriendsFriendInvitationGameModeDescription *desc, const void* userdata, u64 userdata_size) { return ::friendsLaStartSendingFriendInvitation(uid, id_list, id_count, desc, userdata, userdata_size); }
Result LaShowReceivedInvitationDetail(AccountUid uid, FriendsFriendInvitationId invitation_id, FriendsFriendInvitationGroupId invitation_group_id) { return ::friendsLaShowReceivedInvitationDetail(uid, invitation_id, invitation_group_id); }
Result Initialize(FriendsServiceType service_type) { return ::friendsInitialize(service_type); }
void Exit(void) { ::friendsExit(); }
Service* GetServiceSession(void) { return ::friendsGetServiceSession(); }
Service* GetServiceSession_IFriendsService(void) { return ::friendsGetServiceSession_IFriendsService(); }
Result GetUserSetting(AccountUid uid, FriendsUserSetting *user_setting) { return ::friendsGetUserSetting(uid, user_setting); }
Result GetFriendInvitationNotificationEvent(Event *out_event) { return ::friendsGetFriendInvitationNotificationEvent(out_event); }
Result TryPopFriendInvitationNotificationInfo(AccountUid *uid, void* buffer, u64 size, u64 *out_size) { return ::friendsTryPopFriendInvitationNotificationInfo(uid, buffer, size, out_size); }

} // namespace friends

namespace fs {

Result Initialize(void) { return ::fsInitialize(); }
void Exit(void) { ::fsExit(); }
Service* GetServiceSession(void) { return ::fsGetServiceSession(); }
void SetPriority(FsPriority prio) { ::fsSetPriority(prio); }
Result OpenFileSystem(FsFileSystem* out, FsFileSystemType fsType, const char* contentPath) { return ::fsOpenFileSystem(out, fsType, contentPath); }
Result OpenDataFileSystemByCurrentProcess(FsFileSystem *out) { return ::fsOpenDataFileSystemByCurrentProcess(out); }
Result OpenFileSystemWithPatch(FsFileSystem* out, u64 id, FsFileSystemType fsType) { return ::fsOpenFileSystemWithPatch(out, id, fsType); }
Result OpenFileSystemWithId(FsFileSystem* out, u64 id, FsFileSystemType fsType, const char* contentPath, FsContentAttributes attr) { return ::fsOpenFileSystemWithId(out, id, fsType, contentPath, attr); }
Result OpenDataFileSystemByProgramId(FsFileSystem *out, u64 program_id) { return ::fsOpenDataFileSystemByProgramId(out, program_id); }
Result OpenBisFileSystem(FsFileSystem* out, FsBisPartitionId partitionId, const char* string) { return ::fsOpenBisFileSystem(out, partitionId, string); }
Result OpenBisStorage(FsStorage* out, FsBisPartitionId partitionId) { return ::fsOpenBisStorage(out, partitionId); }
Result OpenSdCardFileSystem(FsFileSystem* out) { return ::fsOpenSdCardFileSystem(out); }
Result OpenHostFileSystem(FsFileSystem* out, const char *path) { return ::fsOpenHostFileSystem(out, path); }
Result OpenHostFileSystemWithOption(FsFileSystem* out, const char *path, u32 flags) { return ::fsOpenHostFileSystemWithOption(out, path, flags); }
Result DeleteSaveDataFileSystem(u64 application_id) { return ::fsDeleteSaveDataFileSystem(application_id); }
Result CreateSaveDataFileSystem(const FsSaveDataAttribute* attr, const FsSaveDataCreationInfo* creation_info, const FsSaveDataMetaInfo* meta) { return ::fsCreateSaveDataFileSystem(attr, creation_info, meta); }
Result CreateSaveDataFileSystemBySystemSaveDataId(const FsSaveDataAttribute* attr, const FsSaveDataCreationInfo* creation_info) { return ::fsCreateSaveDataFileSystemBySystemSaveDataId(attr, creation_info); }
Result DeleteSaveDataFileSystemBySaveDataSpaceId(FsSaveDataSpaceId save_data_space_id, u64 saveID) { return ::fsDeleteSaveDataFileSystemBySaveDataSpaceId(save_data_space_id, saveID); }
Result DeleteSaveDataFileSystemBySaveDataAttribute(FsSaveDataSpaceId save_data_space_id, const FsSaveDataAttribute* attr) { return ::fsDeleteSaveDataFileSystemBySaveDataAttribute(save_data_space_id, attr); }
Result IsExFatSupported(bool* out) { return ::fsIsExFatSupported(out); }
Result OpenGameCardFileSystem(FsFileSystem* out, const FsGameCardHandle* handle, FsGameCardPartition partition) { return ::fsOpenGameCardFileSystem(out, handle, partition); }
Result ExtendSaveDataFileSystem(FsSaveDataSpaceId save_data_space_id, u64 saveID, s64 dataSize, s64 journalSize) { return ::fsExtendSaveDataFileSystem(save_data_space_id, saveID, dataSize, journalSize); }
Result OpenSaveDataFileSystem(FsFileSystem* out, FsSaveDataSpaceId save_data_space_id, const FsSaveDataAttribute *attr) { return ::fsOpenSaveDataFileSystem(out, save_data_space_id, attr); }
Result OpenSaveDataFileSystemBySystemSaveDataId(FsFileSystem* out, FsSaveDataSpaceId save_data_space_id, const FsSaveDataAttribute *attr) { return ::fsOpenSaveDataFileSystemBySystemSaveDataId(out, save_data_space_id, attr); }
Result OpenReadOnlySaveDataFileSystem(FsFileSystem* out, FsSaveDataSpaceId save_data_space_id, const FsSaveDataAttribute *attr) { return ::fsOpenReadOnlySaveDataFileSystem(out, save_data_space_id, attr); }
Result ReadSaveDataFileSystemExtraDataBySaveDataSpaceId(void* buf, size_t len, FsSaveDataSpaceId save_data_space_id, u64 saveID) { return ::fsReadSaveDataFileSystemExtraDataBySaveDataSpaceId(buf, len, save_data_space_id, saveID); }
Result ReadSaveDataFileSystemExtraData(void* buf, size_t len, u64 saveID) { return ::fsReadSaveDataFileSystemExtraData(buf, len, saveID); }
Result WriteSaveDataFileSystemExtraData(const void* buf, size_t len, FsSaveDataSpaceId save_data_space_id, u64 saveID) { return ::fsWriteSaveDataFileSystemExtraData(buf, len, save_data_space_id, saveID); }
Result OpenSaveDataInfoReader(FsSaveDataInfoReader* out, FsSaveDataSpaceId save_data_space_id) { return ::fsOpenSaveDataInfoReader(out, save_data_space_id); }
Result OpenSaveDataInfoReaderWithFilter(FsSaveDataInfoReader* out, FsSaveDataSpaceId save_data_space_id, const FsSaveDataFilter *save_data_filter) { return ::fsOpenSaveDataInfoReaderWithFilter(out, save_data_space_id, save_data_filter); }
Result OpenImageDirectoryFileSystem(FsFileSystem* out, FsImageDirectoryId image_directory_id) { return ::fsOpenImageDirectoryFileSystem(out, image_directory_id); }
Result OpenContentStorageFileSystem(FsFileSystem* out, FsContentStorageId content_storage_id) { return ::fsOpenContentStorageFileSystem(out, content_storage_id); }
Result OpenCustomStorageFileSystem(FsFileSystem* out, FsCustomStorageId custom_storage_id) { return ::fsOpenCustomStorageFileSystem(out, custom_storage_id); }
Result OpenDataStorageByCurrentProcess(FsStorage* out) { return ::fsOpenDataStorageByCurrentProcess(out); }
Result OpenDataStorageByProgramId(FsStorage *out, u64 program_id) { return ::fsOpenDataStorageByProgramId(out, program_id); }
Result OpenDataStorageByDataId(FsStorage* out, u64 dataId, NcmStorageId storageId) { return ::fsOpenDataStorageByDataId(out, dataId, storageId); }
Result OpenPatchDataStorageByCurrentProcess(FsStorage* out) { return ::fsOpenPatchDataStorageByCurrentProcess(out); }
Result OpenDataStorageByCurrentProcessForBatchRead(FsStorageForBatchRead* out) { return ::fsOpenDataStorageByCurrentProcessForBatchRead(out); }
Result OpenDataStorageByProgramIdForBatchRead(FsStorageForBatchRead* out, u64 id) { return ::fsOpenDataStorageByProgramIdForBatchRead(out, id); }
Result OpenDataStorageWithProgramIndexForBatchRead(FsStorageForBatchRead* out, u8 program_index) { return ::fsOpenDataStorageWithProgramIndexForBatchRead(out, program_index); }
Result OpenDataStorageByPathForBatchRead(FsStorageForBatchRead* out, const char* contentPath, FsContentAttributes attributes, FsFileSystemType fsType) { return ::fsOpenDataStorageByPathForBatchRead(out, contentPath, attributes, fsType); }
Result OpenDeviceOperator(FsDeviceOperator* out) { return ::fsOpenDeviceOperator(out); }
Result OpenSdCardDetectionEventNotifier(FsEventNotifier* out) { return ::fsOpenSdCardDetectionEventNotifier(out); }
Result IsSignedSystemPartitionOnSdCardValid(bool *out) { return ::fsIsSignedSystemPartitionOnSdCardValid(out); }
Result GetProgramId(u64* out, const char *path, FsContentAttributes attr) { return ::fsGetProgramId(out, path, attr); }
Result GetRightsIdByPath(const char* path, FsRightsId* out_rights_id) { return ::fsGetRightsIdByPath(path, out_rights_id); }
Result GetRightsIdAndKeyGenerationByPath(const char* path, FsContentAttributes attr, u8* out_key_generation, FsRightsId* out_rights_id) { return ::fsGetRightsIdAndKeyGenerationByPath(path, attr, out_key_generation, out_rights_id); }
Result GetContentStorageInfoIndex(s32 *out) { return ::fsGetContentStorageInfoIndex(out); }
Result DisableAutoSaveDataCreation(void) { return ::fsDisableAutoSaveDataCreation(); }
Result SetGlobalAccessLogMode(u32 mode) { return ::fsSetGlobalAccessLogMode(mode); }
Result GetGlobalAccessLogMode(u32* out_mode) { return ::fsGetGlobalAccessLogMode(out_mode); }
Result OutputAccessLogToSdCard(const char *log, size_t size) { return ::fsOutputAccessLogToSdCard(log, size); }
Result GetAndClearErrorInfo(FsFileSystemProxyErrorInfo *out) { return ::fsGetAndClearErrorInfo(out); }
Result GetAndClearMemoryReportInfo(FsMemoryReportInfo* out) { return ::fsGetAndClearMemoryReportInfo(out); }
Result GetProgramIndexForAccessLog(u32 *out_program_index, u32 *out_program_count) { return ::fsGetProgramIndexForAccessLog(out_program_index, out_program_count); }
Result Create_TemporaryStorage(u64 application_id, u64 owner_id, s64 size, u32 flags) { return ::fsCreate_TemporaryStorage(application_id, owner_id, size, flags); }
Result Create_SystemSaveDataWithOwner(FsSaveDataSpaceId save_data_space_id, u64 system_save_data_id, AccountUid uid, u64 owner_id, s64 size, s64 journal_size, u32 flags) { return ::fsCreate_SystemSaveDataWithOwner(save_data_space_id, system_save_data_id, uid, owner_id, size, journal_size, flags); }
Result Create_SystemSaveData(FsSaveDataSpaceId save_data_space_id, u64 system_save_data_id, s64 size, s64 journal_size, u32 flags) { return ::fsCreate_SystemSaveData(save_data_space_id, system_save_data_id, size, journal_size, flags); }
Result Open_SaveData(FsFileSystem* out, u64 application_id, AccountUid uid) { return ::fsOpen_SaveData(out, application_id, uid); }
Result Open_SaveDataReadOnly(FsFileSystem* out, u64 application_id, AccountUid uid) { return ::fsOpen_SaveDataReadOnly(out, application_id, uid); }
Result Open_BcatSaveData(FsFileSystem* out, u64 application_id) { return ::fsOpen_BcatSaveData(out, application_id); }
Result Open_DeviceSaveData(FsFileSystem* out, u64 application_id) { return ::fsOpen_DeviceSaveData(out, application_id); }
Result Open_TemporaryStorage(FsFileSystem* out) { return ::fsOpen_TemporaryStorage(out); }
Result Open_CacheStorage(FsFileSystem* out, u64 application_id, u16 save_data_index) { return ::fsOpen_CacheStorage(out, application_id, save_data_index); }
Result Open_SystemSaveData(FsFileSystem* out, FsSaveDataSpaceId save_data_space_id, u64 system_save_data_id, AccountUid uid) { return ::fsOpen_SystemSaveData(out, save_data_space_id, system_save_data_id, uid); }
Result Open_SystemBcatSaveData(FsFileSystem* out, u64 system_save_data_id) { return ::fsOpen_SystemBcatSaveData(out, system_save_data_id); }
Result FsCreateFile(FsFileSystem* fs, const char* path, s64 size, u32 option) { return ::fsFsCreateFile(fs, path, size, option); }
Result FsDeleteFile(FsFileSystem* fs, const char* path) { return ::fsFsDeleteFile(fs, path); }
Result FsCreateDirectory(FsFileSystem* fs, const char* path) { return ::fsFsCreateDirectory(fs, path); }
Result FsDeleteDirectory(FsFileSystem* fs, const char* path) { return ::fsFsDeleteDirectory(fs, path); }
Result FsDeleteDirectoryRecursively(FsFileSystem* fs, const char* path) { return ::fsFsDeleteDirectoryRecursively(fs, path); }
Result FsRenameFile(FsFileSystem* fs, const char* cur_path, const char* new_path) { return ::fsFsRenameFile(fs, cur_path, new_path); }
Result FsRenameDirectory(FsFileSystem* fs, const char* cur_path, const char* new_path) { return ::fsFsRenameDirectory(fs, cur_path, new_path); }
Result FsGetEntryType(FsFileSystem* fs, const char* path, FsDirEntryType* out) { return ::fsFsGetEntryType(fs, path, out); }
Result FsOpenFile(FsFileSystem* fs, const char* path, u32 mode, FsFile* out) { return ::fsFsOpenFile(fs, path, mode, out); }
Result FsOpenDirectory(FsFileSystem* fs, const char* path, u32 mode, FsDir* out) { return ::fsFsOpenDirectory(fs, path, mode, out); }
Result FsCommit(FsFileSystem* fs) { return ::fsFsCommit(fs); }
Result FsGetFreeSpace(FsFileSystem* fs, const char* path, s64* out) { return ::fsFsGetFreeSpace(fs, path, out); }
Result FsGetTotalSpace(FsFileSystem* fs, const char* path, s64* out) { return ::fsFsGetTotalSpace(fs, path, out); }
Result FsGetFileTimeStampRaw(FsFileSystem* fs, const char* path, FsTimeStampRaw *out) { return ::fsFsGetFileTimeStampRaw(fs, path, out); }
Result FsCleanDirectoryRecursively(FsFileSystem* fs, const char* path) { return ::fsFsCleanDirectoryRecursively(fs, path); }
Result FsQueryEntry(FsFileSystem* fs, void *out, size_t out_size, const void *in, size_t in_size, const char* path, FsFileSystemQueryId query_id) { return ::fsFsQueryEntry(fs, out, out_size, in, in_size, path, query_id); }
Result FsGetFileSystemAttribute(FsFileSystem* fs, FsFileSystemAttribute *out) { return ::fsFsGetFileSystemAttribute(fs, out); }
void FsClose(FsFileSystem* fs) { ::fsFsClose(fs); }
Result FsSetConcatenationFileAttribute(FsFileSystem* fs, const char *path) { return ::fsFsSetConcatenationFileAttribute(fs, path); }
Result FsIsValidSignedSystemPartitionOnSdCard(FsFileSystem* fs, bool *out) { return ::fsFsIsValidSignedSystemPartitionOnSdCard(fs, out); }
Result FileRead(FsFile* f, s64 off, void* buf, u64 read_size, u32 option, u64* bytes_read) { return ::fsFileRead(f, off, buf, read_size, option, bytes_read); }
Result FileWrite(FsFile* f, s64 off, const void* buf, u64 write_size, u32 option) { return ::fsFileWrite(f, off, buf, write_size, option); }
Result FileFlush(FsFile* f) { return ::fsFileFlush(f); }
Result FileSetSize(FsFile* f, s64 sz) { return ::fsFileSetSize(f, sz); }
Result FileGetSize(FsFile* f, s64* out) { return ::fsFileGetSize(f, out); }
Result FileOperateRange(FsFile* f, FsOperationId op_id, s64 off, s64 len, FsRangeInfo* out) { return ::fsFileOperateRange(f, op_id, off, len, out); }
void FileClose(FsFile* f) { ::fsFileClose(f); }
Result DirRead(FsDir* d, s64* total_entries, size_t max_entries, FsDirectoryEntry *buf) { return ::fsDirRead(d, total_entries, max_entries, buf); }
Result DirGetEntryCount(FsDir* d, s64* count) { return ::fsDirGetEntryCount(d, count); }
void DirClose(FsDir* d) { ::fsDirClose(d); }
Result StorageRead(FsStorage* s, s64 off, void* buf, u64 read_size) { return ::fsStorageRead(s, off, buf, read_size); }
Result StorageWrite(FsStorage* s, s64 off, const void* buf, u64 write_size) { return ::fsStorageWrite(s, off, buf, write_size); }
Result StorageFlush(FsStorage* s) { return ::fsStorageFlush(s); }
Result StorageSetSize(FsStorage* s, s64 sz) { return ::fsStorageSetSize(s, sz); }
Result StorageGetSize(FsStorage* s, s64* out) { return ::fsStorageGetSize(s, out); }
Result StorageOperateRange(FsStorage* s, FsOperationId op_id, s64 off, s64 len, FsRangeInfo* out) { return ::fsStorageOperateRange(s, op_id, off, len, out); }
void StorageClose(FsStorage* s) { ::fsStorageClose(s); }
Result StorageForBatchReadRead(FsStorageForBatchRead* s, s64 off, void* buf, u64 read_size) { return ::fsStorageForBatchReadRead(s, off, buf, read_size); }
Result StorageForBatchReadWrite(FsStorageForBatchRead* s, s64 off, const void* buf, u64 write_size) { return ::fsStorageForBatchReadWrite(s, off, buf, write_size); }
Result StorageForBatchReadFlush(FsStorageForBatchRead* s) { return ::fsStorageForBatchReadFlush(s); }
Result StorageForBatchReadSetSize(FsStorageForBatchRead* s, s64 sz) { return ::fsStorageForBatchReadSetSize(s, sz); }
Result StorageForBatchReadGetSize(FsStorageForBatchRead* s, s64* out) { return ::fsStorageForBatchReadGetSize(s, out); }
Result StorageForBatchReadOperateRange(FsStorageForBatchRead* s, FsOperationId op_id, s64 off, s64 len, FsRangeInfo* out) { return ::fsStorageForBatchReadOperateRange(s, op_id, off, len, out); }
Result StorageForBatchReadBatchRead(FsStorageForBatchRead* s, void* out0, void* out1, void* out2, void* out3, void* out4, void* out5, void* out6, const void* in) { return ::fsStorageForBatchReadBatchRead(s, out0, out1, out2, out3, out4, out5, out6, in); }
void StorageForBatchReadClose(FsStorageForBatchRead* s) { ::fsStorageForBatchReadClose(s); }
Result SaveDataInfoReaderRead(FsSaveDataInfoReader *s, FsSaveDataInfo* buf, size_t max_entries, s64* total_entries) { return ::fsSaveDataInfoReaderRead(s, buf, max_entries, total_entries); }
void SaveDataInfoReaderClose(FsSaveDataInfoReader *s) { ::fsSaveDataInfoReaderClose(s); }
Result EventNotifierGetEventHandle(FsEventNotifier* e, Event* out, bool autoclear) { return ::fsEventNotifierGetEventHandle(e, out, autoclear); }
void EventNotifierClose(FsEventNotifier* e) { ::fsEventNotifierClose(e); }
Result DeviceOperatorIsSdCardInserted(FsDeviceOperator* d, bool* out) { return ::fsDeviceOperatorIsSdCardInserted(d, out); }
Result DeviceOperatorGetSdCardSpeedMode(FsDeviceOperator* d, s64* out) { return ::fsDeviceOperatorGetSdCardSpeedMode(d, out); }
Result DeviceOperatorGetSdCardCid(FsDeviceOperator* d, void* dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetSdCardCid(d, dst, dst_size, size); }
Result DeviceOperatorGetSdCardUserAreaSize(FsDeviceOperator* d, s64* out) { return ::fsDeviceOperatorGetSdCardUserAreaSize(d, out); }
Result DeviceOperatorGetSdCardProtectedAreaSize(FsDeviceOperator* d, s64* out) { return ::fsDeviceOperatorGetSdCardProtectedAreaSize(d, out); }
Result DeviceOperatorGetAndClearSdCardErrorInfo(FsDeviceOperator* d, FsStorageErrorInfo* out, s64 *out_log_size, void *dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetAndClearSdCardErrorInfo(d, out, out_log_size, dst, dst_size, size); }
Result DeviceOperatorGetMmcCid(FsDeviceOperator* d, void* dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetMmcCid(d, dst, dst_size, size); }
Result DeviceOperatorGetMmcSpeedMode(FsDeviceOperator* d, s64* out) { return ::fsDeviceOperatorGetMmcSpeedMode(d, out); }
Result DeviceOperatorGetMmcPatrolCount(FsDeviceOperator* d, u32* out) { return ::fsDeviceOperatorGetMmcPatrolCount(d, out); }
Result DeviceOperatorGetAndClearMmcErrorInfo(FsDeviceOperator* d, FsStorageErrorInfo* out, s64 *out_log_size, void *dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetAndClearMmcErrorInfo(d, out, out_log_size, dst, dst_size, size); }
Result DeviceOperatorGetMmcExtendedCsd(FsDeviceOperator* d, void* dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetMmcExtendedCsd(d, dst, dst_size, size); }
Result DeviceOperatorIsGameCardInserted(FsDeviceOperator* d, bool* out) { return ::fsDeviceOperatorIsGameCardInserted(d, out); }
Result DeviceOperatorGetGameCardHandle(FsDeviceOperator* d, FsGameCardHandle* out) { return ::fsDeviceOperatorGetGameCardHandle(d, out); }
Result DeviceOperatorGetGameCardUpdatePartitionInfo(FsDeviceOperator* d, const FsGameCardHandle* handle, FsGameCardUpdatePartitionInfo* out) { return ::fsDeviceOperatorGetGameCardUpdatePartitionInfo(d, handle, out); }
Result DeviceOperatorGetGameCardAttribute(FsDeviceOperator* d, const FsGameCardHandle* handle, u8 *out) { return ::fsDeviceOperatorGetGameCardAttribute(d, handle, out); }
Result DeviceOperatorGetGameCardDeviceCertificate(FsDeviceOperator* d, const FsGameCardHandle* handle, void* dst, size_t dst_size, s64* out_size, s64 size) { return ::fsDeviceOperatorGetGameCardDeviceCertificate(d, handle, dst, dst_size, out_size, size); }
Result DeviceOperatorGetGameCardIdSet(FsDeviceOperator* d, void* dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetGameCardIdSet(d, dst, dst_size, size); }
Result DeviceOperatorGetGameCardErrorReportInfo(FsDeviceOperator* d, FsGameCardErrorReportInfo* out) { return ::fsDeviceOperatorGetGameCardErrorReportInfo(d, out); }
Result DeviceOperatorGetGameCardDeviceId(FsDeviceOperator* d, void* dst, size_t dst_size, s64 size) { return ::fsDeviceOperatorGetGameCardDeviceId(d, dst, dst_size, size); }
Result DeviceOperatorChallengeCardExistence(FsDeviceOperator* d, const FsGameCardHandle* handle, void* dst, size_t dst_size, void* seed, size_t seed_size, void* value, size_t value_size) { return ::fsDeviceOperatorChallengeCardExistence(d, handle, dst, dst_size, seed, seed_size, value, value_size); }
void DeviceOperatorClose(FsDeviceOperator* d) { ::fsDeviceOperatorClose(d); }

} // namespace fs

namespace fsdev {

FsDirectoryEntry* DirGetEntries(fsdev_dir_t *dir) { return ::fsdevDirGetEntries(dir); }
Result MountSdmc(void) { return ::fsdevMountSdmc(); }
Result MountSaveData(const char *name, u64 application_id, AccountUid uid) { return ::fsdevMountSaveData(name, application_id, uid); }
Result MountSaveDataReadOnly(const char *name, u64 application_id, AccountUid uid) { return ::fsdevMountSaveDataReadOnly(name, application_id, uid); }
Result MountBcatSaveData(const char *name, u64 application_id) { return ::fsdevMountBcatSaveData(name, application_id); }
Result MountDeviceSaveData(const char *name, u64 application_id) { return ::fsdevMountDeviceSaveData(name, application_id); }
Result MountTemporaryStorage(const char *name) { return ::fsdevMountTemporaryStorage(name); }
Result MountCacheStorage(const char *name, u64 application_id, u16 save_data_index) { return ::fsdevMountCacheStorage(name, application_id, save_data_index); }
Result MountSystemSaveData(const char *name, FsSaveDataSpaceId save_data_space_id, u64 system_save_data_id, AccountUid uid) { return ::fsdevMountSystemSaveData(name, save_data_space_id, system_save_data_id, uid); }
Result MountSystemBcatSaveData(const char *name, u64 system_save_data_id) { return ::fsdevMountSystemBcatSaveData(name, system_save_data_id); }
int MountDevice(const char *name, FsFileSystem fs) { return ::fsdevMountDevice(name, fs); }
int UnmountDevice(const char *name) { return ::fsdevUnmountDevice(name); }
Result CommitDevice(const char *name) { return ::fsdevCommitDevice(name); }
FsFileSystem* GetDeviceFileSystem(const char *name) { return ::fsdevGetDeviceFileSystem(name); }
int TranslatePath(const char *path, FsFileSystem** device, char *outpath) { return ::fsdevTranslatePath(path, device, outpath); }
Result SetConcatenationFileAttribute(const char *path) { return ::fsdevSetConcatenationFileAttribute(path); }
Result IsValidSignedSystemPartitionOnSdCard(const char *name, bool *out) { return ::fsdevIsValidSignedSystemPartitionOnSdCard(name, out); }
Result CreateFile(const char* path, size_t size, u32 flags) { return ::fsdevCreateFile(path, size, flags); }
Result DeleteDirectoryRecursively(const char *path) { return ::fsdevDeleteDirectoryRecursively(path); }
Result UnmountAll(void) { return ::fsdevUnmountAll(); }
Result GetLastResult(void) { return ::fsdevGetLastResult(); }

} // namespace fsdev

namespace fsldr {

Result Initialize(void) { return ::fsldrInitialize(); }
void Exit(void) { ::fsldrExit(); }
Service* GetServiceSession(void) { return ::fsldrGetServiceSession(); }
Result OpenCodeFileSystem(FsCodeInfo* out_code_info, u64 tid, NcmStorageId storage_id, const char *path, FsContentAttributes attr, FsFileSystem* out) { return ::fsldrOpenCodeFileSystem(out_code_info, tid, storage_id, path, attr, out); }
Result IsArchivedProgram(u64 pid, bool *out) { return ::fsldrIsArchivedProgram(pid, out); }

} // namespace fsldr

namespace fspr {

Result Initialize(void) { return ::fsprInitialize(); }
void Exit(void) { ::fsprExit(); }
Service* GetServiceSession(void) { return ::fsprGetServiceSession(); }
Result RegisterProgram(u64 pid, u64 tid, NcmStorageId sid, const void *fs_access_header, size_t fah_size, const void *fs_access_control, size_t fac_size, u8 fs_access_control_restriction_mode) { return ::fsprRegisterProgram(pid, tid, sid, fs_access_header, fah_size, fs_access_control, fac_size, fs_access_control_restriction_mode); }
Result UnregisterProgram(u64 pid) { return ::fsprUnregisterProgram(pid); }
Result SetCurrentProcess(void) { return ::fsprSetCurrentProcess(); }
Result SetEnabledProgramVerification(bool enabled) { return ::fsprSetEnabledProgramVerification(enabled); }

} // namespace fspr

namespace gpio {

Result Initialize(void) { return ::gpioInitialize(); }
void Exit(void) { ::gpioExit(); }
Service* GetServiceSession(void) { return ::gpioGetServiceSession(); }
Result OpenSession(GpioPadSession *out, GpioPadName name) { return ::gpioOpenSession(out, name); }
Result OpenSession2(GpioPadSession *out, u32 device_code, u32 access_mode) { return ::gpioOpenSession2(out, device_code, access_mode); }
Result IsWakeEventActive(bool *out, GpioPadName name) { return ::gpioIsWakeEventActive(out, name); }
Result IsWakeEventActive2(bool *out, u32 device_code) { return ::gpioIsWakeEventActive2(out, device_code); }
Result PadSetDirection(GpioPadSession *p, GpioDirection dir) { return ::gpioPadSetDirection(p, dir); }
Result PadGetDirection(GpioPadSession *p, GpioDirection *out) { return ::gpioPadGetDirection(p, out); }
Result PadSetInterruptMode(GpioPadSession *p, GpioInterruptMode mode) { return ::gpioPadSetInterruptMode(p, mode); }
Result PadGetInterruptMode(GpioPadSession *p, GpioInterruptMode *out) { return ::gpioPadGetInterruptMode(p, out); }
Result PadSetInterruptEnable(GpioPadSession *p, bool en) { return ::gpioPadSetInterruptEnable(p, en); }
Result PadGetInterruptEnable(GpioPadSession *p, bool *out) { return ::gpioPadGetInterruptEnable(p, out); }
Result PadGetInterruptStatus(GpioPadSession *p, GpioInterruptStatus *out) { return ::gpioPadGetInterruptStatus(p, out); }
Result PadClearInterruptStatus(GpioPadSession *p) { return ::gpioPadClearInterruptStatus(p); }
Result PadSetValue(GpioPadSession *p, GpioValue val) { return ::gpioPadSetValue(p, val); }
Result PadGetValue(GpioPadSession *p, GpioValue *out) { return ::gpioPadGetValue(p, out); }
Result PadBindInterrupt(GpioPadSession *p, Event *out) { return ::gpioPadBindInterrupt(p, out); }
Result PadUnbindInterrupt(GpioPadSession *p) { return ::gpioPadUnbindInterrupt(p); }
Result PadSetDebounceEnabled(GpioPadSession *p, bool en) { return ::gpioPadSetDebounceEnabled(p, en); }
Result PadGetDebounceEnabled(GpioPadSession *p, bool *out) { return ::gpioPadGetDebounceEnabled(p, out); }
Result PadSetDebounceTime(GpioPadSession *p, s32 ms) { return ::gpioPadSetDebounceTime(p, ms); }
Result PadGetDebounceTime(GpioPadSession *p, s32 *out) { return ::gpioPadGetDebounceTime(p, out); }
void PadClose(GpioPadSession *p) { ::gpioPadClose(p); }

} // namespace gpio

namespace grc {

Result TrimGameMovie(GrcGameMovieId *dst_movieid, const GrcGameMovieId *src_movieid, size_t tmem_size, const void* thumbnail, s32 start, s32 end) { return ::grcTrimGameMovie(dst_movieid, src_movieid, tmem_size, thumbnail, start, end); }
void CreateOffscreenRecordingParameter(GrcOffscreenRecordingParameter *param) { ::grcCreateOffscreenRecordingParameter(param); }
Result CreateMovieMaker(GrcMovieMaker *m, size_t size) { return ::grcCreateMovieMaker(m, size); }
void MovieMakerClose(GrcMovieMaker *m) { ::grcMovieMakerClose(m); }
NWindow* MovieMakerGetNWindow(GrcMovieMaker *m) { return ::grcMovieMakerGetNWindow(m); }
Result MovieMakerAbort(GrcMovieMaker *m) { return ::grcMovieMakerAbort(m); }
Result MovieMakerStart(GrcMovieMaker *m, const GrcOffscreenRecordingParameter *param) { return ::grcMovieMakerStart(m, param); }
Result MovieMakerFinish(GrcMovieMaker *m, s32 width, s32 height, const void* userdata, size_t userdata_size, const void* thumbnail, size_t thumbnail_size, CapsApplicationAlbumEntry *entry) { return ::grcMovieMakerFinish(m, width, height, userdata, userdata_size, thumbnail, thumbnail_size, entry); }
Result MovieMakerGetError(GrcMovieMaker *m) { return ::grcMovieMakerGetError(m); }
Result MovieMakerEncodeAudioSample(GrcMovieMaker *m, const void* buffer, size_t size) { return ::grcMovieMakerEncodeAudioSample(m, buffer, size); }

} // namespace grc

namespace grcd {

Result Initialize(void) { return ::grcdInitialize(); }
void Exit(void) { ::grcdExit(); }
Service* GetServiceSession(void) { return ::grcdGetServiceSession(); }
Result Begin(void) { return ::grcdBegin(); }
Result Transfer(GrcStream stream, void* buffer, size_t size, u32 *num_frames, u32 *data_size, u64 *start_timestamp) { return ::grcdTransfer(stream, buffer, size, num_frames, data_size, start_timestamp); }

} // namespace grcd

namespace hid {

void LaCreateControllerSupportArg(HidLaControllerSupportArg *arg) { ::hidLaCreateControllerSupportArg(arg); }
void LaCreateControllerFirmwareUpdateArg(HidLaControllerFirmwareUpdateArg *arg) { ::hidLaCreateControllerFirmwareUpdateArg(arg); }
void LaCreateControllerKeyRemappingArg(HidLaControllerKeyRemappingArg *arg) { ::hidLaCreateControllerKeyRemappingArg(arg); }
Result LaSetExplainText(HidLaControllerSupportArg *arg, const char *str, HidNpadIdType id) { return ::hidLaSetExplainText(arg, str, id); }
Result LaShowControllerSupport(HidLaControllerSupportResultInfo *result_info, const HidLaControllerSupportArg *arg) { return ::hidLaShowControllerSupport(result_info, arg); }
Result LaShowControllerStrapGuide(void) { return ::hidLaShowControllerStrapGuide(); }
Result LaShowControllerFirmwareUpdate(const HidLaControllerFirmwareUpdateArg *arg) { return ::hidLaShowControllerFirmwareUpdate(arg); }
Result LaShowControllerSupportForSystem(HidLaControllerSupportResultInfo *result_info, const HidLaControllerSupportArg *arg, bool flag) { return ::hidLaShowControllerSupportForSystem(result_info, arg, flag); }
Result LaShowControllerFirmwareUpdateForSystem(const HidLaControllerFirmwareUpdateArg *arg, HidLaControllerSupportCaller caller) { return ::hidLaShowControllerFirmwareUpdateForSystem(arg, caller); }
Result LaShowControllerKeyRemappingForSystem(const HidLaControllerKeyRemappingArg *arg, HidLaControllerSupportCaller caller) { return ::hidLaShowControllerKeyRemappingForSystem(arg, caller); }
Result Initialize(void) { return ::hidInitialize(); }
void Exit(void) { ::hidExit(); }
Service* GetServiceSession(void) { return ::hidGetServiceSession(); }
void* GetSharedmemAddr(void) { return ::hidGetSharedmemAddr(); }
void InitializeTouchScreen(void) { ::hidInitializeTouchScreen(); }
size_t GetTouchScreenStates(HidTouchScreenState *states, size_t count) { return ::hidGetTouchScreenStates(states, count); }
void InitializeMouse(void) { ::hidInitializeMouse(); }
size_t GetMouseStates(HidMouseState *states, size_t count) { return ::hidGetMouseStates(states, count); }
void InitializeKeyboard(void) { ::hidInitializeKeyboard(); }
size_t GetKeyboardStates(HidKeyboardState *states, size_t count) { return ::hidGetKeyboardStates(states, count); }
bool KeyboardStateGetKey(const HidKeyboardState *state, HidKeyboardKey key) { return ::hidKeyboardStateGetKey(state, key); }
size_t GetHomeButtonStates(HidHomeButtonState *states, size_t count) { return ::hidGetHomeButtonStates(states, count); }
size_t GetSleepButtonStates(HidSleepButtonState *states, size_t count) { return ::hidGetSleepButtonStates(states, count); }
size_t GetCaptureButtonStates(HidCaptureButtonState *states, size_t count) { return ::hidGetCaptureButtonStates(states, count); }
void InitializeNpad(void) { ::hidInitializeNpad(); }
u32 GetNpadStyleSet(HidNpadIdType id) { return ::hidGetNpadStyleSet(id); }
HidNpadJoyAssignmentMode GetNpadJoyAssignment(HidNpadIdType id) { return ::hidGetNpadJoyAssignment(id); }
Result GetNpadControllerColorSingle(HidNpadIdType id, HidNpadControllerColor *color) { return ::hidGetNpadControllerColorSingle(id, color); }
Result GetNpadControllerColorSplit(HidNpadIdType id, HidNpadControllerColor *color_left, HidNpadControllerColor *color_right) { return ::hidGetNpadControllerColorSplit(id, color_left, color_right); }
u32 GetNpadDeviceType(HidNpadIdType id) { return ::hidGetNpadDeviceType(id); }
void GetNpadSystemProperties(HidNpadIdType id, HidNpadSystemProperties *out) { ::hidGetNpadSystemProperties(id, out); }
void GetNpadSystemButtonProperties(HidNpadIdType id, HidNpadSystemButtonProperties *out) { ::hidGetNpadSystemButtonProperties(id, out); }
void GetNpadPowerInfoSingle(HidNpadIdType id, HidPowerInfo *info) { ::hidGetNpadPowerInfoSingle(id, info); }
void GetNpadPowerInfoSplit(HidNpadIdType id, HidPowerInfo *info_left, HidPowerInfo *info_right) { ::hidGetNpadPowerInfoSplit(id, info_left, info_right); }
u32 GetAppletFooterUiAttributesSet(HidNpadIdType id) { return ::hidGetAppletFooterUiAttributesSet(id); }
HidAppletFooterUiType GetAppletFooterUiTypes(HidNpadIdType id) { return ::hidGetAppletFooterUiTypes(id); }
HidNpadLagerType GetNpadLagerType(HidNpadIdType id) { return ::hidGetNpadLagerType(id); }
size_t GetNpadStatesFullKey(HidNpadIdType id, HidNpadFullKeyState *states, size_t count) { return ::hidGetNpadStatesFullKey(id, states, count); }
size_t GetNpadStatesHandheld(HidNpadIdType id, HidNpadHandheldState *states, size_t count) { return ::hidGetNpadStatesHandheld(id, states, count); }
size_t GetNpadStatesJoyDual(HidNpadIdType id, HidNpadJoyDualState *states, size_t count) { return ::hidGetNpadStatesJoyDual(id, states, count); }
size_t GetNpadStatesJoyLeft(HidNpadIdType id, HidNpadJoyLeftState *states, size_t count) { return ::hidGetNpadStatesJoyLeft(id, states, count); }
size_t GetNpadStatesJoyRight(HidNpadIdType id, HidNpadJoyRightState *states, size_t count) { return ::hidGetNpadStatesJoyRight(id, states, count); }
size_t GetNpadStatesGc(HidNpadIdType id, HidNpadGcState *states, size_t count) { return ::hidGetNpadStatesGc(id, states, count); }
size_t GetNpadStatesPalma(HidNpadIdType id, HidNpadPalmaState *states, size_t count) { return ::hidGetNpadStatesPalma(id, states, count); }
size_t GetNpadStatesLark(HidNpadIdType id, HidNpadLarkState *states, size_t count) { return ::hidGetNpadStatesLark(id, states, count); }
size_t GetNpadStatesHandheldLark(HidNpadIdType id, HidNpadHandheldLarkState *states, size_t count) { return ::hidGetNpadStatesHandheldLark(id, states, count); }
size_t GetNpadStatesLucia(HidNpadIdType id, HidNpadLuciaState *states, size_t count) { return ::hidGetNpadStatesLucia(id, states, count); }
size_t GetNpadStatesLager(HidNpadIdType id, HidNpadLagerState *states, size_t count) { return ::hidGetNpadStatesLager(id, states, count); }
size_t GetNpadStatesSystemExt(HidNpadIdType id, HidNpadSystemExtState *states, size_t count) { return ::hidGetNpadStatesSystemExt(id, states, count); }
size_t GetNpadStatesSystem(HidNpadIdType id, HidNpadSystemState *states, size_t count) { return ::hidGetNpadStatesSystem(id, states, count); }
size_t GetSixAxisSensorStates(HidSixAxisSensorHandle handle, HidSixAxisSensorState *states, size_t count) { return ::hidGetSixAxisSensorStates(handle, states, count); }
void InitializeGesture(void) { ::hidInitializeGesture(); }
size_t GetGestureStates(HidGestureState *states, size_t count) { return ::hidGetGestureStates(states, count); }
Result SendKeyboardLockKeyEvent(u32 events) { return ::hidSendKeyboardLockKeyEvent(events); }
Result GetSixAxisSensorHandles(HidSixAxisSensorHandle *handles, s32 total_handles, HidNpadIdType id, HidNpadStyleTag style) { return ::hidGetSixAxisSensorHandles(handles, total_handles, id, style); }
Result StartSixAxisSensor(HidSixAxisSensorHandle handle) { return ::hidStartSixAxisSensor(handle); }
Result StopSixAxisSensor(HidSixAxisSensorHandle handle) { return ::hidStopSixAxisSensor(handle); }
Result IsSixAxisSensorFusionEnabled(HidSixAxisSensorHandle handle, bool *out) { return ::hidIsSixAxisSensorFusionEnabled(handle, out); }
Result EnableSixAxisSensorFusion(HidSixAxisSensorHandle handle, bool flag) { return ::hidEnableSixAxisSensorFusion(handle, flag); }
Result SetSixAxisSensorFusionParameters(HidSixAxisSensorHandle handle, float unk0, float unk1) { return ::hidSetSixAxisSensorFusionParameters(handle, unk0, unk1); }
Result GetSixAxisSensorFusionParameters(HidSixAxisSensorHandle handle, float *unk0, float *unk1) { return ::hidGetSixAxisSensorFusionParameters(handle, unk0, unk1); }
Result ResetSixAxisSensorFusionParameters(HidSixAxisSensorHandle handle) { return ::hidResetSixAxisSensorFusionParameters(handle); }
Result SetGyroscopeZeroDriftMode(HidSixAxisSensorHandle handle, HidGyroscopeZeroDriftMode mode) { return ::hidSetGyroscopeZeroDriftMode(handle, mode); }
Result GetGyroscopeZeroDriftMode(HidSixAxisSensorHandle handle, HidGyroscopeZeroDriftMode *mode) { return ::hidGetGyroscopeZeroDriftMode(handle, mode); }
Result ResetGyroscopeZeroDriftMode(HidSixAxisSensorHandle handle) { return ::hidResetGyroscopeZeroDriftMode(handle); }
Result IsSixAxisSensorAtRest(HidSixAxisSensorHandle handle, bool *out) { return ::hidIsSixAxisSensorAtRest(handle, out); }
Result IsFirmwareUpdateAvailableForSixAxisSensor(HidSixAxisSensorHandle handle, bool *out) { return ::hidIsFirmwareUpdateAvailableForSixAxisSensor(handle, out); }
Result SetSupportedNpadStyleSet(u32 style_set) { return ::hidSetSupportedNpadStyleSet(style_set); }
Result GetSupportedNpadStyleSet(u32 *style_set) { return ::hidGetSupportedNpadStyleSet(style_set); }
Result SetSupportedNpadIdType(const HidNpadIdType *ids, size_t count) { return ::hidSetSupportedNpadIdType(ids, count); }
Result AcquireNpadStyleSetUpdateEventHandle(HidNpadIdType id, Event* out_event, bool autoclear) { return ::hidAcquireNpadStyleSetUpdateEventHandle(id, out_event, autoclear); }
Result DisconnectNpad(HidNpadIdType id) { return ::hidDisconnectNpad(id); }
Result GetPlayerLedPattern(HidNpadIdType id, u8 *out) { return ::hidGetPlayerLedPattern(id, out); }
Result SetNpadJoyHoldType(HidNpadJoyHoldType type) { return ::hidSetNpadJoyHoldType(type); }
Result GetNpadJoyHoldType(HidNpadJoyHoldType *type) { return ::hidGetNpadJoyHoldType(type); }
Result SetNpadJoyAssignmentModeSingleByDefault(HidNpadIdType id) { return ::hidSetNpadJoyAssignmentModeSingleByDefault(id); }
Result SetNpadJoyAssignmentModeSingle(HidNpadIdType id, HidNpadJoyDeviceType type) { return ::hidSetNpadJoyAssignmentModeSingle(id, type); }
Result SetNpadJoyAssignmentModeDual(HidNpadIdType id) { return ::hidSetNpadJoyAssignmentModeDual(id); }
Result MergeSingleJoyAsDualJoy(HidNpadIdType id0, HidNpadIdType id1) { return ::hidMergeSingleJoyAsDualJoy(id0, id1); }
Result StartLrAssignmentMode(void) { return ::hidStartLrAssignmentMode(); }
Result StopLrAssignmentMode(void) { return ::hidStopLrAssignmentMode(); }
Result SetNpadHandheldActivationMode(HidNpadHandheldActivationMode mode) { return ::hidSetNpadHandheldActivationMode(mode); }
Result GetNpadHandheldActivationMode(HidNpadHandheldActivationMode *out) { return ::hidGetNpadHandheldActivationMode(out); }
Result SwapNpadAssignment(HidNpadIdType id0, HidNpadIdType id1) { return ::hidSwapNpadAssignment(id0, id1); }
Result EnableUnintendedHomeButtonInputProtection(HidNpadIdType id, bool flag) { return ::hidEnableUnintendedHomeButtonInputProtection(id, flag); }
Result SetNpadJoyAssignmentModeSingleWithDestination(HidNpadIdType id, HidNpadJoyDeviceType type, bool *flag, HidNpadIdType *dest) { return ::hidSetNpadJoyAssignmentModeSingleWithDestination(id, type, flag, dest); }
Result SetNpadAnalogStickUseCenterClamp(bool flag) { return ::hidSetNpadAnalogStickUseCenterClamp(flag); }
Result SetNpadCaptureButtonAssignment(HidNpadStyleTag style, u64 buttons) { return ::hidSetNpadCaptureButtonAssignment(style, buttons); }
Result ClearNpadCaptureButtonAssignment(void) { return ::hidClearNpadCaptureButtonAssignment(); }
Result InitializeVibrationDevices(HidVibrationDeviceHandle *handles, s32 total_handles, HidNpadIdType id, HidNpadStyleTag style) { return ::hidInitializeVibrationDevices(handles, total_handles, id, style); }
Result GetVibrationDeviceInfo(HidVibrationDeviceHandle handle, HidVibrationDeviceInfo *out) { return ::hidGetVibrationDeviceInfo(handle, out); }
Result SendVibrationValue(HidVibrationDeviceHandle handle, const HidVibrationValue *value) { return ::hidSendVibrationValue(handle, value); }
Result GetActualVibrationValue(HidVibrationDeviceHandle handle, HidVibrationValue *out) { return ::hidGetActualVibrationValue(handle, out); }
Result PermitVibration(bool flag) { return ::hidPermitVibration(flag); }
Result IsVibrationPermitted(bool *flag) { return ::hidIsVibrationPermitted(flag); }
Result SendVibrationValues(const HidVibrationDeviceHandle *handles, const HidVibrationValue *values, s32 count) { return ::hidSendVibrationValues(handles, values, count); }
Result SendVibrationGcErmCommand(HidVibrationDeviceHandle handle, HidVibrationGcErmCommand cmd) { return ::hidSendVibrationGcErmCommand(handle, cmd); }
Result GetActualVibrationGcErmCommand(HidVibrationDeviceHandle handle, HidVibrationGcErmCommand *out) { return ::hidGetActualVibrationGcErmCommand(handle, out); }
Result BeginPermitVibrationSession(void) { return ::hidBeginPermitVibrationSession(); }
Result EndPermitVibrationSession(void) { return ::hidEndPermitVibrationSession(); }
Result IsVibrationDeviceMounted(HidVibrationDeviceHandle handle, bool *flag) { return ::hidIsVibrationDeviceMounted(handle, flag); }
Result StartSevenSixAxisSensor(void) { return ::hidStartSevenSixAxisSensor(); }
Result StopSevenSixAxisSensor(void) { return ::hidStopSevenSixAxisSensor(); }
Result InitializeSevenSixAxisSensor(void) { return ::hidInitializeSevenSixAxisSensor(); }
Result FinalizeSevenSixAxisSensor(void) { return ::hidFinalizeSevenSixAxisSensor(); }
Result SetSevenSixAxisSensorFusionStrength(float strength) { return ::hidSetSevenSixAxisSensorFusionStrength(strength); }
Result GetSevenSixAxisSensorFusionStrength(float *strength) { return ::hidGetSevenSixAxisSensorFusionStrength(strength); }
Result ResetSevenSixAxisSensorTimestamp(void) { return ::hidResetSevenSixAxisSensorTimestamp(); }
Result GetSevenSixAxisSensorStates(HidSevenSixAxisSensorState *states, size_t count, size_t *total_out) { return ::hidGetSevenSixAxisSensorStates(states, count, total_out); }
Result IsSevenSixAxisSensorAtRest(bool *out) { return ::hidIsSevenSixAxisSensorAtRest(out); }
Result GetSensorFusionError(float *out) { return ::hidGetSensorFusionError(out); }
Result GetGyroBias(UtilFloat3 *out) { return ::hidGetGyroBias(out); }
Result IsUsbFullKeyControllerEnabled(bool *out) { return ::hidIsUsbFullKeyControllerEnabled(out); }
Result EnableUsbFullKeyController(bool flag) { return ::hidEnableUsbFullKeyController(flag); }
Result IsUsbFullKeyControllerConnected(HidNpadIdType id, bool *out) { return ::hidIsUsbFullKeyControllerConnected(id, out); }
Result GetNpadInterfaceType(HidNpadIdType id, u8 *out) { return ::hidGetNpadInterfaceType(id, out); }
Result GetNpadOfHighestBatteryLevel(const HidNpadIdType *ids, size_t count, HidNpadIdType *out) { return ::hidGetNpadOfHighestBatteryLevel(ids, count, out); }
Result GetPalmaConnectionHandle(HidNpadIdType id, HidPalmaConnectionHandle *out) { return ::hidGetPalmaConnectionHandle(id, out); }
Result InitializePalma(HidPalmaConnectionHandle handle) { return ::hidInitializePalma(handle); }
Result AcquirePalmaOperationCompleteEvent(HidPalmaConnectionHandle handle, Event* out_event, bool autoclear) { return ::hidAcquirePalmaOperationCompleteEvent(handle, out_event, autoclear); }
Result GetPalmaOperationInfo(HidPalmaConnectionHandle handle, HidPalmaOperationInfo *out) { return ::hidGetPalmaOperationInfo(handle, out); }
Result PlayPalmaActivity(HidPalmaConnectionHandle handle, u16 val) { return ::hidPlayPalmaActivity(handle, val); }
Result SetPalmaFrModeType(HidPalmaConnectionHandle handle, HidPalmaFrModeType type) { return ::hidSetPalmaFrModeType(handle, type); }
Result ReadPalmaStep(HidPalmaConnectionHandle handle) { return ::hidReadPalmaStep(handle); }
Result EnablePalmaStep(HidPalmaConnectionHandle handle, bool flag) { return ::hidEnablePalmaStep(handle, flag); }
Result ResetPalmaStep(HidPalmaConnectionHandle handle) { return ::hidResetPalmaStep(handle); }
Result ReadPalmaApplicationSection(HidPalmaConnectionHandle handle, s32 inval0, u64 size) { return ::hidReadPalmaApplicationSection(handle, inval0, size); }
Result WritePalmaApplicationSection(HidPalmaConnectionHandle handle, s32 inval0, u64 size, const HidPalmaApplicationSectionAccessBuffer *buf) { return ::hidWritePalmaApplicationSection(handle, inval0, size, buf); }
Result ReadPalmaUniqueCode(HidPalmaConnectionHandle handle) { return ::hidReadPalmaUniqueCode(handle); }
Result SetPalmaUniqueCodeInvalid(HidPalmaConnectionHandle handle) { return ::hidSetPalmaUniqueCodeInvalid(handle); }
Result WritePalmaActivityEntry(HidPalmaConnectionHandle handle, u16 unk, const HidPalmaActivityEntry *entry) { return ::hidWritePalmaActivityEntry(handle, unk, entry); }
Result WritePalmaRgbLedPatternEntry(HidPalmaConnectionHandle handle, u16 unk, const void* buffer, size_t size) { return ::hidWritePalmaRgbLedPatternEntry(handle, unk, buffer, size); }
Result WritePalmaWaveEntry(HidPalmaConnectionHandle handle, HidPalmaWaveSet wave_set, u16 unk, const void* buffer, size_t tmem_size, size_t size) { return ::hidWritePalmaWaveEntry(handle, wave_set, unk, buffer, tmem_size, size); }
Result SetPalmaDataBaseIdentificationVersion(HidPalmaConnectionHandle handle, s32 version) { return ::hidSetPalmaDataBaseIdentificationVersion(handle, version); }
Result GetPalmaDataBaseIdentificationVersion(HidPalmaConnectionHandle handle) { return ::hidGetPalmaDataBaseIdentificationVersion(handle); }
Result SuspendPalmaFeature(HidPalmaConnectionHandle handle, u32 features) { return ::hidSuspendPalmaFeature(handle, features); }
Result ReadPalmaPlayLog(HidPalmaConnectionHandle handle, u16 unk) { return ::hidReadPalmaPlayLog(handle, unk); }
Result ResetPalmaPlayLog(HidPalmaConnectionHandle handle, u16 unk) { return ::hidResetPalmaPlayLog(handle, unk); }
Result SetIsPalmaAllConnectable(bool flag) { return ::hidSetIsPalmaAllConnectable(flag); }
Result SetIsPalmaPairedConnectable(bool flag) { return ::hidSetIsPalmaPairedConnectable(flag); }
Result PairPalma(HidPalmaConnectionHandle handle) { return ::hidPairPalma(handle); }
Result CancelWritePalmaWaveEntry(HidPalmaConnectionHandle handle) { return ::hidCancelWritePalmaWaveEntry(handle); }
Result EnablePalmaBoostMode(bool flag) { return ::hidEnablePalmaBoostMode(flag); }
Result GetPalmaBluetoothAddress(HidPalmaConnectionHandle handle, BtdrvAddress *out) { return ::hidGetPalmaBluetoothAddress(handle, out); }
Result SetDisallowedPalmaConnection(const BtdrvAddress *addrs, s32 count) { return ::hidSetDisallowedPalmaConnection(addrs, count); }
Result SetNpadCommunicationMode(HidNpadCommunicationMode mode) { return ::hidSetNpadCommunicationMode(mode); }
Result GetNpadCommunicationMode(HidNpadCommunicationMode *out) { return ::hidGetNpadCommunicationMode(out); }
Result SetTouchScreenConfiguration(const HidTouchScreenConfigurationForNx *config) { return ::hidSetTouchScreenConfiguration(config); }
Result IsFirmwareUpdateNeededForNotification(bool *out) { return ::hidIsFirmwareUpdateNeededForNotification(out); }

} // namespace hid

namespace hidbus {

Result GetServiceSession(Service* srv_out) { return ::hidbusGetServiceSession(srv_out); }
void* GetSharedmemAddr(void) { return ::hidbusGetSharedmemAddr(); }
Result GetBusHandle(HidbusBusHandle *handle, bool *flag, HidNpadIdType id, HidbusBusType bus_type) { return ::hidbusGetBusHandle(handle, flag, id, bus_type); }
Result Initialize(HidbusBusHandle handle) { return ::hidbusInitialize(handle); }
Result Finalize(HidbusBusHandle handle) { return ::hidbusFinalize(handle); }
Result EnableExternalDevice(HidbusBusHandle handle, bool flag, u32 device_id) { return ::hidbusEnableExternalDevice(handle, flag, device_id); }
Result SendAndReceive(HidbusBusHandle handle, const void* inbuf, size_t inbuf_size, void* outbuf, size_t outbuf_size, u64 *out_size) { return ::hidbusSendAndReceive(handle, inbuf, inbuf_size, outbuf, outbuf_size, out_size); }
Result EnableJoyPollingReceiveMode(HidbusBusHandle handle, const void* inbuf, size_t inbuf_size, void* workbuf, size_t workbuf_size, HidbusJoyPollingMode polling_mode) { return ::hidbusEnableJoyPollingReceiveMode(handle, inbuf, inbuf_size, workbuf, workbuf_size, polling_mode); }
Result DisableJoyPollingReceiveMode(HidbusBusHandle handle) { return ::hidbusDisableJoyPollingReceiveMode(handle); }
Result GetJoyPollingReceivedData(HidbusBusHandle handle, HidbusJoyPollingReceivedData *recv_data, s32 count) { return ::hidbusGetJoyPollingReceivedData(handle, recv_data, count); }

} // namespace hidbus

namespace hiddbg {

Result Initialize(void) { return ::hiddbgInitialize(); }
void Exit(void) { ::hiddbgExit(); }
Service* GetServiceSession(void) { return ::hiddbgGetServiceSession(); }
Result SetDebugPadAutoPilotState(const HiddbgDebugPadAutoPilotState *state) { return ::hiddbgSetDebugPadAutoPilotState(state); }
Result UnsetDebugPadAutoPilotState(void) { return ::hiddbgUnsetDebugPadAutoPilotState(); }
Result SetTouchScreenAutoPilotState(const HidTouchState *states, s32 count) { return ::hiddbgSetTouchScreenAutoPilotState(states, count); }
Result UnsetTouchScreenAutoPilotState(void) { return ::hiddbgUnsetTouchScreenAutoPilotState(); }
Result SetMouseAutoPilotState(const HiddbgMouseAutoPilotState *state) { return ::hiddbgSetMouseAutoPilotState(state); }
Result UnsetMouseAutoPilotState(void) { return ::hiddbgUnsetMouseAutoPilotState(); }
Result SetKeyboardAutoPilotState(const HiddbgKeyboardAutoPilotState *state) { return ::hiddbgSetKeyboardAutoPilotState(state); }
Result UnsetKeyboardAutoPilotState(void) { return ::hiddbgUnsetKeyboardAutoPilotState(); }
Result DeactivateHomeButton(void) { return ::hiddbgDeactivateHomeButton(); }
Result SetSleepButtonAutoPilotState(const HiddbgSleepButtonAutoPilotState *state) { return ::hiddbgSetSleepButtonAutoPilotState(state); }
Result UnsetSleepButtonAutoPilotState(void) { return ::hiddbgUnsetSleepButtonAutoPilotState(); }
Result UpdateControllerColor(u32 colorBody, u32 colorButtons, HidsysUniquePadId unique_pad_id) { return ::hiddbgUpdateControllerColor(colorBody, colorButtons, unique_pad_id); }
Result UpdateDesignInfo(u32 colorBody, u32 colorButtons, u32 colorLeftGrip, u32 colorRightGrip, u8 inval, HidsysUniquePadId unique_pad_id) { return ::hiddbgUpdateDesignInfo(colorBody, colorButtons, colorLeftGrip, colorRightGrip, inval, unique_pad_id); }
Result AcquireOperationEventHandle(Event* out_event, bool autoclear, HidsysUniquePadId unique_pad_id) { return ::hiddbgAcquireOperationEventHandle(out_event, autoclear, unique_pad_id); }
Result ReadSerialFlash(u32 offset, void* buffer, size_t size, HidsysUniquePadId unique_pad_id) { return ::hiddbgReadSerialFlash(offset, buffer, size, unique_pad_id); }
Result WriteSerialFlash(u32 offset, void* buffer, size_t tmem_size, size_t size, HidsysUniquePadId unique_pad_id) { return ::hiddbgWriteSerialFlash(offset, buffer, tmem_size, size, unique_pad_id); }
Result GetOperationResult(HidsysUniquePadId unique_pad_id) { return ::hiddbgGetOperationResult(unique_pad_id); }
Result GetUniquePadDeviceTypeSetInternal(HidsysUniquePadId unique_pad_id, u32 *out) { return ::hiddbgGetUniquePadDeviceTypeSetInternal(unique_pad_id, out); }
Result GetAbstractedPadHandles(HiddbgAbstractedPadHandle *handles, s32 count, s32 *total_out) { return ::hiddbgGetAbstractedPadHandles(handles, count, total_out); }
Result GetAbstractedPadState(HiddbgAbstractedPadHandle handle, HiddbgAbstractedPadState *state) { return ::hiddbgGetAbstractedPadState(handle, state); }
Result GetAbstractedPadsState(HiddbgAbstractedPadHandle *handles, HiddbgAbstractedPadState *states, s32 count, s32 *total_out) { return ::hiddbgGetAbstractedPadsState(handles, states, count, total_out); }
Result SetAutoPilotVirtualPadState(s8 AbstractedVirtualPadId, const HiddbgAbstractedPadState *state) { return ::hiddbgSetAutoPilotVirtualPadState(AbstractedVirtualPadId, state); }
Result UnsetAutoPilotVirtualPadState(s8 AbstractedVirtualPadId) { return ::hiddbgUnsetAutoPilotVirtualPadState(AbstractedVirtualPadId); }
Result UnsetAllAutoPilotVirtualPadState(void) { return ::hiddbgUnsetAllAutoPilotVirtualPadState(); }
Result AttachHdlsWorkBuffer(HiddbgHdlsSessionId *session_id, void *buffer, size_t size) { return ::hiddbgAttachHdlsWorkBuffer(session_id, buffer, size); }
Result ReleaseHdlsWorkBuffer(HiddbgHdlsSessionId session_id) { return ::hiddbgReleaseHdlsWorkBuffer(session_id); }
Result IsHdlsVirtualDeviceAttached(HiddbgHdlsSessionId session_id, HiddbgHdlsHandle handle, bool *out) { return ::hiddbgIsHdlsVirtualDeviceAttached(session_id, handle, out); }
Result DumpHdlsNpadAssignmentState(HiddbgHdlsSessionId session_id, HiddbgHdlsNpadAssignment *state) { return ::hiddbgDumpHdlsNpadAssignmentState(session_id, state); }
Result DumpHdlsStates(HiddbgHdlsSessionId session_id, HiddbgHdlsStateList *state) { return ::hiddbgDumpHdlsStates(session_id, state); }
Result ApplyHdlsNpadAssignmentState(HiddbgHdlsSessionId session_id, const HiddbgHdlsNpadAssignment *state, bool flag) { return ::hiddbgApplyHdlsNpadAssignmentState(session_id, state, flag); }
Result ApplyHdlsStateList(HiddbgHdlsSessionId session_id, const HiddbgHdlsStateList *state) { return ::hiddbgApplyHdlsStateList(session_id, state); }
Result AttachHdlsVirtualDevice(HiddbgHdlsHandle *handle, const HiddbgHdlsDeviceInfo *info) { return ::hiddbgAttachHdlsVirtualDevice(handle, info); }
Result DetachHdlsVirtualDevice(HiddbgHdlsHandle handle) { return ::hiddbgDetachHdlsVirtualDevice(handle); }
Result SetHdlsState(HiddbgHdlsHandle handle, const HiddbgHdlsState *state) { return ::hiddbgSetHdlsState(handle, state); }

} // namespace hiddbg

namespace hidsys {

Result Initialize(void) { return ::hidsysInitialize(); }
void Exit(void) { ::hidsysExit(); }
Service* GetServiceSession(void) { return ::hidsysGetServiceSession(); }
Result SendKeyboardLockKeyEvent(u32 events) { return ::hidsysSendKeyboardLockKeyEvent(events); }
Result AcquireHomeButtonEventHandle(Event* out_event, bool autoclear) { return ::hidsysAcquireHomeButtonEventHandle(out_event, autoclear); }
Result ActivateHomeButton(void) { return ::hidsysActivateHomeButton(); }
Result AcquireSleepButtonEventHandle(Event* out_event, bool autoclear) { return ::hidsysAcquireSleepButtonEventHandle(out_event, autoclear); }
Result ActivateSleepButton(void) { return ::hidsysActivateSleepButton(); }
Result AcquireCaptureButtonEventHandle(Event* out_event, bool autoclear) { return ::hidsysAcquireCaptureButtonEventHandle(out_event, autoclear); }
Result ActivateCaptureButton(void) { return ::hidsysActivateCaptureButton(); }
Result ApplyNpadSystemCommonPolicy(void) { return ::hidsysApplyNpadSystemCommonPolicy(); }
Result GetLastActiveNpad(u32 *out) { return ::hidsysGetLastActiveNpad(out); }
Result GetSupportedNpadStyleSetOfCallerApplet(u32 *out) { return ::hidsysGetSupportedNpadStyleSetOfCallerApplet(out); }
Result GetNpadInterfaceType(HidNpadIdType id, u8 *out) { return ::hidsysGetNpadInterfaceType(id, out); }
Result GetNpadLeftRightInterfaceType(HidNpadIdType id, u8 *out0, u8 *out1) { return ::hidsysGetNpadLeftRightInterfaceType(id, out0, out1); }
Result HasBattery(HidNpadIdType id, bool *out) { return ::hidsysHasBattery(id, out); }
Result HasLeftRightBattery(HidNpadIdType id, bool *out0, bool *out1) { return ::hidsysHasLeftRightBattery(id, out0, out1); }
Result GetUniquePadsFromNpad(HidNpadIdType id, HidsysUniquePadId *unique_pad_ids, s32 count, s32 *total_out) { return ::hidsysGetUniquePadsFromNpad(id, unique_pad_ids, count, total_out); }
Result SetAppletResourceUserId(void) { return ::hidsysSetAppletResourceUserId(); }
Result EnableAppletToGetInput(bool enable) { return ::hidsysEnableAppletToGetInput(enable); }
Result EnableHandheldHids(void) { return ::hidsysEnableHandheldHids(); }
Result DisableHandheldHids(void) { return ::hidsysDisableHandheldHids(); }
Result SetJoyConRailEnabled(bool enable) { return ::hidsysSetJoyConRailEnabled(enable); }
Result IsJoyConRailEnabled(bool *out) { return ::hidsysIsJoyConRailEnabled(out); }
Result IsHandheldHidsEnabled(bool *out) { return ::hidsysIsHandheldHidsEnabled(out); }
Result IsJoyConAttachedOnAllRail(bool *out) { return ::hidsysIsJoyConAttachedOnAllRail(out); }
Result IsInvertedControllerConnectedOnRail(bool *out) { return ::hidsysIsInvertedControllerConnectedOnRail(out); }
Result AcquireUniquePadConnectionEventHandle(Event *out_event) { return ::hidsysAcquireUniquePadConnectionEventHandle(out_event); }
Result GetUniquePadIds(HidsysUniquePadId *unique_pad_ids, s32 count, s32 *total_out) { return ::hidsysGetUniquePadIds(unique_pad_ids, count, total_out); }
Result AcquireJoyDetachOnBluetoothOffEventHandle(Event *out_event, bool autoclear) { return ::hidsysAcquireJoyDetachOnBluetoothOffEventHandle(out_event, autoclear); }
Result GetUniquePadBluetoothAddress(HidsysUniquePadId unique_pad_id, BtdrvAddress *address) { return ::hidsysGetUniquePadBluetoothAddress(unique_pad_id, address); }
Result DisconnectUniquePad(HidsysUniquePadId unique_pad_id) { return ::hidsysDisconnectUniquePad(unique_pad_id); }
Result GetUniquePadType(HidsysUniquePadId unique_pad_id, HidsysUniquePadType *pad_type) { return ::hidsysGetUniquePadType(unique_pad_id, pad_type); }
Result GetUniquePadInterface(HidsysUniquePadId unique_pad_id, HidNpadInterfaceType *interface) { return ::hidsysGetUniquePadInterface(unique_pad_id, interface); }
Result GetUniquePadSerialNumber(HidsysUniquePadId unique_pad_id, HidsysUniquePadSerialNumber *serial) { return ::hidsysGetUniquePadSerialNumber(unique_pad_id, serial); }
Result GetUniquePadControllerNumber(HidsysUniquePadId unique_pad_id, u64 *number) { return ::hidsysGetUniquePadControllerNumber(unique_pad_id, number); }
Result SetNotificationLedPattern(const HidsysNotificationLedPattern *pattern, HidsysUniquePadId unique_pad_id) { return ::hidsysSetNotificationLedPattern(pattern, unique_pad_id); }
Result SetNotificationLedPatternWithTimeout(const HidsysNotificationLedPattern *pattern, HidsysUniquePadId unique_pad_id, u64 timeout) { return ::hidsysSetNotificationLedPatternWithTimeout(pattern, unique_pad_id, timeout); }
Result IsUsbFullKeyControllerEnabled(bool *out) { return ::hidsysIsUsbFullKeyControllerEnabled(out); }
Result EnableUsbFullKeyController(bool flag) { return ::hidsysEnableUsbFullKeyController(flag); }
Result IsUsbConnected(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysIsUsbConnected(unique_pad_id, out); }
Result GetTouchScreenDefaultConfiguration(HidTouchScreenConfigurationForNx *touch_screen_configuration) { return ::hidsysGetTouchScreenDefaultConfiguration(touch_screen_configuration); }
Result IsFirmwareUpdateNeededForNotification(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysIsFirmwareUpdateNeededForNotification(unique_pad_id, out); }
Result LegacyIsButtonConfigSupported(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysLegacyIsButtonConfigSupported(unique_pad_id, out); }
Result IsButtonConfigSupported(BtdrvAddress addr, bool *out) { return ::hidsysIsButtonConfigSupported(addr, out); }
Result IsButtonConfigEmbeddedSupported(bool *out) { return ::hidsysIsButtonConfigEmbeddedSupported(out); }
Result LegacyDeleteButtonConfig(HidsysUniquePadId unique_pad_id) { return ::hidsysLegacyDeleteButtonConfig(unique_pad_id); }
Result DeleteButtonConfig(BtdrvAddress addr) { return ::hidsysDeleteButtonConfig(addr); }
Result DeleteButtonConfigEmbedded(void) { return ::hidsysDeleteButtonConfigEmbedded(); }
Result LegacySetButtonConfigEnabled(HidsysUniquePadId unique_pad_id, bool flag) { return ::hidsysLegacySetButtonConfigEnabled(unique_pad_id, flag); }
Result SetButtonConfigEnabled(BtdrvAddress addr, bool flag) { return ::hidsysSetButtonConfigEnabled(addr, flag); }
Result SetButtonConfigEmbeddedEnabled(bool flag) { return ::hidsysSetButtonConfigEmbeddedEnabled(flag); }
Result LegacyIsButtonConfigEnabled(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysLegacyIsButtonConfigEnabled(unique_pad_id, out); }
Result IsButtonConfigEnabled(BtdrvAddress addr, bool *out) { return ::hidsysIsButtonConfigEnabled(addr, out); }
Result IsButtonConfigEmbeddedEnabled(bool *out) { return ::hidsysIsButtonConfigEmbeddedEnabled(out); }
Result LegacySetButtonConfigEmbedded(HidsysUniquePadId unique_pad_id, const HidsysButtonConfigEmbedded *config) { return ::hidsysLegacySetButtonConfigEmbedded(unique_pad_id, config); }
Result SetButtonConfigEmbedded(const HidsysButtonConfigEmbedded *config) { return ::hidsysSetButtonConfigEmbedded(config); }
Result LegacySetButtonConfigFull(HidsysUniquePadId unique_pad_id, const HidsysButtonConfigFull *config) { return ::hidsysLegacySetButtonConfigFull(unique_pad_id, config); }
Result SetButtonConfigFull(BtdrvAddress addr, const HidsysButtonConfigFull *config) { return ::hidsysSetButtonConfigFull(addr, config); }
Result LegacySetButtonConfigLeft(HidsysUniquePadId unique_pad_id, const HidsysButtonConfigLeft *config) { return ::hidsysLegacySetButtonConfigLeft(unique_pad_id, config); }
Result SetButtonConfigLeft(BtdrvAddress addr, const HidsysButtonConfigLeft *config) { return ::hidsysSetButtonConfigLeft(addr, config); }
Result LegacySetButtonConfigRight(HidsysUniquePadId unique_pad_id, const HidsysButtonConfigRight *config) { return ::hidsysLegacySetButtonConfigRight(unique_pad_id, config); }
Result SetButtonConfigRight(BtdrvAddress addr, const HidsysButtonConfigRight *config) { return ::hidsysSetButtonConfigRight(addr, config); }
Result LegacyGetButtonConfigEmbedded(HidsysUniquePadId unique_pad_id, HidsysButtonConfigEmbedded *config) { return ::hidsysLegacyGetButtonConfigEmbedded(unique_pad_id, config); }
Result GetButtonConfigEmbedded(HidsysButtonConfigEmbedded *config) { return ::hidsysGetButtonConfigEmbedded(config); }
Result LegacyGetButtonConfigFull(HidsysUniquePadId unique_pad_id, HidsysButtonConfigFull *config) { return ::hidsysLegacyGetButtonConfigFull(unique_pad_id, config); }
Result GetButtonConfigFull(BtdrvAddress addr, HidsysButtonConfigFull *config) { return ::hidsysGetButtonConfigFull(addr, config); }
Result LegacyGetButtonConfigLeft(HidsysUniquePadId unique_pad_id, HidsysButtonConfigLeft *config) { return ::hidsysLegacyGetButtonConfigLeft(unique_pad_id, config); }
Result GetButtonConfigLeft(BtdrvAddress addr, HidsysButtonConfigLeft *config) { return ::hidsysGetButtonConfigLeft(addr, config); }
Result LegacyGetButtonConfigRight(HidsysUniquePadId unique_pad_id, HidsysButtonConfigRight *config) { return ::hidsysLegacyGetButtonConfigRight(unique_pad_id, config); }
Result GetButtonConfigRight(BtdrvAddress addr, HidsysButtonConfigRight *config) { return ::hidsysGetButtonConfigRight(addr, config); }
Result IsCustomButtonConfigSupported(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysIsCustomButtonConfigSupported(unique_pad_id, out); }
Result IsDefaultButtonConfigEmbedded(const HidcfgButtonConfigEmbedded *config, bool *out) { return ::hidsysIsDefaultButtonConfigEmbedded(config, out); }
Result IsDefaultButtonConfigFull(const HidcfgButtonConfigFull *config, bool *out) { return ::hidsysIsDefaultButtonConfigFull(config, out); }
Result IsDefaultButtonConfigLeft(const HidcfgButtonConfigLeft *config, bool *out) { return ::hidsysIsDefaultButtonConfigLeft(config, out); }
Result IsDefaultButtonConfigRight(const HidcfgButtonConfigRight *config, bool *out) { return ::hidsysIsDefaultButtonConfigRight(config, out); }
Result IsButtonConfigStorageEmbeddedEmpty(s32 index, bool *out) { return ::hidsysIsButtonConfigStorageEmbeddedEmpty(index, out); }
Result IsButtonConfigStorageFullEmpty(s32 index, bool *out) { return ::hidsysIsButtonConfigStorageFullEmpty(index, out); }
Result IsButtonConfigStorageLeftEmpty(s32 index, bool *out) { return ::hidsysIsButtonConfigStorageLeftEmpty(index, out); }
Result IsButtonConfigStorageRightEmpty(s32 index, bool *out) { return ::hidsysIsButtonConfigStorageRightEmpty(index, out); }
Result GetButtonConfigStorageEmbeddedDeprecated(s32 index, HidcfgButtonConfigEmbedded *config) { return ::hidsysGetButtonConfigStorageEmbeddedDeprecated(index, config); }
Result GetButtonConfigStorageFullDeprecated(s32 index, HidcfgButtonConfigFull *config) { return ::hidsysGetButtonConfigStorageFullDeprecated(index, config); }
Result GetButtonConfigStorageLeftDeprecated(s32 index, HidcfgButtonConfigLeft *config) { return ::hidsysGetButtonConfigStorageLeftDeprecated(index, config); }
Result GetButtonConfigStorageRightDeprecated(s32 index, HidcfgButtonConfigRight *config) { return ::hidsysGetButtonConfigStorageRightDeprecated(index, config); }
Result SetButtonConfigStorageEmbeddedDeprecated(s32 index, const HidcfgButtonConfigEmbedded *config) { return ::hidsysSetButtonConfigStorageEmbeddedDeprecated(index, config); }
Result SetButtonConfigStorageFullDeprecated(s32 index, const HidcfgButtonConfigFull *config) { return ::hidsysSetButtonConfigStorageFullDeprecated(index, config); }
Result SetButtonConfigStorageLeftDeprecated(s32 index, const HidcfgButtonConfigLeft *config) { return ::hidsysSetButtonConfigStorageLeftDeprecated(index, config); }
Result SetButtonConfigStorageRightDeprecated(s32 index, const HidcfgButtonConfigRight *config) { return ::hidsysSetButtonConfigStorageRightDeprecated(index, config); }
Result DeleteButtonConfigStorageEmbedded(s32 index) { return ::hidsysDeleteButtonConfigStorageEmbedded(index); }
Result DeleteButtonConfigStorageFull(s32 index) { return ::hidsysDeleteButtonConfigStorageFull(index); }
Result DeleteButtonConfigStorageLeft(s32 index) { return ::hidsysDeleteButtonConfigStorageLeft(index); }
Result DeleteButtonConfigStorageRight(s32 index) { return ::hidsysDeleteButtonConfigStorageRight(index); }
Result IsUsingCustomButtonConfig(HidsysUniquePadId unique_pad_id, bool *out) { return ::hidsysIsUsingCustomButtonConfig(unique_pad_id, out); }
Result IsAnyCustomButtonConfigEnabled(bool *out) { return ::hidsysIsAnyCustomButtonConfigEnabled(out); }
Result SetAllCustomButtonConfigEnabled(u64 AppletResourceUserId, bool flag) { return ::hidsysSetAllCustomButtonConfigEnabled(AppletResourceUserId, flag); }
Result SetAllDefaultButtonConfig(void) { return ::hidsysSetAllDefaultButtonConfig(); }
Result SetHidButtonConfigEmbedded(HidsysUniquePadId unique_pad_id, const HidcfgButtonConfigEmbedded *config) { return ::hidsysSetHidButtonConfigEmbedded(unique_pad_id, config); }
Result SetHidButtonConfigFull(HidsysUniquePadId unique_pad_id, const HidcfgButtonConfigFull *config) { return ::hidsysSetHidButtonConfigFull(unique_pad_id, config); }
Result SetHidButtonConfigLeft(HidsysUniquePadId unique_pad_id, const HidcfgButtonConfigLeft *config) { return ::hidsysSetHidButtonConfigLeft(unique_pad_id, config); }
Result SetHidButtonConfigRight(HidsysUniquePadId unique_pad_id, const HidcfgButtonConfigRight *config) { return ::hidsysSetHidButtonConfigRight(unique_pad_id, config); }
Result GetHidButtonConfigEmbedded(HidsysUniquePadId unique_pad_id, HidcfgButtonConfigEmbedded *config) { return ::hidsysGetHidButtonConfigEmbedded(unique_pad_id, config); }
Result GetHidButtonConfigFull(HidsysUniquePadId unique_pad_id, HidcfgButtonConfigFull *config) { return ::hidsysGetHidButtonConfigFull(unique_pad_id, config); }
Result GetHidButtonConfigLeft(HidsysUniquePadId unique_pad_id, HidcfgButtonConfigLeft *config) { return ::hidsysGetHidButtonConfigLeft(unique_pad_id, config); }
Result GetHidButtonConfigRight(HidsysUniquePadId unique_pad_id, HidcfgButtonConfigRight *config) { return ::hidsysGetHidButtonConfigRight(unique_pad_id, config); }
Result GetButtonConfigStorageEmbedded(s32 index, HidcfgButtonConfigEmbedded *config, HidcfgStorageName *name) { return ::hidsysGetButtonConfigStorageEmbedded(index, config, name); }
Result GetButtonConfigStorageFull(s32 index, HidcfgButtonConfigFull *config, HidcfgStorageName *name) { return ::hidsysGetButtonConfigStorageFull(index, config, name); }
Result GetButtonConfigStorageLeft(s32 index, HidcfgButtonConfigLeft *config, HidcfgStorageName *name) { return ::hidsysGetButtonConfigStorageLeft(index, config, name); }
Result GetButtonConfigStorageRight(s32 index, HidcfgButtonConfigRight *config, HidcfgStorageName *name) { return ::hidsysGetButtonConfigStorageRight(index, config, name); }
Result SetButtonConfigStorageEmbedded(s32 index, const HidcfgButtonConfigEmbedded *config, const HidcfgStorageName *name) { return ::hidsysSetButtonConfigStorageEmbedded(index, config, name); }
Result SetButtonConfigStorageFull(s32 index, const HidcfgButtonConfigFull *config, const HidcfgStorageName *name) { return ::hidsysSetButtonConfigStorageFull(index, config, name); }
Result SetButtonConfigStorageLeft(s32 index, const HidcfgButtonConfigLeft *config, const HidcfgStorageName *name) { return ::hidsysSetButtonConfigStorageLeft(index, config, name); }
Result SetButtonConfigStorageRight(s32 index, const HidcfgButtonConfigRight *config, const HidcfgStorageName *name) { return ::hidsysSetButtonConfigStorageRight(index, config, name); }

} // namespace hidsys

namespace hipc {

HipcStaticDescriptor MakeSendStatic(const void* buffer, size_t size, u8 index) { return ::hipcMakeSendStatic(buffer, size, index); }
HipcBufferDescriptor MakeBuffer(const void* buffer, size_t size, HipcBufferMode mode) { return ::hipcMakeBuffer(buffer, size, mode); }
HipcRecvListEntry MakeRecvStatic(void* buffer, size_t size) { return ::hipcMakeRecvStatic(buffer, size); }
void* GetStaticAddress(const HipcStaticDescriptor* desc) { return ::hipcGetStaticAddress(desc); }
size_t GetStaticSize(const HipcStaticDescriptor* desc) { return ::hipcGetStaticSize(desc); }
void* GetBufferAddress(const HipcBufferDescriptor* desc) { return ::hipcGetBufferAddress(desc); }
size_t GetBufferSize(const HipcBufferDescriptor* desc) { return ::hipcGetBufferSize(desc); }
HipcRequest CalcRequestLayout(HipcMetadata meta, void* base) { return ::hipcCalcRequestLayout(meta, base); }
HipcRequest MakeRequest(void* base, HipcMetadata meta) { return ::hipcMakeRequest(base, meta); }
HipcParsedRequest ParseRequest(void* base) { return ::hipcParseRequest(base); }
HipcResponse ParseResponse(void* base) { return ::hipcParseResponse(base); }

} // namespace hipc

namespace hmac {

void Sha256ContextCreate(HmacSha256Context *out, const void *key, size_t key_size) { ::hmacSha256ContextCreate(out, key, key_size); }
void Sha256ContextUpdate(HmacSha256Context *ctx, const void *src, size_t size) { ::hmacSha256ContextUpdate(ctx, src, size); }
void Sha256ContextGetMac(HmacSha256Context *ctx, void *dst) { ::hmacSha256ContextGetMac(ctx, dst); }
void Sha256CalculateMac(void *dst, const void *key, size_t key_size, const void *src, size_t size) { ::hmacSha256CalculateMac(dst, key, key_size, src, size); }
void Sha1ContextCreate(HmacSha1Context *out, const void *key, size_t key_size) { ::hmacSha1ContextCreate(out, key, key_size); }
void Sha1ContextUpdate(HmacSha1Context *ctx, const void *src, size_t size) { ::hmacSha1ContextUpdate(ctx, src, size); }
void Sha1ContextGetMac(HmacSha1Context *ctx, void *dst) { ::hmacSha1ContextGetMac(ctx, dst); }
void Sha1CalculateMac(void *dst, const void *key, size_t key_size, const void *src, size_t size) { ::hmacSha1CalculateMac(dst, key, key_size, src, size); }

} // namespace hmac

namespace hosversion {

u32 Get(void) { return ::hosversionGet(); }
void Set(u32 version) { ::hosversionSet(version); }
bool IsAtmosphere(void) { return ::hosversionIsAtmosphere(); }
bool AtLeast(u8 major, u8 minor, u8 micro) { return ::hosversionAtLeast(major, minor, micro); }
bool Before(u8 major, u8 minor, u8 micro) { return ::hosversionBefore(major, minor, micro); }
bool Between(u8 major1, u8 major2) { return ::hosversionBetween(major1, major2); }

} // namespace hosversion

namespace hwopus {

Result DecoderInitialize(HwopusDecoder* decoder, s32 SampleRate, s32 ChannelCount) { return ::hwopusDecoderInitialize(decoder, SampleRate, ChannelCount); }
void DecoderExit(HwopusDecoder* decoder) { ::hwopusDecoderExit(decoder); }
Result DecoderMultistreamInitialize(HwopusDecoder* decoder, s32 SampleRate, s32 ChannelCount, s32 TotalStreamCount, s32 StereoStreamCount, u8 *channel_mapping) { return ::hwopusDecoderMultistreamInitialize(decoder, SampleRate, ChannelCount, TotalStreamCount, StereoStreamCount, channel_mapping); }
Result DecodeInterleaved(HwopusDecoder* decoder, s32 *DecodedDataSize, s32 *DecodedSampleCount, const void* opusin, size_t opusin_size, s16 *pcmbuf, size_t pcmbuf_size) { return ::hwopusDecodeInterleaved(decoder, DecodedDataSize, DecodedSampleCount, opusin, opusin_size, pcmbuf, pcmbuf_size); }

} // namespace hwopus

namespace i2c {

Result Initialize(void) { return ::i2cInitialize(); }
void Exit(void) { ::i2cExit(); }
Service* GetServiceSession(void) { return ::i2cGetServiceSession(); }
Result OpenSession(I2cSession *out, I2cDevice dev) { return ::i2cOpenSession(out, dev); }

} // namespace i2c

namespace i2csession {

Result SendAuto(I2cSession *s, const void *buf, size_t size, I2cTransactionOption option) { return ::i2csessionSendAuto(s, buf, size, option); }
Result ReceiveAuto(I2cSession *s, void *buf, size_t size, I2cTransactionOption option) { return ::i2csessionReceiveAuto(s, buf, size, option); }
Result ExecuteCommandList(I2cSession *s, void *dst, size_t dst_size, const void *cmd_list, size_t cmd_list_size) { return ::i2csessionExecuteCommandList(s, dst, dst_size, cmd_list, cmd_list_size); }
void Close(I2cSession *s) { ::i2csessionClose(s); }

} // namespace i2csession

namespace idlesys {

Result Initialize(void) { return ::idlesysInitialize(); }
void Exit(void) { ::idlesysExit(); }
Service* GetServiceSession(void) { return ::idlesysGetServiceSession(); }
Result ReportUserIsActive(void) { return ::idlesysReportUserIsActive(); }

} // namespace idlesys

namespace insr {

Result Initialize(void) { return ::insrInitialize(); }
void Exit(void) { ::insrExit(); }
Service* GetServiceSession(void) { return ::insrGetServiceSession(); }
Result GetLastTick(u32 id, u64 *tick) { return ::insrGetLastTick(id, tick); }
Result GetReadableEvent(u32 id, Event *out) { return ::insrGetReadableEvent(id, out); }

} // namespace insr

namespace inss {

Result Initialize(void) { return ::inssInitialize(); }
void Exit(void) { ::inssExit(); }
Service* GetServiceSession(void) { return ::inssGetServiceSession(); }
Result GetWritableEvent(u32 id, Event *out) { return ::inssGetWritableEvent(id, out); }

} // namespace inss

namespace irs {

Result Initialize(void) { return ::irsInitialize(); }
void Exit(void) { ::irsExit(); }
Service* GetServiceSession(void) { return ::irsGetServiceSession(); }
void* GetSharedmemAddr(void) { return ::irsGetSharedmemAddr(); }
Result GetIrCameraHandle(IrsIrCameraHandle *handle, HidNpadIdType id) { return ::irsGetIrCameraHandle(handle, id); }
Result GetIrCameraStatus(IrsIrCameraHandle handle, IrsIrCameraStatus *out) { return ::irsGetIrCameraStatus(handle, out); }
Result CheckFirmwareUpdateNecessity(IrsIrCameraHandle handle, bool *out) { return ::irsCheckFirmwareUpdateNecessity(handle, out); }
Result GetImageProcessorStatus(IrsIrCameraHandle handle, IrsImageProcessorStatus *out) { return ::irsGetImageProcessorStatus(handle, out); }
Result StopImageProcessor(IrsIrCameraHandle handle) { return ::irsStopImageProcessor(handle); }
Result StopImageProcessorAsync(IrsIrCameraHandle handle) { return ::irsStopImageProcessorAsync(handle); }
Result RunMomentProcessor(IrsIrCameraHandle handle, const IrsMomentProcessorConfig *config) { return ::irsRunMomentProcessor(handle, config); }
Result GetMomentProcessorStates(IrsIrCameraHandle handle, IrsMomentProcessorState *states, s32 count, s32 *total_out) { return ::irsGetMomentProcessorStates(handle, states, count, total_out); }
IrsMomentStatistic CalculateMomentRegionStatistic(const IrsMomentProcessorState *state, IrsRect rect, s32 region_x, s32 region_y, s32 region_width, s32 region_height) { return ::irsCalculateMomentRegionStatistic(state, rect, region_x, region_y, region_width, region_height); }
Result RunClusteringProcessor(IrsIrCameraHandle handle, const IrsClusteringProcessorConfig *config) { return ::irsRunClusteringProcessor(handle, config); }
Result GetClusteringProcessorStates(IrsIrCameraHandle handle, IrsClusteringProcessorState *states, s32 count, s32 *total_out) { return ::irsGetClusteringProcessorStates(handle, states, count, total_out); }
Result RunImageTransferProcessor(IrsIrCameraHandle handle, const IrsImageTransferProcessorConfig *config, size_t size) { return ::irsRunImageTransferProcessor(handle, config, size); }
Result RunImageTransferExProcessor(IrsIrCameraHandle handle, const IrsImageTransferProcessorExConfig *config, size_t size) { return ::irsRunImageTransferExProcessor(handle, config, size); }
Result GetImageTransferProcessorState(IrsIrCameraHandle handle, void* buffer, size_t size, IrsImageTransferProcessorState *state) { return ::irsGetImageTransferProcessorState(handle, buffer, size, state); }
Result RunPointingProcessor(IrsIrCameraHandle handle) { return ::irsRunPointingProcessor(handle); }
Result GetPointingProcessorMarkerStates(IrsIrCameraHandle handle, IrsPointingProcessorMarkerState *states, s32 count, s32 *total_out) { return ::irsGetPointingProcessorMarkerStates(handle, states, count, total_out); }
Result GetPointingProcessorStates(IrsIrCameraHandle handle, IrsPointingProcessorState *states, s32 count, s32 *total_out) { return ::irsGetPointingProcessorStates(handle, states, count, total_out); }
Result RunTeraPluginProcessor(IrsIrCameraHandle handle, const IrsTeraPluginProcessorConfig *config) { return ::irsRunTeraPluginProcessor(handle, config); }
Result GetTeraPluginProcessorStates(IrsIrCameraHandle handle, IrsTeraPluginProcessorState *states, s32 count, s64 sampling_number, u32 prefix_data, u32 prefix_bitcount, s32 *total_out) { return ::irsGetTeraPluginProcessorStates(handle, states, count, sampling_number, prefix_data, prefix_bitcount, total_out); }
Result RunIrLedProcessor(IrsIrCameraHandle handle, const IrsIrLedProcessorConfig *config) { return ::irsRunIrLedProcessor(handle, config); }
Result RunAdaptiveClusteringProcessor(IrsIrCameraHandle handle, const IrsAdaptiveClusteringProcessorConfig *config) { return ::irsRunAdaptiveClusteringProcessor(handle, config); }
Result RunHandAnalysis(IrsIrCameraHandle handle, const IrsHandAnalysisConfig *config) { return ::irsRunHandAnalysis(handle, config); }
void GetMomentProcessorDefaultConfig(IrsMomentProcessorConfig *config) { ::irsGetMomentProcessorDefaultConfig(config); }
void GetClusteringProcessorDefaultConfig(IrsClusteringProcessorConfig *config) { ::irsGetClusteringProcessorDefaultConfig(config); }
void GetDefaultImageTransferProcessorConfig(IrsImageTransferProcessorConfig *config) { ::irsGetDefaultImageTransferProcessorConfig(config); }
void GetDefaultImageTransferProcessorExConfig(IrsImageTransferProcessorExConfig *config) { ::irsGetDefaultImageTransferProcessorExConfig(config); }
void GetIrLedProcessorDefaultConfig(IrsIrLedProcessorConfig *config) { ::irsGetIrLedProcessorDefaultConfig(config); }

} // namespace irs

namespace jit {

Result Create(Jit* j, size_t size) { return ::jitCreate(j, size); }
Result TransitionToWritable(Jit* j) { return ::jitTransitionToWritable(j); }
Result TransitionToExecutable(Jit* j) { return ::jitTransitionToExecutable(j); }
Result Close(Jit* j) { return ::jitClose(j); }
void* GetRwAddr(Jit* j) { return ::jitGetRwAddr(j); }
void* GetRxAddr(Jit* j) { return ::jitGetRxAddr(j); }

} // namespace jit

namespace lbl {

Result Initialize(void) { return ::lblInitialize(); }
void Exit(void) { ::lblExit(); }
Service* GetServiceSession(void) { return ::lblGetServiceSession(); }
Result SaveCurrentSetting(void) { return ::lblSaveCurrentSetting(); }
Result LoadCurrentSetting(void) { return ::lblLoadCurrentSetting(); }
Result SetCurrentBrightnessSetting(float brightness) { return ::lblSetCurrentBrightnessSetting(brightness); }
Result GetCurrentBrightnessSetting(float *out_value) { return ::lblGetCurrentBrightnessSetting(out_value); }
Result ApplyCurrentBrightnessSettingToBacklight(void) { return ::lblApplyCurrentBrightnessSettingToBacklight(); }
Result GetBrightnessSettingAppliedToBacklight(float *out_value) { return ::lblGetBrightnessSettingAppliedToBacklight(out_value); }
Result SwitchBacklightOn(u64 fade_time) { return ::lblSwitchBacklightOn(fade_time); }
Result SwitchBacklightOff(u64 fade_time) { return ::lblSwitchBacklightOff(fade_time); }
Result GetBacklightSwitchStatus(LblBacklightSwitchStatus *out_value) { return ::lblGetBacklightSwitchStatus(out_value); }
Result EnableDimming(void) { return ::lblEnableDimming(); }
Result DisableDimming(void) { return ::lblDisableDimming(); }
Result IsDimmingEnabled(bool *out_value) { return ::lblIsDimmingEnabled(out_value); }
Result EnableAutoBrightnessControl(void) { return ::lblEnableAutoBrightnessControl(); }
Result DisableAutoBrightnessControl(void) { return ::lblDisableAutoBrightnessControl(); }
Result IsAutoBrightnessControlEnabled(bool *out_value) { return ::lblIsAutoBrightnessControlEnabled(out_value); }
Result SetAmbientLightSensorValue(float value) { return ::lblSetAmbientLightSensorValue(value); }
Result GetAmbientLightSensorValue(bool *over_limit, float *lux) { return ::lblGetAmbientLightSensorValue(over_limit, lux); }
Result IsAmbientLightSensorAvailable(bool *out_value) { return ::lblIsAmbientLightSensorAvailable(out_value); }
Result SetCurrentBrightnessSettingForVrMode(float brightness) { return ::lblSetCurrentBrightnessSettingForVrMode(brightness); }
Result GetCurrentBrightnessSettingForVrMode(float *out_value) { return ::lblGetCurrentBrightnessSettingForVrMode(out_value); }
Result EnableVrMode(void) { return ::lblEnableVrMode(); }
Result DisableVrMode(void) { return ::lblDisableVrMode(); }
Result IsVrModeEnabled(bool *out_value) { return ::lblIsVrModeEnabled(out_value); }

} // namespace lbl

namespace ldn {

Result Initialize(LdnServiceType service_type) { return ::ldnInitialize(service_type); }
void Exit(void) { ::ldnExit(); }
Service* GetServiceSession_LocalCommunicationService(void) { return ::ldnGetServiceSession_LocalCommunicationService(); }
Service* GetServiceSession_IClientProcessMonitor(void) { return ::ldnGetServiceSession_IClientProcessMonitor(); }
Result GetState(LdnState *out) { return ::ldnGetState(out); }
Result GetNetworkInfo(LdnNetworkInfo *out) { return ::ldnGetNetworkInfo(out); }
Result GetIpv4Address(LdnIpv4Address *addr, LdnSubnetMask *mask) { return ::ldnGetIpv4Address(addr, mask); }
Result GetDisconnectReason(LdnDisconnectReason *out) { return ::ldnGetDisconnectReason(out); }
Result GetSecurityParameter(LdnSecurityParameter *out) { return ::ldnGetSecurityParameter(out); }
Result GetNetworkConfig(LdnNetworkConfig *out) { return ::ldnGetNetworkConfig(out); }
Result GetStateChangeEvent(Event* out_event) { return ::ldnGetStateChangeEvent(out_event); }
Result GetNetworkInfoAndHistory(LdnNetworkInfo *network_info, LdnNodeLatestUpdate *nodes, s32 count) { return ::ldnGetNetworkInfoAndHistory(network_info, nodes, count); }
Result Scan(s32 channel, const LdnScanFilter *filter, LdnNetworkInfo *network_info, s32 count, s32 *total_out) { return ::ldnScan(channel, filter, network_info, count, total_out); }
Result ScanPrivate(s32 channel, const LdnScanFilter *filter, LdnNetworkInfo *network_info, s32 count, s32 *total_out) { return ::ldnScanPrivate(channel, filter, network_info, count, total_out); }
Result SetWirelessControllerRestriction(LdnWirelessControllerRestriction restriction) { return ::ldnSetWirelessControllerRestriction(restriction); }
Result SetProtocol(LdnProtocol protocol) { return ::ldnSetProtocol(protocol); }
Result OpenAccessPoint(void) { return ::ldnOpenAccessPoint(); }
Result CloseAccessPoint(void) { return ::ldnCloseAccessPoint(); }
Result CreateNetwork(const LdnSecurityConfig *sec_config, const LdnUserConfig *user_config, const LdnNetworkConfig *network_config) { return ::ldnCreateNetwork(sec_config, user_config, network_config); }
Result CreateNetworkPrivate(const LdnSecurityConfig *sec_config, const LdnSecurityParameter *sec_param, const LdnUserConfig *user_config, const LdnNetworkConfig *network_config, const LdnAddressEntry *addrs, s32 count) { return ::ldnCreateNetworkPrivate(sec_config, sec_param, user_config, network_config, addrs, count); }
Result DestroyNetwork(void) { return ::ldnDestroyNetwork(); }
Result Reject(LdnIpv4Address addr) { return ::ldnReject(addr); }
Result SetAdvertiseData(const void* buffer, size_t size) { return ::ldnSetAdvertiseData(buffer, size); }
Result SetStationAcceptPolicy(LdnAcceptPolicy policy) { return ::ldnSetStationAcceptPolicy(policy); }
Result AddAcceptFilterEntry(LdnMacAddress addr) { return ::ldnAddAcceptFilterEntry(addr); }
Result ClearAcceptFilter(void) { return ::ldnClearAcceptFilter(); }
Result OpenStation(void) { return ::ldnOpenStation(); }
Result CloseStation(void) { return ::ldnCloseStation(); }
Result Connect(const LdnSecurityConfig *sec_config, const LdnUserConfig *user_config, s32 version, u32 option, const LdnNetworkInfo *network_info) { return ::ldnConnect(sec_config, user_config, version, option, network_info); }
Result ConnectPrivate(const LdnSecurityConfig *sec_config, const LdnSecurityParameter *sec_param, const LdnUserConfig *user_config, s32 version, u32 option, const LdnNetworkConfig *network_config) { return ::ldnConnectPrivate(sec_config, sec_param, user_config, version, option, network_config); }
Result Disconnect(void) { return ::ldnDisconnect(); }
Result SetOperationMode(LdnOperationMode mode) { return ::ldnSetOperationMode(mode); }
Result EnableActionFrame(const LdnActionFrameSettings *settings) { return ::ldnEnableActionFrame(settings); }
Result DisableActionFrame(void) { return ::ldnDisableActionFrame(); }
Result SendActionFrame(const void* data, size_t size, LdnMacAddress destination, LdnMacAddress bssid, s16 channel, u32 flags) { return ::ldnSendActionFrame(data, size, destination, bssid, channel, flags); }
Result RecvActionFrame(void* data, size_t size, LdnMacAddress *addr0, LdnMacAddress *addr1, s16 *channel, u32 *out_size, s32 *link_level, u32 flags) { return ::ldnRecvActionFrame(data, size, addr0, addr1, channel, out_size, link_level, flags); }
Result SetHomeChannel(s16 channel) { return ::ldnSetHomeChannel(channel); }
Result SetTxPower(s16 power) { return ::ldnSetTxPower(power); }
Result ResetTxPower(void) { return ::ldnResetTxPower(); }

} // namespace ldn

namespace ldnm {

Result Initialize(void) { return ::ldnmInitialize(); }
void Exit(void) { ::ldnmExit(); }
Service* GetServiceSession_MonitorService(void) { return ::ldnmGetServiceSession_MonitorService(); }
Result GetState(LdnState *out) { return ::ldnmGetState(out); }
Result GetNetworkInfo(LdnNetworkInfo *out) { return ::ldnmGetNetworkInfo(out); }
Result GetIpv4Address(LdnIpv4Address *addr, LdnSubnetMask *mask) { return ::ldnmGetIpv4Address(addr, mask); }
Result GetSecurityParameter(LdnSecurityParameter *out) { return ::ldnmGetSecurityParameter(out); }
Result GetNetworkConfig(LdnNetworkConfig *out) { return ::ldnmGetNetworkConfig(out); }

} // namespace ldnm

namespace ldr {

Result ShellInitialize(void) { return ::ldrShellInitialize(); }
void ShellExit(void) { ::ldrShellExit(); }
Service* ShellGetServiceSession(void) { return ::ldrShellGetServiceSession(); }
Result DmntInitialize(void) { return ::ldrDmntInitialize(); }
void DmntExit(void) { ::ldrDmntExit(); }
Service* DmntGetServiceSession(void) { return ::ldrDmntGetServiceSession(); }
Result PmInitialize(void) { return ::ldrPmInitialize(); }
void PmExit(void) { ::ldrPmExit(); }
Service* PmGetServiceSession(void) { return ::ldrPmGetServiceSession(); }
Result ShellSetProgramArguments(u64 program_id, const void *args, size_t args_size) { return ::ldrShellSetProgramArguments(program_id, args, args_size); }
Result ShellFlushArguments(void) { return ::ldrShellFlushArguments(); }
Result DmntSetProgramArguments(u64 program_id, const void *args, size_t args_size) { return ::ldrDmntSetProgramArguments(program_id, args, args_size); }
Result DmntFlushArguments(void) { return ::ldrDmntFlushArguments(); }
Result DmntGetProcessModuleInfo(u64 pid, LoaderModuleInfo *out_module_infos, size_t max_out_modules, s32 *num_out) { return ::ldrDmntGetProcessModuleInfo(pid, out_module_infos, max_out_modules, num_out); }
Result PmCreateProcess(u64 pin_id, u32 flags, Handle reslimit_h, const LoaderProgramAttributes *attrs, Handle *out_process_h) { return ::ldrPmCreateProcess(pin_id, flags, reslimit_h, attrs, out_process_h); }
Result PmGetProgramInfo(const NcmProgramLocation *loc, const LoaderProgramAttributes *attrs, LoaderProgramInfo *out_program_info) { return ::ldrPmGetProgramInfo(loc, attrs, out_program_info); }
Result PmGetProgramInfoV1(const NcmProgramLocation *loc, LoaderProgramInfoV1 *out_program_info) { return ::ldrPmGetProgramInfoV1(loc, out_program_info); }
Result PmPinProgram(const NcmProgramLocation *loc, u64 *out_pin_id) { return ::ldrPmPinProgram(loc, out_pin_id); }
Result PmUnpinProgram(u64 pin_id) { return ::ldrPmUnpinProgram(pin_id); }
Result PmSetEnabledProgramVerification(bool enabled) { return ::ldrPmSetEnabledProgramVerification(enabled); }
Result RoInitialize(void) { return ::ldrRoInitialize(); }
void RoExit(void) { ::ldrRoExit(); }
Service* RoGetServiceSession(void) { return ::ldrRoGetServiceSession(); }
Result RoLoadNro(u64* out_address, u64 nro_address, u64 nro_size, u64 bss_address, u64 bss_size) { return ::ldrRoLoadNro(out_address, nro_address, nro_size, bss_address, bss_size); }
Result RoUnloadNro(u64 nro_address) { return ::ldrRoUnloadNro(nro_address); }
Result RoLoadNrr(u64 nrr_address, u64 nrr_size) { return ::ldrRoLoadNrr(nrr_address, nrr_size); }
Result RoUnloadNrr(u64 nrr_address) { return ::ldrRoUnloadNrr(nrr_address); }
Result RoLoadNrrEx(u64 nrr_address, u64 nrr_size) { return ::ldrRoLoadNrrEx(nrr_address, nrr_size); }

} // namespace ldr

namespace levent {

void Init(LEvent* le, bool signaled, bool autoclear) { ::leventInit(le, signaled, autoclear); }
bool Wait(LEvent* le, u64 timeout_ns) { return ::leventWait(le, timeout_ns); }
bool TryWait(LEvent* le) { return ::leventTryWait(le); }
void Signal(LEvent* le) { ::leventSignal(le); }
void Clear(LEvent* le) { ::leventClear(le); }

} // namespace levent

namespace libapplet {

void ArgsCreate(LibAppletArgs* a, u32 version) { ::libappletArgsCreate(a, version); }
void ArgsSetPlayStartupSound(LibAppletArgs* a, bool flag) { ::libappletArgsSetPlayStartupSound(a, flag); }
Result CreateWriteStorage(AppletStorage* s, const void* buffer, size_t size) { return ::libappletCreateWriteStorage(s, buffer, size); }
Result ReadStorage(AppletStorage* s, void* buffer, size_t size, size_t *transfer_size) { return ::libappletReadStorage(s, buffer, size, transfer_size); }
Result ArgsPush(LibAppletArgs* a, AppletHolder *h) { return ::libappletArgsPush(a, h); }
Result ArgsPop(LibAppletArgs* a) { return ::libappletArgsPop(a); }
Result PushInData(AppletHolder *h, const void* buffer, size_t size) { return ::libappletPushInData(h, buffer, size); }
Result PopOutData(AppletHolder *h, void* buffer, size_t size, size_t *transfer_size) { return ::libappletPopOutData(h, buffer, size, transfer_size); }
void SetJumpFlag(bool flag) { ::libappletSetJumpFlag(flag); }
Result Start(AppletHolder *h) { return ::libappletStart(h); }
Result Launch(AppletId id, LibAppletArgs *commonargs, const void* arg, size_t arg_size, void* reply, size_t reply_size, size_t *out_reply_size) { return ::libappletLaunch(id, commonargs, arg, arg_size, reply, reply_size, out_reply_size); }
Result RequestHomeMenu(void) { return ::libappletRequestHomeMenu(); }
Result RequestJumpToSystemUpdate(void) { return ::libappletRequestJumpToSystemUpdate(); }
Result RequestToLaunchApplication(u64 application_id, AccountUid uid, const void* buffer, size_t size, u32 sender) { return ::libappletRequestToLaunchApplication(application_id, uid, buffer, size, sender); }
Result RequestJumpToStory(AccountUid uid, u64 application_id) { return ::libappletRequestJumpToStory(uid, application_id); }

} // namespace libapplet

namespace lp2p {

Result Initialize(Lp2pServiceType service_type) { return ::lp2pInitialize(service_type); }
void Exit(void) { ::lp2pExit(); }
Service* GetServiceSession_INetworkService(void) { return ::lp2pGetServiceSession_INetworkService(); }
Service* GetServiceSession_INetworkServiceMonitor(void) { return ::lp2pGetServiceSession_INetworkServiceMonitor(); }
void CreateGroupInfo(Lp2pGroupInfo *info) { ::lp2pCreateGroupInfo(info); }
void CreateGroupInfoScan(Lp2pGroupInfo *info) { ::lp2pCreateGroupInfoScan(info); }
void GroupInfoSetServiceName(Lp2pGroupInfo *info, const char *name) { ::lp2pGroupInfoSetServiceName(info, name); }
void GroupInfoSetFlags(Lp2pGroupInfo *info, s8 *flags, size_t count) { ::lp2pGroupInfoSetFlags(info, flags, count); }
void GroupInfoSetMemberCountMax(Lp2pGroupInfo *info, size_t count) { ::lp2pGroupInfoSetMemberCountMax(info, count); }
void GroupInfoSetFrequencyChannel(Lp2pGroupInfo *info, u16 frequency, s16 channel) { ::lp2pGroupInfoSetFrequencyChannel(info, frequency, channel); }
void GroupInfoSetStealthEnabled(Lp2pGroupInfo *info, bool flag) { ::lp2pGroupInfoSetStealthEnabled(info, flag); }
void GroupInfoSetPresharedKey(Lp2pGroupInfo *info, const void* key, size_t size) { ::lp2pGroupInfoSetPresharedKey(info, key, size); }
Result GroupInfoSetPassphrase(Lp2pGroupInfo *info, const char *passphrase) { return ::lp2pGroupInfoSetPassphrase(info, passphrase); }
Result Scan(const Lp2pGroupInfo *info, Lp2pScanResult *results, s32 count, s32 *total_out) { return ::lp2pScan(info, results, count, total_out); }
Result CreateGroup(const Lp2pGroupInfo *info) { return ::lp2pCreateGroup(info); }
Result DestroyGroup(void) { return ::lp2pDestroyGroup(); }
Result SetAdvertiseData(const void* buffer, size_t size) { return ::lp2pSetAdvertiseData(buffer, size); }
Result SendToOtherGroup(const void* buffer, size_t size, Lp2pMacAddress addr, Lp2pGroupId group_id, s16 frequency, s16 channel, u32 flags) { return ::lp2pSendToOtherGroup(buffer, size, addr, group_id, frequency, channel, flags); }
Result RecvFromOtherGroup(void* buffer, size_t size, u32 flags, Lp2pMacAddress *addr, u16 *unk0, s32 *unk1, u64 *out_size, s32 *unk2) { return ::lp2pRecvFromOtherGroup(buffer, size, flags, addr, unk0, unk1, out_size, unk2); }
Result AddAcceptableGroupId(Lp2pGroupId group_id) { return ::lp2pAddAcceptableGroupId(group_id); }
Result RemoveAcceptableGroupId(void) { return ::lp2pRemoveAcceptableGroupId(); }
Result AttachNetworkInterfaceStateChangeEvent(Event* out_event) { return ::lp2pAttachNetworkInterfaceStateChangeEvent(out_event); }
Result GetNetworkInterfaceLastError(void) { return ::lp2pGetNetworkInterfaceLastError(); }
Result GetRole(u8 *out) { return ::lp2pGetRole(out); }
Result GetAdvertiseData(void* buffer, size_t size, u16 *transfer_size, u16 *original_size) { return ::lp2pGetAdvertiseData(buffer, size, transfer_size, original_size); }
Result GetAdvertiseData2(void* buffer, size_t size, u16 *transfer_size, u16 *original_size) { return ::lp2pGetAdvertiseData2(buffer, size, transfer_size, original_size); }
Result GetGroupInfo(Lp2pGroupInfo *out) { return ::lp2pGetGroupInfo(out); }
Result Join(Lp2pGroupInfo *out, const Lp2pGroupInfo *info) { return ::lp2pJoin(out, info); }
Result GetGroupOwner(Lp2pNodeInfo *out) { return ::lp2pGetGroupOwner(out); }
Result GetIpConfig(Lp2pIpConfig *out) { return ::lp2pGetIpConfig(out); }
Result Leave(u32 *out) { return ::lp2pLeave(out); }
Result AttachJoinEvent(Event* out_event) { return ::lp2pAttachJoinEvent(out_event); }
Result GetMembers(Lp2pNodeInfo *members, s32 count, s32 *total_out) { return ::lp2pGetMembers(members, count, total_out); }

} // namespace lp2p

namespace lr {

Result Initialize(void) { return ::lrInitialize(); }
void Exit(void) { ::lrExit(); }
Service* GetServiceSession(void) { return ::lrGetServiceSession(); }
Result OpenLocationResolver(NcmStorageId storage, LrLocationResolver* out) { return ::lrOpenLocationResolver(storage, out); }
Result OpenRegisteredLocationResolver(LrRegisteredLocationResolver* out) { return ::lrOpenRegisteredLocationResolver(out); }
Result LrResolveProgramPath(LrLocationResolver* lr, u64 tid, char *out) { return ::lrLrResolveProgramPath(lr, tid, out); }
Result LrRedirectProgramPath(LrLocationResolver* lr, u64 tid, const char *path) { return ::lrLrRedirectProgramPath(lr, tid, path); }
Result LrResolveApplicationControlPath(LrLocationResolver* lr, u64 tid, char *out) { return ::lrLrResolveApplicationControlPath(lr, tid, out); }
Result LrResolveApplicationHtmlDocumentPath(LrLocationResolver* lr, u64 tid, char *out) { return ::lrLrResolveApplicationHtmlDocumentPath(lr, tid, out); }
Result LrResolveDataPath(LrLocationResolver* lr, u64 tid, char *out) { return ::lrLrResolveDataPath(lr, tid, out); }
Result LrRedirectApplicationControlPath(LrLocationResolver* lr, u64 tid, u64 tid2, const char *path) { return ::lrLrRedirectApplicationControlPath(lr, tid, tid2, path); }
Result LrRedirectApplicationHtmlDocumentPath(LrLocationResolver* lr, u64 tid, u64 tid2, const char *path) { return ::lrLrRedirectApplicationHtmlDocumentPath(lr, tid, tid2, path); }
Result LrResolveApplicationLegalInformationPath(LrLocationResolver* lr, u64 tid, char *out) { return ::lrLrResolveApplicationLegalInformationPath(lr, tid, out); }
Result LrRedirectApplicationLegalInformationPath(LrLocationResolver* lr, u64 tid, u64 tid2, const char *path) { return ::lrLrRedirectApplicationLegalInformationPath(lr, tid, tid2, path); }
Result LrRefresh(LrLocationResolver* lr) { return ::lrLrRefresh(lr); }
Result LrEraseProgramRedirection(LrLocationResolver* lr, u64 tid) { return ::lrLrEraseProgramRedirection(lr, tid); }
Result RegLrResolveProgramPath(LrRegisteredLocationResolver* reg, u64 tid, char *out) { return ::lrRegLrResolveProgramPath(reg, tid, out); }

} // namespace lr

namespace mii {

Result LaShowMiiEdit(MiiSpecialKeyCode special_key_code) { return ::miiLaShowMiiEdit(special_key_code); }
Result LaAppendMii(MiiSpecialKeyCode special_key_code, s32 *index) { return ::miiLaAppendMii(special_key_code, index); }
Result LaAppendMiiImage(MiiSpecialKeyCode special_key_code, const Uuid *valid_uuid_array, s32 count, s32 *index) { return ::miiLaAppendMiiImage(special_key_code, valid_uuid_array, count, index); }
Result LaUpdateMiiImage(MiiSpecialKeyCode special_key_code, const Uuid *valid_uuid_array, s32 count, Uuid used_uuid, s32 *index) { return ::miiLaUpdateMiiImage(special_key_code, valid_uuid_array, count, used_uuid, index); }
Result LaCreateMii(MiiSpecialKeyCode special_key_code, MiiCharInfo *out_char) { return ::miiLaCreateMii(special_key_code, out_char); }
Result LaEditMii(MiiSpecialKeyCode special_key_code, const MiiCharInfo *in_char, MiiCharInfo *out_char) { return ::miiLaEditMii(special_key_code, in_char, out_char); }
Result Initialize(MiiServiceType service_type) { return ::miiInitialize(service_type); }
void Exit(void) { ::miiExit(); }
Service* GetServiceSession(void) { return ::miiGetServiceSession(); }
Result OpenDatabase(MiiDatabase *out, MiiSpecialKeyCode key_code) { return ::miiOpenDatabase(out, key_code); }
Result DatabaseIsUpdated(MiiDatabase *db, bool *out_updated, MiiSourceFlag flag) { return ::miiDatabaseIsUpdated(db, out_updated, flag); }
Result DatabaseIsFull(MiiDatabase *db, bool *out_full) { return ::miiDatabaseIsFull(db, out_full); }
Result DatabaseGetCount(MiiDatabase *db, s32 *out_count, MiiSourceFlag flag) { return ::miiDatabaseGetCount(db, out_count, flag); }
Result DatabaseGet1(MiiDatabase *db, MiiSourceFlag flag, MiiCharInfo *out_infos, s32 count, s32 *total_out) { return ::miiDatabaseGet1(db, flag, out_infos, count, total_out); }
Result DatabaseBuildRandom(MiiDatabase *db, MiiAge age, MiiGender gender, MiiFaceColor face_color, MiiCharInfo *out_info) { return ::miiDatabaseBuildRandom(db, age, gender, face_color, out_info); }
void DatabaseClose(MiiDatabase *db) { ::miiDatabaseClose(db); }

} // namespace mii

namespace miiimg {

Result Initialize(void) { return ::miiimgInitialize(); }
void Exit(void) { ::miiimgExit(); }
Service* GetServiceSession(void) { return ::miiimgGetServiceSession(); }
Result Reload(void) { return ::miiimgReload(); }
Result GetCount(s32 *out_count) { return ::miiimgGetCount(out_count); }
Result IsEmpty(bool *out_empty) { return ::miiimgIsEmpty(out_empty); }
Result IsFull(bool *out_full) { return ::miiimgIsFull(out_full); }
Result GetAttribute(s32 index, MiiimgImageAttribute *out_attr) { return ::miiimgGetAttribute(index, out_attr); }
Result LoadImage(MiiimgImageId id, void* out_image, size_t out_image_size) { return ::miiimgLoadImage(id, out_image, out_image_size); }

} // namespace miiimg

namespace mmu {

Result Initialize(void) { return ::mmuInitialize(); }
void Exit(void) { ::mmuExit(); }
Service* GetServiceSession(void) { return ::mmuGetServiceSession(); }
Result RequestInitialize(MmuRequest *request, MmuModuleId module, u32 unk, bool autoclear) { return ::mmuRequestInitialize(request, module, unk, autoclear); }
Result RequestFinalize(const MmuRequest *request) { return ::mmuRequestFinalize(request); }
Result RequestGet(const MmuRequest *request, u32 *out_freq_hz) { return ::mmuRequestGet(request, out_freq_hz); }
Result RequestSetAndWait(const MmuRequest *request, u32 freq_hz, s32 timeout) { return ::mmuRequestSetAndWait(request, freq_hz, timeout); }

} // namespace mmu

namespace mutex {

void Init(Mutex* m) { ::mutexInit(m); }
void Lock(Mutex* m) { ::mutexLock(m); }
bool TryLock(Mutex* m) { return ::mutexTryLock(m); }
void Unlock(Mutex* m) { ::mutexUnlock(m); }
bool IsLockedByCurrentThread(const Mutex* m) { return ::mutexIsLockedByCurrentThread(m); }

} // namespace mutex

namespace nacp {

Result GetLanguageEntry(NacpStruct* nacp, NacpLanguageEntry** langentry) { return ::nacpGetLanguageEntry(nacp, langentry); }

} // namespace nacp

namespace ncm {

Result Initialize(void) { return ::ncmInitialize(); }
void Exit(void) { ::ncmExit(); }
Service* GetServiceSession(void) { return ::ncmGetServiceSession(); }
Result CreateContentStorage(NcmStorageId storage_id) { return ::ncmCreateContentStorage(storage_id); }
Result CreateContentMetaDatabase(NcmStorageId storage_id) { return ::ncmCreateContentMetaDatabase(storage_id); }
Result VerifyContentStorage(NcmStorageId storage_id) { return ::ncmVerifyContentStorage(storage_id); }
Result VerifyContentMetaDatabase(NcmStorageId storage_id) { return ::ncmVerifyContentMetaDatabase(storage_id); }
Result OpenContentStorage(NcmContentStorage* out_content_storage, NcmStorageId storage_id) { return ::ncmOpenContentStorage(out_content_storage, storage_id); }
Result OpenContentMetaDatabase(NcmContentMetaDatabase* out_content_meta_database, NcmStorageId storage_id) { return ::ncmOpenContentMetaDatabase(out_content_meta_database, storage_id); }
Result CloseContentStorageForcibly(NcmStorageId storage_id) { return ::ncmCloseContentStorageForcibly(storage_id); }
Result CloseContentMetaDatabaseForcibly(NcmStorageId storage_id) { return ::ncmCloseContentMetaDatabaseForcibly(storage_id); }
Result CleanupContentMetaDatabase(NcmStorageId storage_id) { return ::ncmCleanupContentMetaDatabase(storage_id); }
Result ActivateContentStorage(NcmStorageId storage_id) { return ::ncmActivateContentStorage(storage_id); }
Result InactivateContentStorage(NcmStorageId storage_id) { return ::ncmInactivateContentStorage(storage_id); }
Result ActivateContentMetaDatabase(NcmStorageId storage_id) { return ::ncmActivateContentMetaDatabase(storage_id); }
Result InactivateContentMetaDatabase(NcmStorageId storage_id) { return ::ncmInactivateContentMetaDatabase(storage_id); }
Result InvalidateRightsIdCache(void) { return ::ncmInvalidateRightsIdCache(); }
Result ActivateFsContentStorage(FsContentStorageId fs_storage_id) { return ::ncmActivateFsContentStorage(fs_storage_id); }
void ContentStorageClose(NcmContentStorage* cs) { ::ncmContentStorageClose(cs); }
Result ContentStorageGeneratePlaceHolderId(NcmContentStorage* cs, NcmPlaceHolderId* out_id) { return ::ncmContentStorageGeneratePlaceHolderId(cs, out_id); }
Result ContentStorageCreatePlaceHolder(NcmContentStorage* cs, const NcmContentId* content_id, const NcmPlaceHolderId* placeholder_id, s64 size) { return ::ncmContentStorageCreatePlaceHolder(cs, content_id, placeholder_id, size); }
Result ContentStorageDeletePlaceHolder(NcmContentStorage* cs, const NcmPlaceHolderId* placeholder_id) { return ::ncmContentStorageDeletePlaceHolder(cs, placeholder_id); }
Result ContentStorageHasPlaceHolder(NcmContentStorage* cs, bool* out, const NcmPlaceHolderId* placeholder_id) { return ::ncmContentStorageHasPlaceHolder(cs, out, placeholder_id); }
Result ContentStorageWritePlaceHolder(NcmContentStorage* cs, const NcmPlaceHolderId* placeholder_id, u64 offset, const void* data, size_t data_size) { return ::ncmContentStorageWritePlaceHolder(cs, placeholder_id, offset, data, data_size); }
Result ContentStorageRegister(NcmContentStorage* cs, const NcmContentId* content_id, const NcmPlaceHolderId* placeholder_id) { return ::ncmContentStorageRegister(cs, content_id, placeholder_id); }
Result ContentStorageDelete(NcmContentStorage* cs, const NcmContentId* content_id) { return ::ncmContentStorageDelete(cs, content_id); }
Result ContentStorageHas(NcmContentStorage* cs, bool* out, const NcmContentId* content_id) { return ::ncmContentStorageHas(cs, out, content_id); }
Result ContentStorageGetPath(NcmContentStorage* cs, char* out_path, size_t out_size, const NcmContentId* content_id) { return ::ncmContentStorageGetPath(cs, out_path, out_size, content_id); }
Result ContentStorageGetPlaceHolderPath(NcmContentStorage* cs, char* out_path, size_t out_size, const NcmPlaceHolderId* placeholder_id) { return ::ncmContentStorageGetPlaceHolderPath(cs, out_path, out_size, placeholder_id); }
Result ContentStorageCleanupAllPlaceHolder(NcmContentStorage* cs) { return ::ncmContentStorageCleanupAllPlaceHolder(cs); }
Result ContentStorageListPlaceHolder(NcmContentStorage* cs, NcmPlaceHolderId* out_ids, s32 count, s32* out_count) { return ::ncmContentStorageListPlaceHolder(cs, out_ids, count, out_count); }
Result ContentStorageGetContentCount(NcmContentStorage* cs, s32* out_count) { return ::ncmContentStorageGetContentCount(cs, out_count); }
Result ContentStorageListContentId(NcmContentStorage* cs, NcmContentId* out_ids, s32 count, s32* out_count, s32 start_offset) { return ::ncmContentStorageListContentId(cs, out_ids, count, out_count, start_offset); }
Result ContentStorageGetSizeFromContentId(NcmContentStorage* cs, s64* out_size, const NcmContentId* content_id) { return ::ncmContentStorageGetSizeFromContentId(cs, out_size, content_id); }
Result ContentStorageDisableForcibly(NcmContentStorage* cs) { return ::ncmContentStorageDisableForcibly(cs); }
Result ContentStorageRevertToPlaceHolder(NcmContentStorage* cs, const NcmPlaceHolderId* placeholder_id, const NcmContentId* old_content_id, const NcmContentId* new_content_id) { return ::ncmContentStorageRevertToPlaceHolder(cs, placeholder_id, old_content_id, new_content_id); }
Result ContentStorageSetPlaceHolderSize(NcmContentStorage* cs, const NcmPlaceHolderId* placeholder_id, s64 size) { return ::ncmContentStorageSetPlaceHolderSize(cs, placeholder_id, size); }
Result ContentStorageReadContentIdFile(NcmContentStorage* cs, void* out_data, size_t out_data_size, const NcmContentId* content_id, s64 offset) { return ::ncmContentStorageReadContentIdFile(cs, out_data, out_data_size, content_id, offset); }
Result ContentStorageGetRightsIdFromPlaceHolderId(NcmContentStorage* cs, NcmRightsId* out_rights_id, const NcmPlaceHolderId* placeholder_id, FsContentAttributes attr) { return ::ncmContentStorageGetRightsIdFromPlaceHolderId(cs, out_rights_id, placeholder_id, attr); }
Result ContentStorageGetRightsIdFromContentId(NcmContentStorage* cs, NcmRightsId* out_rights_id, const NcmContentId* content_id, FsContentAttributes attr) { return ::ncmContentStorageGetRightsIdFromContentId(cs, out_rights_id, content_id, attr); }
Result ContentStorageWriteContentForDebug(NcmContentStorage* cs, const NcmContentId* content_id, s64 offset, const void* data, size_t data_size) { return ::ncmContentStorageWriteContentForDebug(cs, content_id, offset, data, data_size); }
Result ContentStorageGetFreeSpaceSize(NcmContentStorage* cs, s64* out_size) { return ::ncmContentStorageGetFreeSpaceSize(cs, out_size); }
Result ContentStorageGetTotalSpaceSize(NcmContentStorage* cs, s64* out_size) { return ::ncmContentStorageGetTotalSpaceSize(cs, out_size); }
Result ContentStorageFlushPlaceHolder(NcmContentStorage* cs) { return ::ncmContentStorageFlushPlaceHolder(cs); }
Result ContentStorageGetSizeFromPlaceHolderId(NcmContentStorage* cs, s64* out_size, const NcmPlaceHolderId* placeholder_id) { return ::ncmContentStorageGetSizeFromPlaceHolderId(cs, out_size, placeholder_id); }
Result ContentStorageRepairInvalidFileAttribute(NcmContentStorage* cs) { return ::ncmContentStorageRepairInvalidFileAttribute(cs); }
Result ContentStorageGetRightsIdFromPlaceHolderIdWithCache(NcmContentStorage* cs, NcmRightsId* out_rights_id, const NcmPlaceHolderId* placeholder_id, const NcmContentId* cache_content_id, FsContentAttributes attr) { return ::ncmContentStorageGetRightsIdFromPlaceHolderIdWithCache(cs, out_rights_id, placeholder_id, cache_content_id, attr); }
Result ContentStorageRegisterPath(NcmContentStorage* cs, const NcmContentId* content_id, const char *path) { return ::ncmContentStorageRegisterPath(cs, content_id, path); }
Result ContentStorageClearRegisteredPath(NcmContentStorage* cs) { return ::ncmContentStorageClearRegisteredPath(cs); }
Result ContentStorageGetProgramId(NcmContentStorage* cs, u64* out, const NcmContentId* content_id, FsContentAttributes attr) { return ::ncmContentStorageGetProgramId(cs, out, content_id, attr); }
void ContentMetaDatabaseClose(NcmContentMetaDatabase* db) { ::ncmContentMetaDatabaseClose(db); }
Result ContentMetaDatabaseSet(NcmContentMetaDatabase* db, const NcmContentMetaKey* key, const void* data, u64 data_size) { return ::ncmContentMetaDatabaseSet(db, key, data, data_size); }
Result ContentMetaDatabaseGet(NcmContentMetaDatabase* db, const NcmContentMetaKey* key, u64* out_size, void* out_data, u64 out_data_size) { return ::ncmContentMetaDatabaseGet(db, key, out_size, out_data, out_data_size); }
Result ContentMetaDatabaseRemove(NcmContentMetaDatabase* db, const NcmContentMetaKey *key) { return ::ncmContentMetaDatabaseRemove(db, key); }
Result ContentMetaDatabaseGetContentIdByType(NcmContentMetaDatabase* db, NcmContentId* out_content_id, const NcmContentMetaKey* key, NcmContentType type) { return ::ncmContentMetaDatabaseGetContentIdByType(db, out_content_id, key, type); }
Result ContentMetaDatabaseListContentInfo(NcmContentMetaDatabase* db, s32* out_entries_written, NcmContentInfo* out_info, s32 count, const NcmContentMetaKey* key, s32 start_index) { return ::ncmContentMetaDatabaseListContentInfo(db, out_entries_written, out_info, count, key, start_index); }
Result ContentMetaDatabaseList(NcmContentMetaDatabase* db, s32* out_entries_total, s32* out_entries_written, NcmContentMetaKey* out_keys, s32 count, NcmContentMetaType meta_type, u64 id, u64 id_min, u64 id_max, NcmContentInstallType install_type) { return ::ncmContentMetaDatabaseList(db, out_entries_total, out_entries_written, out_keys, count, meta_type, id, id_min, id_max, install_type); }
Result ContentMetaDatabaseGetLatestContentMetaKey(NcmContentMetaDatabase* db, NcmContentMetaKey* out_key, u64 id) { return ::ncmContentMetaDatabaseGetLatestContentMetaKey(db, out_key, id); }
Result ContentMetaDatabaseListApplication(NcmContentMetaDatabase* db, s32* out_entries_total, s32* out_entries_written, NcmApplicationContentMetaKey* out_keys, s32 count, NcmContentMetaType meta_type) { return ::ncmContentMetaDatabaseListApplication(db, out_entries_total, out_entries_written, out_keys, count, meta_type); }
Result ContentMetaDatabaseHas(NcmContentMetaDatabase* db, bool* out, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseHas(db, out, key); }
Result ContentMetaDatabaseHasAll(NcmContentMetaDatabase* db, bool* out, const NcmContentMetaKey* keys, s32 count) { return ::ncmContentMetaDatabaseHasAll(db, out, keys, count); }
Result ContentMetaDatabaseGetSize(NcmContentMetaDatabase* db, u64* out_size, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseGetSize(db, out_size, key); }
Result ContentMetaDatabaseGetRequiredSystemVersion(NcmContentMetaDatabase* db, u32* out_version, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseGetRequiredSystemVersion(db, out_version, key); }
Result ContentMetaDatabaseGetPatchContentMetaId(NcmContentMetaDatabase* db, u64* out_patch_id, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseGetPatchContentMetaId(db, out_patch_id, key); }
Result ContentMetaDatabaseDisableForcibly(NcmContentMetaDatabase* db) { return ::ncmContentMetaDatabaseDisableForcibly(db); }
Result ContentMetaDatabaseLookupOrphanContent(NcmContentMetaDatabase* db, bool* out_orphaned, const NcmContentId* content_ids, s32 count) { return ::ncmContentMetaDatabaseLookupOrphanContent(db, out_orphaned, content_ids, count); }
Result ContentMetaDatabaseCommit(NcmContentMetaDatabase* db) { return ::ncmContentMetaDatabaseCommit(db); }
Result ContentMetaDatabaseHasContent(NcmContentMetaDatabase* db, bool* out, const NcmContentMetaKey* key, const NcmContentId* content_id) { return ::ncmContentMetaDatabaseHasContent(db, out, key, content_id); }
Result ContentMetaDatabaseListContentMetaInfo(NcmContentMetaDatabase* db, s32* out_entries_written, void* out_meta_info, s32 count, const NcmContentMetaKey* key, s32 start_index) { return ::ncmContentMetaDatabaseListContentMetaInfo(db, out_entries_written, out_meta_info, count, key, start_index); }
Result ContentMetaDatabaseGetAttributes(NcmContentMetaDatabase* db, const NcmContentMetaKey* key, u8* out) { return ::ncmContentMetaDatabaseGetAttributes(db, key, out); }
Result ContentMetaDatabaseGetRequiredApplicationVersion(NcmContentMetaDatabase* db, u32* out_version, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseGetRequiredApplicationVersion(db, out_version, key); }
Result ContentMetaDatabaseGetContentIdByTypeAndIdOffset(NcmContentMetaDatabase* db, NcmContentId* out_content_id, const NcmContentMetaKey* key, NcmContentType type, u8 id_offset) { return ::ncmContentMetaDatabaseGetContentIdByTypeAndIdOffset(db, out_content_id, key, type, id_offset); }
Result ContentMetaDatabaseGetPlatform(NcmContentMetaDatabase* db, u8* out, const NcmContentMetaKey* key) { return ::ncmContentMetaDatabaseGetPlatform(db, out, key); }
void ContentInfoSizeToU64(const NcmContentInfo *info, u64 *out) { ::ncmContentInfoSizeToU64(info, out); }
void U64ToContentInfoSize(const u64 size, NcmContentInfo *info) { ::ncmU64ToContentInfoSize(size, info); }

} // namespace ncm

namespace news {

Result Initialize(NewsServiceType service_type) { return ::newsInitialize(service_type); }
void Exit(void) { ::newsExit(); }
Service * GetServiceSession(void) { return ::newsGetServiceSession(); }
Result CreateNewlyArrivedEventHolder(NewsNewlyArrivedEventHolder *out) { return ::newsCreateNewlyArrivedEventHolder(out); }
Result CreateNewsDataService(NewsDataService *out) { return ::newsCreateNewsDataService(out); }
Result CreateNewsDatabaseService(NewsDatabaseService *out) { return ::newsCreateNewsDatabaseService(out); }
Result CreateOverwriteEventHolder(NewsOverwriteEventHolder *out) { return ::newsCreateOverwriteEventHolder(out); }
Result PostLocalNews(const void *news, size_t size) { return ::newsPostLocalNews(news, size); }
Result SetPassphrase(u64 program_id, const char *passphrase) { return ::newsSetPassphrase(program_id, passphrase); }
Result GetSubscriptionStatus(const char *filter, u32 *status) { return ::newsGetSubscriptionStatus(filter, status); }
Result GetTopicList(u32 channel, u32 *out_count, NewsTopicName *out, u32 max_count) { return ::newsGetTopicList(channel, out_count, out, max_count); }
Result GetSavedataUsage(u64 *current, u64 *total) { return ::newsGetSavedataUsage(current, total); }
Result IsSystemUpdateRequired(bool *out) { return ::newsIsSystemUpdateRequired(out); }
Result GetDatabaseVersion(u32 *version) { return ::newsGetDatabaseVersion(version); }
Result RequestImmediateReception(const char *filter) { return ::newsRequestImmediateReception(filter); }
Result SetSubscriptionStatus(const char *filter, u32 status) { return ::newsSetSubscriptionStatus(filter, status); }
Result ClearStorage(void) { return ::newsClearStorage(); }
Result ClearSubscriptionStatusAll(void) { return ::newsClearSubscriptionStatusAll(); }
Result GetNewsDatabaseDump(void *buffer, u64 size, u64 *out) { return ::newsGetNewsDatabaseDump(buffer, size, out); }
void NewlyArrivedEventHolderClose(NewsNewlyArrivedEventHolder *srv) { ::newsNewlyArrivedEventHolderClose(srv); }
Result NewlyArrivedEventHolderGet(NewsNewlyArrivedEventHolder *srv, Event *out) { return ::newsNewlyArrivedEventHolderGet(srv, out); }
void DataClose(NewsDataService *srv) { ::newsDataClose(srv); }
Result DataOpen(NewsDataService *srv, const char *file_name) { return ::newsDataOpen(srv, file_name); }
Result DataOpenWithNewsRecordV1(NewsDataService *srv, NewsRecordV1 *record) { return ::newsDataOpenWithNewsRecordV1(srv, record); }
Result DataRead(NewsDataService *srv, u64 *bytes_read, u64 offset, void *out, size_t out_size) { return ::newsDataRead(srv, bytes_read, offset, out, out_size); }
Result DataGetSize(NewsDataService *srv, u64 *size) { return ::newsDataGetSize(srv, size); }
Result DataOpenWithNewsRecord(NewsDataService *srv, NewsRecord *record) { return ::newsDataOpenWithNewsRecord(srv, record); }
void DatabaseClose(NewsDatabaseService *srv) { ::newsDatabaseClose(srv); }
Result DatabaseGetListV1(NewsDatabaseService *srv, NewsRecordV1 *out, u32 max_count, const char *where, const char *order, u32 *count, u32 offset) { return ::newsDatabaseGetListV1(srv, out, max_count, where, order, count, offset); }
Result DatabaseCount(NewsDatabaseService *srv, const char *filter, u32 *count) { return ::newsDatabaseCount(srv, filter, count); }
Result DatabaseGetList(NewsDatabaseService *srv, NewsRecord *out, u32 max_count, const char *where, const char *order, u32 *count, u32 offset) { return ::newsDatabaseGetList(srv, out, max_count, where, order, count, offset); }
void OverwriteEventHolderClose(NewsOverwriteEventHolder *srv) { ::newsOverwriteEventHolderClose(srv); }
Result OverwriteEventHolderGet(NewsOverwriteEventHolder *srv, Event *out) { return ::newsOverwriteEventHolderGet(srv, out); }

} // namespace news

namespace nfc {

Result Initialize(NfcServiceType service_type) { return ::nfcInitialize(service_type); }
void Exit(void) { ::nfcExit(); }
Result MfInitialize(void) { return ::nfcMfInitialize(); }
void MfExit(void) { ::nfcMfExit(); }
Service* GetServiceSession(void) { return ::nfcGetServiceSession(); }
Service* GetServiceSession_Interface(void) { return ::nfcGetServiceSession_Interface(); }
Service* MfGetServiceSession(void) { return ::nfcMfGetServiceSession(); }
Service* MfGetServiceSession_Interface(void) { return ::nfcMfGetServiceSession_Interface(); }
Result ListDevices(s32 *total_out, NfcDeviceHandle *out, s32 count) { return ::nfcListDevices(total_out, out, count); }
Result StartDetection(const NfcDeviceHandle *handle, NfcProtocol protocol) { return ::nfcStartDetection(handle, protocol); }
Result StopDetection(const NfcDeviceHandle *handle) { return ::nfcStopDetection(handle); }
Result MfListDevices(s32 *total_out, NfcDeviceHandle *out, s32 count) { return ::nfcMfListDevices(total_out, out, count); }
Result MfStartDetection(const NfcDeviceHandle *handle) { return ::nfcMfStartDetection(handle); }
Result MfStopDetection(const NfcDeviceHandle *handle) { return ::nfcMfStopDetection(handle); }
Result GetTagInfo(const NfcDeviceHandle *handle, NfcTagInfo *out) { return ::nfcGetTagInfo(handle, out); }
Result MfGetTagInfo(const NfcDeviceHandle *handle, NfcTagInfo *out) { return ::nfcMfGetTagInfo(handle, out); }
Result AttachActivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfcAttachActivateEvent(handle, out_event); }
Result AttachDeactivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfcAttachDeactivateEvent(handle, out_event); }
Result MfAttachActivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfcMfAttachActivateEvent(handle, out_event); }
Result MfAttachDeactivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfcMfAttachDeactivateEvent(handle, out_event); }
Result GetState(NfcState *out) { return ::nfcGetState(out); }
Result GetDeviceState(const NfcDeviceHandle *handle, NfcDeviceState *out) { return ::nfcGetDeviceState(handle, out); }
Result GetNpadId(const NfcDeviceHandle *handle, u32 *out) { return ::nfcGetNpadId(handle, out); }
Result MfGetState(NfcState *out) { return ::nfcMfGetState(out); }
Result MfGetDeviceState(const NfcDeviceHandle *handle, NfcMifareDeviceState *out) { return ::nfcMfGetDeviceState(handle, out); }
Result MfGetNpadId(const NfcDeviceHandle *handle, u32 *out) { return ::nfcMfGetNpadId(handle, out); }
Result AttachAvailabilityChangeEvent(Event *out_event) { return ::nfcAttachAvailabilityChangeEvent(out_event); }
Result MfAttachAvailabilityChangeEvent(Event *out_event) { return ::nfcMfAttachAvailabilityChangeEvent(out_event); }
Result IsNfcEnabled(bool *out) { return ::nfcIsNfcEnabled(out); }
Result ReadMifare(const NfcDeviceHandle *handle, NfcMifareReadBlockData *out_block_data, const NfcMifareReadBlockParameter *read_block_parameter, s32 count) { return ::nfcReadMifare(handle, out_block_data, read_block_parameter, count); }
Result WriteMifare(const NfcDeviceHandle *handle, const NfcMifareWriteBlockParameter *write_block_parameter, s32 count) { return ::nfcWriteMifare(handle, write_block_parameter, count); }
Result MfReadMifare(const NfcDeviceHandle *handle, NfcMifareReadBlockData *out_block_data, const NfcMifareReadBlockParameter *read_block_parameter, s32 count) { return ::nfcMfReadMifare(handle, out_block_data, read_block_parameter, count); }
Result MfWriteMifare(const NfcDeviceHandle *handle, const NfcMifareWriteBlockParameter *write_block_parameter, s32 count) { return ::nfcMfWriteMifare(handle, write_block_parameter, count); }
Result SendCommandByPassThrough(const NfcDeviceHandle *handle, u64 timeout, const void* cmd_buf, size_t cmd_buf_size, void* reply_buf, size_t reply_buf_size, u64 *out_size) { return ::nfcSendCommandByPassThrough(handle, timeout, cmd_buf, cmd_buf_size, reply_buf, reply_buf_size, out_size); }
Result KeepPassThroughSession(const NfcDeviceHandle *handle) { return ::nfcKeepPassThroughSession(handle); }
Result ReleasePassThroughSession(const NfcDeviceHandle *handle) { return ::nfcReleasePassThroughSession(handle); }

} // namespace nfc

namespace nfp {

Result LaStartNicknameAndOwnerSettings(const NfpLaAmiiboSettingsStartParam *in_param, const NfpTagInfo *in_tag_info, const NfpRegisterInfo *in_reg_info, NfpTagInfo *out_tag_info, NfcDeviceHandle *handle, bool *reg_info_flag, NfpRegisterInfo *out_reg_info) { return ::nfpLaStartNicknameAndOwnerSettings(in_param, in_tag_info, in_reg_info, out_tag_info, handle, reg_info_flag, out_reg_info); }
Result LaStartGameDataEraser(const NfpLaAmiiboSettingsStartParam *in_param, const NfpTagInfo *in_tag_info, NfpTagInfo *out_tag_info, NfcDeviceHandle *handle) { return ::nfpLaStartGameDataEraser(in_param, in_tag_info, out_tag_info, handle); }
Result LaStartRestorer(const NfpLaAmiiboSettingsStartParam *in_param, const NfpTagInfo *in_tag_info, NfpTagInfo *out_tag_info, NfcDeviceHandle *handle) { return ::nfpLaStartRestorer(in_param, in_tag_info, out_tag_info, handle); }
Result LaStartFormatter(const NfpLaAmiiboSettingsStartParam *in_param, NfpTagInfo *out_tag_info, NfcDeviceHandle *handle) { return ::nfpLaStartFormatter(in_param, out_tag_info, handle); }
Result Initialize(NfpServiceType service_type) { return ::nfpInitialize(service_type); }
void Exit(void) { ::nfpExit(); }
Service* GetServiceSession(void) { return ::nfpGetServiceSession(); }
Service* GetServiceSession_Interface(void) { return ::nfpGetServiceSession_Interface(); }
Result ListDevices(s32 *total_out, NfcDeviceHandle *out, s32 count) { return ::nfpListDevices(total_out, out, count); }
Result StartDetection(const NfcDeviceHandle *handle) { return ::nfpStartDetection(handle); }
Result StopDetection(const NfcDeviceHandle *handle) { return ::nfpStopDetection(handle); }
Result Mount(const NfcDeviceHandle *handle, NfpDeviceType device_type, NfpMountTarget mount_target) { return ::nfpMount(handle, device_type, mount_target); }
Result Unmount(const NfcDeviceHandle *handle) { return ::nfpUnmount(handle); }
Result OpenApplicationArea(const NfcDeviceHandle *handle, u32 app_id) { return ::nfpOpenApplicationArea(handle, app_id); }
Result GetApplicationArea(const NfcDeviceHandle *handle, void* buf, size_t buf_size, u32 *out_size) { return ::nfpGetApplicationArea(handle, buf, buf_size, out_size); }
Result SetApplicationArea(const NfcDeviceHandle *handle, const void* buf, size_t buf_size) { return ::nfpSetApplicationArea(handle, buf, buf_size); }
Result Flush(const NfcDeviceHandle *handle) { return ::nfpFlush(handle); }
Result Restore(const NfcDeviceHandle *handle) { return ::nfpRestore(handle); }
Result CreateApplicationArea(const NfcDeviceHandle *handle, u32 app_id, const void* buf, size_t buf_size) { return ::nfpCreateApplicationArea(handle, app_id, buf, buf_size); }
Result RecreateApplicationArea(const NfcDeviceHandle *handle, u32 app_id, const void* buf, size_t buf_size) { return ::nfpRecreateApplicationArea(handle, app_id, buf, buf_size); }
Result GetApplicationAreaSize(const NfcDeviceHandle *handle, u32 *out_app_area_size) { return ::nfpGetApplicationAreaSize(handle, out_app_area_size); }
Result DeleteApplicationArea(const NfcDeviceHandle *handle) { return ::nfpDeleteApplicationArea(handle); }
Result ExistsApplicationArea(const NfcDeviceHandle *handle, bool *out) { return ::nfpExistsApplicationArea(handle, out); }
Result GetTagInfo(const NfcDeviceHandle *handle, NfpTagInfo *out) { return ::nfpGetTagInfo(handle, out); }
Result GetRegisterInfo(const NfcDeviceHandle *handle, NfpRegisterInfo *out) { return ::nfpGetRegisterInfo(handle, out); }
Result GetCommonInfo(const NfcDeviceHandle *handle, NfpCommonInfo *out) { return ::nfpGetCommonInfo(handle, out); }
Result GetModelInfo(const NfcDeviceHandle *handle, NfpModelInfo *out) { return ::nfpGetModelInfo(handle, out); }
Result GetAdminInfo(const NfcDeviceHandle *handle, NfpAdminInfo *out) { return ::nfpGetAdminInfo(handle, out); }
Result AttachActivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfpAttachActivateEvent(handle, out_event); }
Result AttachDeactivateEvent(const NfcDeviceHandle *handle, Event *out_event) { return ::nfpAttachDeactivateEvent(handle, out_event); }
Result GetState(NfcState *out) { return ::nfpGetState(out); }
Result GetDeviceState(const NfcDeviceHandle *handle, NfpDeviceState *out) { return ::nfpGetDeviceState(handle, out); }
Result GetNpadId(const NfcDeviceHandle *handle, u32 *out) { return ::nfpGetNpadId(handle, out); }
Result AttachAvailabilityChangeEvent(Event *out_event) { return ::nfpAttachAvailabilityChangeEvent(out_event); }
Result Format(const NfcDeviceHandle *handle) { return ::nfpFormat(handle); }
Result GetRegisterInfoPrivate(const NfcDeviceHandle *handle, NfpRegisterInfoPrivate *out) { return ::nfpGetRegisterInfoPrivate(handle, out); }
Result SetRegisterInfoPrivate(const NfcDeviceHandle *handle, const NfpRegisterInfoPrivate *register_info_private) { return ::nfpSetRegisterInfoPrivate(handle, register_info_private); }
Result DeleteRegisterInfo(const NfcDeviceHandle *handle) { return ::nfpDeleteRegisterInfo(handle); }
Result GetAll(const NfcDeviceHandle *handle, NfpData *out) { return ::nfpGetAll(handle, out); }
Result SetAll(const NfcDeviceHandle *handle, const NfpData *nfp_data) { return ::nfpSetAll(handle, nfp_data); }
Result FlushDebug(const NfcDeviceHandle *handle) { return ::nfpFlushDebug(handle); }
Result BreakTag(const NfcDeviceHandle *handle, NfpBreakType break_type) { return ::nfpBreakTag(handle, break_type); }
Result ReadBackupData(const NfcDeviceHandle *handle, void* out_buf, size_t buf_size, u32 *out_size) { return ::nfpReadBackupData(handle, out_buf, buf_size, out_size); }
Result WriteBackupData(const NfcDeviceHandle *handle, const void* buf, size_t buf_size) { return ::nfpWriteBackupData(handle, buf, buf_size); }
Result WriteNtf(const NfcDeviceHandle *handle, u32 write_type, const void* buf, size_t buf_size) { return ::nfpWriteNtf(handle, write_type, buf, buf_size); }

} // namespace nfp

namespace nifm {

Result LaHandleNetworkRequestResult(NifmRequest* r) { return ::nifmLaHandleNetworkRequestResult(r); }
Result Initialize(NifmServiceType service_type) { return ::nifmInitialize(service_type); }
void Exit(void) { ::nifmExit(); }
Service* GetServiceSession_StaticService(void) { return ::nifmGetServiceSession_StaticService(); }
Service* GetServiceSession_GeneralService(void) { return ::nifmGetServiceSession_GeneralService(); }
NifmClientId GetClientId(void) { return ::nifmGetClientId(); }
Result CreateRequest(NifmRequest* r, bool autoclear) { return ::nifmCreateRequest(r, autoclear); }
Result GetCurrentNetworkProfile(NifmNetworkProfileData *profile) { return ::nifmGetCurrentNetworkProfile(profile); }
Result EnumerateNetworkProfiles(NifmNetworkProfileType type, NifmNetworkProfileBasicInfo* buffer, s32 max_entries, s32* total_entries) { return ::nifmEnumerateNetworkProfiles(type, buffer, max_entries, total_entries); }
Result GetNetworkProfile(Uuid uuid, NifmNetworkProfileData *profile) { return ::nifmGetNetworkProfile(uuid, profile); }
Result SetNetworkProfile(const NifmNetworkProfileData *profile, Uuid *uuid) { return ::nifmSetNetworkProfile(profile, uuid); }
Result GetCurrentIpAddress(u32* out) { return ::nifmGetCurrentIpAddress(out); }
Result GetCurrentIpConfigInfo(u32 *current_addr, u32 *subnet_mask, u32 *gateway, u32 *primary_dns_server, u32 *secondary_dns_server) { return ::nifmGetCurrentIpConfigInfo(current_addr, subnet_mask, gateway, primary_dns_server, secondary_dns_server); }
Result SetWirelessCommunicationEnabled(bool enable) { return ::nifmSetWirelessCommunicationEnabled(enable); }
Result IsWirelessCommunicationEnabled(bool* out) { return ::nifmIsWirelessCommunicationEnabled(out); }
Result GetInternetConnectionStatus(NifmInternetConnectionType* connectionType, u32* wifiStrength, NifmInternetConnectionStatus* connectionStatus) { return ::nifmGetInternetConnectionStatus(connectionType, wifiStrength, connectionStatus); }
Result IsEthernetCommunicationEnabled(bool* out) { return ::nifmIsEthernetCommunicationEnabled(out); }
bool IsAnyInternetRequestAccepted(NifmClientId id) { return ::nifmIsAnyInternetRequestAccepted(id); }
Result IsAnyForegroundRequestAccepted(bool* out) { return ::nifmIsAnyForegroundRequestAccepted(out); }
Result PutToSleep(void) { return ::nifmPutToSleep(); }
Result WakeUp(void) { return ::nifmWakeUp(); }
Result SetWowlDelayedWakeTime(s32 val) { return ::nifmSetWowlDelayedWakeTime(val); }
void RequestClose(NifmRequest* r) { ::nifmRequestClose(r); }
Result GetRequestState(NifmRequest* r, NifmRequestState *out) { return ::nifmGetRequestState(r, out); }
Result GetResult(NifmRequest* r) { return ::nifmGetResult(r); }
Result RequestCancel(NifmRequest* r) { return ::nifmRequestCancel(r); }
Result RequestSubmit(NifmRequest* r) { return ::nifmRequestSubmit(r); }
Result RequestSubmitAndWait(NifmRequest* r) { return ::nifmRequestSubmitAndWait(r); }
Result RequestGetAppletInfo(NifmRequest* r, u32 theme_color, void* buffer, size_t size, u32 *applet_id, u32 *mode, u32 *out_size) { return ::nifmRequestGetAppletInfo(r, theme_color, buffer, size, applet_id, mode, out_size); }
Result RequestSetKeptInSleep(NifmRequest* r, bool flag) { return ::nifmRequestSetKeptInSleep(r, flag); }
Result RequestRegisterSocketDescriptor(NifmRequest* r, int sockfd) { return ::nifmRequestRegisterSocketDescriptor(r, sockfd); }
Result RequestUnregisterSocketDescriptor(NifmRequest* r, int sockfd) { return ::nifmRequestUnregisterSocketDescriptor(r, sockfd); }
Result RequestSetNetworkProfileId(NifmRequest* r, Uuid uuid) { return ::nifmRequestSetNetworkProfileId(r, uuid); }

} // namespace nifm

namespace nim {

Result Initialize(void) { return ::nimInitialize(); }
void Exit(void) { ::nimExit(); }
Service* GetServiceSession(void) { return ::nimGetServiceSession(); }
Result ListSystemUpdateTask(s32 *out_count, NimSystemUpdateTaskId *out_task_ids, size_t max_task_ids) { return ::nimListSystemUpdateTask(out_count, out_task_ids, max_task_ids); }
Result DestroySystemUpdateTask(const NimSystemUpdateTaskId *task_id) { return ::nimDestroySystemUpdateTask(task_id); }

} // namespace nim

namespace notif {

Result Initialize(NotifServiceType service_type) { return ::notifInitialize(service_type); }
void Exit(void) { ::notifExit(); }
Service* GetServiceSession(void) { return ::notifGetServiceSession(); }
void AlarmSettingCreate(NotifAlarmSetting *alarm_setting) { ::notifAlarmSettingCreate(alarm_setting); }
void AlarmSettingSetIsMuted(NotifAlarmSetting *alarm_setting, bool flag) { ::notifAlarmSettingSetIsMuted(alarm_setting, flag); }
void AlarmSettingSetUid(NotifAlarmSetting *alarm_setting, AccountUid uid) { ::notifAlarmSettingSetUid(alarm_setting, uid); }
Result AlarmSettingIsEnabled(NotifAlarmSetting *alarm_setting, u32 day_of_week, bool *out) { return ::notifAlarmSettingIsEnabled(alarm_setting, day_of_week, out); }
Result AlarmSettingGet(NotifAlarmSetting *alarm_setting, u32 day_of_week, NotifAlarmTime *out) { return ::notifAlarmSettingGet(alarm_setting, day_of_week, out); }
Result AlarmSettingEnable(NotifAlarmSetting *alarm_setting, u32 day_of_week, s32 hour, s32 minute) { return ::notifAlarmSettingEnable(alarm_setting, day_of_week, hour, minute); }
Result AlarmSettingDisable(NotifAlarmSetting *alarm_setting, u32 day_of_week) { return ::notifAlarmSettingDisable(alarm_setting, day_of_week); }
Result RegisterAlarmSetting(u16 *alarm_setting_id, const NotifAlarmSetting *alarm_setting, const void* buffer, size_t size) { return ::notifRegisterAlarmSetting(alarm_setting_id, alarm_setting, buffer, size); }
Result UpdateAlarmSetting(const NotifAlarmSetting *alarm_setting, const void* buffer, size_t size) { return ::notifUpdateAlarmSetting(alarm_setting, buffer, size); }
Result ListAlarmSettings(NotifAlarmSetting *alarm_settings, s32 count, s32 *total_out) { return ::notifListAlarmSettings(alarm_settings, count, total_out); }
Result LoadApplicationParameter(u16 alarm_setting_id, void* buffer, size_t size, u32 *actual_size) { return ::notifLoadApplicationParameter(alarm_setting_id, buffer, size, actual_size); }
Result DeleteAlarmSetting(u16 alarm_setting_id) { return ::notifDeleteAlarmSetting(alarm_setting_id); }
Result GetNotificationSystemEvent(Event *out_event) { return ::notifGetNotificationSystemEvent(out_event); }
Result TryPopNotifiedApplicationParameter(void* buffer, u64 size, u64 *out_size) { return ::notifTryPopNotifiedApplicationParameter(buffer, size, out_size); }

} // namespace notif

namespace ns {

Result Initialize(void) { return ::nsInitialize(); }
void Exit(void) { ::nsExit(); }
Service* GetServiceSession_GetterInterface(void) { return ::nsGetServiceSession_GetterInterface(); }
Service* GetServiceSession_ApplicationManagerInterface(void) { return ::nsGetServiceSession_ApplicationManagerInterface(); }
Result GetDynamicRightsInterface(Service* srv_out) { return ::nsGetDynamicRightsInterface(srv_out); }
Result GetReadOnlyApplicationControlDataInterface(Service* srv_out) { return ::nsGetReadOnlyApplicationControlDataInterface(srv_out); }
Result GetReadOnlyApplicationRecordInterface(Service* srv_out) { return ::nsGetReadOnlyApplicationRecordInterface(srv_out); }
Result GetECommerceInterface(Service* srv_out) { return ::nsGetECommerceInterface(srv_out); }
Result GetApplicationVersionInterface(Service* srv_out) { return ::nsGetApplicationVersionInterface(srv_out); }
Result GetFactoryResetInterface(Service* srv_out) { return ::nsGetFactoryResetInterface(srv_out); }
Result GetAccountProxyInterface(Service* srv_out) { return ::nsGetAccountProxyInterface(srv_out); }
Result GetApplicationManagerInterface(Service* srv_out) { return ::nsGetApplicationManagerInterface(srv_out); }
Result GetDownloadTaskInterface(Service* srv_out) { return ::nsGetDownloadTaskInterface(srv_out); }
Result GetContentManagementInterface(Service* srv_out) { return ::nsGetContentManagementInterface(srv_out); }
Result GetDocumentInterface(Service* srv_out) { return ::nsGetDocumentInterface(srv_out); }
Result GetApplicationControlData(NsApplicationControlSource source, u64 application_id, NsApplicationControlData* buffer, size_t size, u64* actual_size) { return ::nsGetApplicationControlData(source, application_id, buffer, size, actual_size); }
Result GetApplicationControlData2(NsApplicationControlSource source, u64 application_id, NsApplicationControlData* buffer, size_t size, u8 flag1, u8 acd_idx, u64* actual_size, u32* unk) { return ::nsGetApplicationControlData2(source, application_id, buffer, size, flag1, acd_idx, actual_size, unk); }
Result GetApplicationDesiredLanguage(NacpStruct *nacp, NacpLanguageEntry **langentry) { return ::nsGetApplicationDesiredLanguage(nacp, langentry); }
Result RequestLinkDevice(AsyncResult *a, AccountUid uid) { return ::nsRequestLinkDevice(a, uid); }
Result RequestSyncRights(AsyncResult *a) { return ::nsRequestSyncRights(a); }
Result RequestUnlinkDevice(AsyncResult *a, AccountUid uid) { return ::nsRequestUnlinkDevice(a, uid); }
Result ResetToFactorySettings(void) { return ::nsResetToFactorySettings(); }
Result ResetToFactorySettingsWithoutUserSaveData(void) { return ::nsResetToFactorySettingsWithoutUserSaveData(); }
Result ResetToFactorySettingsForRefurbishment(void) { return ::nsResetToFactorySettingsForRefurbishment(); }
Result ResetToFactorySettingsWithPlatformRegion(void) { return ::nsResetToFactorySettingsWithPlatformRegion(); }
Result ResetToFactorySettingsWithPlatformRegionAuthentication(void) { return ::nsResetToFactorySettingsWithPlatformRegionAuthentication(); }
Result ListApplicationRecord(NsApplicationRecord* records, s32 count, s32 entry_offset, s32* out_entrycount) { return ::nsListApplicationRecord(records, count, entry_offset, out_entrycount); }
Result GetApplicationRecordUpdateSystemEvent(Event* out_event) { return ::nsGetApplicationRecordUpdateSystemEvent(out_event); }
Result GetApplicationViewDeprecated(NsApplicationViewDeprecated *views, const u64 *application_ids, s32 count) { return ::nsGetApplicationViewDeprecated(views, application_ids, count); }
Result DeleteApplicationEntity(u64 application_id) { return ::nsDeleteApplicationEntity(application_id); }
Result DeleteApplicationCompletely(u64 application_id) { return ::nsDeleteApplicationCompletely(application_id); }
Result DeleteRedundantApplicationEntity(void) { return ::nsDeleteRedundantApplicationEntity(); }
Result IsApplicationEntityMovable(u64 application_id, NcmStorageId storage_id, bool *out) { return ::nsIsApplicationEntityMovable(application_id, storage_id, out); }
Result MoveApplicationEntity(u64 application_id, NcmStorageId storage_id) { return ::nsMoveApplicationEntity(application_id, storage_id); }
Result RequestApplicationUpdateInfo(AsyncValue *a, u64 application_id) { return ::nsRequestApplicationUpdateInfo(a, application_id); }
Result CancelApplicationDownload(u64 application_id) { return ::nsCancelApplicationDownload(application_id); }
Result ResumeApplicationDownload(u64 application_id) { return ::nsResumeApplicationDownload(application_id); }
Result CheckApplicationLaunchVersion(u64 application_id) { return ::nsCheckApplicationLaunchVersion(application_id); }
Result CalculateApplicationDownloadRequiredSize(u64 application_id, NcmStorageId *storage_id, s64 *size) { return ::nsCalculateApplicationDownloadRequiredSize(application_id, storage_id, size); }
Result CleanupSdCard(void) { return ::nsCleanupSdCard(); }
Result GetSdCardMountStatusChangedEvent(Event* out_event) { return ::nsGetSdCardMountStatusChangedEvent(out_event); }
Result GetGameCardUpdateDetectionEvent(Event* out_event) { return ::nsGetGameCardUpdateDetectionEvent(out_event); }
Result DisableApplicationAutoDelete(u64 application_id) { return ::nsDisableApplicationAutoDelete(application_id); }
Result EnableApplicationAutoDelete(u64 application_id) { return ::nsEnableApplicationAutoDelete(application_id); }
Result SetApplicationTerminateResult(u64 application_id, Result res) { return ::nsSetApplicationTerminateResult(application_id, res); }
Result ClearApplicationTerminateResult(u64 application_id) { return ::nsClearApplicationTerminateResult(application_id); }
Result GetLastSdCardMountUnexpectedResult(void) { return ::nsGetLastSdCardMountUnexpectedResult(); }
Result GetRequestServerStopper(NsRequestServerStopper *r) { return ::nsGetRequestServerStopper(r); }
Result CancelApplicationApplyDelta(u64 application_id) { return ::nsCancelApplicationApplyDelta(application_id); }
Result ResumeApplicationApplyDelta(u64 application_id) { return ::nsResumeApplicationApplyDelta(application_id); }
Result CalculateApplicationApplyDeltaRequiredSize(u64 application_id, NcmStorageId *storage_id, s64 *size) { return ::nsCalculateApplicationApplyDeltaRequiredSize(application_id, storage_id, size); }
Result ResumeAll(void) { return ::nsResumeAll(); }
Result GetStorageSize(NcmStorageId storage_id, s64 *total_space_size, s64 *free_space_size) { return ::nsGetStorageSize(storage_id, total_space_size, free_space_size); }
Result RequestUpdateApplication2(AsyncResult *a, u64 application_id) { return ::nsRequestUpdateApplication2(a, application_id); }
Result DeleteUserSaveDataAll(NsProgressMonitorForDeleteUserSaveDataAll *p, AccountUid uid) { return ::nsDeleteUserSaveDataAll(p, uid); }
Result DeleteUserSystemSaveData(AccountUid uid, u64 system_save_data_id) { return ::nsDeleteUserSystemSaveData(uid, system_save_data_id); }
Result DeleteSaveData(FsSaveDataSpaceId save_data_space_id, u64 save_data_id) { return ::nsDeleteSaveData(save_data_space_id, save_data_id); }
Result UnregisterNetworkServiceAccount(AccountUid uid) { return ::nsUnregisterNetworkServiceAccount(uid); }
Result UnregisterNetworkServiceAccountWithUserSaveDataDeletion(AccountUid uid) { return ::nsUnregisterNetworkServiceAccountWithUserSaveDataDeletion(uid); }
Result RequestDownloadApplicationControlData(AsyncResult *a, u64 application_id) { return ::nsRequestDownloadApplicationControlData(a, application_id); }
Result ListApplicationTitle(AsyncValue *a, NsApplicationControlSource source, const u64 *application_ids, s32 count, void* buffer, size_t size) { return ::nsListApplicationTitle(a, source, application_ids, count, buffer, size); }
Result ListApplicationTitle2(AsyncValue *a, NsApplicationControlSource source, const u64 *application_ids, s32 count, void* buffer, size_t size) { return ::nsListApplicationTitle2(a, source, application_ids, count, buffer, size); }
Result ListApplicationIcon(AsyncValue *a, NsApplicationControlSource source, const u64 *application_ids, s32 count, void* buffer, size_t size) { return ::nsListApplicationIcon(a, source, application_ids, count, buffer, size); }
Result RequestCheckGameCardRegistration(AsyncResult *a, u64 application_id) { return ::nsRequestCheckGameCardRegistration(a, application_id); }
Result RequestGameCardRegistrationGoldPoint(AsyncValue *a, AccountUid uid, u64 application_id) { return ::nsRequestGameCardRegistrationGoldPoint(a, uid, application_id); }
Result RequestRegisterGameCard(AsyncResult *a, AccountUid uid, u64 application_id, s32 inval) { return ::nsRequestRegisterGameCard(a, uid, application_id, inval); }
Result GetGameCardMountFailureEvent(Event* out_event) { return ::nsGetGameCardMountFailureEvent(out_event); }
Result IsGameCardInserted(bool *out) { return ::nsIsGameCardInserted(out); }
Result EnsureGameCardAccess(void) { return ::nsEnsureGameCardAccess(); }
Result GetLastGameCardMountFailureResult(void) { return ::nsGetLastGameCardMountFailureResult(); }
Result ListApplicationIdOnGameCard(u64 *application_ids, s32 count, s32 *total_out) { return ::nsListApplicationIdOnGameCard(application_ids, count, total_out); }
Result TouchApplication(u64 application_id) { return ::nsTouchApplication(application_id); }
Result IsApplicationUpdateRequested(u64 application_id, bool *flag, u32 *out) { return ::nsIsApplicationUpdateRequested(application_id, flag, out); }
Result WithdrawApplicationUpdateRequest(u64 application_id) { return ::nsWithdrawApplicationUpdateRequest(application_id); }
Result RequestVerifyAddOnContentsRights(NsProgressAsyncResult *a, u64 application_id) { return ::nsRequestVerifyAddOnContentsRights(a, application_id); }
Result RequestVerifyApplication(NsProgressAsyncResult *a, u64 application_id, u32 unk, void* buffer, size_t size) { return ::nsRequestVerifyApplication(a, application_id, unk, buffer, size); }
Result IsAnyApplicationEntityInstalled(u64 application_id, bool *out) { return ::nsIsAnyApplicationEntityInstalled(application_id, out); }
Result CleanupUnavailableAddOnContents(u64 application_id, AccountUid uid) { return ::nsCleanupUnavailableAddOnContents(application_id, uid); }
Result EstimateSizeToMove(u8 *storage_ids, s32 count, NcmStorageId storage_id, u32 flags, u64 application_id, s64 *out) { return ::nsEstimateSizeToMove(storage_ids, count, storage_id, flags, application_id, out); }
Result FormatSdCard(void) { return ::nsFormatSdCard(); }
Result NeedsSystemUpdateToFormatSdCard(bool *out) { return ::nsNeedsSystemUpdateToFormatSdCard(out); }
Result GetLastSdCardFormatUnexpectedResult(void) { return ::nsGetLastSdCardFormatUnexpectedResult(); }
Result GetApplicationView(NsApplicationView *views, const u64 *application_ids, s32 count) { return ::nsGetApplicationView(views, application_ids, count); }
Result GetApplicationViewDownloadErrorContext(u64 application_id, ErrorContext *context) { return ::nsGetApplicationViewDownloadErrorContext(application_id, context); }
Result GetApplicationViewWithPromotionInfo(NsApplicationViewWithPromotionInfo *out, const u64 *application_ids, s32 count) { return ::nsGetApplicationViewWithPromotionInfo(out, application_ids, count); }
Result RequestDownloadApplicationPrepurchasedRights(AsyncResult *a, u64 application_id) { return ::nsRequestDownloadApplicationPrepurchasedRights(a, application_id); }
Result GetSystemDeliveryInfo(NsSystemDeliveryInfo *info) { return ::nsGetSystemDeliveryInfo(info); }
Result SelectLatestSystemDeliveryInfo(const NsSystemDeliveryInfo *sys_list, s32 sys_count, const NsSystemDeliveryInfo *base_info, const NsApplicationDeliveryInfo *app_list, s32 app_count, s32 *index) { return ::nsSelectLatestSystemDeliveryInfo(sys_list, sys_count, base_info, app_list, app_count, index); }
Result VerifyDeliveryProtocolVersion(const NsSystemDeliveryInfo *info) { return ::nsVerifyDeliveryProtocolVersion(info); }
Result GetApplicationDeliveryInfo(NsApplicationDeliveryInfo *info, s32 count, u64 application_id, u32 attr, s32 *total_out) { return ::nsGetApplicationDeliveryInfo(info, count, application_id, attr, total_out); }
Result HasAllContentsToDeliver(const NsApplicationDeliveryInfo* info, s32 count, bool *out) { return ::nsHasAllContentsToDeliver(info, count, out); }
Result CompareApplicationDeliveryInfo(const NsApplicationDeliveryInfo *info0, s32 count0, const NsApplicationDeliveryInfo *info1, s32 count1, s32 *out) { return ::nsCompareApplicationDeliveryInfo(info0, count0, info1, count1, out); }
Result CanDeliverApplication(const NsApplicationDeliveryInfo *info0, s32 count0, const NsApplicationDeliveryInfo *info1, s32 count1, bool *out) { return ::nsCanDeliverApplication(info0, count0, info1, count1, out); }
Result ListContentMetaKeyToDeliverApplication(NcmContentMetaKey *meta, s32 meta_count, s32 meta_index, const NsApplicationDeliveryInfo *info, s32 info_count, s32 *total_out) { return ::nsListContentMetaKeyToDeliverApplication(meta, meta_count, meta_index, info, info_count, total_out); }
Result NeedsSystemUpdateToDeliverApplication(const NsApplicationDeliveryInfo *info, s32 count, const NsSystemDeliveryInfo *sys_info, bool *out) { return ::nsNeedsSystemUpdateToDeliverApplication(info, count, sys_info, out); }
Result EstimateRequiredSize(const NcmContentMetaKey *meta, s32 count, s64 *out) { return ::nsEstimateRequiredSize(meta, count, out); }
Result RequestReceiveApplication(AsyncResult *a, u32 addr, u16 port, u64 application_id, const NcmContentMetaKey *meta, s32 count, NcmStorageId storage_id) { return ::nsRequestReceiveApplication(a, addr, port, application_id, meta, count, storage_id); }
Result CommitReceiveApplication(u64 application_id) { return ::nsCommitReceiveApplication(application_id); }
Result GetReceiveApplicationProgress(u64 application_id, NsReceiveApplicationProgress *out) { return ::nsGetReceiveApplicationProgress(application_id, out); }
Result RequestSendApplication(AsyncResult *a, u32 addr, u16 port, u64 application_id, const NcmContentMetaKey *meta, s32 count) { return ::nsRequestSendApplication(a, addr, port, application_id, meta, count); }
Result GetSendApplicationProgress(u64 application_id, NsSendApplicationProgress *out) { return ::nsGetSendApplicationProgress(application_id, out); }
Result CompareSystemDeliveryInfo(const NsSystemDeliveryInfo *info0, const NsSystemDeliveryInfo *info1, s32 *out) { return ::nsCompareSystemDeliveryInfo(info0, info1, out); }
Result ListNotCommittedContentMeta(NcmContentMetaKey *meta, s32 count, u64 application_id, s32 unk, s32 *total_out) { return ::nsListNotCommittedContentMeta(meta, count, application_id, unk, total_out); }
Result GetApplicationDeliveryInfoHash(const NsApplicationDeliveryInfo *info, s32 count, u8 *out_hash) { return ::nsGetApplicationDeliveryInfoHash(info, count, out_hash); }
Result GetApplicationTerminateResult(u64 application_id, Result *res) { return ::nsGetApplicationTerminateResult(application_id, res); }
Result GetApplicationRightsOnClient(NsApplicationRightsOnClient *rights, s32 count, u64 application_id, AccountUid uid, u32 flags, s32 *total_out) { return ::nsGetApplicationRightsOnClient(rights, count, application_id, uid, flags, total_out); }
Result RequestNoDownloadRightsErrorResolution(AsyncValue *a, u64 application_id) { return ::nsRequestNoDownloadRightsErrorResolution(a, application_id); }
Result RequestResolveNoDownloadRightsError(AsyncValue *a, u64 application_id) { return ::nsRequestResolveNoDownloadRightsError(a, application_id); }
Result GetPromotionInfo(NsPromotionInfo *promotion, u64 application_id, AccountUid uid) { return ::nsGetPromotionInfo(promotion, application_id, uid); }
Result ClearTaskStatusList(void) { return ::nsClearTaskStatusList(); }
Result RequestDownloadTaskList(void) { return ::nsRequestDownloadTaskList(); }
Result RequestEnsureDownloadTask(AsyncResult *a) { return ::nsRequestEnsureDownloadTask(a); }
Result ListDownloadTaskStatus(NsDownloadTaskStatus* tasks, s32 count, s32 *total_out) { return ::nsListDownloadTaskStatus(tasks, count, total_out); }
Result RequestDownloadTaskListData(AsyncValue *a) { return ::nsRequestDownloadTaskListData(a); }
Result TryCommitCurrentApplicationDownloadTask(void) { return ::nsTryCommitCurrentApplicationDownloadTask(); }
Result EnableAutoCommit(void) { return ::nsEnableAutoCommit(); }
Result DisableAutoCommit(void) { return ::nsDisableAutoCommit(); }
Result TriggerDynamicCommitEvent(void) { return ::nsTriggerDynamicCommitEvent(); }
Result CalculateApplicationOccupiedSize(u64 application_id, NsApplicationOccupiedSize *out) { return ::nsCalculateApplicationOccupiedSize(application_id, out); }
Result CheckSdCardMountStatus(void) { return ::nsCheckSdCardMountStatus(); }
Result GetTotalSpaceSize(NcmStorageId storage_id, s64 *size) { return ::nsGetTotalSpaceSize(storage_id, size); }
Result GetFreeSpaceSize(NcmStorageId storage_id, s64 *size) { return ::nsGetFreeSpaceSize(storage_id, size); }
Result CountApplicationContentMeta(u64 application_id, s32 *out) { return ::nsCountApplicationContentMeta(application_id, out); }
Result ListApplicationContentMetaStatus(u64 application_id, s32 index, NsApplicationContentMetaStatus* list, s32 count, s32* out_entrycount) { return ::nsListApplicationContentMetaStatus(application_id, index, list, count, out_entrycount); }
Result IsAnyApplicationRunning(bool *out) { return ::nsIsAnyApplicationRunning(out); }
void RequestServerStopperClose(NsRequestServerStopper *r) { ::nsRequestServerStopperClose(r); }
Result ProgressMonitorForDeleteUserSaveDataAllClose(NsProgressMonitorForDeleteUserSaveDataAll *p) { return ::nsProgressMonitorForDeleteUserSaveDataAllClose(p); }
Result ProgressMonitorForDeleteUserSaveDataAllGetSystemEvent(NsProgressMonitorForDeleteUserSaveDataAll *p, Event* out_event) { return ::nsProgressMonitorForDeleteUserSaveDataAllGetSystemEvent(p, out_event); }
Result ProgressMonitorForDeleteUserSaveDataAllIsFinished(NsProgressMonitorForDeleteUserSaveDataAll *p, bool *out) { return ::nsProgressMonitorForDeleteUserSaveDataAllIsFinished(p, out); }
Result ProgressMonitorForDeleteUserSaveDataAllGetResult(NsProgressMonitorForDeleteUserSaveDataAll *p) { return ::nsProgressMonitorForDeleteUserSaveDataAllGetResult(p); }
Result ProgressMonitorForDeleteUserSaveDataAllGetProgress(NsProgressMonitorForDeleteUserSaveDataAll *p, NsProgressForDeleteUserSaveDataAll *progress) { return ::nsProgressMonitorForDeleteUserSaveDataAllGetProgress(p, progress); }
void ProgressAsyncResultClose(NsProgressAsyncResult *a) { ::nsProgressAsyncResultClose(a); }
Result ProgressAsyncResultWait(NsProgressAsyncResult *a, u64 timeout) { return ::nsProgressAsyncResultWait(a, timeout); }
Result ProgressAsyncResultGet(NsProgressAsyncResult *a) { return ::nsProgressAsyncResultGet(a); }
Result ProgressAsyncResultCancel(NsProgressAsyncResult *a) { return ::nsProgressAsyncResultCancel(a); }
Result ProgressAsyncResultGetProgress(NsProgressAsyncResult *a, void* buffer, size_t size) { return ::nsProgressAsyncResultGetProgress(a, buffer, size); }
Result ProgressAsyncResultGetDetailResult(NsProgressAsyncResult *a) { return ::nsProgressAsyncResultGetDetailResult(a); }
Result ProgressAsyncResultGetErrorContext(NsProgressAsyncResult *a, ErrorContext *context) { return ::nsProgressAsyncResultGetErrorContext(a, context); }

} // namespace ns

namespace nsdev {

Result Initialize(void) { return ::nsdevInitialize(); }
void Exit(void) { ::nsdevExit(); }
Service* GetServiceSession(void) { return ::nsdevGetServiceSession(); }
Result LaunchProgram(u64* out_pid, const NsLaunchProperties* properties, u32 flags) { return ::nsdevLaunchProgram(out_pid, properties, flags); }
Result TerminateProcess(u64 pid) { return ::nsdevTerminateProcess(pid); }
Result TerminateProgram(u64 tid) { return ::nsdevTerminateProgram(tid); }
Result GetShellEvent(Event* out_event) { return ::nsdevGetShellEvent(out_event); }
Result GetShellEventInfo(NsShellEventInfo* out) { return ::nsdevGetShellEventInfo(out); }
Result TerminateApplication(void) { return ::nsdevTerminateApplication(); }
Result PrepareLaunchProgramFromHost(NsLaunchProperties* out, const char* path, size_t path_len) { return ::nsdevPrepareLaunchProgramFromHost(out, path, path_len); }
Result LaunchApplicationForDevelop(u64* out_pid, u64 application_id, u32 flags) { return ::nsdevLaunchApplicationForDevelop(out_pid, application_id, flags); }
Result LaunchApplicationFromHost(u64* out_pid, const char* path, size_t path_len, u32 flags) { return ::nsdevLaunchApplicationFromHost(out_pid, path, path_len, flags); }
Result LaunchApplicationWithStorageIdForDevelop(u64* out_pid, u64 application_id, u32 flags, u8 app_storage_id, u8 patch_storage_id) { return ::nsdevLaunchApplicationWithStorageIdForDevelop(out_pid, application_id, flags, app_storage_id, patch_storage_id); }
Result IsSystemMemoryResourceLimitBoosted(bool* out) { return ::nsdevIsSystemMemoryResourceLimitBoosted(out); }
Result GetRunningApplicationProcessIdForDevelop(u64* out_pid) { return ::nsdevGetRunningApplicationProcessIdForDevelop(out_pid); }
Result SetCurrentApplicationRightsEnvironmentCanBeActiveForDevelop(bool can_be_active) { return ::nsdevSetCurrentApplicationRightsEnvironmentCanBeActiveForDevelop(can_be_active); }

} // namespace nsdev

namespace nssu {

Result Initialize(void) { return ::nssuInitialize(); }
void Exit(void) { ::nssuExit(); }
Service* GetServiceSession(void) { return ::nssuGetServiceSession(); }
Result GetBackgroundNetworkUpdateState(NsBackgroundNetworkUpdateState *out) { return ::nssuGetBackgroundNetworkUpdateState(out); }
Result OpenSystemUpdateControl(NsSystemUpdateControl *c) { return ::nssuOpenSystemUpdateControl(c); }
Result NotifyExFatDriverRequired(void) { return ::nssuNotifyExFatDriverRequired(); }
Result ClearExFatDriverStatusForDebug(void) { return ::nssuClearExFatDriverStatusForDebug(); }
Result RequestBackgroundNetworkUpdate(void) { return ::nssuRequestBackgroundNetworkUpdate(); }
Result NotifyBackgroundNetworkUpdate(const NcmContentMetaKey *key) { return ::nssuNotifyBackgroundNetworkUpdate(key); }
Result NotifyExFatDriverDownloadedForDebug(void) { return ::nssuNotifyExFatDriverDownloadedForDebug(); }
Result GetSystemUpdateNotificationEventForContentDelivery(Event* out_event) { return ::nssuGetSystemUpdateNotificationEventForContentDelivery(out_event); }
Result NotifySystemUpdateForContentDelivery(void) { return ::nssuNotifySystemUpdateForContentDelivery(); }
Result PrepareShutdown(void) { return ::nssuPrepareShutdown(); }
Result DestroySystemUpdateTask(void) { return ::nssuDestroySystemUpdateTask(); }
Result RequestSendSystemUpdate(AsyncResult *a, u32 addr, u16 port, NsSystemDeliveryInfo *info) { return ::nssuRequestSendSystemUpdate(a, addr, port, info); }
Result GetSendSystemUpdateProgress(NsSystemUpdateProgress *out) { return ::nssuGetSendSystemUpdateProgress(out); }
void ControlClose(NsSystemUpdateControl *c) { ::nssuControlClose(c); }
Result ControlHasDownloaded(NsSystemUpdateControl *c, bool* out) { return ::nssuControlHasDownloaded(c, out); }
Result ControlRequestCheckLatestUpdate(NsSystemUpdateControl *c, AsyncValue *a) { return ::nssuControlRequestCheckLatestUpdate(c, a); }
Result ControlRequestDownloadLatestUpdate(NsSystemUpdateControl *c, AsyncResult *a) { return ::nssuControlRequestDownloadLatestUpdate(c, a); }
Result ControlGetDownloadProgress(NsSystemUpdateControl *c, NsSystemUpdateProgress *out) { return ::nssuControlGetDownloadProgress(c, out); }
Result ControlApplyDownloadedUpdate(NsSystemUpdateControl *c) { return ::nssuControlApplyDownloadedUpdate(c); }
Result ControlRequestPrepareCardUpdate(NsSystemUpdateControl *c, AsyncResult *a) { return ::nssuControlRequestPrepareCardUpdate(c, a); }
Result ControlGetPrepareCardUpdateProgress(NsSystemUpdateControl *c, NsSystemUpdateProgress *out) { return ::nssuControlGetPrepareCardUpdateProgress(c, out); }
Result ControlHasPreparedCardUpdate(NsSystemUpdateControl *c, bool* out) { return ::nssuControlHasPreparedCardUpdate(c, out); }
Result ControlApplyCardUpdate(NsSystemUpdateControl *c) { return ::nssuControlApplyCardUpdate(c); }
Result ControlGetDownloadedEulaDataSize(NsSystemUpdateControl *c, const char* path, u64 *filesize) { return ::nssuControlGetDownloadedEulaDataSize(c, path, filesize); }
Result ControlGetDownloadedEulaData(NsSystemUpdateControl *c, const char* path, void* buffer, size_t size, u64 *filesize) { return ::nssuControlGetDownloadedEulaData(c, path, buffer, size, filesize); }
Result ControlSetupCardUpdate(NsSystemUpdateControl *c, void* buffer, size_t size) { return ::nssuControlSetupCardUpdate(c, buffer, size); }
Result ControlGetPreparedCardUpdateEulaDataSize(NsSystemUpdateControl *c, const char* path, u64 *filesize) { return ::nssuControlGetPreparedCardUpdateEulaDataSize(c, path, filesize); }
Result ControlGetPreparedCardUpdateEulaData(NsSystemUpdateControl *c, const char* path, void* buffer, size_t size, u64 *filesize) { return ::nssuControlGetPreparedCardUpdateEulaData(c, path, buffer, size, filesize); }
Result ControlSetupCardUpdateViaSystemUpdater(NsSystemUpdateControl *c, void* buffer, size_t size) { return ::nssuControlSetupCardUpdateViaSystemUpdater(c, buffer, size); }
Result ControlHasReceived(NsSystemUpdateControl *c, bool* out) { return ::nssuControlHasReceived(c, out); }
Result ControlRequestReceiveSystemUpdate(NsSystemUpdateControl *c, AsyncResult *a, u32 addr, u16 port, NsSystemDeliveryInfo *info) { return ::nssuControlRequestReceiveSystemUpdate(c, a, addr, port, info); }
Result ControlGetReceiveProgress(NsSystemUpdateControl *c, NsSystemUpdateProgress *out) { return ::nssuControlGetReceiveProgress(c, out); }
Result ControlApplyReceivedUpdate(NsSystemUpdateControl *c) { return ::nssuControlApplyReceivedUpdate(c); }
Result ControlGetReceivedEulaDataSize(NsSystemUpdateControl *c, const char* path, u64 *filesize) { return ::nssuControlGetReceivedEulaDataSize(c, path, filesize); }
Result ControlGetReceivedEulaData(NsSystemUpdateControl *c, const char* path, void* buffer, size_t size, u64 *filesize) { return ::nssuControlGetReceivedEulaData(c, path, buffer, size, filesize); }
Result ControlSetupToReceiveSystemUpdate(NsSystemUpdateControl *c) { return ::nssuControlSetupToReceiveSystemUpdate(c); }
Result ControlRequestCheckLatestUpdateIncludesRebootlessUpdate(NsSystemUpdateControl *c, AsyncValue *a) { return ::nssuControlRequestCheckLatestUpdateIncludesRebootlessUpdate(c, a); }

} // namespace nssu

namespace nsvm {

Result Initialize(void) { return ::nsvmInitialize(); }
void Exit(void) { ::nsvmExit(); }
Service* GetServiceSession(void) { return ::nsvmGetServiceSession(); }
Result NeedsUpdateVulnerability(bool *out) { return ::nsvmNeedsUpdateVulnerability(out); }
Result GetSafeSystemVersion(NcmContentMetaKey *out) { return ::nsvmGetSafeSystemVersion(out); }

} // namespace nsvm

namespace nv {

Result AddressSpaceCreate(NvAddressSpace* a, u32 page_size) { return ::nvAddressSpaceCreate(a, page_size); }
void AddressSpaceClose(NvAddressSpace* a) { ::nvAddressSpaceClose(a); }
Result AddressSpaceAlloc(NvAddressSpace* a, bool sparse, u64 size, iova_t* iova_out) { return ::nvAddressSpaceAlloc(a, sparse, size, iova_out); }
Result AddressSpaceAllocFixed(NvAddressSpace* a, bool sparse, u64 size, iova_t iova) { return ::nvAddressSpaceAllocFixed(a, sparse, size, iova); }
Result AddressSpaceFree(NvAddressSpace* a, iova_t iova, u64 size) { return ::nvAddressSpaceFree(a, iova, size); }
Result AddressSpaceMap(NvAddressSpace* a, u32 nvmap_handle, bool is_gpu_cacheable, NvKind kind, iova_t* iova_out) { return ::nvAddressSpaceMap(a, nvmap_handle, is_gpu_cacheable, kind, iova_out); }
Result AddressSpaceMapFixed(NvAddressSpace* a, u32 nvmap_handle, bool is_gpu_cacheable, NvKind kind, iova_t iova) { return ::nvAddressSpaceMapFixed(a, nvmap_handle, is_gpu_cacheable, kind, iova); }
Result AddressSpaceModify(NvAddressSpace* a, iova_t iova, u64 offset, u64 size, NvKind kind) { return ::nvAddressSpaceModify(a, iova, offset, size, kind); }
Result AddressSpaceUnmap(NvAddressSpace* a, iova_t iova) { return ::nvAddressSpaceUnmap(a, iova); }
Result ChannelCreate(NvChannel* c, const char* dev) { return ::nvChannelCreate(c, dev); }
void ChannelClose(NvChannel* c) { ::nvChannelClose(c); }
Result ChannelSetPriority(NvChannel* c, NvChannelPriority prio) { return ::nvChannelSetPriority(c, prio); }
Result ChannelSetTimeout(NvChannel* c, u32 timeout) { return ::nvChannelSetTimeout(c, timeout); }
Result FenceInit(void) { return ::nvFenceInit(); }
void FenceExit(void) { ::nvFenceExit(); }
u32 FenceGetFd(void) { return ::nvFenceGetFd(); }
Result FenceWait(NvFence* f, s32 timeout_us) { return ::nvFenceWait(f, timeout_us); }
void MultiFenceCreate(NvMultiFence* mf, const NvFence* fence) { ::nvMultiFenceCreate(mf, fence); }
Result MultiFenceWait(NvMultiFence* mf, s32 timeout_us) { return ::nvMultiFenceWait(mf, timeout_us); }
Result GpuInit(void) { return ::nvGpuInit(); }
void GpuExit(void) { ::nvGpuExit(); }
const nvioctl_gpu_characteristics* GpuGetCharacteristics(void) { return ::nvGpuGetCharacteristics(); }
u32 GpuGetZcullCtxSize(void) { return ::nvGpuGetZcullCtxSize(); }
const nvioctl_zcull_info* GpuGetZcullInfo(void) { return ::nvGpuGetZcullInfo(); }
const u32* GpuGetTpcMasks(u32 *num_masks_out) { return ::nvGpuGetTpcMasks(num_masks_out); }
Result GpuZbcGetActiveSlotMask(u32 *out_slot, u32 *out_mask) { return ::nvGpuZbcGetActiveSlotMask(out_slot, out_mask); }
Result GpuZbcAddColor(const u32 color_l2[4], const u32 color_ds[4], u32 format) { return ::nvGpuZbcAddColor(color_l2, color_ds, format); }
Result GpuZbcAddDepth(float depth) { return ::nvGpuZbcAddDepth(depth); }
Result GpuGetTimestamp(u64 *ts) { return ::nvGpuGetTimestamp(ts); }
Result GpuChannelCreate(NvGpuChannel* c, struct NvAddressSpace* as, NvChannelPriority prio) { return ::nvGpuChannelCreate(c, as, prio); }
void GpuChannelClose(NvGpuChannel* c) { ::nvGpuChannelClose(c); }
Result GpuChannelZcullBind(NvGpuChannel* c, iova_t iova) { return ::nvGpuChannelZcullBind(c, iova); }
Result GpuChannelAppendEntry(NvGpuChannel* c, iova_t start, size_t num_cmds, u32 flags, u32 flush_threshold) { return ::nvGpuChannelAppendEntry(c, start, num_cmds, flags, flush_threshold); }
Result GpuChannelKickoff(NvGpuChannel* c) { return ::nvGpuChannelKickoff(c); }
Result GpuChannelGetErrorNotification(NvGpuChannel* c, NvNotification* notif) { return ::nvGpuChannelGetErrorNotification(c, notif); }
Result GpuChannelGetErrorInfo(NvGpuChannel* c, NvError* error) { return ::nvGpuChannelGetErrorInfo(c, error); }
u32 GpuChannelGetSyncpointId(NvGpuChannel* c) { return ::nvGpuChannelGetSyncpointId(c); }
void GpuChannelGetFence(NvGpuChannel* c, NvFence* fence_out) { ::nvGpuChannelGetFence(c, fence_out); }
void GpuChannelIncrFence(NvGpuChannel* c) { ::nvGpuChannelIncrFence(c); }
Result MapInit(void) { return ::nvMapInit(); }
u32 MapGetFd(void) { return ::nvMapGetFd(); }
void MapExit(void) { ::nvMapExit(); }
Result MapCreate(NvMap* m, void* cpu_addr, u32 size, u32 align, NvKind kind, bool is_cpu_cacheable) { return ::nvMapCreate(m, cpu_addr, size, align, kind, is_cpu_cacheable); }
Result MapLoadRemote(NvMap* m, u32 id) { return ::nvMapLoadRemote(m, id); }
void MapClose(NvMap* m) { ::nvMapClose(m); }
u32 MapGetHandle(NvMap* m) { return ::nvMapGetHandle(m); }
u32 MapGetId(NvMap* m) { return ::nvMapGetId(m); }
u32 MapGetSize(NvMap* m) { return ::nvMapGetSize(m); }
void* MapGetCpuAddr(NvMap* m) { return ::nvMapGetCpuAddr(m); }
bool MapIsRemote(NvMap* m) { return ::nvMapIsRemote(m); }
NvKind MapGetKind(NvMap* m) { return ::nvMapGetKind(m); }
Result Initialize(void) { return ::nvInitialize(); }
void Exit(void) { ::nvExit(); }
Service* GetServiceSession(void) { return ::nvGetServiceSession(); }
Result Open(u32 *fd, const char *devicepath) { return ::nvOpen(fd, devicepath); }
Result Ioctl(u32 fd, u32 request, void* argp) { return ::nvIoctl(fd, request, argp); }
Result Ioctl2(u32 fd, u32 request, void* argp, const void* inbuf, size_t inbuf_size) { return ::nvIoctl2(fd, request, argp, inbuf, inbuf_size); }
Result Ioctl3(u32 fd, u32 request, void* argp, void* outbuf, size_t outbuf_size) { return ::nvIoctl3(fd, request, argp, outbuf, outbuf_size); }
Result Close(u32 fd) { return ::nvClose(fd); }
Result QueryEvent(u32 fd, u32 event_id, Event *event_out) { return ::nvQueryEvent(fd, event_id, event_out); }
Result ConvertError(int rc) { return ::nvConvertError(rc); }

} // namespace nv

namespace nvioctl {

Result NvhostCtrl_SyncptRead(u32 fd, u32 id, u32* out) { return ::nvioctlNvhostCtrl_SyncptRead(fd, id, out); }
Result NvhostCtrl_SyncptIncr(u32 fd, u32 id) { return ::nvioctlNvhostCtrl_SyncptIncr(fd, id); }
Result NvhostCtrl_SyncptWait(u32 fd, u32 id, u32 threshold, u32 timeout) { return ::nvioctlNvhostCtrl_SyncptWait(fd, id, threshold, timeout); }
Result NvhostCtrl_EventSignal(u32 fd, u32 event_id) { return ::nvioctlNvhostCtrl_EventSignal(fd, event_id); }
Result NvhostCtrl_EventWait(u32 fd, u32 syncpt_id, u32 threshold, s32 timeout, u32 event_id, u32 *out) { return ::nvioctlNvhostCtrl_EventWait(fd, syncpt_id, threshold, timeout, event_id, out); }
Result NvhostCtrl_EventWaitAsync(u32 fd, u32 syncpt_id, u32 threshold, s32 timeout, u32 event_id) { return ::nvioctlNvhostCtrl_EventWaitAsync(fd, syncpt_id, threshold, timeout, event_id); }
Result NvhostCtrl_EventRegister(u32 fd, u32 event_id) { return ::nvioctlNvhostCtrl_EventRegister(fd, event_id); }
Result NvhostCtrl_EventUnregister(u32 fd, u32 event_id) { return ::nvioctlNvhostCtrl_EventUnregister(fd, event_id); }
Result NvhostCtrlGpu_ZCullGetCtxSize(u32 fd, u32 *out) { return ::nvioctlNvhostCtrlGpu_ZCullGetCtxSize(fd, out); }
Result NvhostCtrlGpu_ZCullGetInfo(u32 fd, nvioctl_zcull_info *out) { return ::nvioctlNvhostCtrlGpu_ZCullGetInfo(fd, out); }
Result NvhostCtrlGpu_ZbcSetTable(u32 fd, const u32 color_ds[4], const u32 color_l2[4], u32 depth, u32 format, u32 type) { return ::nvioctlNvhostCtrlGpu_ZbcSetTable(fd, color_ds, color_l2, depth, format, type); }
Result NvhostCtrlGpu_ZbcQueryTable(u32 fd, u32 index, nvioctl_zbc_entry *out) { return ::nvioctlNvhostCtrlGpu_ZbcQueryTable(fd, index, out); }
Result NvhostCtrlGpu_GetCharacteristics(u32 fd, nvioctl_gpu_characteristics *out) { return ::nvioctlNvhostCtrlGpu_GetCharacteristics(fd, out); }
Result NvhostCtrlGpu_GetTpcMasks(u32 fd, void *buffer, size_t size) { return ::nvioctlNvhostCtrlGpu_GetTpcMasks(fd, buffer, size); }
Result NvhostCtrlGpu_ZbcGetActiveSlotMask(u32 fd, nvioctl_zbc_slot_mask *out) { return ::nvioctlNvhostCtrlGpu_ZbcGetActiveSlotMask(fd, out); }
Result NvhostCtrlGpu_GetGpuTime(u32 fd, nvioctl_gpu_time *out) { return ::nvioctlNvhostCtrlGpu_GetGpuTime(fd, out); }
Result NvhostAsGpu_BindChannel(u32 fd, u32 channel_fd) { return ::nvioctlNvhostAsGpu_BindChannel(fd, channel_fd); }
Result NvhostAsGpu_AllocSpace(u32 fd, u32 pages, u32 page_size, u32 flags, u64 align_or_offset, u64 *offset) { return ::nvioctlNvhostAsGpu_AllocSpace(fd, pages, page_size, flags, align_or_offset, offset); }
Result NvhostAsGpu_FreeSpace(u32 fd, u64 offset, u32 pages, u32 page_size) { return ::nvioctlNvhostAsGpu_FreeSpace(fd, offset, pages, page_size); }
Result NvhostAsGpu_MapBufferEx(u32 fd, u32 flags, u32 kind, u32 nvmap_handle, u32 page_size, u64 buffer_offset, u64 mapping_size, u64 input_offset, u64 *offset) { return ::nvioctlNvhostAsGpu_MapBufferEx(fd, flags, kind, nvmap_handle, page_size, buffer_offset, mapping_size, input_offset, offset); }
Result NvhostAsGpu_UnmapBuffer(u32 fd, u64 offset) { return ::nvioctlNvhostAsGpu_UnmapBuffer(fd, offset); }
Result NvhostAsGpu_GetVARegions(u32 fd, nvioctl_va_region regions[2]) { return ::nvioctlNvhostAsGpu_GetVARegions(fd, regions); }
Result NvhostAsGpu_InitializeEx(u32 fd, u32 flags, u32 big_page_size) { return ::nvioctlNvhostAsGpu_InitializeEx(fd, flags, big_page_size); }
Result Nvmap_Create(u32 fd, u32 size, u32 *nvmap_handle) { return ::nvioctlNvmap_Create(fd, size, nvmap_handle); }
Result Nvmap_FromId(u32 fd, u32 id, u32 *nvmap_handle) { return ::nvioctlNvmap_FromId(fd, id, nvmap_handle); }
Result Nvmap_Alloc(u32 fd, u32 nvmap_handle, u32 heapmask, u32 flags, u32 align, u8 kind, void* addr) { return ::nvioctlNvmap_Alloc(fd, nvmap_handle, heapmask, flags, align, kind, addr); }
Result Nvmap_Free(u32 fd, u32 nvmap_handle) { return ::nvioctlNvmap_Free(fd, nvmap_handle); }
Result Nvmap_Param(u32 fd, u32 nvmap_handle, NvMapParam param, u32 *result) { return ::nvioctlNvmap_Param(fd, nvmap_handle, param, result); }
Result Nvmap_GetId(u32 fd, u32 nvmap_handle, u32 *id) { return ::nvioctlNvmap_GetId(fd, nvmap_handle, id); }
Result Channel_SetNvmapFd(u32 fd, u32 nvmap_fd) { return ::nvioctlChannel_SetNvmapFd(fd, nvmap_fd); }
Result Channel_SubmitGpfifo(u32 fd, nvioctl_gpfifo_entry *entries, u32 num_entries, u32 flags, nvioctl_fence *fence_inout) { return ::nvioctlChannel_SubmitGpfifo(fd, entries, num_entries, flags, fence_inout); }
Result Channel_KickoffPb(u32 fd, nvioctl_gpfifo_entry *entries, u32 num_entries, u32 flags, nvioctl_fence *fence_inout) { return ::nvioctlChannel_KickoffPb(fd, entries, num_entries, flags, fence_inout); }
Result Channel_AllocObjCtx(u32 fd, u32 class_num, u32 flags, u64* id_out) { return ::nvioctlChannel_AllocObjCtx(fd, class_num, flags, id_out); }
Result Channel_ZCullBind(u32 fd, u64 gpu_va, u32 mode) { return ::nvioctlChannel_ZCullBind(fd, gpu_va, mode); }
Result Channel_SetErrorNotifier(u32 fd, u32 enable) { return ::nvioctlChannel_SetErrorNotifier(fd, enable); }
Result Channel_GetErrorInfo(u32 fd, NvError* out) { return ::nvioctlChannel_GetErrorInfo(fd, out); }
Result Channel_GetErrorNotification(u32 fd, NvNotification* out) { return ::nvioctlChannel_GetErrorNotification(fd, out); }
Result Channel_SetPriority(u32 fd, u32 priority) { return ::nvioctlChannel_SetPriority(fd, priority); }
Result Channel_SetTimeout(u32 fd, u32 timeout) { return ::nvioctlChannel_SetTimeout(fd, timeout); }
Result Channel_AllocGpfifoEx2(u32 fd, u32 num_entries, u32 flags, u32 unk0, u32 unk1, u32 unk2, u32 unk3, nvioctl_fence *fence_out) { return ::nvioctlChannel_AllocGpfifoEx2(fd, num_entries, flags, unk0, unk1, unk2, unk3, fence_out); }
Result Channel_SetUserData(u32 fd, void* addr) { return ::nvioctlChannel_SetUserData(fd, addr); }
Result Channel_Submit(u32 fd, const nvioctl_cmdbuf *cmdbufs, u32 num_cmdbufs, const nvioctl_reloc *relocs, const nvioctl_reloc_shift *reloc_shifts, u32 num_relocs, const nvioctl_syncpt_incr *syncpt_incrs, u32 num_syncpt_incrs, nvioctl_fence *fences, u32 num_fences) { return ::nvioctlChannel_Submit(fd, cmdbufs, num_cmdbufs, relocs, reloc_shifts, num_relocs, syncpt_incrs, num_syncpt_incrs, fences, num_fences); }
Result Channel_GetSyncpt(u32 fd, u32 module_id, u32 *syncpt) { return ::nvioctlChannel_GetSyncpt(fd, module_id, syncpt); }
Result Channel_GetModuleClockRate(u32 fd, u32 module_id, u32 *freq) { return ::nvioctlChannel_GetModuleClockRate(fd, module_id, freq); }
Result Channel_SetModuleClockRate(u32 fd, u32 module_id, u32 freq) { return ::nvioctlChannel_SetModuleClockRate(fd, module_id, freq); }
Result Channel_MapCommandBuffer(u32 fd, nvioctl_command_buffer_map *maps, u32 num_maps, bool compressed) { return ::nvioctlChannel_MapCommandBuffer(fd, maps, num_maps, compressed); }
Result Channel_UnmapCommandBuffer(u32 fd, const nvioctl_command_buffer_map *maps, u32 num_maps, bool compressed) { return ::nvioctlChannel_UnmapCommandBuffer(fd, maps, num_maps, compressed); }
Result Channel_SetSubmitTimeout(u32 fd, u32 timeout) { return ::nvioctlChannel_SetSubmitTimeout(fd, timeout); }

} // namespace nvioctl

namespace nwindow {

bool IsValid(NWindow* nw) { return ::nwindowIsValid(nw); }
NWindow* GetDefault(void) { return ::nwindowGetDefault(); }
Result Create(NWindow* nw, Service* binder_session, s32 binder_id, bool producer_controlled_by_app) { return ::nwindowCreate(nw, binder_session, binder_id, producer_controlled_by_app); }
Result CreateFromLayer(NWindow* nw, const ViLayer* layer) { return ::nwindowCreateFromLayer(nw, layer); }
void Close(NWindow* nw) { ::nwindowClose(nw); }
Result GetDimensions(NWindow* nw, u32* out_width, u32* out_height) { return ::nwindowGetDimensions(nw, out_width, out_height); }
Result SetDimensions(NWindow* nw, u32 width, u32 height) { return ::nwindowSetDimensions(nw, width, height); }
Result SetCrop(NWindow* nw, s32 left, s32 top, s32 right, s32 bottom) { return ::nwindowSetCrop(nw, left, top, right, bottom); }
Result SetTransform(NWindow* nw, u32 transform) { return ::nwindowSetTransform(nw, transform); }
Result SetSwapInterval(NWindow* nw, u32 swap_interval) { return ::nwindowSetSwapInterval(nw, swap_interval); }
bool IsConsumerRunningBehind(NWindow* nw) { return ::nwindowIsConsumerRunningBehind(nw); }
Result ConfigureBuffer(NWindow* nw, s32 slot, NvGraphicBuffer* buf) { return ::nwindowConfigureBuffer(nw, slot, buf); }
Result DequeueBuffer(NWindow* nw, s32* out_slot, NvMultiFence* out_fence) { return ::nwindowDequeueBuffer(nw, out_slot, out_fence); }
Result CancelBuffer(NWindow* nw, s32 slot, const NvMultiFence* fence) { return ::nwindowCancelBuffer(nw, slot, fence); }
Result QueueBuffer(NWindow* nw, s32 slot, const NvMultiFence* fence) { return ::nwindowQueueBuffer(nw, slot, fence); }
Result ReleaseBuffers(NWindow* nw) { return ::nwindowReleaseBuffers(nw); }

} // namespace nwindow

namespace nxlink {

int ConnectToHost(bool redirStdout, bool redirStderr) { return ::nxlinkConnectToHost(redirStdout, redirStderr); }
int Stdio(void) { return ::nxlinkStdio(); }
int StdioForDebug(void) { return ::nxlinkStdioForDebug(); }

} // namespace nxlink

namespace omm {

Result Initialize(void) { return ::ommInitialize(); }
void Exit(void) { ::ommExit(); }
Service* GetServiceSession(void) { return ::ommGetServiceSession(); }
Result GetDefaultDisplayResolution(s32* width, s32* height) { return ::ommGetDefaultDisplayResolution(width, height); }
Result GetOperationMode(OmmOperationMode* s) { return ::ommGetOperationMode(s); }
Result SetOperationModePolicy(OmmOperationModePolicy value) { return ::ommSetOperationModePolicy(value); }

} // namespace omm

namespace ovlnrcv {

Result Initialize(void) { return ::ovlnrcvInitialize(); }
void Exit(void) { ::ovlnrcvExit(); }
Service* GetServiceSession(void) { return ::ovlnrcvGetServiceSession(); }
Result OpenReceiver(OvlnReceiver *r) { return ::ovlnrcvOpenReceiver(r); }
void CloseReceiver(OvlnReceiver *r) { ::ovlnrcvCloseReceiver(r); }
Result AddSource(OvlnReceiver *r, const OvlnSourceName *name) { return ::ovlnrcvAddSource(r, name); }
Result RemoveSource(OvlnReceiver *r, const OvlnSourceName *name) { return ::ovlnrcvRemoveSource(r, name); }
Result GetReceiveEventHandle(OvlnReceiver *r, Event* out_event) { return ::ovlnrcvGetReceiveEventHandle(r, out_event); }
Result Receive(OvlnReceiver *r, OvlnRawMessage *message) { return ::ovlnrcvReceive(r, message); }
Result ReceiveWithTick(OvlnReceiver *r, OvlnRawMessage *message, s64 *tick) { return ::ovlnrcvReceiveWithTick(r, message, tick); }

} // namespace ovlnrcv

namespace ovlnsnd {

Result Initialize(void) { return ::ovlnsndInitialize(); }
void Exit(void) { ::ovlnsndExit(); }
Service* GetServiceSession(void) { return ::ovlnsndGetServiceSession(); }
Result OpenSender(OvlnSender *s, const OvlnSourceName *name, OvlnQueueAttribute attribute) { return ::ovlnsndOpenSender(s, name, attribute); }
void CloseSender(OvlnSender *s) { ::ovlnsndCloseSender(s); }
Result Send(OvlnSender *s, OvlnSendOption option, const OvlnRawMessage *message) { return ::ovlnsndSend(s, option, message); }
Result GetUnreceivedMessageCount(OvlnSender *s, u32 *count) { return ::ovlnsndGetUnreceivedMessageCount(s, count); }

} // namespace ovlnsnd

namespace pad {

void ConfigureInput(u32 max_players, u32 style_set) { ::padConfigureInput(max_players, style_set); }
void InitializeWithMask(PadState* pad, u64 mask) { ::padInitializeWithMask(pad, mask); }
void InitializeAny(PadState* pad) { ::padInitializeAny(pad); }
void InitializeDefault(PadState* pad) { ::padInitializeDefault(pad); }
void Update(PadState* pad) { ::padUpdate(pad); }
bool IsHandheld(const PadState* pad) { return ::padIsHandheld(pad); }
bool IsNpadActive(const PadState* pad, HidNpadIdType id) { return ::padIsNpadActive(pad, id); }
u32 GetStyleSet(const PadState* pad) { return ::padGetStyleSet(pad); }
u32 GetAttributes(const PadState* pad) { return ::padGetAttributes(pad); }
bool IsConnected(const PadState* pad) { return ::padIsConnected(pad); }
u64 GetButtons(const PadState* pad) { return ::padGetButtons(pad); }
u64 GetButtonsDown(const PadState* pad) { return ::padGetButtonsDown(pad); }
u64 GetButtonsUp(const PadState* pad) { return ::padGetButtonsUp(pad); }
HidAnalogStickState GetStickPos(const PadState* pad, unsigned i) { return ::padGetStickPos(pad, i); }
u32 GetGcTriggerPos(const PadState* pad, unsigned i) { return ::padGetGcTriggerPos(pad, i); }
void RepeaterInitialize(PadRepeater* r, u16 delay, u16 repeat) { ::padRepeaterInitialize(r, delay, repeat); }
void RepeaterUpdate(PadRepeater* r, u64 button_mask) { ::padRepeaterUpdate(r, button_mask); }
u64 RepeaterGetButtons(const PadRepeater* r) { return ::padRepeaterGetButtons(r); }

} // namespace pad

namespace parcel {

void Create(Parcel *ctx) { ::parcelCreate(ctx); }
Result Transact(Binder *session, u32 code, Parcel *in_parcel, Parcel *reply_parcel) { return ::parcelTransact(session, code, in_parcel, reply_parcel); }
void* WriteData(Parcel *ctx, const void* data, size_t data_size) { return ::parcelWriteData(ctx, data, data_size); }
void* ReadData(Parcel *ctx, void* data, size_t data_size) { return ::parcelReadData(ctx, data, data_size); }
void WriteInt32(Parcel *ctx, s32 val) { ::parcelWriteInt32(ctx, val); }
void WriteUInt32(Parcel *ctx, u32 val) { ::parcelWriteUInt32(ctx, val); }
void WriteString16(Parcel *ctx, const char *str) { ::parcelWriteString16(ctx, str); }
s32 ReadInt32(Parcel *ctx) { return ::parcelReadInt32(ctx); }
u32 ReadUInt32(Parcel *ctx) { return ::parcelReadUInt32(ctx); }
void WriteInterfaceToken(Parcel *ctx, const char *str) { ::parcelWriteInterfaceToken(ctx, str); }
void* ReadFlattenedObject(Parcel *ctx, size_t *size) { return ::parcelReadFlattenedObject(ctx, size); }
void* WriteFlattenedObject(Parcel *ctx, const void* data, size_t size) { return ::parcelWriteFlattenedObject(ctx, data, size); }

} // namespace parcel

namespace pctl {

Result Initialize(void) { return ::pctlInitialize(); }
void Exit(void) { ::pctlExit(); }
Service* GetServiceSession(void) { return ::pctlGetServiceSession(); }
Service* GetServiceSession_Service(void) { return ::pctlGetServiceSession_Service(); }
Result IsRestrictionTemporaryUnlocked(bool *flag) { return ::pctlIsRestrictionTemporaryUnlocked(flag); }
Result ConfirmStereoVisionPermission(void) { return ::pctlConfirmStereoVisionPermission(); }
Result IsRestrictionEnabled(bool *flag) { return ::pctlIsRestrictionEnabled(flag); }
Result GetSafetyLevel(u32 *safety_level) { return ::pctlGetSafetyLevel(safety_level); }
Result GetCurrentSettings(PctlRestrictionSettings *settings) { return ::pctlGetCurrentSettings(settings); }
Result GetFreeCommunicationApplicationListCount(u32 *count) { return ::pctlGetFreeCommunicationApplicationListCount(count); }
Result ResetConfirmedStereoVisionPermission(void) { return ::pctlResetConfirmedStereoVisionPermission(); }
Result IsStereoVisionPermitted(bool *flag) { return ::pctlIsStereoVisionPermitted(flag); }
Result IsPairingActive(bool *flag) { return ::pctlIsPairingActive(flag); }
Result GetSynchronizationEvent(Event* out_event) { return ::pctlGetSynchronizationEvent(out_event); }
Result GetPlayTimerEventToRequestSuspension(Event* out_event) { return ::pctlGetPlayTimerEventToRequestSuspension(out_event); }
Result IsPlayTimerAlarmDisabled(bool *flag) { return ::pctlIsPlayTimerAlarmDisabled(flag); }
Result GetUnlinkedEvent(Event* out_event) { return ::pctlGetUnlinkedEvent(out_event); }

} // namespace pctl

namespace pctlauth {

Result Show(bool flag) { return ::pctlauthShow(flag); }
Result ShowEx(u8 arg0, u8 arg1, u8 arg2) { return ::pctlauthShowEx(arg0, arg1, arg2); }
Result ShowForConfiguration(void) { return ::pctlauthShowForConfiguration(); }
Result RegisterPasscode(void) { return ::pctlauthRegisterPasscode(); }
Result ChangePasscode(void) { return ::pctlauthChangePasscode(); }

} // namespace pctlauth

namespace pcv {

Result Initialize(void) { return ::pcvInitialize(); }
void Exit(void) { ::pcvExit(); }
Service* GetServiceSession(void) { return ::pcvGetServiceSession(); }
Result GetModuleId(PcvModuleId *module_id, PcvModule module) { return ::pcvGetModuleId(module_id, module); }
Result GetClockRate(PcvModule module, u32 *out_hz) { return ::pcvGetClockRate(module, out_hz); }
Result SetClockRate(PcvModule module, u32 hz) { return ::pcvSetClockRate(module, hz); }
Result SetVoltageEnabled(u32 power_domain, bool state) { return ::pcvSetVoltageEnabled(power_domain, state); }
Result GetVoltageEnabled(bool *isEnabled, u32 power_domain) { return ::pcvGetVoltageEnabled(isEnabled, power_domain); }
Result GetPossibleClockRates(PcvModule module, u32 *rates, s32 max_count, PcvClockRatesListType *out_type, s32 *out_count) { return ::pcvGetPossibleClockRates(module, rates, max_count, out_type, out_count); }

} // namespace pcv

namespace pdm {

u64 PlayTimestampToPosix(u32 timestamp) { return ::pdmPlayTimestampToPosix(timestamp); }

} // namespace pdm

namespace pdmqry {

Result Initialize(void) { return ::pdmqryInitialize(); }
void Exit(void) { ::pdmqryExit(); }
Service* GetServiceSession(void) { return ::pdmqryGetServiceSession(); }
Result QueryAppletEvent(s32 entry_index, bool flag, PdmAppletEvent *events, s32 count, s32 *total_out) { return ::pdmqryQueryAppletEvent(entry_index, flag, events, count, total_out); }
Result QueryPlayStatisticsByApplicationId(u64 application_id, bool flag, PdmPlayStatistics *stats) { return ::pdmqryQueryPlayStatisticsByApplicationId(application_id, flag, stats); }
Result QueryPlayStatisticsByApplicationIdAndUserAccountId(u64 application_id, AccountUid uid, bool flag, PdmPlayStatistics *stats) { return ::pdmqryQueryPlayStatisticsByApplicationIdAndUserAccountId(application_id, uid, flag, stats); }
Result QueryLastPlayTime(bool flag, PdmLastPlayTime *playtimes, const u64 *application_ids, s32 count, s32 *total_out) { return ::pdmqryQueryLastPlayTime(flag, playtimes, application_ids, count, total_out); }
Result QueryPlayEvent(s32 entry_index, PdmPlayEvent *events, s32 count, s32 *total_out) { return ::pdmqryQueryPlayEvent(entry_index, events, count, total_out); }
Result GetAvailablePlayEventRange(s32 *total_entries, s32 *start_entry_index, s32 *end_entry_index) { return ::pdmqryGetAvailablePlayEventRange(total_entries, start_entry_index, end_entry_index); }
Result QueryAccountEvent(s32 entry_index, PdmAccountEvent *events, s32 count, s32 *total_out) { return ::pdmqryQueryAccountEvent(entry_index, events, count, total_out); }
Result QueryAccountPlayEvent(s32 entry_index, AccountUid uid, PdmAccountPlayEvent *events, s32 count, s32 *total_out) { return ::pdmqryQueryAccountPlayEvent(entry_index, uid, events, count, total_out); }
Result GetAvailableAccountPlayEventRange(AccountUid uid, s32 *total_entries, s32 *start_entry_index, s32 *end_entry_index) { return ::pdmqryGetAvailableAccountPlayEventRange(uid, total_entries, start_entry_index, end_entry_index); }
Result QueryRecentlyPlayedApplication(AccountUid uid, bool flag, u64 *application_ids, s32 count, s32 *total_out) { return ::pdmqryQueryRecentlyPlayedApplication(uid, flag, application_ids, count, total_out); }
Result GetRecentlyPlayedApplicationUpdateEvent(Event* out_event) { return ::pdmqryGetRecentlyPlayedApplicationUpdateEvent(out_event); }

} // namespace pdmqry

namespace pgl {

Result Initialize(void) { return ::pglInitialize(); }
void Exit(void) { ::pglExit(); }
Service* GetServiceSessionCmif(void) { return ::pglGetServiceSessionCmif(); }
TipcService* GetServiceSessionTipc(void) { return ::pglGetServiceSessionTipc(); }
Result LaunchProgram(u64 *out_pid, const NcmProgramLocation *loc, u32 pm_launch_flags, u8 pgl_launch_flags) { return ::pglLaunchProgram(out_pid, loc, pm_launch_flags, pgl_launch_flags); }
Result TerminateProcess(u64 pid) { return ::pglTerminateProcess(pid); }
Result LaunchProgramFromHost(u64 *out_pid, const char *content_path, u32 pm_launch_flags) { return ::pglLaunchProgramFromHost(out_pid, content_path, pm_launch_flags); }
Result GetHostProgramLaunchProperty(PglProgramLaunchProperty *out, const char *content_path) { return ::pglGetHostProgramLaunchProperty(out, content_path); }
Result GetRunningApplicationProcessId(u64 *out_pid) { return ::pglGetRunningApplicationProcessId(out_pid); }
Result BoostSystemMemoryResourceLimit(u64 size) { return ::pglBoostSystemMemoryResourceLimit(size); }
Result IsRunningProcess(bool *out, u64 pid) { return ::pglIsRunningProcess(out, pid); }
Result EnableApplicationCrashReport(bool en) { return ::pglEnableApplicationCrashReport(en); }
Result IsApplicationCrashReportEnabled(bool *out) { return ::pglIsApplicationCrashReportEnabled(out); }
Result EnableApplicationAllThreadDumpOnCrash(bool en) { return ::pglEnableApplicationAllThreadDumpOnCrash(en); }
Result GetProcessId(u64 *out_pid, u64 program_id) { return ::pglGetProcessId(out_pid, program_id); }
Result TriggerSnapShotDumper(PglSnapShotDumpType dump_type, const char *arg) { return ::pglTriggerSnapShotDumper(dump_type, arg); }
Result CreateShellEvent(PglEventObserver *out) { return ::pglCreateShellEvent(out); }
Result EnableApplicationCrashReport2(u64 pid, bool en) { return ::pglEnableApplicationCrashReport2(pid, en); }
Result EventObserverGetShellEvent(PglEventObserver *observer, Event *out) { return ::pglEventObserverGetShellEvent(observer, out); }
Result EventObserverGetShellEventInfo(PglEventObserver *observer, PmProcessEventInfo *out) { return ::pglEventObserverGetShellEventInfo(observer, out); }
void EventObserverClose(PglEventObserver *observer) { ::pglEventObserverClose(observer); }

} // namespace pgl

namespace pl {

Result Initialize(PlServiceType service_type) { return ::plInitialize(service_type); }
void Exit(void) { ::plExit(); }
Service* GetServiceSession(void) { return ::plGetServiceSession(); }
void* GetSharedmemAddr(void) { return ::plGetSharedmemAddr(); }
Result GetSharedFontByType(PlFontData* font, PlSharedFontType SharedFontType) { return ::plGetSharedFontByType(font, SharedFontType); }
Result GetSharedFont(u64 LanguageCode, PlFontData* fonts, s32 max_fonts, s32* total_fonts) { return ::plGetSharedFont(LanguageCode, fonts, max_fonts, total_fonts); }

} // namespace pl

namespace pmbm {

Result Initialize(void) { return ::pmbmInitialize(); }
void Exit(void) { ::pmbmExit(); }
Service* GetServiceSession(void) { return ::pmbmGetServiceSession(); }
Result GetBootMode(PmBootMode *out) { return ::pmbmGetBootMode(out); }
Result SetMaintenanceBoot(void) { return ::pmbmSetMaintenanceBoot(); }

} // namespace pmbm

namespace pmdmnt {

Result Initialize(void) { return ::pmdmntInitialize(); }
void Exit(void) { ::pmdmntExit(); }
Service* GetServiceSession(void) { return ::pmdmntGetServiceSession(); }
Result GetJitDebugProcessIdList(u32* out_count, u64* out_pids, size_t max_pids) { return ::pmdmntGetJitDebugProcessIdList(out_count, out_pids, max_pids); }
Result StartProcess(u64 pid) { return ::pmdmntStartProcess(pid); }
Result GetProcessId(u64* pid_out, u64 program_id) { return ::pmdmntGetProcessId(pid_out, program_id); }
Result HookToCreateProcess(Event* out, u64 program_id) { return ::pmdmntHookToCreateProcess(out, program_id); }
Result GetApplicationProcessId(u64* pid_out) { return ::pmdmntGetApplicationProcessId(pid_out); }
Result HookToCreateApplicationProcess(Event* out) { return ::pmdmntHookToCreateApplicationProcess(out); }
Result ClearHook(u32 which) { return ::pmdmntClearHook(which); }
Result GetProgramId(u64* program_id_out, u64 pid) { return ::pmdmntGetProgramId(program_id_out, pid); }

} // namespace pmdmnt

namespace pminfo {

Result Initialize(void) { return ::pminfoInitialize(); }
void Exit(void) { ::pminfoExit(); }
Service* GetServiceSession(void) { return ::pminfoGetServiceSession(); }
Result GetProgramId(u64* program_id_out, u64 pid) { return ::pminfoGetProgramId(program_id_out, pid); }
Result GetAppletCurrentResourceLimitValues(PmResourceLimitValues* out) { return ::pminfoGetAppletCurrentResourceLimitValues(out); }
Result GetAppletPeakResourceLimitValues(PmResourceLimitValues* out) { return ::pminfoGetAppletPeakResourceLimitValues(out); }

} // namespace pminfo

namespace pmshell {

Result Initialize(void) { return ::pmshellInitialize(); }
void Exit(void) { ::pmshellExit(); }
Service* GetServiceSession(void) { return ::pmshellGetServiceSession(); }
Result LaunchProgram(u32 launch_flags, const NcmProgramLocation *location, u64 *pid) { return ::pmshellLaunchProgram(launch_flags, location, pid); }
Result TerminateProcess(u64 processID) { return ::pmshellTerminateProcess(processID); }
Result TerminateProgram(u64 program_id) { return ::pmshellTerminateProgram(program_id); }
Result GetProcessEventHandle(Event* out) { return ::pmshellGetProcessEventHandle(out); }
Result GetProcessEventInfo(PmProcessEventInfo* out) { return ::pmshellGetProcessEventInfo(out); }
Result CleanupProcess(u64 pid) { return ::pmshellCleanupProcess(pid); }
Result ClearJitDebugOccured(u64 pid) { return ::pmshellClearJitDebugOccured(pid); }
Result NotifyBootFinished(void) { return ::pmshellNotifyBootFinished(); }
Result GetApplicationProcessIdForShell(u64* pid_out) { return ::pmshellGetApplicationProcessIdForShell(pid_out); }
Result BoostSystemMemoryResourceLimit(u64 boost_size) { return ::pmshellBoostSystemMemoryResourceLimit(boost_size); }
Result BoostApplicationThreadResourceLimit(void) { return ::pmshellBoostApplicationThreadResourceLimit(); }
Result BoostSystemThreadResourceLimit(void) { return ::pmshellBoostSystemThreadResourceLimit(); }
Result GetProcessId(u64* pid_out, u64 program_id) { return ::pmshellGetProcessId(pid_out, program_id); }

} // namespace pmshell

namespace psc {

Result PmModuleGetRequest(PscPmModule *module, PscPmState *out_state, u32 *out_flags) { return ::pscPmModuleGetRequest(module, out_state, out_flags); }
Result PmModuleAcknowledge(PscPmModule *module, PscPmState state) { return ::pscPmModuleAcknowledge(module, state); }
Result PmModuleFinalize(PscPmModule *module) { return ::pscPmModuleFinalize(module); }
void PmModuleClose(PscPmModule *module) { ::pscPmModuleClose(module); }

} // namespace psc

namespace pscm {

Result Initialize(void) { return ::pscmInitialize(); }
void Exit(void) { ::pscmExit(); }
Service* GetServiceSession(void) { return ::pscmGetServiceSession(); }
Result GetPmModule(PscPmModule *out, PscPmModuleId module_id, const u32 *dependencies, size_t dependency_count, bool autoclear) { return ::pscmGetPmModule(out, module_id, dependencies, dependency_count, autoclear); }

} // namespace pscm

namespace psel {

Result UiCreate(PselUiSettings *ui, PselUiMode mode) { return ::pselUiCreate(ui, mode); }
void UiAddUser(PselUiSettings *ui, AccountUid user_id) { ::pselUiAddUser(ui, user_id); }
void UiSetAllowUserCreation(PselUiSettings *ui, bool flag) { ::pselUiSetAllowUserCreation(ui, flag); }
void UiSetNetworkServiceRequired(PselUiSettings *ui, bool flag) { ::pselUiSetNetworkServiceRequired(ui, flag); }
void UiSetSkipButtonEnabled(PselUiSettings *ui, bool flag) { ::pselUiSetSkipButtonEnabled(ui, flag); }
Result UiShow(PselUiSettings *ui, AccountUid *out_user) { return ::pselUiShow(ui, out_user); }
Result ShowUserSelectorForSystem(AccountUid *out_user, const PselUserSelectionSettings *settings, const PselUserSelectionSettingsForSystemService *settings_system) { return ::pselShowUserSelectorForSystem(out_user, settings, settings_system); }
Result ShowUserSelectorForLauncher(AccountUid *out_user, const PselUserSelectionSettings *settings, u64 application_id) { return ::pselShowUserSelectorForLauncher(out_user, settings, application_id); }
Result ShowUserSelector(AccountUid *out_user, const PselUserSelectionSettings *settings) { return ::pselShowUserSelector(out_user, settings); }
Result ShowUserCreator(void) { return ::pselShowUserCreator(); }
Result ShowUserIconEditor(AccountUid user) { return ::pselShowUserIconEditor(user); }
Result ShowUserNicknameEditor(AccountUid user) { return ::pselShowUserNicknameEditor(user); }
Result ShowUserCreatorForStarter(void) { return ::pselShowUserCreatorForStarter(); }
Result ShowNintendoAccountNnidLinker(AccountUid user) { return ::pselShowNintendoAccountNnidLinker(user); }
Result ShowUserQualificationPromoter(AccountUid user) { return ::pselShowUserQualificationPromoter(user); }

} // namespace psel

namespace psm {

Result Initialize(void) { return ::psmInitialize(); }
void Exit(void) { ::psmExit(); }
Service* GetServiceSession(void) { return ::psmGetServiceSession(); }
Result GetBatteryChargePercentage(u32 *out) { return ::psmGetBatteryChargePercentage(out); }
Result GetChargerType(PsmChargerType *out) { return ::psmGetChargerType(out); }
Result EnableBatteryCharging(void) { return ::psmEnableBatteryCharging(); }
Result DisableBatteryCharging(void) { return ::psmDisableBatteryCharging(); }
Result IsBatteryChargingEnabled(bool *out) { return ::psmIsBatteryChargingEnabled(out); }
Result AcquireControllerPowerSupply(void) { return ::psmAcquireControllerPowerSupply(); }
Result ReleaseControllerPowerSupply(void) { return ::psmReleaseControllerPowerSupply(); }
Result EnableEnoughPowerChargeEmulation(void) { return ::psmEnableEnoughPowerChargeEmulation(); }
Result DisableEnoughPowerChargeEmulation(void) { return ::psmDisableEnoughPowerChargeEmulation(); }
Result EnableFastBatteryCharging(void) { return ::psmEnableFastBatteryCharging(); }
Result DisableFastBatteryCharging(void) { return ::psmDisableFastBatteryCharging(); }
Result GetBatteryVoltageState(PsmBatteryVoltageState *out) { return ::psmGetBatteryVoltageState(out); }
Result GetRawBatteryChargePercentage(double *out) { return ::psmGetRawBatteryChargePercentage(out); }
Result IsEnoughPowerSupplied(bool *out) { return ::psmIsEnoughPowerSupplied(out); }
Result GetBatteryAgePercentage(double *out) { return ::psmGetBatteryAgePercentage(out); }
Result GetBatteryChargeInfoEvent(Event* out_event, bool autoclear) { return ::psmGetBatteryChargeInfoEvent(out_event, autoclear); }
Result GetBatteryChargeInfoFields(PsmBatteryChargeInfoFields *out_fields) { return ::psmGetBatteryChargeInfoFields(out_fields); }
Result GetBatteryChargeCalibratedEvent(Event* out_event, bool autoclear) { return ::psmGetBatteryChargeCalibratedEvent(out_event, autoclear); }
Result BindStateChangeEvent(PsmSession* s, bool ChargerType, bool PowerSupply, bool BatteryVoltage) { return ::psmBindStateChangeEvent(s, ChargerType, PowerSupply, BatteryVoltage); }
Result WaitStateChangeEvent(PsmSession* s, u64 timeout) { return ::psmWaitStateChangeEvent(s, timeout); }
Result UnbindStateChangeEvent(PsmSession* s) { return ::psmUnbindStateChangeEvent(s); }

} // namespace psm

namespace random {

void Get(void* buf, size_t len) { ::randomGet(buf, len); }
u64 Get64(void) { return ::randomGet64(); }

} // namespace random

namespace resolver {

Result GetLastResult(void) { return ::resolverGetLastResult(); }
u32 GetCancelHandle(void) { return ::resolverGetCancelHandle(); }
bool GetEnableServiceDiscovery(void) { return ::resolverGetEnableServiceDiscovery(); }
bool GetEnableDnsCache(void) { return ::resolverGetEnableDnsCache(); }
void SetEnableServiceDiscovery(bool enable) { ::resolverSetEnableServiceDiscovery(enable); }
void SetEnableDnsCache(bool enable) { ::resolverSetEnableDnsCache(enable); }
Result Cancel(u32 handle) { return ::resolverCancel(handle); }
Result RemoveHostnameFromCache(const char* hostname) { return ::resolverRemoveHostnameFromCache(hostname); }
Result RemoveIpAddressFromCache(u32 ip) { return ::resolverRemoveIpAddressFromCache(ip); }

} // namespace resolver

namespace ringcon {

Result Create(RingCon *c, HidNpadIdType id) { return ::ringconCreate(c, id); }
void Close(RingCon *c) { ::ringconClose(c); }
u32 GetErrorFlags(RingCon *c) { return ::ringconGetErrorFlags(c); }
bool GetErrorFlag(RingCon *c, RingConErrorFlag flag) { return ::ringconGetErrorFlag(c, flag); }
RingConFwVersion GetFwVersion(RingCon *c) { return ::ringconGetFwVersion(c); }
void GetId(RingCon *c, u64 *id_l, u64 *id_h) { ::ringconGetId(c, id_l, id_h); }
s16 GetUnkCal(RingCon *c) { return ::ringconGetUnkCal(c); }
s32 GetTotalPushCount(RingCon *c) { return ::ringconGetTotalPushCount(c); }
void GetManuCal(RingCon *c, RingConManuCal *out) { ::ringconGetManuCal(c, out); }
void GetUserCal(RingCon *c, RingConUserCal *out) { ::ringconGetUserCal(c, out); }
Result UpdateUserCal(RingCon *c, RingConUserCal cal) { return ::ringconUpdateUserCal(c, cal); }
Result ReadFwVersion(RingCon *c, RingConFwVersion *out) { return ::ringconReadFwVersion(c, out); }
Result ReadId(RingCon *c, u64 *id_l, u64 *id_h) { return ::ringconReadId(c, id_l, id_h); }
Result GetPollingData(RingCon *c, RingConPollingData *out, s32 count, s32 *total_out) { return ::ringconGetPollingData(c, out, count, total_out); }
Result Cmdx00020105(RingCon *c, u32 *out) { return ::ringconCmdx00020105(c, out); }
Result ReadManuCal(RingCon *c, RingConManuCal *out) { return ::ringconReadManuCal(c, out); }
Result ReadUnkCal(RingCon *c, s16 *out) { return ::ringconReadUnkCal(c, out); }
Result ReadUserCal(RingCon *c, RingConUserCal *out) { return ::ringconReadUserCal(c, out); }
Result ReadRepCount(RingCon *c, s32 *out, RingConDataValid *data_valid) { return ::ringconReadRepCount(c, out, data_valid); }
Result ReadTotalPushCount(RingCon *c, s32 *out, RingConDataValid *data_valid) { return ::ringconReadTotalPushCount(c, out, data_valid); }
Result ResetRepCount(RingCon *c) { return ::ringconResetRepCount(c); }
Result WriteUserCal(RingCon *c, RingConUserCal cal) { return ::ringconWriteUserCal(c, cal); }

} // namespace ringcon

namespace rmutex {

void Init(RMutex* m) { ::rmutexInit(m); }
void Lock(RMutex* m) { ::rmutexLock(m); }
bool TryLock(RMutex* m) { return ::rmutexTryLock(m); }
void Unlock(RMutex* m) { ::rmutexUnlock(m); }

} // namespace rmutex

namespace ro {

Result DmntInitialize(void) { return ::roDmntInitialize(); }
void DmntExit(void) { ::roDmntExit(); }
Service* DmntGetServiceSession(void) { return ::roDmntGetServiceSession(); }
Result DmntGetProcessModuleInfo(u64 pid, LoaderModuleInfo *out_module_infos, size_t max_out_modules, s32 *num_out) { return ::roDmntGetProcessModuleInfo(pid, out_module_infos, max_out_modules, num_out); }

} // namespace ro

namespace ro1 {

Result Initialize(void) { return ::ro1Initialize(); }
void Exit(void) { ::ro1Exit(); }
Service* GetServiceSession(void) { return ::ro1GetServiceSession(); }
Result LoadNro(u64* out_address, u64 nro_address, u64 nro_size, u64 bss_address, u64 bss_size) { return ::ro1LoadNro(out_address, nro_address, nro_size, bss_address, bss_size); }
Result UnloadNro(u64 nro_address) { return ::ro1UnloadNro(nro_address); }
Result LoadNrr(u64 nrr_address, u64 nrr_size) { return ::ro1LoadNrr(nrr_address, nrr_size); }
Result UnloadNrr(u64 nrr_address) { return ::ro1UnloadNrr(nrr_address); }
Result LoadNrrEx(u64 nrr_address, u64 nrr_size) { return ::ro1LoadNrrEx(nrr_address, nrr_size); }

} // namespace ro1

namespace romfs {

Result MountSelf(const char *name) { return ::romfsMountSelf(name); }
Result MountFromFile(FsFile file, u64 offset, const char *name) { return ::romfsMountFromFile(file, offset, name); }
Result MountFromStorage(FsStorage storage, u64 offset, const char *name) { return ::romfsMountFromStorage(storage, offset, name); }
Result MountFromCurrentProcess(const char *name) { return ::romfsMountFromCurrentProcess(name); }
Result MountDataStorageFromProgram(u64 program_id, const char *name) { return ::romfsMountDataStorageFromProgram(program_id, name); }
Result MountFromFsdev(const char *path, u64 offset, const char *name) { return ::romfsMountFromFsdev(path, offset, name); }
Result MountFromDataArchive(u64 dataId, NcmStorageId storageId, const char *name) { return ::romfsMountFromDataArchive(dataId, storageId, name); }
Result Unmount(const char *name) { return ::romfsUnmount(name); }
Result Init(void) { return ::romfsInit(); }
Result Exit(void) { return ::romfsExit(); }

} // namespace romfs

namespace rwlock {

void Init(RwLock* r) { ::rwlockInit(r); }
void ReadLock(RwLock* r) { ::rwlockReadLock(r); }
bool TryReadLock(RwLock* r) { return ::rwlockTryReadLock(r); }
void ReadUnlock(RwLock* r) { ::rwlockReadUnlock(r); }
void WriteLock(RwLock* r) { ::rwlockWriteLock(r); }
bool TryWriteLock(RwLock* r) { return ::rwlockTryWriteLock(r); }
void WriteUnlock(RwLock* r) { ::rwlockWriteUnlock(r); }
bool IsWriteLockHeldByCurrentThread(RwLock* r) { return ::rwlockIsWriteLockHeldByCurrentThread(r); }
bool IsOwnedByCurrentThread(RwLock* r) { return ::rwlockIsOwnedByCurrentThread(r); }

} // namespace rwlock

namespace semaphore {

void Init(Semaphore *s, u64 initial_count) { ::semaphoreInit(s, initial_count); }
void Signal(Semaphore *s) { ::semaphoreSignal(s); }
void Wait(Semaphore *s) { ::semaphoreWait(s); }
bool TryWait(Semaphore *s) { return ::semaphoreTryWait(s); }

} // namespace semaphore

namespace service {

bool IsActive(Service* s) { return ::serviceIsActive(s); }
bool IsOverride(Service* s) { return ::serviceIsOverride(s); }
bool IsDomain(Service* s) { return ::serviceIsDomain(s); }
bool IsDomainSubservice(Service* s) { return ::serviceIsDomainSubservice(s); }
u32 GetObjectId(Service* s) { return ::serviceGetObjectId(s); }
void Create(Service* s, Handle h) { ::serviceCreate(s, h); }
void CreateNonDomainSubservice(Service* s, Service* parent, Handle h) { ::serviceCreateNonDomainSubservice(s, parent, h); }
void CreateDomainSubservice(Service* s, Service* parent, u32 object_id) { ::serviceCreateDomainSubservice(s, parent, object_id); }
void Close(Service* s) { ::serviceClose(s); }
Result Clone(Service* s, Service* out_s) { return ::serviceClone(s, out_s); }
Result CloneEx(Service* s, u32 tag, Service* out_s) { return ::serviceCloneEx(s, tag, out_s); }
Result ConvertToDomain(Service* s) { return ::serviceConvertToDomain(s); }
void RequestFormatProcessBuffer(CmifRequestFormat* fmt, u32 attr) { ::_serviceRequestFormatProcessBuffer(fmt, attr); }
void RequestProcessBuffer(CmifRequest* req, const SfBuffer* buf, u32 attr) { ::_serviceRequestProcessBuffer(req, buf, attr); }
void* MakeRequest(Service* s, u32 request_id, u32 context, u32 data_size, bool send_pid, const SfBufferAttrs buffer_attrs, const SfBuffer* buffers, u32 num_objects, const Service* const* objects, u32 num_handles, const Handle* handles) { return ::serviceMakeRequest(s, request_id, context, data_size, send_pid, buffer_attrs, buffers, num_objects, objects, num_handles, handles); }
void ResponseGetHandle(CmifResponse* res, SfOutHandleAttr type, Handle* out) { ::_serviceResponseGetHandle(res, type, out); }
Result ParseResponse(Service* s, u32 out_size, void** out_data, u32 num_out_objects, Service* out_objects, const SfOutHandleAttrs out_handle_attrs, Handle* out_handles) { return ::serviceParseResponse(s, out_size, out_data, num_out_objects, out_objects, out_handle_attrs, out_handles); }
Result DispatchImpl(Service* s, u32 request_id, const void* in_data, u32 in_data_size, void* out_data, u32 out_data_size, SfDispatchParams disp) { return ::serviceDispatchImpl(s, request_id, in_data, in_data_size, out_data, out_data_size, disp); }

} // namespace service

namespace sessionmgr {

Result Create(SessionMgr* mgr, Handle root_session, u32 num_sessions) { return ::sessionmgrCreate(mgr, root_session, num_sessions); }
void Close(SessionMgr* mgr) { ::sessionmgrClose(mgr); }
int AttachClient(SessionMgr* mgr) { return ::sessionmgrAttachClient(mgr); }
void DetachClient(SessionMgr* mgr, int slot) { ::sessionmgrDetachClient(mgr, slot); }
Handle GetClientSession(SessionMgr* mgr, int slot) { return ::sessionmgrGetClientSession(mgr, slot); }

} // namespace sessionmgr

namespace set {

Result Initialize(void) { return ::setInitialize(); }
void Exit(void) { ::setExit(); }
Service* GetServiceSession(void) { return ::setGetServiceSession(); }
Result MakeLanguage(u64 LanguageCode, SetLanguage *Language) { return ::setMakeLanguage(LanguageCode, Language); }
Result MakeLanguageCode(SetLanguage Language, u64 *LanguageCode) { return ::setMakeLanguageCode(Language, LanguageCode); }
Result GetSystemLanguage(u64 *LanguageCode) { return ::setGetSystemLanguage(LanguageCode); }
Result GetLanguageCode(u64 *LanguageCode) { return ::setGetLanguageCode(LanguageCode); }
Result GetAvailableLanguageCodes(s32 *total_entries, u64 *LanguageCodes, size_t max_entries) { return ::setGetAvailableLanguageCodes(total_entries, LanguageCodes, max_entries); }
Result GetAvailableLanguageCodeCount(s32 *total) { return ::setGetAvailableLanguageCodeCount(total); }
Result GetRegionCode(SetRegion *out) { return ::setGetRegionCode(out); }
Result GetQuestFlag(bool *out) { return ::setGetQuestFlag(out); }
Result GetDeviceNickname(SetSysDeviceNickName *nickname) { return ::setGetDeviceNickname(nickname); }

} // namespace set

namespace setcal {

Result Initialize(void) { return ::setcalInitialize(); }
void Exit(void) { ::setcalExit(); }
Service* GetServiceSession(void) { return ::setcalGetServiceSession(); }
Result GetBdAddress(SetCalBdAddress *out) { return ::setcalGetBdAddress(out); }
Result GetConfigurationId1(SetCalConfigurationId1 *out) { return ::setcalGetConfigurationId1(out); }
Result GetAccelerometerOffset(SetCalAccelerometerOffset *out) { return ::setcalGetAccelerometerOffset(out); }
Result GetAccelerometerScale(SetCalAccelerometerScale *out) { return ::setcalGetAccelerometerScale(out); }
Result GetGyroscopeOffset(SetCalGyroscopeOffset *out) { return ::setcalGetGyroscopeOffset(out); }
Result GetGyroscopeScale(SetCalGyroscopeScale *out) { return ::setcalGetGyroscopeScale(out); }
Result GetWirelessLanMacAddress(SetCalMacAddress *out) { return ::setcalGetWirelessLanMacAddress(out); }
Result GetWirelessLanCountryCodeCount(s32 *out_count) { return ::setcalGetWirelessLanCountryCodeCount(out_count); }
Result GetWirelessLanCountryCodes(s32 *total_out, SetCalCountryCode *codes, s32 count) { return ::setcalGetWirelessLanCountryCodes(total_out, codes, count); }
Result GetSerialNumber(SetCalSerialNumber *out) { return ::setcalGetSerialNumber(out); }
Result SetInitialSystemAppletProgramId(u64 program_id) { return ::setcalSetInitialSystemAppletProgramId(program_id); }
Result SetOverlayDispProgramId(u64 program_id) { return ::setcalSetOverlayDispProgramId(program_id); }
Result GetBatteryLot(SetBatteryLot *out) { return ::setcalGetBatteryLot(out); }
Result GetEciDeviceCertificate(SetCalEccB233DeviceCertificate *out) { return ::setcalGetEciDeviceCertificate(out); }
Result GetEticketDeviceCertificate(SetCalRsa2048DeviceCertificate *out) { return ::setcalGetEticketDeviceCertificate(out); }
Result GetSslKey(SetCalSslKey *out) { return ::setcalGetSslKey(out); }
Result GetSslCertificate(SetCalSslCertificate *out) { return ::setcalGetSslCertificate(out); }
Result GetGameCardKey(SetCalGameCardKey *out) { return ::setcalGetGameCardKey(out); }
Result GetGameCardCertificate(SetCalGameCardCertificate *out) { return ::setcalGetGameCardCertificate(out); }
Result GetEciDeviceKey(SetCalEccB233DeviceKey *out) { return ::setcalGetEciDeviceKey(out); }
Result GetEticketDeviceKey(SetCalRsa2048DeviceKey *out) { return ::setcalGetEticketDeviceKey(out); }
Result GetSpeakerParameter(SetCalSpeakerParameter *out) { return ::setcalGetSpeakerParameter(out); }
Result GetLcdVendorId(u32 *out_vendor_id) { return ::setcalGetLcdVendorId(out_vendor_id); }
Result GetEciDeviceCertificate2(SetCalRsa2048DeviceCertificate *out) { return ::setcalGetEciDeviceCertificate2(out); }
Result GetEciDeviceKey2(SetCalRsa2048DeviceKey *out) { return ::setcalGetEciDeviceKey2(out); }
Result GetAmiiboKey(SetCalAmiiboKey *out) { return ::setcalGetAmiiboKey(out); }
Result GetAmiiboEcqvCertificate(SetCalAmiiboEcqvCertificate *out) { return ::setcalGetAmiiboEcqvCertificate(out); }
Result GetAmiiboEcdsaCertificate(SetCalAmiiboEcdsaCertificate *out) { return ::setcalGetAmiiboEcdsaCertificate(out); }
Result GetAmiiboEcqvBlsKey(SetCalAmiiboEcqvBlsKey *out) { return ::setcalGetAmiiboEcqvBlsKey(out); }
Result GetAmiiboEcqvBlsCertificate(SetCalAmiiboEcqvBlsCertificate *out) { return ::setcalGetAmiiboEcqvBlsCertificate(out); }
Result GetAmiiboEcqvBlsRootCertificate(SetCalAmiiboEcqvBlsRootCertificate *out) { return ::setcalGetAmiiboEcqvBlsRootCertificate(out); }
Result GetUsbTypeCPowerSourceCircuitVersion(u8 *out_version) { return ::setcalGetUsbTypeCPowerSourceCircuitVersion(out_version); }
Result GetAnalogStickModuleTypeL(u8 *out_type) { return ::setcalGetAnalogStickModuleTypeL(out_type); }
Result GetAnalogStickModelParameterL(SetCalAnalogStickModelParameter *out) { return ::setcalGetAnalogStickModelParameterL(out); }
Result GetAnalogStickFactoryCalibrationL(SetCalAnalogStickFactoryCalibration *out) { return ::setcalGetAnalogStickFactoryCalibrationL(out); }
Result GetAnalogStickModuleTypeR(u8 *out_type) { return ::setcalGetAnalogStickModuleTypeR(out_type); }
Result GetAnalogStickModelParameterR(SetCalAnalogStickModelParameter *out) { return ::setcalGetAnalogStickModelParameterR(out); }
Result GetAnalogStickFactoryCalibrationR(SetCalAnalogStickFactoryCalibration *out) { return ::setcalGetAnalogStickFactoryCalibrationR(out); }
Result GetConsoleSixAxisSensorModuleType(u8 *out_type) { return ::setcalGetConsoleSixAxisSensorModuleType(out_type); }
Result GetConsoleSixAxisSensorHorizontalOffset(SetCalConsoleSixAxisSensorHorizontalOffset *out) { return ::setcalGetConsoleSixAxisSensorHorizontalOffset(out); }
Result GetBatteryVersion(u8 *out_version) { return ::setcalGetBatteryVersion(out_version); }
Result GetDeviceId(u64 *out_device_id) { return ::setcalGetDeviceId(out_device_id); }
Result GetConsoleSixAxisSensorMountType(u8 *out_type) { return ::setcalGetConsoleSixAxisSensorMountType(out_type); }

} // namespace setcal

namespace setfd {

Result Initialize(void) { return ::setfdInitialize(); }
void Exit(void) { ::setfdExit(); }
Service* GetServiceSession(void) { return ::setfdGetServiceSession(); }
Result SetSettingsItemValue(const char *name, const char *item_key, const void *value_in, s64 value_in_size) { return ::setfdSetSettingsItemValue(name, item_key, value_in, value_in_size); }
Result ResetSettingsItemValue(const char *name, const char *item_key) { return ::setfdResetSettingsItemValue(name, item_key); }

} // namespace setfd

namespace setsys {

Result Initialize(void) { return ::setsysInitialize(); }
void Exit(void) { ::setsysExit(); }
Service* GetServiceSession(void) { return ::setsysGetServiceSession(); }
Result SetLanguageCode(u64 LanguageCode) { return ::setsysSetLanguageCode(LanguageCode); }
Result SetNetworkSettings(const SetSysNetworkSettings *settings, s32 count) { return ::setsysSetNetworkSettings(settings, count); }
Result GetNetworkSettings(s32 *total_out, SetSysNetworkSettings *settings, s32 count) { return ::setsysGetNetworkSettings(total_out, settings, count); }
Result GetFirmwareVersion(SetSysFirmwareVersion *out) { return ::setsysGetFirmwareVersion(out); }
Result GetFirmwareVersionDigest(SetSysFirmwareVersionDigest *out) { return ::setsysGetFirmwareVersionDigest(out); }
Result GetLockScreenFlag(bool *out) { return ::setsysGetLockScreenFlag(out); }
Result SetLockScreenFlag(bool flag) { return ::setsysSetLockScreenFlag(flag); }
Result GetBacklightSettings(SetSysBacklightSettings *out) { return ::setsysGetBacklightSettings(out); }
Result SetBacklightSettings(const SetSysBacklightSettings *settings) { return ::setsysSetBacklightSettings(settings); }
Result SetBluetoothDevicesSettings(const SetSysBluetoothDevicesSettings *settings, s32 count) { return ::setsysSetBluetoothDevicesSettings(settings, count); }
Result GetBluetoothDevicesSettings(s32 *total_out, SetSysBluetoothDevicesSettings *settings, s32 count) { return ::setsysGetBluetoothDevicesSettings(total_out, settings, count); }
Result GetExternalSteadyClockSourceId(Uuid *out) { return ::setsysGetExternalSteadyClockSourceId(out); }
Result SetExternalSteadyClockSourceId(const Uuid *uuid) { return ::setsysSetExternalSteadyClockSourceId(uuid); }
Result GetUserSystemClockContext(TimeSystemClockContext *out) { return ::setsysGetUserSystemClockContext(out); }
Result SetUserSystemClockContext(const TimeSystemClockContext *context) { return ::setsysSetUserSystemClockContext(context); }
Result GetAccountSettings(SetSysAccountSettings *out) { return ::setsysGetAccountSettings(out); }
Result SetAccountSettings(SetSysAccountSettings settings) { return ::setsysSetAccountSettings(settings); }
Result GetAudioVolume(SetSysAudioDevice device, SetSysAudioVolume *out) { return ::setsysGetAudioVolume(device, out); }
Result SetAudioVolume(SetSysAudioDevice device, const SetSysAudioVolume *volume) { return ::setsysSetAudioVolume(device, volume); }
Result GetEulaVersions(s32 *total_out, SetSysEulaVersion *versions, s32 count) { return ::setsysGetEulaVersions(total_out, versions, count); }
Result SetEulaVersions(const SetSysEulaVersion *versions, s32 count) { return ::setsysSetEulaVersions(versions, count); }
Result GetColorSetId(ColorSetId *out) { return ::setsysGetColorSetId(out); }
Result SetColorSetId(ColorSetId id) { return ::setsysSetColorSetId(id); }
Result GetConsoleInformationUploadFlag(bool *out) { return ::setsysGetConsoleInformationUploadFlag(out); }
Result SetConsoleInformationUploadFlag(bool flag) { return ::setsysSetConsoleInformationUploadFlag(flag); }
Result GetAutomaticApplicationDownloadFlag(bool *out) { return ::setsysGetAutomaticApplicationDownloadFlag(out); }
Result SetAutomaticApplicationDownloadFlag(bool flag) { return ::setsysSetAutomaticApplicationDownloadFlag(flag); }
Result GetNotificationSettings(SetSysNotificationSettings *out) { return ::setsysGetNotificationSettings(out); }
Result SetNotificationSettings(const SetSysNotificationSettings *settings) { return ::setsysSetNotificationSettings(settings); }
Result GetAccountNotificationSettings(s32 *total_out, SetSysAccountNotificationSettings *settings, s32 count) { return ::setsysGetAccountNotificationSettings(total_out, settings, count); }
Result SetAccountNotificationSettings(const SetSysAccountNotificationSettings *settings, s32 count) { return ::setsysSetAccountNotificationSettings(settings, count); }
Result GetVibrationMasterVolume(float *out) { return ::setsysGetVibrationMasterVolume(out); }
Result SetVibrationMasterVolume(float volume) { return ::setsysSetVibrationMasterVolume(volume); }
Result GetSettingsItemValueSize(const char *name, const char *item_key, u64 *size_out) { return ::setsysGetSettingsItemValueSize(name, item_key, size_out); }
Result GetSettingsItemValue(const char *name, const char *item_key, void *value_out, size_t value_out_size, u64 *size_out) { return ::setsysGetSettingsItemValue(name, item_key, value_out, value_out_size, size_out); }
Result GetTvSettings(SetSysTvSettings *out) { return ::setsysGetTvSettings(out); }
Result SetTvSettings(const SetSysTvSettings *settings) { return ::setsysSetTvSettings(settings); }
Result GetEdid(SetSysEdid *out) { return ::setsysGetEdid(out); }
Result SetEdid(const SetSysEdid *edid) { return ::setsysSetEdid(edid); }
Result GetAudioOutputMode(SetSysAudioOutputModeTarget target, SetSysAudioOutputMode *out) { return ::setsysGetAudioOutputMode(target, out); }
Result SetAudioOutputMode(SetSysAudioOutputModeTarget target, SetSysAudioOutputMode mode) { return ::setsysSetAudioOutputMode(target, mode); }
Result GetSpeakerAutoMuteFlag(bool *out) { return ::setsysGetSpeakerAutoMuteFlag(out); }
Result SetSpeakerAutoMuteFlag(bool flag) { return ::setsysSetSpeakerAutoMuteFlag(flag); }
Result GetQuestFlag(bool *out) { return ::setsysGetQuestFlag(out); }
Result SetQuestFlag(bool flag) { return ::setsysSetQuestFlag(flag); }
Result GetDataDeletionSettings(SetSysDataDeletionSettings *out) { return ::setsysGetDataDeletionSettings(out); }
Result SetDataDeletionSettings(const SetSysDataDeletionSettings *settings) { return ::setsysSetDataDeletionSettings(settings); }
Result GetInitialSystemAppletProgramId(u64 *out) { return ::setsysGetInitialSystemAppletProgramId(out); }
Result GetOverlayDispProgramId(u64 *out) { return ::setsysGetOverlayDispProgramId(out); }
Result GetDeviceTimeZoneLocationName(TimeLocationName *out) { return ::setsysGetDeviceTimeZoneLocationName(out); }
Result SetDeviceTimeZoneLocationName(const TimeLocationName *name) { return ::setsysSetDeviceTimeZoneLocationName(name); }
Result GetWirelessCertificationFileSize(u64 *out_size) { return ::setsysGetWirelessCertificationFileSize(out_size); }
Result GetWirelessCertificationFile(void* buffer, size_t size, u64 *out_size) { return ::setsysGetWirelessCertificationFile(buffer, size, out_size); }
Result SetRegionCode(SetRegion region) { return ::setsysSetRegionCode(region); }
Result GetNetworkSystemClockContext(TimeSystemClockContext *out) { return ::setsysGetNetworkSystemClockContext(out); }
Result SetNetworkSystemClockContext(const TimeSystemClockContext *context) { return ::setsysSetNetworkSystemClockContext(context); }
Result IsUserSystemClockAutomaticCorrectionEnabled(bool *out) { return ::setsysIsUserSystemClockAutomaticCorrectionEnabled(out); }
Result SetUserSystemClockAutomaticCorrectionEnabled(bool flag) { return ::setsysSetUserSystemClockAutomaticCorrectionEnabled(flag); }
Result GetDebugModeFlag(bool *out) { return ::setsysGetDebugModeFlag(out); }
Result GetPrimaryAlbumStorage(SetSysPrimaryAlbumStorage *out) { return ::setsysGetPrimaryAlbumStorage(out); }
Result SetPrimaryAlbumStorage(SetSysPrimaryAlbumStorage storage) { return ::setsysSetPrimaryAlbumStorage(storage); }
Result GetUsb30EnableFlag(bool *out) { return ::setsysGetUsb30EnableFlag(out); }
Result SetUsb30EnableFlag(bool flag) { return ::setsysSetUsb30EnableFlag(flag); }
Result GetBatteryLot(SetBatteryLot *out) { return ::setsysGetBatteryLot(out); }
Result GetSerialNumber(SetSysSerialNumber *out) { return ::setsysGetSerialNumber(out); }
Result GetNfcEnableFlag(bool *out) { return ::setsysGetNfcEnableFlag(out); }
Result SetNfcEnableFlag(bool flag) { return ::setsysSetNfcEnableFlag(flag); }
Result GetSleepSettings(SetSysSleepSettings *out) { return ::setsysGetSleepSettings(out); }
Result SetSleepSettings(const SetSysSleepSettings *settings) { return ::setsysSetSleepSettings(settings); }
Result GetWirelessLanEnableFlag(bool *out) { return ::setsysGetWirelessLanEnableFlag(out); }
Result SetWirelessLanEnableFlag(bool flag) { return ::setsysSetWirelessLanEnableFlag(flag); }
Result GetInitialLaunchSettings(SetSysInitialLaunchSettings *out) { return ::setsysGetInitialLaunchSettings(out); }
Result SetInitialLaunchSettings(const SetSysInitialLaunchSettings *settings) { return ::setsysSetInitialLaunchSettings(settings); }
Result GetDeviceNickname(SetSysDeviceNickName *nickname) { return ::setsysGetDeviceNickname(nickname); }
Result SetDeviceNickname(const SetSysDeviceNickName *nickname) { return ::setsysSetDeviceNickname(nickname); }
Result GetProductModel(SetSysProductModel *model) { return ::setsysGetProductModel(model); }
Result GetLdnChannel(s32 *out) { return ::setsysGetLdnChannel(out); }
Result SetLdnChannel(s32 channel) { return ::setsysSetLdnChannel(channel); }
Result AcquireTelemetryDirtyFlagEventHandle(Event *out_event) { return ::setsysAcquireTelemetryDirtyFlagEventHandle(out_event); }
Result GetTelemetryDirtyFlags(u64 *flags_0, u64 *flags_1) { return ::setsysGetTelemetryDirtyFlags(flags_0, flags_1); }
Result GetPtmBatteryLot(SetBatteryLot *out) { return ::setsysGetPtmBatteryLot(out); }
Result SetPtmBatteryLot(const SetBatteryLot *lot) { return ::setsysSetPtmBatteryLot(lot); }
Result GetPtmFuelGaugeParameter(SetSysPtmFuelGaugeParameter *out) { return ::setsysGetPtmFuelGaugeParameter(out); }
Result SetPtmFuelGaugeParameter(const SetSysPtmFuelGaugeParameter *parameter) { return ::setsysSetPtmFuelGaugeParameter(parameter); }
Result GetBluetoothEnableFlag(bool *out) { return ::setsysGetBluetoothEnableFlag(out); }
Result SetBluetoothEnableFlag(bool flag) { return ::setsysSetBluetoothEnableFlag(flag); }
Result GetMiiAuthorId(Uuid *out) { return ::setsysGetMiiAuthorId(out); }
Result SetShutdownRtcValue(u64 value) { return ::setsysSetShutdownRtcValue(value); }
Result GetShutdownRtcValue(u64 *out) { return ::setsysGetShutdownRtcValue(out); }
Result AcquireFatalDirtyFlagEventHandle(Event *out_event) { return ::setsysAcquireFatalDirtyFlagEventHandle(out_event); }
Result GetFatalDirtyFlags(u64 *flags_0, u64 *flags_1) { return ::setsysGetFatalDirtyFlags(flags_0, flags_1); }
Result GetAutoUpdateEnableFlag(bool *out) { return ::setsysGetAutoUpdateEnableFlag(out); }
Result SetAutoUpdateEnableFlag(bool flag) { return ::setsysSetAutoUpdateEnableFlag(flag); }
Result GetNxControllerSettings(s32 *total_out, SetSysNxControllerLegacySettings *settings, s32 count) { return ::setsysGetNxControllerSettings(total_out, settings, count); }
Result SetNxControllerSettings(const SetSysNxControllerLegacySettings *settings, s32 count) { return ::setsysSetNxControllerSettings(settings, count); }
Result GetBatteryPercentageFlag(bool *out) { return ::setsysGetBatteryPercentageFlag(out); }
Result SetBatteryPercentageFlag(bool flag) { return ::setsysSetBatteryPercentageFlag(flag); }
Result GetExternalRtcResetFlag(bool *out) { return ::setsysGetExternalRtcResetFlag(out); }
Result SetExternalRtcResetFlag(bool flag) { return ::setsysSetExternalRtcResetFlag(flag); }
Result GetUsbFullKeyEnableFlag(bool *out) { return ::setsysGetUsbFullKeyEnableFlag(out); }
Result SetUsbFullKeyEnableFlag(bool flag) { return ::setsysSetUsbFullKeyEnableFlag(flag); }
Result SetExternalSteadyClockInternalOffset(u64 offset) { return ::setsysSetExternalSteadyClockInternalOffset(offset); }
Result GetExternalSteadyClockInternalOffset(u64 *out) { return ::setsysGetExternalSteadyClockInternalOffset(out); }
Result GetBacklightSettingsEx(SetSysBacklightSettingsEx *out) { return ::setsysGetBacklightSettingsEx(out); }
Result SetBacklightSettingsEx(const SetSysBacklightSettingsEx *settings) { return ::setsysSetBacklightSettingsEx(settings); }
Result GetHeadphoneVolumeWarningCount(u32 *out) { return ::setsysGetHeadphoneVolumeWarningCount(out); }
Result SetHeadphoneVolumeWarningCount(u32 count) { return ::setsysSetHeadphoneVolumeWarningCount(count); }
Result GetBluetoothAfhEnableFlag(bool *out) { return ::setsysGetBluetoothAfhEnableFlag(out); }
Result SetBluetoothAfhEnableFlag(bool flag) { return ::setsysSetBluetoothAfhEnableFlag(flag); }
Result GetBluetoothBoostEnableFlag(bool *out) { return ::setsysGetBluetoothBoostEnableFlag(out); }
Result SetBluetoothBoostEnableFlag(bool flag) { return ::setsysSetBluetoothBoostEnableFlag(flag); }
Result GetInRepairProcessEnableFlag(bool *out) { return ::setsysGetInRepairProcessEnableFlag(out); }
Result SetInRepairProcessEnableFlag(bool flag) { return ::setsysSetInRepairProcessEnableFlag(flag); }
Result GetHeadphoneVolumeUpdateFlag(bool *out) { return ::setsysGetHeadphoneVolumeUpdateFlag(out); }
Result SetHeadphoneVolumeUpdateFlag(bool flag) { return ::setsysSetHeadphoneVolumeUpdateFlag(flag); }
Result NeedsToUpdateHeadphoneVolume(u8 *a0, u8 *a1, u8 *a2, bool flag) { return ::setsysNeedsToUpdateHeadphoneVolume(a0, a1, a2, flag); }
Result GetPushNotificationActivityModeOnSleep(u32 *out) { return ::setsysGetPushNotificationActivityModeOnSleep(out); }
Result SetPushNotificationActivityModeOnSleep(u32 mode) { return ::setsysSetPushNotificationActivityModeOnSleep(mode); }
Result GetServiceDiscoveryControlSettings(SetSysServiceDiscoveryControlSettings *out) { return ::setsysGetServiceDiscoveryControlSettings(out); }
Result SetServiceDiscoveryControlSettings(SetSysServiceDiscoveryControlSettings settings) { return ::setsysSetServiceDiscoveryControlSettings(settings); }
Result GetErrorReportSharePermission(SetSysErrorReportSharePermission *out) { return ::setsysGetErrorReportSharePermission(out); }
Result SetErrorReportSharePermission(SetSysErrorReportSharePermission permission) { return ::setsysSetErrorReportSharePermission(permission); }
Result GetAppletLaunchFlags(u32 *out) { return ::setsysGetAppletLaunchFlags(out); }
Result SetAppletLaunchFlags(u32 flags) { return ::setsysSetAppletLaunchFlags(flags); }
Result GetConsoleSixAxisSensorAccelerationBias(SetSysConsoleSixAxisSensorAccelerationBias *out) { return ::setsysGetConsoleSixAxisSensorAccelerationBias(out); }
Result SetConsoleSixAxisSensorAccelerationBias(const SetSysConsoleSixAxisSensorAccelerationBias *bias) { return ::setsysSetConsoleSixAxisSensorAccelerationBias(bias); }
Result GetConsoleSixAxisSensorAngularVelocityBias(SetSysConsoleSixAxisSensorAngularVelocityBias *out) { return ::setsysGetConsoleSixAxisSensorAngularVelocityBias(out); }
Result SetConsoleSixAxisSensorAngularVelocityBias(const SetSysConsoleSixAxisSensorAngularVelocityBias *bias) { return ::setsysSetConsoleSixAxisSensorAngularVelocityBias(bias); }
Result GetConsoleSixAxisSensorAccelerationGain(SetSysConsoleSixAxisSensorAccelerationGain *out) { return ::setsysGetConsoleSixAxisSensorAccelerationGain(out); }
Result SetConsoleSixAxisSensorAccelerationGain(const SetSysConsoleSixAxisSensorAccelerationGain *gain) { return ::setsysSetConsoleSixAxisSensorAccelerationGain(gain); }
Result GetConsoleSixAxisSensorAngularVelocityGain(SetSysConsoleSixAxisSensorAngularVelocityGain *out) { return ::setsysGetConsoleSixAxisSensorAngularVelocityGain(out); }
Result SetConsoleSixAxisSensorAngularVelocityGain(const SetSysConsoleSixAxisSensorAngularVelocityGain *gain) { return ::setsysSetConsoleSixAxisSensorAngularVelocityGain(gain); }
Result GetKeyboardLayout(::SetKeyboardLayout *out) { return ::setsysGetKeyboardLayout(out); }
Result SetKeyboardLayout(::SetKeyboardLayout layout) { return ::setsysSetKeyboardLayout(layout); }
Result GetWebInspectorFlag(bool *out) { return ::setsysGetWebInspectorFlag(out); }
Result GetAllowedSslHosts(s32 *total_out, SetSysAllowedSslHosts *out, s32 count) { return ::setsysGetAllowedSslHosts(total_out, out, count); }
Result GetHostFsMountPoint(SetSysHostFsMountPoint *out) { return ::setsysGetHostFsMountPoint(out); }
Result GetRequiresRunRepairTimeReviser(bool *out) { return ::setsysGetRequiresRunRepairTimeReviser(out); }
Result SetRequiresRunRepairTimeReviser(bool flag) { return ::setsysSetRequiresRunRepairTimeReviser(flag); }
Result SetBlePairingSettings(const SetSysBlePairingSettings *settings, s32 count) { return ::setsysSetBlePairingSettings(settings, count); }
Result GetBlePairingSettings(s32 *total_out, SetSysBlePairingSettings *settings, s32 count) { return ::setsysGetBlePairingSettings(total_out, settings, count); }
Result GetConsoleSixAxisSensorAngularVelocityTimeBias(SetSysConsoleSixAxisSensorAngularVelocityTimeBias *out) { return ::setsysGetConsoleSixAxisSensorAngularVelocityTimeBias(out); }
Result SetConsoleSixAxisSensorAngularVelocityTimeBias(const SetSysConsoleSixAxisSensorAngularVelocityTimeBias *bias) { return ::setsysSetConsoleSixAxisSensorAngularVelocityTimeBias(bias); }
Result GetConsoleSixAxisSensorAngularAcceleration(SetSysConsoleSixAxisSensorAngularAcceleration *out) { return ::setsysGetConsoleSixAxisSensorAngularAcceleration(out); }
Result SetConsoleSixAxisSensorAngularAcceleration(const SetSysConsoleSixAxisSensorAngularAcceleration *acceleration) { return ::setsysSetConsoleSixAxisSensorAngularAcceleration(acceleration); }
Result GetRebootlessSystemUpdateVersion(SetSysRebootlessSystemUpdateVersion *out) { return ::setsysGetRebootlessSystemUpdateVersion(out); }
Result GetDeviceTimeZoneLocationUpdatedTime(TimeSteadyClockTimePoint *out) { return ::setsysGetDeviceTimeZoneLocationUpdatedTime(out); }
Result SetDeviceTimeZoneLocationUpdatedTime(const TimeSteadyClockTimePoint *time_point) { return ::setsysSetDeviceTimeZoneLocationUpdatedTime(time_point); }
Result GetUserSystemClockAutomaticCorrectionUpdatedTime(TimeSteadyClockTimePoint *out) { return ::setsysGetUserSystemClockAutomaticCorrectionUpdatedTime(out); }
Result SetUserSystemClockAutomaticCorrectionUpdatedTime(const TimeSteadyClockTimePoint *time_point) { return ::setsysSetUserSystemClockAutomaticCorrectionUpdatedTime(time_point); }
Result GetAccountOnlineStorageSettings(s32 *total_out, SetSysAccountOnlineStorageSettings *settings, s32 count) { return ::setsysGetAccountOnlineStorageSettings(total_out, settings, count); }
Result SetAccountOnlineStorageSettings(const SetSysAccountOnlineStorageSettings *settings, s32 count) { return ::setsysSetAccountOnlineStorageSettings(settings, count); }
Result GetPctlReadyFlag(bool *out) { return ::setsysGetPctlReadyFlag(out); }
Result SetPctlReadyFlag(bool flag) { return ::setsysSetPctlReadyFlag(flag); }
Result GetAnalogStickUserCalibrationL(SetSysAnalogStickUserCalibration *out) { return ::setsysGetAnalogStickUserCalibrationL(out); }
Result SetAnalogStickUserCalibrationL(const SetSysAnalogStickUserCalibration *calibration) { return ::setsysSetAnalogStickUserCalibrationL(calibration); }
Result GetAnalogStickUserCalibrationR(SetSysAnalogStickUserCalibration *out) { return ::setsysGetAnalogStickUserCalibrationR(out); }
Result SetAnalogStickUserCalibrationR(const SetSysAnalogStickUserCalibration *calibration) { return ::setsysSetAnalogStickUserCalibrationR(calibration); }
Result GetPtmBatteryVersion(u8 *out) { return ::setsysGetPtmBatteryVersion(out); }
Result SetPtmBatteryVersion(u8 version) { return ::setsysSetPtmBatteryVersion(version); }
Result GetUsb30HostEnableFlag(bool *out) { return ::setsysGetUsb30HostEnableFlag(out); }
Result SetUsb30HostEnableFlag(bool flag) { return ::setsysSetUsb30HostEnableFlag(flag); }
Result GetUsb30DeviceEnableFlag(bool *out) { return ::setsysGetUsb30DeviceEnableFlag(out); }
Result SetUsb30DeviceEnableFlag(bool flag) { return ::setsysSetUsb30DeviceEnableFlag(flag); }
Result GetThemeId(s32 type, SetSysThemeId *out) { return ::setsysGetThemeId(type, out); }
Result SetThemeId(s32 type, const SetSysThemeId *theme_id) { return ::setsysSetThemeId(type, theme_id); }
Result GetChineseTraditionalInputMethod(::SetChineseTraditionalInputMethod *out) { return ::setsysGetChineseTraditionalInputMethod(out); }
Result SetChineseTraditionalInputMethod(::SetChineseTraditionalInputMethod method) { return ::setsysSetChineseTraditionalInputMethod(method); }
Result GetPtmCycleCountReliability(SetSysPtmCycleCountReliability *out) { return ::setsysGetPtmCycleCountReliability(out); }
Result SetPtmCycleCountReliability(SetSysPtmCycleCountReliability reliability) { return ::setsysSetPtmCycleCountReliability(reliability); }
Result GetHomeMenuScheme(SetSysHomeMenuScheme *out) { return ::setsysGetHomeMenuScheme(out); }
Result GetThemeSettings(SetSysThemeSettings *out) { return ::setsysGetThemeSettings(out); }
Result SetThemeSettings(const SetSysThemeSettings *settings) { return ::setsysSetThemeSettings(settings); }
Result GetThemeKey(FsArchiveMacKey *out) { return ::setsysGetThemeKey(out); }
Result SetThemeKey(const FsArchiveMacKey *key) { return ::setsysSetThemeKey(key); }
Result GetZoomFlag(bool *out) { return ::setsysGetZoomFlag(out); }
Result SetZoomFlag(bool flag) { return ::setsysSetZoomFlag(flag); }
Result GetT(bool *out) { return ::setsysGetT(out); }
Result SetT(bool flag) { return ::setsysSetT(flag); }
Result GetPlatformRegion(SetSysPlatformRegion *out) { return ::setsysGetPlatformRegion(out); }
Result SetPlatformRegion(SetSysPlatformRegion region) { return ::setsysSetPlatformRegion(region); }
Result GetHomeMenuSchemeModel(u32 *out) { return ::setsysGetHomeMenuSchemeModel(out); }
Result GetMemoryUsageRateFlag(bool *out) { return ::setsysGetMemoryUsageRateFlag(out); }
Result GetTouchScreenMode(SetSysTouchScreenMode *out) { return ::setsysGetTouchScreenMode(out); }
Result SetTouchScreenMode(SetSysTouchScreenMode mode) { return ::setsysSetTouchScreenMode(mode); }
Result GetButtonConfigSettingsFull(s32 *total_out, SetSysButtonConfigSettings *settings, s32 count) { return ::setsysGetButtonConfigSettingsFull(total_out, settings, count); }
Result SetButtonConfigSettingsFull(const SetSysButtonConfigSettings *settings, s32 count) { return ::setsysSetButtonConfigSettingsFull(settings, count); }
Result GetButtonConfigSettingsEmbedded(s32 *total_out, SetSysButtonConfigSettings *settings, s32 count) { return ::setsysGetButtonConfigSettingsEmbedded(total_out, settings, count); }
Result SetButtonConfigSettingsEmbedded(const SetSysButtonConfigSettings *settings, s32 count) { return ::setsysSetButtonConfigSettingsEmbedded(settings, count); }
Result GetButtonConfigSettingsLeft(s32 *total_out, SetSysButtonConfigSettings *settings, s32 count) { return ::setsysGetButtonConfigSettingsLeft(total_out, settings, count); }
Result SetButtonConfigSettingsLeft(const SetSysButtonConfigSettings *settings, s32 count) { return ::setsysSetButtonConfigSettingsLeft(settings, count); }
Result GetButtonConfigSettingsRight(s32 *total_out, SetSysButtonConfigSettings *settings, s32 count) { return ::setsysGetButtonConfigSettingsRight(total_out, settings, count); }
Result SetButtonConfigSettingsRight(const SetSysButtonConfigSettings *settings, s32 count) { return ::setsysSetButtonConfigSettingsRight(settings, count); }
Result GetButtonConfigRegisteredSettingsEmbedded(SetSysButtonConfigRegisteredSettings *settings) { return ::setsysGetButtonConfigRegisteredSettingsEmbedded(settings); }
Result SetButtonConfigRegisteredSettingsEmbedded(const SetSysButtonConfigRegisteredSettings *settings) { return ::setsysSetButtonConfigRegisteredSettingsEmbedded(settings); }
Result GetButtonConfigRegisteredSettings(s32 *total_out, SetSysButtonConfigRegisteredSettings *settings, s32 count) { return ::setsysGetButtonConfigRegisteredSettings(total_out, settings, count); }
Result SetButtonConfigRegisteredSettings(const SetSysButtonConfigRegisteredSettings *settings, s32 count) { return ::setsysSetButtonConfigRegisteredSettings(settings, count); }
Result GetFieldTestingFlag(bool *out) { return ::setsysGetFieldTestingFlag(out); }
Result SetFieldTestingFlag(bool flag) { return ::setsysSetFieldTestingFlag(flag); }
Result GetNxControllerSettingsEx(s32 *total_out, SetSysNxControllerSettings *settings, s32 count) { return ::setsysGetNxControllerSettingsEx(total_out, settings, count); }
Result SetNxControllerSettingsEx(const SetSysNxControllerSettings *settings, s32 count) { return ::setsysSetNxControllerSettingsEx(settings, count); }

} // namespace setsys

namespace sha1 {

void ContextCreate(Sha1Context *out) { ::sha1ContextCreate(out); }
void ContextUpdate(Sha1Context *ctx, const void *src, size_t size) { ::sha1ContextUpdate(ctx, src, size); }
void ContextGetHash(Sha1Context *ctx, void *dst) { ::sha1ContextGetHash(ctx, dst); }
void CalculateHash(void *dst, const void *src, size_t size) { ::sha1CalculateHash(dst, src, size); }

} // namespace sha1

namespace sha256 {

void ContextCreate(Sha256Context *out) { ::sha256ContextCreate(out); }
void ContextUpdate(Sha256Context *ctx, const void *src, size_t size) { ::sha256ContextUpdate(ctx, src, size); }
void ContextGetHash(Sha256Context *ctx, void *dst) { ::sha256ContextGetHash(ctx, dst); }
void CalculateHash(void *dst, const void *src, size_t size) { ::sha256CalculateHash(dst, src, size); }

} // namespace sha256

namespace shmem {

Result Create(SharedMemory* s, size_t size, Permission local_perm, Permission remote_perm) { return ::shmemCreate(s, size, local_perm, remote_perm); }
void LoadRemote(SharedMemory* s, Handle handle, size_t size, Permission perm) { ::shmemLoadRemote(s, handle, size, perm); }
Result Map(SharedMemory* s) { return ::shmemMap(s); }
Result Unmap(SharedMemory* s) { return ::shmemUnmap(s); }
void* GetAddr(SharedMemory* s) { return ::shmemGetAddr(s); }
Result Close(SharedMemory* s) { return ::shmemClose(s); }

} // namespace shmem

namespace sm {

u64 ServiceNameToU64(SmServiceName name) { return ::smServiceNameToU64(name); }
SmServiceName ServiceNameFromU64(u64 name) { return ::smServiceNameFromU64(name); }
bool ServiceNamesAreEqual(SmServiceName a, SmServiceName b) { return ::smServiceNamesAreEqual(a, b); }
SmServiceName EncodeName(const char* name) { return ::smEncodeName(name); }
Result Initialize(void) { return ::smInitialize(); }
void Exit(void) { ::smExit(); }
Result GetServiceWrapper(Service* service_out, SmServiceName name) { return ::smGetServiceWrapper(service_out, name); }
Result GetServiceOriginal(Handle* handle_out, SmServiceName name) { return ::smGetServiceOriginal(handle_out, name); }
Result GetService(Service* service_out, const char* name) { return ::smGetService(service_out, name); }
Handle GetServiceOverride(SmServiceName name) { return ::smGetServiceOverride(name); }
Result RegisterService(Handle* handle_out, SmServiceName name, bool is_light, s32 max_sessions) { return ::smRegisterService(handle_out, name, is_light, max_sessions); }
Result RegisterServiceCmif(Handle* handle_out, SmServiceName name, bool is_light, s32 max_sessions) { return ::smRegisterServiceCmif(handle_out, name, is_light, max_sessions); }
Result RegisterServiceTipc(Handle* handle_out, SmServiceName name, bool is_light, s32 max_sessions) { return ::smRegisterServiceTipc(handle_out, name, is_light, max_sessions); }
Result UnregisterService(SmServiceName name) { return ::smUnregisterService(name); }
Result UnregisterServiceCmif(SmServiceName name) { return ::smUnregisterServiceCmif(name); }
Result UnregisterServiceTipc(SmServiceName name) { return ::smUnregisterServiceTipc(name); }
Result DetachClient(void) { return ::smDetachClient(); }
Result DetachClientCmif(void) { return ::smDetachClientCmif(); }
Result DetachClientTipc(void) { return ::smDetachClientTipc(); }
Service * GetServiceSession(void) { return ::smGetServiceSession(); }
TipcService * GetServiceSessionTipc(void) { return ::smGetServiceSessionTipc(); }
void AddOverrideHandle(SmServiceName name, Handle handle) { ::smAddOverrideHandle(name, handle); }
Result ManagerInitialize(void) { return ::smManagerInitialize(); }
void ManagerExit(void) { ::smManagerExit(); }
Result ManagerRegisterProcess(u64 pid, const void *acid_sac, size_t acid_sac_size, const void *aci0_sac, size_t aci0_sac_size) { return ::smManagerRegisterProcess(pid, acid_sac, acid_sac_size, aci0_sac, aci0_sac_size); }
Result ManagerUnregisterProcess(u64 pid) { return ::smManagerUnregisterProcess(pid); }
Result ManagerCmifInitialize(void) { return ::smManagerCmifInitialize(); }
void ManagerCmifExit(void) { ::smManagerCmifExit(); }
Service* ManagerCmifGetServiceSession(void) { return ::smManagerCmifGetServiceSession(); }
Result ManagerCmifRegisterProcess(u64 pid, const void *acid_sac, size_t acid_sac_size, const void *aci0_sac, size_t aci0_sac_size) { return ::smManagerCmifRegisterProcess(pid, acid_sac, acid_sac_size, aci0_sac, aci0_sac_size); }
Result ManagerCmifUnregisterProcess(u64 pid) { return ::smManagerCmifUnregisterProcess(pid); }
Result ManagerTipcInitialize(void) { return ::smManagerTipcInitialize(); }
void ManagerTipcExit(void) { ::smManagerTipcExit(); }
TipcService* ManagerTipcGetServiceSession(void) { return ::smManagerTipcGetServiceSession(); }
Result ManagerTipcRegisterProcess(u64 pid, const void *acid_sac, size_t acid_sac_size, const void *aci0_sac, size_t aci0_sac_size) { return ::smManagerTipcRegisterProcess(pid, acid_sac, acid_sac_size, aci0_sac, aci0_sac_size); }
Result ManagerTipcUnregisterProcess(u64 pid) { return ::smManagerTipcUnregisterProcess(pid); }

} // namespace sm

namespace socket {

const SocketInitConfig * GetDefaultInitConfig(void) { return ::socketGetDefaultInitConfig(); }
Result Initialize(const SocketInitConfig *config) { return ::socketInitialize(config); }
Result GetLastResult(void) { return ::socketGetLastResult(); }
void Exit(void) { ::socketExit(); }
Result InitializeDefault(void) { return ::socketInitializeDefault(); }
int SslConnectionSetSocketDescriptor(SslConnection *c, int sockfd) { return ::socketSslConnectionSetSocketDescriptor(c, sockfd); }
int SslConnectionGetSocketDescriptor(SslConnection *c) { return ::socketSslConnectionGetSocketDescriptor(c); }
#ifdef _SOCKLEN_T_DECLARED
int SslConnectionSetDtlsSocketDescriptor(SslConnection *c, int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return ::socketSslConnectionSetDtlsSocketDescriptor(c, sockfd, addr, addrlen); }
#endif
int NifmRequestRegisterSocketDescriptor(NifmRequest* r, int sockfd) { return ::socketNifmRequestRegisterSocketDescriptor(r, sockfd); }
int NifmRequestUnregisterSocketDescriptor(NifmRequest* r, int sockfd) { return ::socketNifmRequestUnregisterSocketDescriptor(r, sockfd); }

} // namespace socket

namespace spl {

Result Initialize(void) { return ::splInitialize(); }
void Exit(void) { ::splExit(); }
Service* GetServiceSession(void) { return ::splGetServiceSession(); }
Result CryptoInitialize(void) { return ::splCryptoInitialize(); }
void CryptoExit(void) { ::splCryptoExit(); }
Service* CryptoGetServiceSession(void) { return ::splCryptoGetServiceSession(); }
Result SslInitialize(void) { return ::splSslInitialize(); }
void SslExit(void) { ::splSslExit(); }
Service* SslGetServiceSession(void) { return ::splSslGetServiceSession(); }
Result EsInitialize(void) { return ::splEsInitialize(); }
void EsExit(void) { ::splEsExit(); }
Service* EsGetServiceSession(void) { return ::splEsGetServiceSession(); }
Result FsInitialize(void) { return ::splFsInitialize(); }
void FsExit(void) { ::splFsExit(); }
Service* FsGetServiceSession(void) { return ::splFsGetServiceSession(); }
Result ManuInitialize(void) { return ::splManuInitialize(); }
void ManuExit(void) { ::splManuExit(); }
Service* ManuGetServiceSession(void) { return ::splManuGetServiceSession(); }
Result GetConfig(SplConfigItem config_item, u64 *out_config) { return ::splGetConfig(config_item, out_config); }
Result UserExpMod(const void *input, const void *modulus, const void *exp, size_t exp_size, void *dst) { return ::splUserExpMod(input, modulus, exp, exp_size, dst); }
Result SetConfig(SplConfigItem config_item, u64 value) { return ::splSetConfig(config_item, value); }
Result GetRandomBytes(void *out, size_t out_size) { return ::splGetRandomBytes(out, out_size); }
Result IsDevelopment(bool *out_is_development) { return ::splIsDevelopment(out_is_development); }
Result SetBootReason(u32 value) { return ::splSetBootReason(value); }
Result GetBootReason(u32 *out_value) { return ::splGetBootReason(out_value); }
Result CryptoGenerateAesKek(const void *wrapped_kek, u32 key_generation, u32 option, void *out_sealed_kek) { return ::splCryptoGenerateAesKek(wrapped_kek, key_generation, option, out_sealed_kek); }
Result CryptoLoadAesKey(const void *sealed_kek, const void *wrapped_key, u32 keyslot) { return ::splCryptoLoadAesKey(sealed_kek, wrapped_key, keyslot); }
Result CryptoGenerateAesKey(const void *sealed_kek, const void *wrapped_key, void *out_sealed_key) { return ::splCryptoGenerateAesKey(sealed_kek, wrapped_key, out_sealed_key); }
Result CryptoDecryptAesKey(const void *wrapped_key, u32 key_generation, u32 option, void *out_sealed_key) { return ::splCryptoDecryptAesKey(wrapped_key, key_generation, option, out_sealed_key); }
Result CryptoCryptAesCtr(const void *input, void *output, size_t size, u32 keyslot, const void *ctr) { return ::splCryptoCryptAesCtr(input, output, size, keyslot, ctr); }
Result CryptoComputeCmac(const void *input, size_t size, u32 keyslot, void *out_cmac) { return ::splCryptoComputeCmac(input, size, keyslot, out_cmac); }
Result CryptoLockAesEngine(u32 *out_keyslot) { return ::splCryptoLockAesEngine(out_keyslot); }
Result CryptoUnlockAesEngine(u32 keyslot) { return ::splCryptoUnlockAesEngine(keyslot); }
Result CryptoGetSecurityEngineEvent(Event *out_event) { return ::splCryptoGetSecurityEngineEvent(out_event); }
Result RsaDecryptPrivateKey(const void *sealed_kek, const void *wrapped_key, const void *wrapped_rsa_key, size_t wrapped_rsa_key_size, RsaKeyVersion version, void *dst, size_t dst_size) { return ::splRsaDecryptPrivateKey(sealed_kek, wrapped_key, wrapped_rsa_key, wrapped_rsa_key_size, version, dst, dst_size); }
Result SslLoadSecureExpModKey(const void *sealed_kek, const void *wrapped_key, const void *wrapped_rsa_key, size_t wrapped_rsa_key_size) { return ::splSslLoadSecureExpModKey(sealed_kek, wrapped_key, wrapped_rsa_key, wrapped_rsa_key_size); }
Result SslSecureExpMod(const void *input, const void *modulus, void *dst) { return ::splSslSecureExpMod(input, modulus, dst); }
Result EsLoadRsaOaepKey(const void *sealed_kek, const void *wrapped_key, const void *wrapped_rsa_key, size_t wrapped_rsa_key_size, RsaKeyVersion version) { return ::splEsLoadRsaOaepKey(sealed_kek, wrapped_key, wrapped_rsa_key, wrapped_rsa_key_size, version); }
Result EsUnwrapRsaOaepWrappedTitlekey(const void *rsa_wrapped_titlekey, const void *modulus, const void *label_hash, size_t label_hash_size, u32 key_generation, void *out_sealed_titlekey) { return ::splEsUnwrapRsaOaepWrappedTitlekey(rsa_wrapped_titlekey, modulus, label_hash, label_hash_size, key_generation, out_sealed_titlekey); }
Result EsUnwrapAesWrappedTitlekey(const void *aes_wrapped_titlekey, u32 key_generation, void *out_sealed_titlekey) { return ::splEsUnwrapAesWrappedTitlekey(aes_wrapped_titlekey, key_generation, out_sealed_titlekey); }
Result EsLoadSecureExpModKey(const void *sealed_kek, const void *wrapped_key, const void *wrapped_rsa_key, size_t wrapped_rsa_key_size) { return ::splEsLoadSecureExpModKey(sealed_kek, wrapped_key, wrapped_rsa_key, wrapped_rsa_key_size); }
Result EsSecureExpMod(const void *input, const void *modulus, void *dst) { return ::splEsSecureExpMod(input, modulus, dst); }
Result EsUnwrapElicenseKey(const void *rsa_wrapped_elicense_key, const void *modulus, const void *label_hash, size_t label_hash_size, u32 key_generation, void *out_sealed_elicense_key) { return ::splEsUnwrapElicenseKey(rsa_wrapped_elicense_key, modulus, label_hash, label_hash_size, key_generation, out_sealed_elicense_key); }
Result EsLoadElicenseKey(const void *sealed_elicense_key, u32 keyslot) { return ::splEsLoadElicenseKey(sealed_elicense_key, keyslot); }
Result FsLoadSecureExpModKey(const void *sealed_kek, const void *wrapped_key, const void *wrapped_rsa_key, size_t wrapped_rsa_key_size, RsaKeyVersion version) { return ::splFsLoadSecureExpModKey(sealed_kek, wrapped_key, wrapped_rsa_key, wrapped_rsa_key_size, version); }
Result FsSecureExpMod(const void *input, const void *modulus, void *dst) { return ::splFsSecureExpMod(input, modulus, dst); }
Result FsGenerateSpecificAesKey(const void *wrapped_key, u32 key_generation, u32 option, void *out_sealed_key) { return ::splFsGenerateSpecificAesKey(wrapped_key, key_generation, option, out_sealed_key); }
Result FsLoadTitlekey(const void *sealed_titlekey, u32 keyslot) { return ::splFsLoadTitlekey(sealed_titlekey, keyslot); }
Result FsGetPackage2Hash(void *out_hash) { return ::splFsGetPackage2Hash(out_hash); }
Result ManuEncryptRsaKeyForImport(const void *sealed_kek_pre, const void *wrapped_key_pre, const void *sealed_kek_post, const void *wrapped_kek_post, u32 option, const void *wrapped_rsa_key, void *out_wrapped_rsa_key, size_t rsa_key_size) { return ::splManuEncryptRsaKeyForImport(sealed_kek_pre, wrapped_key_pre, sealed_kek_post, wrapped_kek_post, option, wrapped_rsa_key, out_wrapped_rsa_key, rsa_key_size); }

} // namespace spl

namespace spsm {

Result Initialize(void) { return ::spsmInitialize(); }
void Exit(void) { ::spsmExit(); }
Service* GetServiceSession(void) { return ::spsmGetServiceSession(); }
Result Shutdown(bool reboot) { return ::spsmShutdown(reboot); }
Result PutErrorState(void) { return ::spsmPutErrorState(); }

} // namespace spsm

namespace ssl {

Result Initialize(u32 num_sessions) { return ::sslInitialize(num_sessions); }
void Exit(void) { ::sslExit(); }
Service* GetServiceSession(void) { return ::sslGetServiceSession(); }
Result CreateContext(SslContext *c, u32 ssl_version) { return ::sslCreateContext(c, ssl_version); }
Result GetContextCount(u32 *out) { return ::sslGetContextCount(out); }
Result GetCertificates(void* buffer, u32 size, u32 *ca_cert_ids, u32 count, u32 *total_out) { return ::sslGetCertificates(buffer, size, ca_cert_ids, count, total_out); }
Result GetCertificateBufSize(u32 *ca_cert_ids, u32 count, u32 *out) { return ::sslGetCertificateBufSize(ca_cert_ids, count, out); }
Result FlushSessionCache(const char *str, size_t str_bufsize, SslFlushSessionCacheOptionType type, u32 *out) { return ::sslFlushSessionCache(str, str_bufsize, type, out); }
Result SetDebugOption(const void* buffer, size_t size, SslDebugOptionType type) { return ::sslSetDebugOption(buffer, size, type); }
Result GetDebugOption(void* buffer, size_t size, SslDebugOptionType type) { return ::sslGetDebugOption(buffer, size, type); }
Result ClearTls12FallbackFlag(void) { return ::sslClearTls12FallbackFlag(); }
Result SetThreadCoreMask(u64 mask) { return ::sslSetThreadCoreMask(mask); }
Result GetThreadCoreMask(u64 *out) { return ::sslGetThreadCoreMask(out); }
void ContextClose(SslContext *c) { ::sslContextClose(c); }
Result ContextSetOption(SslContext *c, SslContextOption option, s32 value) { return ::sslContextSetOption(c, option, value); }
Result ContextGetOption(SslContext *c, SslContextOption option, s32 *out) { return ::sslContextGetOption(c, option, out); }
Result ContextCreateConnection(SslContext *c, SslConnection *conn) { return ::sslContextCreateConnection(c, conn); }
Result ContextGetConnectionCount(SslContext *c, u32 *out) { return ::sslContextGetConnectionCount(c, out); }
Result ContextImportServerPki(SslContext *c, const void* buffer, u32 size, SslCertificateFormat format, u64 *id) { return ::sslContextImportServerPki(c, buffer, size, format, id); }
Result ContextImportClientPki(SslContext *c, const void* pkcs12, u32 pkcs12_size, const char *pw, u32 pw_size, u64 *id) { return ::sslContextImportClientPki(c, pkcs12, pkcs12_size, pw, pw_size, id); }
Result ContextRemovePki(SslContext *c, u64 id) { return ::sslContextRemovePki(c, id); }
Result ContextRegisterInternalPki(SslContext *c, SslInternalPki internal_pki, u64 *id) { return ::sslContextRegisterInternalPki(c, internal_pki, id); }
Result ContextAddPolicyOid(SslContext *c, const char *str, u32 str_bufsize) { return ::sslContextAddPolicyOid(c, str, str_bufsize); }
Result ContextImportCrl(SslContext *c, const void* buffer, u32 size, u64 *id) { return ::sslContextImportCrl(c, buffer, size, id); }
Result ContextImportClientCertKeyPki(SslContext *c, const void* cert, u32 cert_size, const void* key, u32 key_size, SslCertificateFormat format, u64 *id) { return ::sslContextImportClientCertKeyPki(c, cert, cert_size, key, key_size, format, id); }
Result ContextGeneratePrivateKeyAndCert(SslContext *c, void* cert, u32 cert_size, void* key, u32 key_size, u32 val, const SslKeyAndCertParams *params, u32 *out_certsize, u32 *out_keysize) { return ::sslContextGeneratePrivateKeyAndCert(c, cert, cert_size, key, key_size, val, params, out_certsize, out_keysize); }
Result ContextCreateConnectionForSystem(SslContext *c, SslConnection *conn) { return ::sslContextCreateConnectionForSystem(c, conn); }
void ConnectionClose(SslConnection *c) { ::sslConnectionClose(c); }
Result ConnectionSetSocketDescriptor(SslConnection *c, int sockfd, int *out_sockfd) { return ::sslConnectionSetSocketDescriptor(c, sockfd, out_sockfd); }
Result ConnectionSetHostName(SslConnection *c, const char* str, u32 str_bufsize) { return ::sslConnectionSetHostName(c, str, str_bufsize); }
Result ConnectionSetVerifyOption(SslConnection *c, u32 verify_option) { return ::sslConnectionSetVerifyOption(c, verify_option); }
Result ConnectionSetIoMode(SslConnection *c, SslIoMode mode) { return ::sslConnectionSetIoMode(c, mode); }
Result ConnectionGetSocketDescriptor(SslConnection *c, int *sockfd) { return ::sslConnectionGetSocketDescriptor(c, sockfd); }
Result ConnectionGetHostName(SslConnection *c, char *str, u32 str_bufsize, u32 *out) { return ::sslConnectionGetHostName(c, str, str_bufsize, out); }
Result ConnectionGetVerifyOption(SslConnection *c, u32 *out) { return ::sslConnectionGetVerifyOption(c, out); }
Result ConnectionGetIoMode(SslConnection *c, SslIoMode *out) { return ::sslConnectionGetIoMode(c, out); }
Result ConnectionDoHandshake(SslConnection *c, u32 *out_size, u32 *total_certs, void* server_certbuf, u32 server_certbuf_size) { return ::sslConnectionDoHandshake(c, out_size, total_certs, server_certbuf, server_certbuf_size); }
Result ConnectionGetServerCertDetail(const void* certbuf, u32 certbuf_size, u32 cert_index, void** cert, u32 *cert_size) { return ::sslConnectionGetServerCertDetail(certbuf, certbuf_size, cert_index, cert, cert_size); }
Result ConnectionRead(SslConnection *c, void* buffer, u32 size, u32 *out_size) { return ::sslConnectionRead(c, buffer, size, out_size); }
Result ConnectionWrite(SslConnection *c, const void* buffer, u32 size, u32 *out_size) { return ::sslConnectionWrite(c, buffer, size, out_size); }
Result ConnectionPending(SslConnection *c, s32 *out) { return ::sslConnectionPending(c, out); }
Result ConnectionPeek(SslConnection *c, void* buffer, u32 size, u32 *out_size) { return ::sslConnectionPeek(c, buffer, size, out_size); }
Result ConnectionPoll(SslConnection *c, u32 in_pollevent, u32 *out_pollevent, u32 timeout) { return ::sslConnectionPoll(c, in_pollevent, out_pollevent, timeout); }
Result ConnectionGetVerifyCertError(SslConnection *c) { return ::sslConnectionGetVerifyCertError(c); }
Result ConnectionGetNeededServerCertBufferSize(SslConnection *c, u32 *out) { return ::sslConnectionGetNeededServerCertBufferSize(c, out); }
Result ConnectionSetSessionCacheMode(SslConnection *c, SslSessionCacheMode mode) { return ::sslConnectionSetSessionCacheMode(c, mode); }
Result ConnectionGetSessionCacheMode(SslConnection *c, SslSessionCacheMode *out) { return ::sslConnectionGetSessionCacheMode(c, out); }
Result ConnectionFlushSessionCache(SslConnection *c) { return ::sslConnectionFlushSessionCache(c); }
Result ConnectionSetRenegotiationMode(SslConnection *c, SslRenegotiationMode mode) { return ::sslConnectionSetRenegotiationMode(c, mode); }
Result ConnectionGetRenegotiationMode(SslConnection *c, SslRenegotiationMode *out) { return ::sslConnectionGetRenegotiationMode(c, out); }
Result ConnectionSetOption(SslConnection *c, SslOptionType option, bool flag) { return ::sslConnectionSetOption(c, option, flag); }
Result ConnectionGetOption(SslConnection *c, SslOptionType option, bool *out) { return ::sslConnectionGetOption(c, option, out); }
Result ConnectionGetVerifyCertErrors(SslConnection *c, u32 *out0, u32 *out1, Result *errors, u32 count) { return ::sslConnectionGetVerifyCertErrors(c, out0, out1, errors, count); }
Result ConnectionGetCipherInfo(SslConnection *c, SslCipherInfo *out) { return ::sslConnectionGetCipherInfo(c, out); }
Result ConnectionSetNextAlpnProto(SslConnection *c, const u8 *buffer, u32 size) { return ::sslConnectionSetNextAlpnProto(c, buffer, size); }
Result ConnectionGetNextAlpnProto(SslConnection *c, SslAlpnProtoState *state, u32 *out, u8 *buffer, u32 size) { return ::sslConnectionGetNextAlpnProto(c, state, out, buffer, size); }
Result ConnectionSetDtlsSocketDescriptor(SslConnection *c, int sockfd, const void* buf, size_t size, int *out_sockfd) { return ::sslConnectionSetDtlsSocketDescriptor(c, sockfd, buf, size, out_sockfd); }
Result ConnectionGetDtlsHandshakeTimeout(SslConnection *c, u64 *out) { return ::sslConnectionGetDtlsHandshakeTimeout(c, out); }
Result ConnectionSetPrivateOption(SslConnection *c, SslPrivateOptionType option, u32 value) { return ::sslConnectionSetPrivateOption(c, option, value); }
Result ConnectionSetSrtpCiphers(SslConnection *c, const u16 *ciphers, u32 count) { return ::sslConnectionSetSrtpCiphers(c, ciphers, count); }
Result ConnectionGetSrtpCipher(SslConnection *c, u16 *out) { return ::sslConnectionGetSrtpCipher(c, out); }
Result ConnectionExportKeyingMaterial(SslConnection *c, u8 *outbuf, u32 outbuf_size, const char *label, u32 label_size, const void* context, u32 context_size) { return ::sslConnectionExportKeyingMaterial(c, outbuf, outbuf_size, label, label_size, context, context_size); }
Result ConnectionSetIoTimeout(SslConnection *c, u32 timeout) { return ::sslConnectionSetIoTimeout(c, timeout); }
Result ConnectionGetIoTimeout(SslConnection *c, u32 *out) { return ::sslConnectionGetIoTimeout(c, out); }

} // namespace ssl

namespace svc {

Result SetHeapSize(void** out_addr, u64 size) { return ::svcSetHeapSize(out_addr, size); }
Result SetMemoryPermission(void* addr, u64 size, u32 perm) { return ::svcSetMemoryPermission(addr, size, perm); }
Result SetMemoryAttribute(void* addr, u64 size, u32 val0, u32 val1) { return ::svcSetMemoryAttribute(addr, size, val0, val1); }
Result MapMemory(void* dst_addr, void* src_addr, u64 size) { return ::svcMapMemory(dst_addr, src_addr, size); }
Result UnmapMemory(void* dst_addr, void* src_addr, u64 size) { return ::svcUnmapMemory(dst_addr, src_addr, size); }
Result QueryMemory(MemoryInfo* meminfo_ptr, u32 *pageinfo, u64 addr) { return ::svcQueryMemory(meminfo_ptr, pageinfo, addr); }
[[noreturn]] void ExitProcess(void) { ::svcExitProcess(); }
Result CreateThread(Handle* out, void* entry, void* arg, void* stack_top, int prio, int cpuid) { return ::svcCreateThread(out, entry, arg, stack_top, prio, cpuid); }
Result StartThread(Handle handle) { return ::svcStartThread(handle); }
[[noreturn]] void ExitThread(void) { ::svcExitThread(); }
void SleepThread(s64 nano) { ::svcSleepThread(nano); }
Result GetThreadPriority(s32* priority, Handle handle) { return ::svcGetThreadPriority(priority, handle); }
Result SetThreadPriority(Handle handle, u32 priority) { return ::svcSetThreadPriority(handle, priority); }
Result GetThreadCoreMask(s32* preferred_core, u64* affinity_mask, Handle handle) { return ::svcGetThreadCoreMask(preferred_core, affinity_mask, handle); }
Result SetThreadCoreMask(Handle handle, s32 preferred_core, u32 affinity_mask) { return ::svcSetThreadCoreMask(handle, preferred_core, affinity_mask); }
u32 GetCurrentProcessorNumber(void) { return ::svcGetCurrentProcessorNumber(); }
Result SignalEvent(Handle handle) { return ::svcSignalEvent(handle); }
Result ClearEvent(Handle handle) { return ::svcClearEvent(handle); }
Result MapSharedMemory(Handle handle, void* addr, size_t size, u32 perm) { return ::svcMapSharedMemory(handle, addr, size, perm); }
Result UnmapSharedMemory(Handle handle, void* addr, size_t size) { return ::svcUnmapSharedMemory(handle, addr, size); }
Result CreateTransferMemory(Handle* out, void* addr, size_t size, u32 perm) { return ::svcCreateTransferMemory(out, addr, size, perm); }
Result CloseHandle(Handle handle) { return ::svcCloseHandle(handle); }
Result ResetSignal(Handle handle) { return ::svcResetSignal(handle); }
Result WaitSynchronization(s32* index, const Handle* handles, s32 handleCount, u64 timeout) { return ::svcWaitSynchronization(index, handles, handleCount, timeout); }
Result WaitSynchronizationSingle(Handle handle, u64 timeout) { return ::svcWaitSynchronizationSingle(handle, timeout); }
Result CancelSynchronization(Handle thread) { return ::svcCancelSynchronization(thread); }
Result ArbitrateLock(u32 wait_tag, u32* tag_location, u32 self_tag) { return ::svcArbitrateLock(wait_tag, tag_location, self_tag); }
Result ArbitrateUnlock(u32* tag_location) { return ::svcArbitrateUnlock(tag_location); }
Result WaitProcessWideKeyAtomic(u32* key, u32* tag_location, u32 self_tag, u64 timeout) { return ::svcWaitProcessWideKeyAtomic(key, tag_location, self_tag, timeout); }
void SignalProcessWideKey(u32* key, s32 num) { ::svcSignalProcessWideKey(key, num); }
u64 GetSystemTick(void) { return ::svcGetSystemTick(); }
Result ConnectToNamedPort(Handle* session, const char* name) { return ::svcConnectToNamedPort(session, name); }
Result SendSyncRequestLight(Handle session) { return ::svcSendSyncRequestLight(session); }
Result SendSyncRequest(Handle session) { return ::svcSendSyncRequest(session); }
Result SendSyncRequestWithUserBuffer(void* usrBuffer, u64 size, Handle session) { return ::svcSendSyncRequestWithUserBuffer(usrBuffer, size, session); }
Result SendAsyncRequestWithUserBuffer(Handle* handle, void* usrBuffer, u64 size, Handle session) { return ::svcSendAsyncRequestWithUserBuffer(handle, usrBuffer, size, session); }
Result GetProcessId(u64 *processID, Handle handle) { return ::svcGetProcessId(processID, handle); }
Result GetThreadId(u64 *threadID, Handle handle) { return ::svcGetThreadId(threadID, handle); }
Result Break(u32 breakReason, uintptr_t address, uintptr_t size) { return ::svcBreak(breakReason, address, size); }
Result OutputDebugString(const char *str, u64 size) { return ::svcOutputDebugString(str, size); }
[[noreturn]] void ReturnFromException(Result res) { ::svcReturnFromException(res); }
Result GetInfo(u64* out, u32 id0, Handle handle, u64 id1) { return ::svcGetInfo(out, id0, handle, id1); }
void FlushEntireDataCache(void) { ::svcFlushEntireDataCache(); }
Result FlushDataCache(void *address, size_t size) { return ::svcFlushDataCache(address, size); }
Result MapPhysicalMemory(void *address, u64 size) { return ::svcMapPhysicalMemory(address, size); }
Result UnmapPhysicalMemory(void *address, u64 size) { return ::svcUnmapPhysicalMemory(address, size); }
Result GetDebugFutureThreadInfo(LastThreadContext *out_context, u64 *out_thread_id, Handle debug, s64 ns) { return ::svcGetDebugFutureThreadInfo(out_context, out_thread_id, debug, ns); }
Result GetLastThreadInfo(LastThreadContext *out_context, u64 *out_tls_address, u32 *out_flags) { return ::svcGetLastThreadInfo(out_context, out_tls_address, out_flags); }
Result GetResourceLimitLimitValue(s64 *out, Handle reslimit_h, LimitableResource which) { return ::svcGetResourceLimitLimitValue(out, reslimit_h, which); }
Result GetResourceLimitCurrentValue(s64 *out, Handle reslimit_h, LimitableResource which) { return ::svcGetResourceLimitCurrentValue(out, reslimit_h, which); }
Result SetThreadActivity(Handle thread, ThreadActivity paused) { return ::svcSetThreadActivity(thread, paused); }
Result GetThreadContext3(ThreadContext* ctx, Handle thread) { return ::svcGetThreadContext3(ctx, thread); }
Result WaitForAddress(void *address, u32 arb_type, s64 value, s64 timeout) { return ::svcWaitForAddress(address, arb_type, value, timeout); }
Result SignalToAddress(void *address, u32 signal_type, s32 value, s32 count) { return ::svcSignalToAddress(address, signal_type, value, count); }
void SynchronizePreemptionState(void) { ::svcSynchronizePreemptionState(); }
Result GetResourceLimitPeakValue(s64 *out, Handle reslimit_h, LimitableResource which) { return ::svcGetResourceLimitPeakValue(out, reslimit_h, which); }
Result CreateIoPool(Handle *out_handle, u32 pool_type) { return ::svcCreateIoPool(out_handle, pool_type); }
Result CreateIoRegion(Handle *out_handle, Handle io_pool_h, u64 physical_address, u64 size, u32 memory_mapping, u32 perm) { return ::svcCreateIoRegion(out_handle, io_pool_h, physical_address, size, memory_mapping, perm); }
void DumpInfo(u32 dump_info_type, u64 arg0) { ::svcDumpInfo(dump_info_type, arg0); }
void KernelDebug(u32 kern_debug_type, u64 arg0, u64 arg1, u64 arg2) { ::svcKernelDebug(kern_debug_type, arg0, arg1, arg2); }
void ChangeKernelTraceState(u32 kern_trace_state) { ::svcChangeKernelTraceState(kern_trace_state); }
Result CreateSession(Handle *server_handle, Handle *client_handle, u32 unk0, u64 unk1) { return ::svcCreateSession(server_handle, client_handle, unk0, unk1); }
Result AcceptSession(Handle *session_handle, Handle port_handle) { return ::svcAcceptSession(session_handle, port_handle); }
Result ReplyAndReceiveLight(Handle handle) { return ::svcReplyAndReceiveLight(handle); }
Result ReplyAndReceive(s32* index, const Handle* handles, s32 handleCount, Handle replyTarget, u64 timeout) { return ::svcReplyAndReceive(index, handles, handleCount, replyTarget, timeout); }
Result ReplyAndReceiveWithUserBuffer(s32* index, void* usrBuffer, u64 size, const Handle* handles, s32 handleCount, Handle replyTarget, u64 timeout) { return ::svcReplyAndReceiveWithUserBuffer(index, usrBuffer, size, handles, handleCount, replyTarget, timeout); }
Result CreateEvent(Handle* server_handle, Handle* client_handle) { return ::svcCreateEvent(server_handle, client_handle); }
Result MapIoRegion(Handle io_region_h, void *address, u64 size, u32 perm) { return ::svcMapIoRegion(io_region_h, address, size, perm); }
Result UnmapIoRegion(Handle io_region_h, void *address, u64 size) { return ::svcUnmapIoRegion(io_region_h, address, size); }
Result MapPhysicalMemoryUnsafe(void *address, u64 size) { return ::svcMapPhysicalMemoryUnsafe(address, size); }
Result UnmapPhysicalMemoryUnsafe(void *address, u64 size) { return ::svcUnmapPhysicalMemoryUnsafe(address, size); }
Result SetUnsafeLimit(u64 size) { return ::svcSetUnsafeLimit(size); }
Result CreateCodeMemory(Handle* code_handle, void* src_addr, u64 size) { return ::svcCreateCodeMemory(code_handle, src_addr, size); }
Result ControlCodeMemory(Handle code_handle, CodeMapOperation op, void* dst_addr, u64 size, u64 perm) { return ::svcControlCodeMemory(code_handle, op, dst_addr, size, perm); }
void SleepSystem(void) { ::svcSleepSystem(); }
Result ReadWriteRegister(u32* outVal, u64 regAddr, u32 rwMask, u32 inVal) { return ::svcReadWriteRegister(outVal, regAddr, rwMask, inVal); }
Result SetProcessActivity(Handle process, ProcessActivity paused) { return ::svcSetProcessActivity(process, paused); }
Result CreateSharedMemory(Handle* out, size_t size, u32 local_perm, u32 other_perm) { return ::svcCreateSharedMemory(out, size, local_perm, other_perm); }
Result MapTransferMemory(Handle tmem_handle, void* addr, size_t size, u32 perm) { return ::svcMapTransferMemory(tmem_handle, addr, size, perm); }
Result UnmapTransferMemory(Handle tmem_handle, void* addr, size_t size) { return ::svcUnmapTransferMemory(tmem_handle, addr, size); }
Result CreateInterruptEvent(Handle* handle, u64 irqNum, u32 flag) { return ::svcCreateInterruptEvent(handle, irqNum, flag); }
Result QueryPhysicalAddress(PhysicalMemoryInfo *out, u64 virtaddr) { return ::svcQueryPhysicalAddress(out, virtaddr); }
Result QueryMemoryMapping(u64* virtaddr, u64* out_size, u64 physaddr, u64 size) { return ::svcQueryMemoryMapping(virtaddr, out_size, physaddr, size); }
Result LegacyQueryIoMapping(u64* virtaddr, u64 physaddr, u64 size) { return ::svcLegacyQueryIoMapping(virtaddr, physaddr, size); }
Result CreateDeviceAddressSpace(Handle *handle, u64 dev_addr, u64 dev_size) { return ::svcCreateDeviceAddressSpace(handle, dev_addr, dev_size); }
Result AttachDeviceAddressSpace(u64 device, Handle handle) { return ::svcAttachDeviceAddressSpace(device, handle); }
Result DetachDeviceAddressSpace(u64 device, Handle handle) { return ::svcDetachDeviceAddressSpace(device, handle); }
Result MapDeviceAddressSpaceByForce(Handle handle, Handle proc_handle, u64 map_addr, u64 dev_size, u64 dev_addr, u32 option) { return ::svcMapDeviceAddressSpaceByForce(handle, proc_handle, map_addr, dev_size, dev_addr, option); }
Result MapDeviceAddressSpaceAligned(Handle handle, Handle proc_handle, u64 map_addr, u64 dev_size, u64 dev_addr, u32 option) { return ::svcMapDeviceAddressSpaceAligned(handle, proc_handle, map_addr, dev_size, dev_addr, option); }
Result MapDeviceAddressSpace(u64 *out_mapped_size, Handle handle, Handle proc_handle, u64 map_addr, u64 dev_size, u64 dev_addr, u32 perm) { return ::svcMapDeviceAddressSpace(out_mapped_size, handle, proc_handle, map_addr, dev_size, dev_addr, perm); }
Result UnmapDeviceAddressSpace(Handle handle, Handle proc_handle, u64 map_addr, u64 map_size, u64 dev_addr) { return ::svcUnmapDeviceAddressSpace(handle, proc_handle, map_addr, map_size, dev_addr); }
Result InvalidateProcessDataCache(Handle process, uintptr_t address, size_t size) { return ::svcInvalidateProcessDataCache(process, address, size); }
Result StoreProcessDataCache(Handle process, uintptr_t address, size_t size) { return ::svcStoreProcessDataCache(process, address, size); }
Result FlushProcessDataCache(Handle process, uintptr_t address, size_t size) { return ::svcFlushProcessDataCache(process, address, size); }
Result DebugActiveProcess(Handle* debug, u64 processID) { return ::svcDebugActiveProcess(debug, processID); }
Result BreakDebugProcess(Handle debug) { return ::svcBreakDebugProcess(debug); }
Result TerminateDebugProcess(Handle debug) { return ::svcTerminateDebugProcess(debug); }
Result GetDebugEvent(DebugEventInfo* event_out, Handle debug) { return ::svcGetDebugEvent(event_out, debug); }
Result ContinueDebugEvent(Handle debug, u32 flags, u64* tid_list, u32 num_tids) { return ::svcContinueDebugEvent(debug, flags, tid_list, num_tids); }
Result LegacyContinueDebugEvent(Handle debug, u32 flags, u64 threadID) { return ::svcLegacyContinueDebugEvent(debug, flags, threadID); }
Result GetDebugThreadContext(ThreadContext* ctx, Handle debug, u64 threadID, u32 flags) { return ::svcGetDebugThreadContext(ctx, debug, threadID, flags); }
Result SetDebugThreadContext(Handle debug, u64 threadID, const ThreadContext* ctx, u32 flags) { return ::svcSetDebugThreadContext(debug, threadID, ctx, flags); }
Result GetProcessList(s32 *num_out, u64 *pids_out, u32 max_pids) { return ::svcGetProcessList(num_out, pids_out, max_pids); }
Result GetThreadList(s32 *num_out, u64 *tids_out, u32 max_tids, Handle debug) { return ::svcGetThreadList(num_out, tids_out, max_tids, debug); }
Result QueryDebugProcessMemory(MemoryInfo* meminfo_ptr, u32* pageinfo, Handle debug, u64 addr) { return ::svcQueryDebugProcessMemory(meminfo_ptr, pageinfo, debug, addr); }
Result ReadDebugProcessMemory(void* buffer, Handle debug, u64 addr, u64 size) { return ::svcReadDebugProcessMemory(buffer, debug, addr, size); }
Result WriteDebugProcessMemory(Handle debug, const void* buffer, u64 addr, u64 size) { return ::svcWriteDebugProcessMemory(debug, buffer, addr, size); }
Result SetHardwareBreakPoint(u32 which, u64 flags, u64 value) { return ::svcSetHardwareBreakPoint(which, flags, value); }
Result GetDebugThreadParam(u64* out_64, u32* out_32, Handle debug, u64 threadID, DebugThreadParam param) { return ::svcGetDebugThreadParam(out_64, out_32, debug, threadID, param); }
Result GetSystemInfo(u64* out, u64 id0, Handle handle, u64 id1) { return ::svcGetSystemInfo(out, id0, handle, id1); }
Result CreatePort(Handle* portServer, Handle *portClient, s32 max_sessions, bool is_light, const char* name) { return ::svcCreatePort(portServer, portClient, max_sessions, is_light, name); }
Result ManageNamedPort(Handle* portServer, const char* name, s32 maxSessions) { return ::svcManageNamedPort(portServer, name, maxSessions); }
Result ConnectToPort(Handle* session, Handle port) { return ::svcConnectToPort(session, port); }
Result SetProcessMemoryPermission(Handle proc, u64 addr, u64 size, u32 perm) { return ::svcSetProcessMemoryPermission(proc, addr, size, perm); }
Result MapProcessMemory(void* dst, Handle proc, u64 src, u64 size) { return ::svcMapProcessMemory(dst, proc, src, size); }
Result UnmapProcessMemory(void* dst, Handle proc, u64 src, u64 size) { return ::svcUnmapProcessMemory(dst, proc, src, size); }
Result QueryProcessMemory(MemoryInfo* meminfo_ptr, u32 *pageinfo, Handle proc, u64 addr) { return ::svcQueryProcessMemory(meminfo_ptr, pageinfo, proc, addr); }
Result MapProcessCodeMemory(Handle proc, u64 dst, u64 src, u64 size) { return ::svcMapProcessCodeMemory(proc, dst, src, size); }
Result UnmapProcessCodeMemory(Handle proc, u64 dst, u64 src, u64 size) { return ::svcUnmapProcessCodeMemory(proc, dst, src, size); }
Result CreateProcess(Handle* out, const void* proc_info, const u32* caps, u64 cap_num) { return ::svcCreateProcess(out, proc_info, caps, cap_num); }
Result StartProcess(Handle proc, s32 main_prio, s32 default_cpu, u32 stack_size) { return ::svcStartProcess(proc, main_prio, default_cpu, stack_size); }
Result TerminateProcess(Handle proc) { return ::svcTerminateProcess(proc); }
Result GetProcessInfo(s64 *out, Handle proc, ProcessInfoType which) { return ::svcGetProcessInfo(out, proc, which); }
Result CreateResourceLimit(Handle* out) { return ::svcCreateResourceLimit(out); }
Result SetResourceLimitLimitValue(Handle reslimit, LimitableResource which, u64 value) { return ::svcSetResourceLimitLimitValue(reslimit, which, value); }
void CallSecureMonitor(SecmonArgs* regs) { ::svcCallSecureMonitor(regs); }
Result MapInsecurePhysicalMemory(void *address, u64 size) { return ::svcMapInsecurePhysicalMemory(address, size); }
Result UnmapInsecurePhysicalMemory(void *address, u64 size) { return ::svcUnmapInsecurePhysicalMemory(address, size); }

} // namespace svc

namespace swkbd {

Result Create(SwkbdConfig* c, s32 max_dictwords) { return ::swkbdCreate(c, max_dictwords); }
void Close(SwkbdConfig* c) { ::swkbdClose(c); }
void ConfigMakePresetDefault(SwkbdConfig* c) { ::swkbdConfigMakePresetDefault(c); }
void ConfigMakePresetPassword(SwkbdConfig* c) { ::swkbdConfigMakePresetPassword(c); }
void ConfigMakePresetUserName(SwkbdConfig* c) { ::swkbdConfigMakePresetUserName(c); }
void ConfigMakePresetDownloadCode(SwkbdConfig* c) { ::swkbdConfigMakePresetDownloadCode(c); }
void ConfigSetOkButtonText(SwkbdConfig* c, const char* str) { ::swkbdConfigSetOkButtonText(c, str); }
void ConfigSetLeftOptionalSymbolKey(SwkbdConfig* c, const char* str) { ::swkbdConfigSetLeftOptionalSymbolKey(c, str); }
void ConfigSetRightOptionalSymbolKey(SwkbdConfig* c, const char* str) { ::swkbdConfigSetRightOptionalSymbolKey(c, str); }
void ConfigSetHeaderText(SwkbdConfig* c, const char* str) { ::swkbdConfigSetHeaderText(c, str); }
void ConfigSetSubText(SwkbdConfig* c, const char* str) { ::swkbdConfigSetSubText(c, str); }
void ConfigSetGuideText(SwkbdConfig* c, const char* str) { ::swkbdConfigSetGuideText(c, str); }
void ConfigSetInitialText(SwkbdConfig* c, const char* str) { ::swkbdConfigSetInitialText(c, str); }
void ConfigSetDictionary(SwkbdConfig* c, const SwkbdDictWord *input, s32 entries) { ::swkbdConfigSetDictionary(c, input, entries); }
Result ConfigSetCustomizedDictionaries(SwkbdConfig* c, const SwkbdCustomizedDictionarySet *dic) { return ::swkbdConfigSetCustomizedDictionaries(c, dic); }
void ConfigSetTextCheckCallback(SwkbdConfig* c, SwkbdTextCheckCb cb) { ::swkbdConfigSetTextCheckCallback(c, cb); }
void ConfigSetType(SwkbdConfig* c, SwkbdType type) { ::swkbdConfigSetType(c, type); }
void ConfigSetDicFlag(SwkbdConfig* c, u8 flag) { ::swkbdConfigSetDicFlag(c, flag); }
void ConfigSetKeySetDisableBitmask(SwkbdConfig* c, u32 keySetDisableBitmask) { ::swkbdConfigSetKeySetDisableBitmask(c, keySetDisableBitmask); }
void ConfigSetInitialCursorPos(SwkbdConfig* c, u32 initialCursorPos) { ::swkbdConfigSetInitialCursorPos(c, initialCursorPos); }
void ConfigSetStringLenMax(SwkbdConfig* c, u32 stringLenMax) { ::swkbdConfigSetStringLenMax(c, stringLenMax); }
void ConfigSetStringLenMin(SwkbdConfig* c, u32 stringLenMin) { ::swkbdConfigSetStringLenMin(c, stringLenMin); }
void ConfigSetPasswordFlag(SwkbdConfig* c, u32 flag) { ::swkbdConfigSetPasswordFlag(c, flag); }
void ConfigSetTextDrawType(SwkbdConfig* c, SwkbdTextDrawType textDrawType) { ::swkbdConfigSetTextDrawType(c, textDrawType); }
void ConfigSetReturnButtonFlag(SwkbdConfig* c, u16 flag) { ::swkbdConfigSetReturnButtonFlag(c, flag); }
void ConfigSetBlurBackground(SwkbdConfig* c, u8 blurBackground) { ::swkbdConfigSetBlurBackground(c, blurBackground); }
void ConfigSetTextGrouping(SwkbdConfig* c, u32 index, u32 value) { ::swkbdConfigSetTextGrouping(c, index, value); }
void ConfigSetUnkFlag(SwkbdConfig* c, u8 flag) { ::swkbdConfigSetUnkFlag(c, flag); }
void ConfigSetTrigger(SwkbdConfig* c, u8 trigger) { ::swkbdConfigSetTrigger(c, trigger); }
Result Show(SwkbdConfig* c, char* out_string, size_t out_string_size) { return ::swkbdShow(c, out_string, out_string_size); }
Result InlineCreate(SwkbdInline* s) { return ::swkbdInlineCreate(s); }
Result InlineClose(SwkbdInline* s) { return ::swkbdInlineClose(s); }
Result InlineLaunch(SwkbdInline* s) { return ::swkbdInlineLaunch(s); }
Result InlineLaunchForLibraryApplet(SwkbdInline* s, u8 mode, u8 unk_x5) { return ::swkbdInlineLaunchForLibraryApplet(s, mode, unk_x5); }
void InlineGetWindowSize(s32 *width, s32 *height) { ::swkbdInlineGetWindowSize(width, height); }
Result InlineGetImageMemoryRequirement(u64 *out_size, u64 *out_alignment) { return ::swkbdInlineGetImageMemoryRequirement(out_size, out_alignment); }
Result InlineGetImage(SwkbdInline* s, void* buffer, u64 size, bool *data_available) { return ::swkbdInlineGetImage(s, buffer, size, data_available); }
s32 InlineGetMaxHeight(SwkbdInline* s) { return ::swkbdInlineGetMaxHeight(s); }
s32 InlineGetMiniaturizedHeight(SwkbdInline* s) { return ::swkbdInlineGetMiniaturizedHeight(s); }
s32 InlineGetTouchRectangles(SwkbdInline* s, SwkbdRect *keytop, SwkbdRect *footer) { return ::swkbdInlineGetTouchRectangles(s, keytop, footer); }
bool InlineIsUsedTouchPointByKeyboard(SwkbdInline* s, s32 x, s32 y) { return ::swkbdInlineIsUsedTouchPointByKeyboard(s, x, y); }
Result InlineUpdate(SwkbdInline* s, SwkbdState* out_state) { return ::swkbdInlineUpdate(s, out_state); }
void InlineSetFinishedInitializeCallback(SwkbdInline* s, VoidFn cb) { ::swkbdInlineSetFinishedInitializeCallback(s, cb); }
void InlineSetDecidedCancelCallback(SwkbdInline* s, VoidFn cb) { ::swkbdInlineSetDecidedCancelCallback(s, cb); }
void InlineSetChangedStringCallback(SwkbdInline* s, SwkbdChangedStringCb cb) { ::swkbdInlineSetChangedStringCallback(s, cb); }
void InlineSetChangedStringV2Callback(SwkbdInline* s, SwkbdChangedStringV2Cb cb) { ::swkbdInlineSetChangedStringV2Callback(s, cb); }
void InlineSetMovedCursorCallback(SwkbdInline* s, SwkbdMovedCursorCb cb) { ::swkbdInlineSetMovedCursorCallback(s, cb); }
void InlineSetMovedCursorV2Callback(SwkbdInline* s, SwkbdMovedCursorV2Cb cb) { ::swkbdInlineSetMovedCursorV2Callback(s, cb); }
void InlineSetMovedTabCallback(SwkbdInline* s, SwkbdMovedTabCb cb) { ::swkbdInlineSetMovedTabCallback(s, cb); }
void InlineSetDecidedEnterCallback(SwkbdInline* s, SwkbdDecidedEnterCb cb) { ::swkbdInlineSetDecidedEnterCallback(s, cb); }
void InlineSetReleasedUserWordInfoCallback(SwkbdInline* s, VoidFn cb) { ::swkbdInlineSetReleasedUserWordInfoCallback(s, cb); }
void InlineAppear(SwkbdInline* s, const SwkbdAppearArg* arg) { ::swkbdInlineAppear(s, arg); }
void InlineAppearEx(SwkbdInline* s, const SwkbdAppearArg* arg, u8 trigger) { ::swkbdInlineAppearEx(s, arg, trigger); }
void InlineDisappear(SwkbdInline* s) { ::swkbdInlineDisappear(s); }
void InlineMakeAppearArg(SwkbdAppearArg* arg, SwkbdType type) { ::swkbdInlineMakeAppearArg(arg, type); }
void InlineAppearArgSetOkButtonText(SwkbdAppearArg* arg, const char* str) { ::swkbdInlineAppearArgSetOkButtonText(arg, str); }
void InlineAppearArgSetLeftButtonText(SwkbdAppearArg* arg, const char* str) { ::swkbdInlineAppearArgSetLeftButtonText(arg, str); }
void InlineAppearArgSetRightButtonText(SwkbdAppearArg* arg, const char* str) { ::swkbdInlineAppearArgSetRightButtonText(arg, str); }
void InlineAppearArgSetStringLenMax(SwkbdAppearArg* arg, s32 stringLenMax) { ::swkbdInlineAppearArgSetStringLenMax(arg, stringLenMax); }
void InlineAppearArgSetStringLenMin(SwkbdAppearArg* arg, s32 stringLenMin) { ::swkbdInlineAppearArgSetStringLenMin(arg, stringLenMin); }
void InlineSetVolume(SwkbdInline* s, float volume) { ::swkbdInlineSetVolume(s, volume); }
void InlineSetInputText(SwkbdInline* s, const char* str) { ::swkbdInlineSetInputText(s, str); }
void InlineSetCursorPos(SwkbdInline* s, s32 pos) { ::swkbdInlineSetCursorPos(s, pos); }
Result InlineSetUserWordInfo(SwkbdInline* s, const SwkbdDictWord *input, s32 entries) { return ::swkbdInlineSetUserWordInfo(s, input, entries); }
Result InlineUnsetUserWordInfo(SwkbdInline* s) { return ::swkbdInlineUnsetUserWordInfo(s); }
void InlineSetUtf8Mode(SwkbdInline* s, bool flag) { ::swkbdInlineSetUtf8Mode(s, flag); }
Result InlineSetCustomizeDic(SwkbdInline* s, void* buffer, size_t size, SwkbdCustomizeDicInfo *info) { return ::swkbdInlineSetCustomizeDic(s, buffer, size, info); }
void InlineUnsetCustomizeDic(SwkbdInline* s) { ::swkbdInlineUnsetCustomizeDic(s); }
Result InlineSetCustomizedDictionaries(SwkbdInline* s, const SwkbdCustomizedDictionarySet *dic) { return ::swkbdInlineSetCustomizedDictionaries(s, dic); }
Result InlineUnsetCustomizedDictionaries(SwkbdInline* s) { return ::swkbdInlineUnsetCustomizedDictionaries(s); }
void InlineSetInputModeFadeType(SwkbdInline* s, u8 type) { ::swkbdInlineSetInputModeFadeType(s, type); }
void InlineSetAlphaEnabledInInputMode(SwkbdInline* s, bool flag) { ::swkbdInlineSetAlphaEnabledInInputMode(s, flag); }
void InlineSetKeytopBgAlpha(SwkbdInline* s, float alpha) { ::swkbdInlineSetKeytopBgAlpha(s, alpha); }
void InlineSetFooterBgAlpha(SwkbdInline* s, float alpha) { ::swkbdInlineSetFooterBgAlpha(s, alpha); }
void InlineSetKeytopScale(SwkbdInline* s, float scale) { ::swkbdInlineSetKeytopScale(s, scale); }
void InlineSetKeytopTranslate(SwkbdInline* s, float x, float y) { ::swkbdInlineSetKeytopTranslate(s, x, y); }
void InlineSetKeytopAsFloating(SwkbdInline* s, bool flag) { ::swkbdInlineSetKeytopAsFloating(s, flag); }
void InlineSetFooterScalable(SwkbdInline* s, bool flag) { ::swkbdInlineSetFooterScalable(s, flag); }
void InlineSetTouchFlag(SwkbdInline* s, bool flag) { ::swkbdInlineSetTouchFlag(s, flag); }
void InlineSetHardwareKeyboardFlag(SwkbdInline* s, bool flag) { ::swkbdInlineSetHardwareKeyboardFlag(s, flag); }
void InlineSetDirectionalButtonAssignFlag(SwkbdInline* s, bool flag) { ::swkbdInlineSetDirectionalButtonAssignFlag(s, flag); }
void InlineSetSeGroup(SwkbdInline* s, u8 seGroup, bool flag) { ::swkbdInlineSetSeGroup(s, seGroup, flag); }
void InlineSetBackspaceFlag(SwkbdInline* s, bool flag) { ::swkbdInlineSetBackspaceFlag(s, flag); }

} // namespace swkbd

namespace tc {

Result Initialize(void) { return ::tcInitialize(); }
void Exit(void) { ::tcExit(); }
Service* GetServiceSession(void) { return ::tcGetServiceSession(); }
Result EnableFanControl(void) { return ::tcEnableFanControl(); }
Result DisableFanControl(void) { return ::tcDisableFanControl(); }
Result IsFanControlEnabled(bool *status) { return ::tcIsFanControlEnabled(status); }
Result GetSkinTemperatureMilliC(s32 *skinTemp) { return ::tcGetSkinTemperatureMilliC(skinTemp); }

} // namespace tc

namespace thread {

bool ContextIsAArch64(const ThreadContext *ctx) { return ::threadContextIsAArch64(ctx); }
bool ExceptionIsAArch64(const ThreadExceptionDump *ctx) { return ::threadExceptionIsAArch64(ctx); }
Result Create(Thread* t, ThreadFunc entry, void* arg, void *stack_mem, size_t stack_sz, int prio, int cpuid) { return ::threadCreate(t, entry, arg, stack_mem, stack_sz, prio, cpuid); }
Result Start(Thread* t) { return ::threadStart(t); }
[[noreturn]] void Exit(void) { ::threadExit(); }
Result WaitForExit(Thread* t) { return ::threadWaitForExit(t); }
Result Close(Thread* t) { return ::threadClose(t); }
Result Pause(Thread* t) { return ::threadPause(t); }
Result Resume(Thread* t) { return ::threadResume(t); }
Result DumpContext(ThreadContext* ctx, Thread* t) { return ::threadDumpContext(ctx, t); }
Thread * GetSelf(void) { return ::threadGetSelf(); }
Handle GetCurHandle(void) { return ::threadGetCurHandle(); }
s32 TlsAlloc(void (* destructor)(void*)) { return ::threadTlsAlloc(destructor); }
void* TlsGet(s32 slot_id) { return ::threadTlsGet(slot_id); }
void TlsSet(s32 slot_id, void* value) { ::threadTlsSet(slot_id, value); }
void TlsFree(s32 slot_id) { ::threadTlsFree(slot_id); }

} // namespace thread

namespace time {

Result Initialize(void) { return ::timeInitialize(); }
void Exit(void) { ::timeExit(); }
Service* GetServiceSession(void) { return ::timeGetServiceSession(); }
Service* GetServiceSession_SystemClock(TimeType type) { return ::timeGetServiceSession_SystemClock(type); }
Service* GetServiceSession_SteadyClock(void) { return ::timeGetServiceSession_SteadyClock(); }
Service* GetServiceSession_TimeZoneService(void) { return ::timeGetServiceSession_TimeZoneService(); }
void* GetSharedmemAddr(void) { return ::timeGetSharedmemAddr(); }
Result GetStandardSteadyClockTimePoint(TimeSteadyClockTimePoint *out) { return ::timeGetStandardSteadyClockTimePoint(out); }
Result GetStandardSteadyClockInternalOffset(s64 *out) { return ::timeGetStandardSteadyClockInternalOffset(out); }
Result GetCurrentTime(TimeType type, u64 *timestamp) { return ::timeGetCurrentTime(type, timestamp); }
Result SetCurrentTime(TimeType type, u64 timestamp) { return ::timeSetCurrentTime(type, timestamp); }
Result GetDeviceLocationName(TimeLocationName *name) { return ::timeGetDeviceLocationName(name); }
Result SetDeviceLocationName(const TimeLocationName *name) { return ::timeSetDeviceLocationName(name); }
Result GetTotalLocationNameCount(s32 *total_location_name_count) { return ::timeGetTotalLocationNameCount(total_location_name_count); }
Result LoadLocationNameList(s32 index, TimeLocationName *location_name_array, s32 location_name_max, s32 *location_name_count) { return ::timeLoadLocationNameList(index, location_name_array, location_name_max, location_name_count); }
Result LoadTimeZoneRule(const TimeLocationName *name, TimeZoneRule *rule) { return ::timeLoadTimeZoneRule(name, rule); }
Result ToCalendarTime(const TimeZoneRule *rule, u64 timestamp, TimeCalendarTime *caltime, TimeCalendarAdditionalInfo *info) { return ::timeToCalendarTime(rule, timestamp, caltime, info); }
Result ToCalendarTimeWithMyRule(u64 timestamp, TimeCalendarTime *caltime, TimeCalendarAdditionalInfo *info) { return ::timeToCalendarTimeWithMyRule(timestamp, caltime, info); }
Result ToPosixTime(const TimeZoneRule *rule, const TimeCalendarTime *caltime, u64 *timestamp_list, s32 timestamp_list_count, s32 *timestamp_count) { return ::timeToPosixTime(rule, caltime, timestamp_list, timestamp_list_count, timestamp_count); }
Result ToPosixTimeWithMyRule(const TimeCalendarTime *caltime, u64 *timestamp_list, s32 timestamp_list_count, s32 *timestamp_count) { return ::timeToPosixTimeWithMyRule(caltime, timestamp_list, timestamp_list_count, timestamp_count); }

} // namespace time

namespace tipc {

void Create(TipcService* s, Handle h) { ::tipcCreate(s, h); }
void Close(TipcService* s) { ::tipcClose(s); }
void RequestInBuffer(HipcRequest* req, const void* buffer, size_t size, HipcBufferMode mode) { ::tipcRequestInBuffer(req, buffer, size, mode); }
void RequestOutBuffer(HipcRequest* req, void* buffer, size_t size, HipcBufferMode mode) { ::tipcRequestOutBuffer(req, buffer, size, mode); }
void RequestInOutBuffer(HipcRequest* req, void* buffer, size_t size, HipcBufferMode mode) { ::tipcRequestInOutBuffer(req, buffer, size, mode); }
void RequestHandle(HipcRequest* req, Handle handle) { ::tipcRequestHandle(req, handle); }
void RequestFormatProcessBuffer(TipcRequestFormat* fmt, u32 attr) { ::_tipcRequestFormatProcessBuffer(fmt, attr); }
void RequestProcessBuffer(HipcRequest* req, const SfBuffer* buf, u32 attr) { ::_tipcRequestProcessBuffer(req, buf, attr); }
void* MakeRequest(u32 request_id, u32 data_size, bool send_pid, const SfBufferAttrs buffer_attrs, const SfBuffer* buffers, u32 num_handles, const Handle* handles) { return ::tipcMakeRequest(request_id, data_size, send_pid, buffer_attrs, buffers, num_handles, handles); }
Handle ResponseGetCopyHandle(HipcResponse* res) { return ::tipcResponseGetCopyHandle(res); }
Handle ResponseGetMoveHandle(HipcResponse* res) { return ::tipcResponseGetMoveHandle(res); }
void ResponseGetHandle(HipcResponse* res, SfOutHandleAttr type, Handle* out) { ::_tipcResponseGetHandle(res, type, out); }
Result ParseResponse(u32 out_size, void** out_data, u32 num_out_objects, TipcService* out_objects, const SfOutHandleAttrs out_handle_attrs, Handle* out_handles) { return ::tipcParseResponse(out_size, out_data, num_out_objects, out_objects, out_handle_attrs, out_handles); }
Result DispatchImpl(TipcService* s, u32 request_id, const void* in_data, u32 in_data_size, void* out_data, u32 out_data_size, TipcDispatchParams disp) { return ::tipcDispatchImpl(s, request_id, in_data, in_data_size, out_data, out_data_size, disp); }

} // namespace tipc

namespace tmem {

Result Create(TransferMemory* t, size_t size, Permission perm) { return ::tmemCreate(t, size, perm); }
Result CreateFromMemory(TransferMemory* t, void* buf, size_t size, Permission perm) { return ::tmemCreateFromMemory(t, buf, size, perm); }
void LoadRemote(TransferMemory* t, Handle handle, size_t size, Permission perm) { ::tmemLoadRemote(t, handle, size, perm); }
Result Map(TransferMemory* t) { return ::tmemMap(t); }
Result Unmap(TransferMemory* t) { return ::tmemUnmap(t); }
void* GetAddr(TransferMemory* t) { return ::tmemGetAddr(t); }
Result CloseHandle(TransferMemory* t) { return ::tmemCloseHandle(t); }
Result WaitForPermission(TransferMemory* t, Permission perm) { return ::tmemWaitForPermission(t, perm); }
Result Close(TransferMemory* t) { return ::tmemClose(t); }

} // namespace tmem

namespace ts {

Result Initialize(void) { return ::tsInitialize(); }
void Exit(void) { ::tsExit(); }
Service* GetServiceSession(void) { return ::tsGetServiceSession(); }
Result GetTemperatureRange(TsLocation location, s32 *min_temperature, s32 *max_temperature) { return ::tsGetTemperatureRange(location, min_temperature, max_temperature); }
Result GetTemperature(TsLocation location, s32 *temperature) { return ::tsGetTemperature(location, temperature); }
Result GetTemperatureMilliC(TsLocation location, s32 *temperature) { return ::tsGetTemperatureMilliC(location, temperature); }
Result OpenSession(TsSession *s, u32 device_code) { return ::tsOpenSession(s, device_code); }
Result SessionGetTemperature(TsSession *s, float *temperature) { return ::tsSessionGetTemperature(s, temperature); }
void SessionClose(TsSession *s) { ::tsSessionClose(s); }

} // namespace ts

namespace uart {

Result Initialize(void) { return ::uartInitialize(); }
void Exit(void) { ::uartExit(); }
Service* GetServiceSession(void) { return ::uartGetServiceSession(); }
Result HasPort(UartPort port, bool *out) { return ::uartHasPort(port, out); }
Result HasPortForDev(UartPortForDev port, bool *out) { return ::uartHasPortForDev(port, out); }
Result IsSupportedBaudRate(UartPort port, u32 baud_rate, bool *out) { return ::uartIsSupportedBaudRate(port, baud_rate, out); }
Result IsSupportedBaudRateForDev(UartPortForDev port, u32 baud_rate, bool *out) { return ::uartIsSupportedBaudRateForDev(port, baud_rate, out); }
Result IsSupportedFlowControlMode(UartPort port, UartFlowControlMode flow_control_mode, bool *out) { return ::uartIsSupportedFlowControlMode(port, flow_control_mode, out); }
Result IsSupportedFlowControlModeForDev(UartPortForDev port, UartFlowControlMode flow_control_mode, bool *out) { return ::uartIsSupportedFlowControlModeForDev(port, flow_control_mode, out); }
Result CreatePortSession(UartPortSession *s) { return ::uartCreatePortSession(s); }
Result IsSupportedPortEvent(UartPort port, UartPortEventType port_event_type, bool *out) { return ::uartIsSupportedPortEvent(port, port_event_type, out); }
Result IsSupportedPortEventForDev(UartPortForDev port, UartPortEventType port_event_type, bool *out) { return ::uartIsSupportedPortEventForDev(port, port_event_type, out); }
Result IsSupportedDeviceVariation(UartPort port, u32 device_variation, bool *out) { return ::uartIsSupportedDeviceVariation(port, device_variation, out); }
Result IsSupportedDeviceVariationForDev(UartPortForDev port, u32 device_variation, bool *out) { return ::uartIsSupportedDeviceVariationForDev(port, device_variation, out); }
void PortSessionClose(UartPortSession* s) { ::uartPortSessionClose(s); }
Result PortSessionOpenPort(UartPortSession* s, bool *out, UartPort port, u32 baud_rate, UartFlowControlMode flow_control_mode, u32 device_variation, bool is_invert_tx, bool is_invert_rx, bool is_invert_rts, bool is_invert_cts, void* send_buffer, u64 send_buffer_length, void* receive_buffer, u64 receive_buffer_length) { return ::uartPortSessionOpenPort(s, out, port, baud_rate, flow_control_mode, device_variation, is_invert_tx, is_invert_rx, is_invert_rts, is_invert_cts, send_buffer, send_buffer_length, receive_buffer, receive_buffer_length); }
Result PortSessionOpenPortForDev(UartPortSession* s, bool *out, UartPortForDev port, u32 baud_rate, UartFlowControlMode flow_control_mode, u32 device_variation, bool is_invert_tx, bool is_invert_rx, bool is_invert_rts, bool is_invert_cts, void* send_buffer, u64 send_buffer_length, void* receive_buffer, u64 receive_buffer_length) { return ::uartPortSessionOpenPortForDev(s, out, port, baud_rate, flow_control_mode, device_variation, is_invert_tx, is_invert_rx, is_invert_rts, is_invert_cts, send_buffer, send_buffer_length, receive_buffer, receive_buffer_length); }
Result PortSessionGetWritableLength(UartPortSession* s, u64 *out) { return ::uartPortSessionGetWritableLength(s, out); }
Result PortSessionSend(UartPortSession* s, const void* in_data, size_t size, u64 *out) { return ::uartPortSessionSend(s, in_data, size, out); }
Result PortSessionGetReadableLength(UartPortSession* s, u64 *out) { return ::uartPortSessionGetReadableLength(s, out); }
Result PortSessionReceive(UartPortSession* s, void* out_data, size_t size, u64 *out) { return ::uartPortSessionReceive(s, out_data, size, out); }
Result PortSessionBindPortEvent(UartPortSession* s, UartPortEventType port_event_type, s64 threshold, bool *out, Event *out_event) { return ::uartPortSessionBindPortEvent(s, port_event_type, threshold, out, out_event); }
Result PortSessionUnbindPortEvent(UartPortSession* s, UartPortEventType port_event_type, bool *out) { return ::uartPortSessionUnbindPortEvent(s, port_event_type, out); }

} // namespace uart

namespace uevent {

void Create(UEvent* e, bool auto_clear) { ::ueventCreate(e, auto_clear); }
void Clear(UEvent* e) { ::ueventClear(e); }
void Signal(UEvent* e) { ::ueventSignal(e); }

} // namespace uevent

namespace usb {

Result CommsInitialize(void) { return ::usbCommsInitialize(); }
Result CommsInitializeEx(u32 num_interfaces, const UsbCommsInterfaceInfo *infos, u16 idVendor, u16 idProduct) { return ::usbCommsInitializeEx(num_interfaces, infos, idVendor, idProduct); }
void CommsExit(void) { ::usbCommsExit(); }
void CommsSetErrorHandling(bool flag) { ::usbCommsSetErrorHandling(flag); }
size_t CommsRead(void* buffer, size_t size) { return ::usbCommsRead(buffer, size); }
size_t CommsWrite(const void* buffer, size_t size) { return ::usbCommsWrite(buffer, size); }
size_t CommsReadEx(void* buffer, size_t size, u32 interface) { return ::usbCommsReadEx(buffer, size, interface); }
size_t CommsWriteEx(const void* buffer, size_t size, u32 interface) { return ::usbCommsWriteEx(buffer, size, interface); }
Event * CommsGetReadCompletionEvent(u32 interface) { return ::usbCommsGetReadCompletionEvent(interface); }
Result CommsReadAsync(void *buffer, size_t size, u32 *urbId, u32 interface) { return ::usbCommsReadAsync(buffer, size, urbId, interface); }
Result CommsGetReadResult(u32 urbId, u32 *transferredSize, u32 interface) { return ::usbCommsGetReadResult(urbId, transferredSize, interface); }
Event * CommsGetWriteCompletionEvent(u32 interface) { return ::usbCommsGetWriteCompletionEvent(interface); }
Result CommsWriteAsync(void *buffer, size_t size, u32 *urbId, u32 interface) { return ::usbCommsWriteAsync(buffer, size, urbId, interface); }
Result CommsGetWriteResult(u32 urbId, u32 *transferredSize, u32 interface) { return ::usbCommsGetWriteResult(urbId, transferredSize, interface); }
Result DsInitialize(void) { return ::usbDsInitialize(); }
void DsExit(void) { ::usbDsExit(); }
Service* DsGetServiceSession(void) { return ::usbDsGetServiceSession(); }
Result DsWaitReady(u64 timeout) { return ::usbDsWaitReady(timeout); }
Result DsParseReportData(UsbDsReportData *reportdata, u32 urbId, u32 *requestedSize, u32 *transferredSize) { return ::usbDsParseReportData(reportdata, urbId, requestedSize, transferredSize); }
Event* DsGetStateChangeEvent(void) { return ::usbDsGetStateChangeEvent(); }
Result DsGetState(UsbState* out) { return ::usbDsGetState(out); }
Result DsGetDsInterface(UsbDsInterface** out, struct usb_interface_descriptor* descriptor, const char* interface_name) { return ::usbDsGetDsInterface(out, descriptor, interface_name); }
Result DsSetVidPidBcd(const UsbDsDeviceInfo* deviceinfo) { return ::usbDsSetVidPidBcd(deviceinfo); }
Result DsRegisterInterface(UsbDsInterface** out) { return ::usbDsRegisterInterface(out); }
Result DsRegisterInterfaceEx(UsbDsInterface** out, u8 intf_num) { return ::usbDsRegisterInterfaceEx(out, intf_num); }
Result DsClearDeviceData(void) { return ::usbDsClearDeviceData(); }
Result DsAddUsbStringDescriptor(u8* out_index, const char* string) { return ::usbDsAddUsbStringDescriptor(out_index, string); }
Result DsAddUsbLanguageStringDescriptor(u8* out_index, const u16* lang_ids, u16 num_langs) { return ::usbDsAddUsbLanguageStringDescriptor(out_index, lang_ids, num_langs); }
Result DsDeleteUsbStringDescriptor(u8 index) { return ::usbDsDeleteUsbStringDescriptor(index); }
Result DsSetUsbDeviceDescriptor(UsbDeviceSpeed speed, struct usb_device_descriptor* descriptor) { return ::usbDsSetUsbDeviceDescriptor(speed, descriptor); }
Result DsSetBinaryObjectStore(const void* bos, size_t bos_size) { return ::usbDsSetBinaryObjectStore(bos, bos_size); }
Result DsEnable(void) { return ::usbDsEnable(); }
Result DsDisable(void) { return ::usbDsDisable(); }
Result DsGetSpeed(UsbDeviceSpeed *out) { return ::usbDsGetSpeed(out); }
void DsInterface_Close(UsbDsInterface* interface) { ::usbDsInterface_Close(interface); }
Result DsInterface_GetSetupPacket(UsbDsInterface* interface, void* buffer, size_t size) { return ::usbDsInterface_GetSetupPacket(interface, buffer, size); }
Result DsInterface_EnableInterface(UsbDsInterface* interface) { return ::usbDsInterface_EnableInterface(interface); }
Result DsInterface_DisableInterface(UsbDsInterface* interface) { return ::usbDsInterface_DisableInterface(interface); }
Result DsInterface_CtrlInPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32* urbId) { return ::usbDsInterface_CtrlInPostBufferAsync(interface, buffer, size, urbId); }
Result DsInterface_CtrlOutPostBufferAsync(UsbDsInterface* interface, void* buffer, size_t size, u32* urbId) { return ::usbDsInterface_CtrlOutPostBufferAsync(interface, buffer, size, urbId); }
Result DsInterface_GetCtrlInReportData(UsbDsInterface* interface, UsbDsReportData* out) { return ::usbDsInterface_GetCtrlInReportData(interface, out); }
Result DsInterface_GetCtrlOutReportData(UsbDsInterface* interface, UsbDsReportData* out) { return ::usbDsInterface_GetCtrlOutReportData(interface, out); }
Result DsInterface_StallCtrl(UsbDsInterface* interface) { return ::usbDsInterface_StallCtrl(interface); }
Result DsInterface_GetDsEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, struct usb_endpoint_descriptor* descriptor) { return ::usbDsInterface_GetDsEndpoint(interface, endpoint, descriptor); }
Result DsInterface_RegisterEndpoint(UsbDsInterface* interface, UsbDsEndpoint** endpoint, u8 endpoint_address) { return ::usbDsInterface_RegisterEndpoint(interface, endpoint, endpoint_address); }
Result DsInterface_AppendConfigurationData(UsbDsInterface* interface, UsbDeviceSpeed speed, const void* buffer, size_t size) { return ::usbDsInterface_AppendConfigurationData(interface, speed, buffer, size); }
void DsEndpoint_Close(UsbDsEndpoint* endpoint) { ::usbDsEndpoint_Close(endpoint); }
Result DsEndpoint_Cancel(UsbDsEndpoint* endpoint) { return ::usbDsEndpoint_Cancel(endpoint); }
Result DsEndpoint_PostBufferAsync(UsbDsEndpoint* endpoint, void* buffer, size_t size, u32* urbId) { return ::usbDsEndpoint_PostBufferAsync(endpoint, buffer, size, urbId); }
Result DsEndpoint_GetReportData(UsbDsEndpoint* endpoint, UsbDsReportData* out) { return ::usbDsEndpoint_GetReportData(endpoint, out); }
Result DsEndpoint_Stall(UsbDsEndpoint* endpoint) { return ::usbDsEndpoint_Stall(endpoint); }
Result DsEndpoint_SetZlt(UsbDsEndpoint* endpoint, bool zlt) { return ::usbDsEndpoint_SetZlt(endpoint, zlt); }
Result HsInitialize(void) { return ::usbHsInitialize(); }
void HsExit(void) { ::usbHsExit(); }
Service* HsGetServiceSession(void) { return ::usbHsGetServiceSession(); }
Event* HsGetInterfaceStateChangeEvent(void) { return ::usbHsGetInterfaceStateChangeEvent(); }
Result HsQueryAllInterfaces(const UsbHsInterfaceFilter* filter, UsbHsInterface* interfaces, size_t interfaces_maxsize, s32* total_entries) { return ::usbHsQueryAllInterfaces(filter, interfaces, interfaces_maxsize, total_entries); }
Result HsQueryAvailableInterfaces(const UsbHsInterfaceFilter* filter, UsbHsInterface* interfaces, size_t interfaces_maxsize, s32* total_entries) { return ::usbHsQueryAvailableInterfaces(filter, interfaces, interfaces_maxsize, total_entries); }
Result HsQueryAcquiredInterfaces(UsbHsInterface* interfaces, size_t interfaces_maxsize, s32* total_entries) { return ::usbHsQueryAcquiredInterfaces(interfaces, interfaces_maxsize, total_entries); }
Result HsCreateInterfaceAvailableEvent(Event* out_event, bool autoclear, u8 index, const UsbHsInterfaceFilter* filter) { return ::usbHsCreateInterfaceAvailableEvent(out_event, autoclear, index, filter); }
Result HsDestroyInterfaceAvailableEvent(Event* event, u8 index) { return ::usbHsDestroyInterfaceAvailableEvent(event, index); }
Result HsAcquireUsbIf(UsbHsClientIfSession* s, UsbHsInterface *interface) { return ::usbHsAcquireUsbIf(s, interface); }
void HsIfClose(UsbHsClientIfSession* s) { ::usbHsIfClose(s); }
bool HsIfIsActive(UsbHsClientIfSession* s) { return ::usbHsIfIsActive(s); }
s32 HsIfGetID(UsbHsClientIfSession* s) { return ::usbHsIfGetID(s); }
Result HsIfSetInterface(UsbHsClientIfSession* s, UsbHsInterfaceInfo* inf, u8 id) { return ::usbHsIfSetInterface(s, inf, id); }
Result HsIfGetInterface(UsbHsClientIfSession* s, UsbHsInterfaceInfo* inf) { return ::usbHsIfGetInterface(s, inf); }
Result HsIfGetAlternateInterface(UsbHsClientIfSession* s, UsbHsInterfaceInfo* inf, u8 id) { return ::usbHsIfGetAlternateInterface(s, inf, id); }
Result HsIfGetCurrentFrame(UsbHsClientIfSession* s, u32* out) { return ::usbHsIfGetCurrentFrame(s, out); }
Result HsIfCtrlXfer(UsbHsClientIfSession* s, u8 bmRequestType, u8 bRequest, u16 wValue, u16 wIndex, u16 wLength, void* buffer, u32* transferredSize) { return ::usbHsIfCtrlXfer(s, bmRequestType, bRequest, wValue, wIndex, wLength, buffer, transferredSize); }
Result HsIfOpenUsbEp(UsbHsClientIfSession* s, UsbHsClientEpSession* ep, u16 maxUrbCount, u32 maxXferSize, struct usb_endpoint_descriptor *desc) { return ::usbHsIfOpenUsbEp(s, ep, maxUrbCount, maxXferSize, desc); }
Result HsIfResetDevice(UsbHsClientIfSession* s) { return ::usbHsIfResetDevice(s); }
void HsEpClose(UsbHsClientEpSession* s) { ::usbHsEpClose(s); }
Event* HsEpGetXferEvent(UsbHsClientEpSession* s) { return ::usbHsEpGetXferEvent(s); }
u32 HsEpGetReportRingSize(UsbHsClientEpSession* s) { return ::usbHsEpGetReportRingSize(s); }
Result HsEpPostBufferAsync(UsbHsClientEpSession* s, void* buffer, u32 size, u64 id, u32* xferId) { return ::usbHsEpPostBufferAsync(s, buffer, size, id, xferId); }
Result HsEpGetXferReport(UsbHsClientEpSession* s, UsbHsXferReport* reports, u32 max_reports, u32* count) { return ::usbHsEpGetXferReport(s, reports, max_reports, count); }
Result HsEpPostBuffer(UsbHsClientEpSession* s, void* buffer, u32 size, u32* transferredSize) { return ::usbHsEpPostBuffer(s, buffer, size, transferredSize); }
Result HsEpBatchBufferAsync(UsbHsClientEpSession* s, void* buffer, u32* urbs, u32 urbCount, u64 id, u32 unk1, u32 unk2, u32* xferId) { return ::usbHsEpBatchBufferAsync(s, buffer, urbs, urbCount, id, unk1, unk2, xferId); }
Result HsEpCreateSmmuSpace(UsbHsClientEpSession* s, void* buffer, u32 size) { return ::usbHsEpCreateSmmuSpace(s, buffer, size); }
Result HsEpShareReportRing(UsbHsClientEpSession* s, void* buffer, size_t size) { return ::usbHsEpShareReportRing(s, buffer, size); }

} // namespace usb

namespace utf {

ssize_t DecodeUtf8(uint32_t *out, const uint8_t *in) { return ::decode_utf8(out, in); }
ssize_t DecodeUtf16(uint32_t *out, const uint16_t *in) { return ::decode_utf16(out, in); }
ssize_t EncodeUtf8(uint8_t *out, uint32_t in) { return ::encode_utf8(out, in); }
ssize_t EncodeUtf16(uint16_t *out, uint32_t in) { return ::encode_utf16(out, in); }
ssize_t Utf8ToUtf16(uint16_t *out, const uint8_t *in, size_t len) { return ::utf8_to_utf16(out, in, len); }
ssize_t Utf8ToUtf32(uint32_t *out, const uint8_t *in, size_t len) { return ::utf8_to_utf32(out, in, len); }
ssize_t Utf16ToUtf8(uint8_t *out, const uint16_t *in, size_t len) { return ::utf16_to_utf8(out, in, len); }
ssize_t Utf16ToUtf32(uint32_t *out, const uint16_t *in, size_t len) { return ::utf16_to_utf32(out, in, len); }
ssize_t Utf32ToUtf8(uint8_t *out, const uint32_t *in, size_t len) { return ::utf32_to_utf8(out, in, len); }
ssize_t Utf32ToUtf16(uint16_t *out, const uint32_t *in, size_t len) { return ::utf32_to_utf16(out, in, len); }

} // namespace utf

namespace utimer {

void Create(UTimer* t, u64 interval, TimerType type) { ::utimerCreate(t, interval, type); }
void Start(UTimer* t) { ::utimerStart(t); }
void Stop(UTimer* t) { ::utimerStop(t); }

} // namespace utimer

namespace vi {

Result Initialize(ViServiceType service_type) { return ::viInitialize(service_type); }
void Exit(void) { ::viExit(); }
Service* GetSession_IApplicationDisplayService(void) { return ::viGetSession_IApplicationDisplayService(); }
Service* GetSession_IHOSBinderDriverRelay(void) { return ::viGetSession_IHOSBinderDriverRelay(); }
Service* GetSession_ISystemDisplayService(void) { return ::viGetSession_ISystemDisplayService(); }
Service* GetSession_IManagerDisplayService(void) { return ::viGetSession_IManagerDisplayService(); }
Service* GetSession_IHOSBinderDriverIndirect(void) { return ::viGetSession_IHOSBinderDriverIndirect(); }
Result SetContentVisibility(bool v) { return ::viSetContentVisibility(v); }
Result OpenDisplay(const char *display_name, ViDisplay *display) { return ::viOpenDisplay(display_name, display); }
Result CloseDisplay(ViDisplay *display) { return ::viCloseDisplay(display); }
Result OpenDefaultDisplay(ViDisplay *display) { return ::viOpenDefaultDisplay(display); }
Result GetDisplayResolution(ViDisplay *display, s32 *width, s32 *height) { return ::viGetDisplayResolution(display, width, height); }
Result GetDisplayLogicalResolution(ViDisplay *display, s32 *width, s32 *height) { return ::viGetDisplayLogicalResolution(display, width, height); }
Result SetDisplayMagnification(ViDisplay *display, s32 x, s32 y, s32 width, s32 height) { return ::viSetDisplayMagnification(display, x, y, width, height); }
Result GetDisplayVsyncEvent(ViDisplay *display, Event *event_out) { return ::viGetDisplayVsyncEvent(display, event_out); }
Result SetDisplayPowerState(ViDisplay *display, ViPowerState state) { return ::viSetDisplayPowerState(display, state); }
Result SetDisplayAlpha(ViDisplay *display, float alpha) { return ::viSetDisplayAlpha(display, alpha); }
Result GetZOrderCountMin(ViDisplay *display, s32 *z) { return ::viGetZOrderCountMin(display, z); }
Result GetZOrderCountMax(ViDisplay *display, s32 *z) { return ::viGetZOrderCountMax(display, z); }
Result CreateLayer(const ViDisplay *display, ViLayer *layer) { return ::viCreateLayer(display, layer); }
Result CreateManagedLayer(const ViDisplay *display, ViLayerFlags layer_flags, u64 aruid, u64 *layer_id) { return ::viCreateManagedLayer(display, layer_flags, aruid, layer_id); }
Result SetLayerSize(ViLayer *layer, s32 width, s32 height) { return ::viSetLayerSize(layer, width, height); }
Result SetLayerZ(ViLayer *layer, s32 z) { return ::viSetLayerZ(layer, z); }
Result SetLayerPosition(ViLayer *layer, float x, float y) { return ::viSetLayerPosition(layer, x, y); }
Result CloseLayer(ViLayer *layer) { return ::viCloseLayer(layer); }
Result DestroyManagedLayer(ViLayer *layer) { return ::viDestroyManagedLayer(layer); }
Result SetLayerScalingMode(ViLayer *layer, ViScalingMode scaling_mode) { return ::viSetLayerScalingMode(layer, scaling_mode); }
Result GetIndirectLayerImageMap(void* buffer, size_t size, s32 width, s32 height, u64 IndirectLayerConsumerHandle, u64 *out_size, u64 *out_stride) { return ::viGetIndirectLayerImageMap(buffer, size, width, height, IndirectLayerConsumerHandle, out_size, out_stride); }
Result GetIndirectLayerImageRequiredMemoryInfo(s32 width, s32 height, u64 *out_size, u64 *out_alignment) { return ::viGetIndirectLayerImageRequiredMemoryInfo(width, height, out_size, out_alignment); }
Result ManagerPrepareFatal(void) { return ::viManagerPrepareFatal(); }
Result ManagerShowFatal(void) { return ::viManagerShowFatal(); }
Result ManagerDrawFatalRectangle(s32 x, s32 y, s32 end_x, s32 end_y, ViColorRgba4444 color) { return ::viManagerDrawFatalRectangle(x, y, end_x, end_y, color); }
Result ManagerDrawFatalText32(s32 *out_advance, s32 x, s32 y, const u32 *utf32_codepoints, size_t num_codepoints, float scale_x, float scale_y, PlSharedFontType font_type, ViColorRgba8888 bg_color, ViColorRgba8888 fg_color, s32 initial_advance) { return ::viManagerDrawFatalText32(out_advance, x, y, utf32_codepoints, num_codepoints, scale_x, scale_y, font_type, bg_color, fg_color, initial_advance); }

} // namespace vi

namespace virtmem {

void Lock(void) { ::virtmemLock(); }
void Unlock(void) { ::virtmemUnlock(); }
void* FindAslr(size_t size, size_t guard_size) { return ::virtmemFindAslr(size, guard_size); }
void* FindStack(size_t size, size_t guard_size) { return ::virtmemFindStack(size, guard_size); }
void* FindCodeMemory(size_t size, size_t guard_size) { return ::virtmemFindCodeMemory(size, guard_size); }
VirtmemReservation* AddReservation(void* mem, size_t size) { return ::virtmemAddReservation(mem, size); }
void RemoveReservation(VirtmemReservation* rv) { ::virtmemRemoveReservation(rv); }

} // namespace virtmem

namespace wait {

Result Objects(s32* idx_out, const Waiter* objects, s32 num_objects, u64 timeout) { return ::waitObjects(idx_out, objects, num_objects, timeout); }
Result Handles(s32* idx_out, const Handle* handles, s32 num_handles, u64 timeout) { return ::waitHandles(idx_out, handles, num_handles, timeout); }
Result Single(Waiter w, u64 timeout) { return ::waitSingle(w, timeout); }
Result SingleHandle(Handle h, u64 timeout) { return ::waitSingleHandle(h, timeout); }

} // namespace wait

namespace waiter {

Waiter ForEvent(Event* t) { return ::waiterForEvent(t); }
Waiter ForThread(Thread* t) { return ::waiterForThread(t); }
Waiter ForUEvent(UEvent* e) { return ::waiterForUEvent(e); }
Waiter ForUTimer(UTimer* t) { return ::waiterForUTimer(t); }
Waiter ForHandle(Handle h) { return ::waiterForHandle(h); }

} // namespace waiter

namespace web {

void WifiCreate(WebWifiConfig* config, const char* conntest_url, const char* initial_url, Uuid uuid, u32 rev) { ::webWifiCreate(config, conntest_url, initial_url, uuid, rev); }
Result WifiShow(WebWifiConfig* config, WebWifiReturnValue *out) { return ::webWifiShow(config, out); }
Result PageCreate(WebCommonConfig* config, const char* url) { return ::webPageCreate(config, url); }
Result NewsCreate(WebCommonConfig* config, const char* url) { return ::webNewsCreate(config, url); }
Result YouTubeVideoCreate(WebCommonConfig* config, const char* url) { return ::webYouTubeVideoCreate(config, url); }
Result OfflineCreate(WebCommonConfig* config, WebDocumentKind docKind, u64 id, const char* docPath) { return ::webOfflineCreate(config, docKind, id, docPath); }
Result ShareCreate(WebCommonConfig* config, WebShareStartPage page) { return ::webShareCreate(config, page); }
Result LobbyCreate(WebCommonConfig* config) { return ::webLobbyCreate(config); }
Result ConfigSetCallbackUrl(WebCommonConfig* config, const char* url) { return ::webConfigSetCallbackUrl(config, url); }
Result ConfigSetCallbackableUrl(WebCommonConfig* config, const char* url) { return ::webConfigSetCallbackableUrl(config, url); }
Result ConfigSetWhitelist(WebCommonConfig* config, const char* whitelist) { return ::webConfigSetWhitelist(config, whitelist); }
Result ConfigSetUid(WebCommonConfig* config, AccountUid uid) { return ::webConfigSetUid(config, uid); }
Result ConfigSetAlbumEntry(WebCommonConfig* config, const CapsAlbumEntry *entry) { return ::webConfigSetAlbumEntry(config, entry); }
Result ConfigSetScreenShot(WebCommonConfig* config, bool flag) { return ::webConfigSetScreenShot(config, flag); }
Result ConfigSetEcClientCert(WebCommonConfig* config, bool flag) { return ::webConfigSetEcClientCert(config, flag); }
Result ConfigSetPlayReport(WebCommonConfig* config, bool flag) { return ::webConfigSetPlayReport(config, flag); }
Result ConfigSetBootDisplayKind(WebCommonConfig* config, WebBootDisplayKind kind) { return ::webConfigSetBootDisplayKind(config, kind); }
Result ConfigSetBackgroundKind(WebCommonConfig* config, WebBackgroundKind kind) { return ::webConfigSetBackgroundKind(config, kind); }
Result ConfigSetFooter(WebCommonConfig* config, bool flag) { return ::webConfigSetFooter(config, flag); }
Result ConfigSetPointer(WebCommonConfig* config, bool flag) { return ::webConfigSetPointer(config, flag); }
Result ConfigSetLeftStickMode(WebCommonConfig* config, WebLeftStickMode mode) { return ::webConfigSetLeftStickMode(config, mode); }
Result ConfigSetKeyRepeatFrame(WebCommonConfig* config, s32 inval0, s32 inval1) { return ::webConfigSetKeyRepeatFrame(config, inval0, inval1); }
Result ConfigSetDisplayUrlKind(WebCommonConfig* config, bool kind) { return ::webConfigSetDisplayUrlKind(config, kind); }
Result ConfigSetBootAsMediaPlayer(WebCommonConfig* config, bool flag) { return ::webConfigSetBootAsMediaPlayer(config, flag); }
Result ConfigSetShopJump(WebCommonConfig* config, bool flag) { return ::webConfigSetShopJump(config, flag); }
Result ConfigSetMediaPlayerUserGestureRestriction(WebCommonConfig* config, bool flag) { return ::webConfigSetMediaPlayerUserGestureRestriction(config, flag); }
Result ConfigSetMediaAutoPlay(WebCommonConfig* config, bool flag) { return ::webConfigSetMediaAutoPlay(config, flag); }
Result ConfigSetLobbyParameter(WebCommonConfig* config, const char* str) { return ::webConfigSetLobbyParameter(config, str); }
Result ConfigSetApplicationAlbumEntry(WebCommonConfig* config, CapsApplicationAlbumEntry *entry) { return ::webConfigSetApplicationAlbumEntry(config, entry); }
Result ConfigSetJsExtension(WebCommonConfig* config, bool flag) { return ::webConfigSetJsExtension(config, flag); }
Result ConfigSetAdditionalCommentText(WebCommonConfig* config, const char* str) { return ::webConfigSetAdditionalCommentText(config, str); }
Result ConfigSetTouchEnabledOnContents(WebCommonConfig* config, bool flag) { return ::webConfigSetTouchEnabledOnContents(config, flag); }
Result ConfigSetUserAgentAdditionalString(WebCommonConfig* config, const char* str) { return ::webConfigSetUserAgentAdditionalString(config, str); }
Result ConfigSetAdditionalMediaData(WebCommonConfig* config, const u8* data, size_t size) { return ::webConfigSetAdditionalMediaData(config, data, size); }
Result ConfigSetMediaPlayerAutoClose(WebCommonConfig* config, bool flag) { return ::webConfigSetMediaPlayerAutoClose(config, flag); }
Result ConfigSetPageCache(WebCommonConfig* config, bool flag) { return ::webConfigSetPageCache(config, flag); }
Result ConfigSetWebAudio(WebCommonConfig* config, bool flag) { return ::webConfigSetWebAudio(config, flag); }
Result ConfigSetFooterFixedKind(WebCommonConfig* config, WebFooterFixedKind kind) { return ::webConfigSetFooterFixedKind(config, kind); }
Result ConfigSetPageFade(WebCommonConfig* config, bool flag) { return ::webConfigSetPageFade(config, flag); }
Result ConfigSetMediaCreatorApplicationRatingAge(WebCommonConfig* config, const s8 *data) { return ::webConfigSetMediaCreatorApplicationRatingAge(config, data); }
Result ConfigSetBootLoadingIcon(WebCommonConfig* config, bool flag) { return ::webConfigSetBootLoadingIcon(config, flag); }
Result ConfigSetPageScrollIndicator(WebCommonConfig* config, bool flag) { return ::webConfigSetPageScrollIndicator(config, flag); }
Result ConfigSetMediaPlayerSpeedControl(WebCommonConfig* config, bool flag) { return ::webConfigSetMediaPlayerSpeedControl(config, flag); }
Result ConfigAddAlbumEntryAndMediaData(WebCommonConfig* config, const CapsAlbumEntry *entry, const u8* data, size_t size) { return ::webConfigAddAlbumEntryAndMediaData(config, entry, data, size); }
Result ConfigSetBootFooterButtonVisible(WebCommonConfig* config, WebFooterButtonId button, bool visible) { return ::webConfigSetBootFooterButtonVisible(config, button, visible); }
Result ConfigSetOverrideWebAudioVolume(WebCommonConfig* config, float value) { return ::webConfigSetOverrideWebAudioVolume(config, value); }
Result ConfigSetOverrideMediaAudioVolume(WebCommonConfig* config, float value) { return ::webConfigSetOverrideMediaAudioVolume(config, value); }
Result ConfigSetBootMode(WebCommonConfig* config, WebSessionBootMode mode) { return ::webConfigSetBootMode(config, mode); }
Result ConfigSetMediaPlayerUi(WebCommonConfig* config, bool flag) { return ::webConfigSetMediaPlayerUi(config, flag); }
Result ConfigSetTransferMemory(WebCommonConfig* config, bool flag) { return ::webConfigSetTransferMemory(config, flag); }
Result ConfigShow(WebCommonConfig* config, WebCommonReply *out) { return ::webConfigShow(config, out); }
Result ConfigRequestExit(WebCommonConfig* config) { return ::webConfigRequestExit(config); }
Result ReplyGetExitReason(WebCommonReply *reply, WebExitReason *exitReason) { return ::webReplyGetExitReason(reply, exitReason); }
Result ReplyGetLastUrl(WebCommonReply *reply, char *outstr, size_t outstr_maxsize, size_t *out_size) { return ::webReplyGetLastUrl(reply, outstr, outstr_maxsize, out_size); }
Result ReplyGetSharePostResult(WebCommonReply *reply, u32 *sharePostResult) { return ::webReplyGetSharePostResult(reply, sharePostResult); }
Result ReplyGetPostServiceName(WebCommonReply *reply, char *outstr, size_t outstr_maxsize, size_t *out_size) { return ::webReplyGetPostServiceName(reply, outstr, outstr_maxsize, out_size); }
Result ReplyGetPostId(WebCommonReply *reply, char *outstr, size_t outstr_maxsize, size_t *out_size) { return ::webReplyGetPostId(reply, outstr, outstr_maxsize, out_size); }
Result ReplyGetMediaPlayerAutoClosedByCompletion(WebCommonReply *reply, bool *flag) { return ::webReplyGetMediaPlayerAutoClosedByCompletion(reply, flag); }
void SessionCreate(WebSession *s, WebCommonConfig* config) { ::webSessionCreate(s, config); }
void SessionClose(WebSession *s) { ::webSessionClose(s); }
Result SessionStart(WebSession *s, Event **out_event) { return ::webSessionStart(s, out_event); }
Result SessionWaitForExit(WebSession *s, WebCommonReply *out) { return ::webSessionWaitForExit(s, out); }
Result SessionRequestExit(WebSession *s) { return ::webSessionRequestExit(s); }
Result SessionAppear(WebSession *s, bool *flag) { return ::webSessionAppear(s, flag); }
Result SessionTrySendContentMessage(WebSession *s, const char *content, u32 size, bool *flag) { return ::webSessionTrySendContentMessage(s, content, size, flag); }
Result SessionTryReceiveContentMessage(WebSession *s, char *content, u64 size, u64 *out_size, bool *flag) { return ::webSessionTryReceiveContentMessage(s, content, size, out_size, flag); }

} // namespace web

namespace wlaninf {

Result Initialize(void) { return ::wlaninfInitialize(); }
void Exit(void) { ::wlaninfExit(); }
Service* GetServiceSession(void) { return ::wlaninfGetServiceSession(); }
Result GetState(WlanInfState* out) { return ::wlaninfGetState(out); }
Result GetRSSI(s32* out) { return ::wlaninfGetRSSI(out); }

} // namespace wlaninf

} // namespace nx


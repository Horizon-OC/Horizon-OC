class OtherMenu;

void StartMiscThread() {
    // Wait for existing thread to exit
    threadWaitForExit(&t0);
    
    // Clear the thread exit event for new thread
    leventClear(&threadexit);
    
    // Close and recreate thread
    threadClose(&t0);
    threadCreate(&t0, Misc2, NULL, NULL, 0x1000, 0x3F, 3);
    threadStart(&t0);
}

void EndMiscThread() {
    // Signal the thread exit event
    leventSignal(&threadexit);
    
    // Wait for thread to exit
    threadWaitForExit(&t0);
    
    // Close thread handle
    threadClose(&t0);
}

class MiscOverlay : public tsl::Gui {
private:
    char DSP_Load_c[16];
    // Separated value buffers for NV clocks
    char NVDEC_value_c[16] = "";
    char NVENC_value_c[16] = "";
    char NVJPG_value_c[16] = "";
    char Nifm_pass[96];
    FullSettings settings;
public:
    MiscOverlay() { 
        GetConfigSettings(&settings);
        disableJumpTo = true;
        smInitialize();
        nifmCheck = nifmInitialize(NifmServiceType_Admin);
        if (R_SUCCEEDED(mmuInitialize())) {
            nvdecCheck = mmuRequestInitialize(&nvdecRequest, MmuModuleId(5), 8, false);
            nvencCheck = mmuRequestInitialize(&nvencRequest, MmuModuleId(6), 8, false);
            nvjpgCheck = mmuRequestInitialize(&nvjpgRequest, MmuModuleId(7), 8, false);
        }

        if (R_SUCCEEDED(audsnoopInitialize())) 
            audsnoopCheck = audsnoopEnableDspUsageMeasurement();

        smExit();
        StartMiscThread();
        // tsl::elm::g_disableMenuCacheOnReturn.store(true, std::memory_order_release);
    }

    ~MiscOverlay() {
        EndMiscThread();
        nifmExit();
        mmuRequestFinalize(&nvdecRequest);
        mmuRequestFinalize(&nvencRequest);
        mmuRequestFinalize(&nvjpgRequest);
        mmuExit();
        if (R_SUCCEEDED(audsnoopCheck)) {
            audsnoopDisableDspUsageMeasurement();
        }
        audsnoopExit();
    }

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            static constexpr u16 X_OFFSET = 30;
            static constexpr u16 NV_VALUE_X = 120;  // Aligned X position for NV values

            ///DSP
            if (R_SUCCEEDED(audsnoopCheck)) {
                renderer->drawString(DSP_Load_c, false, X_OFFSET, 120, 20, (settings.textColor));
            }

            //Multimedia engines
            if (R_SUCCEEDED(nvdecCheck | nvencCheck | nvjpgCheck)) {
                renderer->drawString("Multimedia Clock Rates", false, X_OFFSET, 165, 20, (settings.catColor1));
                
                u16 currentY = 185;
                
                if (R_SUCCEEDED(nvdecCheck)) {
                    renderer->drawString("NVDEC", false, X_OFFSET+15, currentY, 15, (settings.catColor2));
                    renderer->drawString(NVDEC_value_c, false, NV_VALUE_X, currentY, 15, (settings.textColor));
                    currentY += 15;
                }
                if (R_SUCCEEDED(nvencCheck)) {
                    renderer->drawString("NVENC", false, X_OFFSET+15, currentY, 15, (settings.catColor2));
                    renderer->drawString(NVENC_value_c, false, NV_VALUE_X, currentY, 15, (settings.textColor));
                    currentY += 15;
                }
                if (R_SUCCEEDED(nvjpgCheck)) {
                    renderer->drawString("NVJPG", false, X_OFFSET+15, currentY, 15, (settings.catColor2));
                    renderer->drawString(NVJPG_value_c, false, NV_VALUE_X, currentY, 15, (settings.textColor));
                }
            }

            if (R_SUCCEEDED(nifmCheck)) {
                renderer->drawString("Network", false, X_OFFSET, 255, 20, (settings.catColor1));
                if (!Nifm_internet_rc) {
                    if (NifmConnectionType == NifmInternetConnectionType_WiFi) {
                        renderer->drawString("Type: Wi-Fi", false, X_OFFSET, 280, 18, (settings.catColor2));
                        if (!Nifm_profile_rc) {
                            if (Nifm_showpass)
                                renderer->drawString(Nifm_pass, false, X_OFFSET, 305, 15, (settings.textColor));
                            else
                                renderer->drawString("Press Y to show password", false, X_OFFSET, 305, 15, (settings.textColor));
                        }
                    }
                    else if (NifmConnectionType == NifmInternetConnectionType_Ethernet)
                        renderer->drawString("Type: Ethernet", false, X_OFFSET, 280, 18, (settings.textColor));
                }
                else
                    renderer->drawString("Type: Not connected", false, X_OFFSET, 280, 18, (settings.textColor));
            }

            
        });
        
        // tsl::elm::g_disableMenuCacheOnReturn.store(true, std::memory_order_release);
        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("Horizon OC Monitor", APP_VERSION, true);
        rootFrame->setContent(Status);
        
        return rootFrame;
    }

    virtual void update() override {

        snprintf(DSP_Load_c, sizeof DSP_Load_c, "DSP usage: %u%%", DSP_Load_u);
        
        // Format just the values for NV clocks
        snprintf(NVDEC_value_c, sizeof(NVDEC_value_c), "%.1f MHz", (float)NVDEC_Hz / 1000000);
        snprintf(NVENC_value_c, sizeof(NVENC_value_c), "%.1f MHz", (float)NVENC_Hz / 1000000);
        snprintf(NVJPG_value_c, sizeof(NVJPG_value_c), "%.1f MHz", (float)NVJPG_Hz / 1000000);
        
        char pass_temp1[25] = "";
        char pass_temp2[25] = "";
        char pass_temp3[17] = "";
        if (Nifm_profile.wireless_setting_data.passphrase_len > 48) {
            memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
            memcpy(&pass_temp2, &(Nifm_profile.wireless_setting_data.passphrase[24]), 24);
            memcpy(&pass_temp3, &(Nifm_profile.wireless_setting_data.passphrase[48]), 16);
        }
        else if (Nifm_profile.wireless_setting_data.passphrase_len > 24) {
            memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
            memcpy(&pass_temp2, &(Nifm_profile.wireless_setting_data.passphrase[24]), 24);
        }
        else {
            memcpy(&pass_temp1, &(Nifm_profile.wireless_setting_data.passphrase[0]), 24);
        }
        snprintf(Nifm_pass, sizeof Nifm_pass, "%s\n%s\n%s", pass_temp1, pass_temp2, pass_temp3);
        
        static bool skipOnce = true;
    
        if (!skipOnce) {
            static bool runOnce = true;
            if (runOnce) {
                isRendering = true;
                leventClear(&renderingStopEvent);
                runOnce = false;
            }
        } else {
            skipOnce = false;
        }
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (keysHeld & KEY_Y) {
            Nifm_showpass = true;
        }
        else Nifm_showpass = false;

        if (keysDown & KEY_B) {
            isRendering = false;
            leventSignal(&renderingStopEvent);
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            lastSelectedItem = "Miscellaneous";
            lastMode = "";
            tsl::swapTo<OtherMenu>();
            return true;
        }
        return false;
    }
};
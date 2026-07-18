class MainMenu;

class MiniOverlay : public tsl::Gui {
private:
    char GPU_Load_c[32] = "";
    char Rotation_SpeedLevel_c[64] = "";
    char RAM_var_compressed_c[128] = "";
    char Battery_c[64] = "";
    char soc_temperature_c[64] = "";
    char skin_temperature_c[64] = "";
	char CPU_temp_c[32];
    char GPU_temp_c[32];
    char RAM_temp_c[32];

    uint32_t rectangleWidth;
    char Variables[512];
    size_t fontsize;
    MiniSettings settings;
    bool Initialized = false;
    ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
    uint64_t systemtickfrequency_impl = systemtickfrequency;
    int frameOffsetX, frameOffsetY;
    int topPadding, bottomPadding;
    bool isDragging = false;

    bool skipOnce = true;
    bool runOnce = true;

    size_t framePadding = 10;
    static constexpr int screenWidth = 1280;
    static constexpr int screenHeight = 720;

    struct ButtonState {
        std::atomic<bool> minusDragActive{false};
        std::atomic<bool> plusDragActive{false};
    } buttonState;

    Thread touchPollThread;
    std::atomic<bool> touchPollRunning{false};
public:
    MiniOverlay() {
        CPU_temp_c[0] = '\0';
        GPU_temp_c[0] = '\0';
        RAM_temp_c[0] = '\0';

        tsl::hlp::requestForeground(false);
        disableJumpTo = true;
        //tsl::initializeUltrahandSettings();
        PowerConsumption = 0.0f;
        batTimeEstimate = -1;
        strcpy(Battery_c, "-.-- W-.-% [--:--]"); // Default display

        GetConfigSettings(&settings);
        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            fontsize = settings.handheldFontSize;
        }
        else fontsize = settings.dockedFontSize;

        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().removeScreenshotStacks();
        }
        mutexInit(&mutex_BatteryChecker);
        mutexInit(&mutex_Misc);
        //alphabackground = 0x0;
        frameOffsetX = settings.frameOffsetX;
        frameOffsetY = settings.frameOffsetY;
        framePadding = settings.framePadding;
        topPadding = 5;
        bottomPadding = 2;

        if (ult::limitedMemory) {
            tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
        }

        FullMode = false;
        TeslaFPS = settings.refreshRate;
        systemtickfrequency_impl /= settings.refreshRate;
        deactivateOriginalFooter = true;
        realVoltsPolling = settings.realVolts;
        StartThreads();

        // Get initial battery readings BEFORE starting threads
        if (R_SUCCEEDED(psmCheck) && R_SUCCEEDED(i2cCheck)) {
            uint16_t data = 0;
            float tempA = 0.0;

            // Get initial power consumption
            Max17050ReadReg(MAX17050_AvgCurrent, &data);
            tempA = (1.5625 / (max17050SenseResistor * max17050CGain)) * (s16)data;
            PowerConsumption = tempA * batVoltageAvg / 1000000.0; // Rough initial estimate

            // Get initial battery info
            psmGetBatteryChargeInfoFields(psmService, &_batteryChargeInfoFields);

            // Get initial time estimate
            if (tempA >= 0) {
                batTimeEstimate = -1;
            } else {
                Max17050ReadReg(MAX17050_TTE, &data);
                const float batteryTimeEstimateInMinutes = (5.625 * data) / 60;
                if (batteryTimeEstimateInMinutes > (99.0*60.0)+59.0) {
                    batTimeEstimate = (99*60)+59;
                } else {
                    batTimeEstimate = (int16_t)batteryTimeEstimateInMinutes;
                }
            }
        } else {
            // Fallback if checks failed
            PowerConsumption = 0.0f;
            batTimeEstimate = -1;
            _batteryChargeInfoFields = {0};
        }

        // Now format the initial Battery_c string
        char remainingBatteryLife[8];
        const float drawW = (fabsf(PowerConsumption) < 0.01f) ? 0.0f : PowerConsumption;

        if (batTimeEstimate >= 0 && !(drawW <= 0.01f && drawW >= -0.01f)) {
            snprintf(remainingBatteryLife, sizeof(remainingBatteryLife),
                     "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
        } else {
            strcpy(remainingBatteryLife, "--:--");
        }

        if (!settings.invertBatteryDisplay) {
            snprintf(Battery_c, sizeof(Battery_c),
                     "%.2f W%.1f%% [%s]",
                     drawW,
                     (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                     remainingBatteryLife);
        } else {
            snprintf(Battery_c, sizeof(Battery_c),
                     "%.1f%% [%s]%.2f W",
                     (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                     remainingBatteryLife,
                     drawW);
        }




        // Start touch polling thread for instant response at low FPS
        touchPollRunning.store(true, std::memory_order_release);
        threadCreate(&touchPollThread, [](void* arg) -> void {
            MiniOverlay* overlay = static_cast<MiniOverlay*>(arg);

            // Allow only Player 1 and handheld mode
            const HidNpadIdType id_list[2] = { HidNpadIdType_No1, HidNpadIdType_Handheld };

            // Configure HID system to only listen to these IDs
            hidSetSupportedNpadIdType(id_list, 2);

            // Configure input for up to 2 supported controllers (P1 + Handheld)
            padConfigureInput(2, HidNpadStyleSet_NpadStandard | HidNpadStyleTag_NpadSystemExt);

            // Initialize separate pad states for both controllers
            PadState pad_p1;
            PadState pad_handheld;
            padInitialize(&pad_p1, HidNpadIdType_No1);
            padInitialize(&pad_handheld, HidNpadIdType_Handheld);

            u64 minusHoldStart = 0;
            u64 plusHoldStart = 0;
            static constexpr u64 HOLD_THRESHOLD_NS = 500'000'000ULL;

            HidTouchScreenState state = {0};
            bool inputDetected;
            size_t actualEntryCount;

            while (overlay->touchPollRunning.load(std::memory_order_acquire)) {
                // Only poll when rendering and not dragging
                {
                    inputDetected = false;

                    // Check touch in bounds
                    if (hidGetTouchScreenStates(&state, 1) && state.count > 0) {
                        const int touchX = state.touches[0].x;
                        const int touchY = state.touches[0].y;

                        // Calculate bounds (same logic as handleInput)
                        const uint32_t margin = (overlay->fontsize * 4);
                        const int overlayX = overlay->frameOffsetX;
                        const int overlayY = overlay->frameOffsetY;
                        const int overlayWidth = margin + overlay->rectangleWidth + (overlay->fontsize / 3);

                        // Calculate height from Variables string
                        actualEntryCount = 1;
                        for (size_t i = 0; overlay->Variables[i] != '\0'; i++) {
                            if (overlay->Variables[i] == '\n') {
                                actualEntryCount++;
                            }
                        }
                        const int overlayHeight = ((overlay->fontsize + overlay->settings.spacing) * actualEntryCount) +
                                                 (overlay->fontsize / 3) + overlay->settings.spacing +
                                                 overlay->topPadding + overlay->bottomPadding;

                        // Add touch padding
                        const int touchPadding = 4;
                        const int touchableX = overlayX - touchPadding;
                        const int touchableY = overlayY - touchPadding;
                        const int touchableWidth = overlayWidth + (touchPadding * 2);
                        const int touchableHeight = overlayHeight + (touchPadding * 2);

                        // Check if touch is within bounds
                        if (touchX >= touchableX && touchX <= touchableX + touchableWidth &&
                            touchY >= touchableY && touchY <= touchableY + touchableHeight) {
                            inputDetected = true;
                        }
                    }

                    // Poll buttons from both controllers
                    padUpdate(&pad_p1);
                    padUpdate(&pad_handheld);
                    //const u64 keysHeld_p1 = padGetButtons(&pad_p1);
                    //const u64 keysHeld_handheld = padGetButtons(&pad_handheld);

                    // Combine input from both controllers
                    const u64 keysHeld = padGetButtons(&pad_p1) | padGetButtons(&pad_handheld);
                    const u64 now = armTicksToNs(armGetSystemTick());

                    // Track MINUS hold duration
                    if ((keysHeld & KEY_MINUS) && !(keysHeld & ~KEY_MINUS & ALL_KEYS_MASK)) {
                        if (minusHoldStart == 0) {
                            minusHoldStart = now;
                        }
                        if (now - minusHoldStart >= HOLD_THRESHOLD_NS) {
                            // Long enough to start drag
                            inputDetected = true;
                            overlay->buttonState.minusDragActive.exchange(true, std::memory_order_acq_rel);
                        }
                    }

                    // Track PLUS hold duration
                    else if ((keysHeld & KEY_PLUS) && !(keysHeld & ~KEY_PLUS & ALL_KEYS_MASK)) {
                        if (plusHoldStart == 0) {
                            plusHoldStart = now;
                        }
                        if (now - plusHoldStart >= HOLD_THRESHOLD_NS) {
                            // Long enough to start drag
                            inputDetected = true;
                            overlay->buttonState.plusDragActive.exchange(true, std::memory_order_acq_rel);
                        }
                    }

                    else {
                        minusHoldStart = plusHoldStart = 0;
                        overlay->buttonState.minusDragActive.exchange(false, std::memory_order_acq_rel);
                        overlay->buttonState.plusDragActive.exchange(false, std::memory_order_acq_rel);
                    }

                    // Disable rendering on any input, re-enable when no input
                    static bool resetOnce = true;
                    if (inputDetected) {
                        if (resetOnce && isRendering) {
                            isRendering = false;
                            leventSignal(&renderingStopEvent);
                            resetOnce = false;
                        }
                    } else {
                        resetOnce = true;
                    }
                }

                if (ult::limitedMemory) {
                    static auto lastUnderscanPixels = std::make_pair(0, 0);

                    if (lastUnderscanPixels != tsl::impl::currentUnderscanPixels) {
                        for (int i = 0; i < 2; i++) {
                            tsl::gfx::Renderer::get().updateLayerSize();
                            tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(overlay->frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
                        }
                    }
                    lastUnderscanPixels = tsl::impl::currentUnderscanPixels;
                }

                svcSleepThread(32000000ULL); // 32ms polling
            }
        }, this, NULL, 0x1000, 0x2B, -2);
        threadStart(&touchPollThread);
    }
    ~MiniOverlay() {
        // Stop touch polling thread
        touchPollRunning.store(false, std::memory_order_release);
        threadWaitForExit(&touchPollThread);
        threadClose(&touchPollThread);

        CloseThreads();
        FullMode = true;
        fixForeground = true;
        //tsl::hlp::requestForeground(true);
        //alphabackground = 0xD;
        deactivateOriginalFooter = false;
    }

    resolutionCalls m_resolutionRenderCalls[8] = {0};
    resolutionCalls m_resolutionViewportCalls[8] = {0};
    resolutionCalls m_resolutionOutput[8] = {0};
    uint8_t resolutionLookup = 0;
    bool resolutionShow = false;

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            //static constexpr u16 frameWidth = 448;

            // Cache parsed settings and calculations
            static std::vector<std::string> showKeys;
            static std::string lastShowSetting;
            static bool needsRecalc = true;
            static std::vector<std::string> labelLines; // Store individual label lines
            static std::string lastVariables; // Track changes in Variables content
            static size_t entryCount = 0;
            static uint32_t cachedHeight = 0;
            static int cachedBaseX = 0, cachedBaseY = 0;
            static bool lastGameRunning = false; // Track game state changes

            // Check if we need to recalculate due to content changes
            const bool contentChanged = (std::string(Variables) != lastVariables) ||
                                (GameRunning != lastGameRunning);

            // Only recalculate if settings changed or content changed
            if (settings.show != lastShowSetting || !Initialized || contentChanged) {
                showKeys.clear();

                // Parse once and cache
                size_t start = 0, end = 0;
                const std::string& show = settings.show;
                while ((end = show.find('+', start)) != std::string::npos) {
                    showKeys.emplace_back(show.substr(start, end - start));
                    start = end + 1;
                }
                if (start < show.length()) {
                    showKeys.emplace_back(show.substr(start));
                }

                lastShowSetting = settings.show;
                lastVariables = Variables;
                lastGameRunning = GameRunning;
                needsRecalc = true;
            }

            // Initial width calculation (only once)
            if (!Initialized) {

                rectangleWidth = 0;
                //std::pair<u32, u32> dimensions;
                u32 width;

                for (const auto& key : showKeys) {
                    if (key == "CPU") {
                        //dimensions = renderer->drawString("[100%,100%,100%,100%]@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));
                        if (!settings.realVolts) {
                            if (settings.showFullCPU)
                                width = renderer->getTextDimensions("[100%,100%,100%,100%]@4444.4", false, fontsize).first;
                            else
                                width = renderer->getTextDimensions("100%@4444.4", false, fontsize).first;
                        } else {
                            if (settings.showFullCPU)
                                width = renderer->getTextDimensions("[100%,100%,100%,100%]@4444.4444 mV", false, fontsize).first;
                            else
                                width = renderer->getTextDimensions("100%@4444.4444 mV", false, fontsize).first;
                        }
						    if (settings.realTemps) {
								width += renderer->getTextDimensions(" 88.8°C ", false, fontsize).first;
							}
                    } else if (key == "RAM" && settings.ramInfoMode == "Bandwidth" && R_SUCCEEDED(hocclkCheck)) {
                        width = renderer->getTextDimensions("99.9GB/s@4444.4", false, fontsize).first;
                        if (settings.realVolts) {
                            if (isMariko) {
                                if (settings.showVDD2 && settings.decimalVDD2 && settings.showVDDQ)
                                    width += renderer->getTextDimensions("4444.4444 mV  4444 mV", false, fontsize).first;
                                else if (settings.showVDD2 && !settings.decimalVDD2 && settings.showVDDQ)
                                    width += renderer->getTextDimensions("4444 mV  4444 mV", false, fontsize).first;
                                else if (settings.showVDD2 && settings.decimalVDD2)
                                    width += renderer->getTextDimensions("4444.4 mV", false, fontsize).first;
                                else if (settings.showVDD2 && !settings.decimalVDD2)
                                    width += renderer->getTextDimensions("4444 mV", false, fontsize).first;
                                else if (settings.showVDDQ)
                                    width += renderer->getTextDimensions("4444 mV", false, fontsize).first;
                            } else {
                                if (settings.decimalVDD2)
                                    width += renderer->getTextDimensions("4444.44 mV", false, fontsize).first;
                                else
                                    width += renderer->getTextDimensions("4444 mV", false, fontsize).first;
                            }
                        }
                        if (settings.realTemps) {
                            width += renderer->getTextDimensions("   88.8°C   ", false, fontsize).first;
                        }
                    } else if (key == "GPU" || (key == "RAM" && settings.showpartLoad && R_SUCCEEDED(hocclkCheck))) {
                        //dimensions = renderer->drawString("100.0%@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));

                        if (!settings.showpartLoadCPUGPU) {
                            if (!settings.realVolts) {
                                width = renderer->getTextDimensions("100%@4444.4", false, fontsize).first;
                            } else {
                                width = renderer->getTextDimensions("100%@4444.4444 mV", false, fontsize).first;
                            }
                        } else {
                            if (!settings.realVolts) {
                                width = renderer->getTextDimensions("100%[100%,100%]@4444.4", false, fontsize).first;
                            } else {
                                width = renderer->getTextDimensions("100%[100%,100%]@4444.4444 mV", false, fontsize).first;
                            }
                        }
                        if (settings.realTemps) {
                            width += renderer->getTextDimensions("   88.8°C   ", false, fontsize).first;
                        }
                    } else if (key == "RAM" && (!settings.showpartLoad || R_FAILED(hocclkCheck))) {
                        //dimensions = renderer->drawString("44444444MB@4444.4", false, 0, 0, fontsize, renderer->a(0x0000));
                        if (!settings.realVolts) {
                            width = renderer->getTextDimensions("100%@4444.4", false, fontsize).first;
						if (settings.realTemps) {
							width += renderer->getTextDimensions("  88.8°C  ", false, fontsize).first;
							}
                        } else {
                            if (isMariko) {
                                if (settings.showVDD2 && settings.decimalVDD2 && settings.showVDDQ)
                                    width = renderer->getTextDimensions("100%@4444.44444.4444 mV", false, fontsize).first;
                                else if (settings.showVDD2 && !settings.decimalVDD2 && settings.showVDDQ)
                                    width = renderer->getTextDimensions("100%@4444.44444444 mV", false, fontsize).first;
                                else if (settings.showVDD2 && settings.decimalVDD2)
                                    width = renderer->getTextDimensions("100%@4444.44444.4 mV", false, fontsize).first;
                                else if (settings.showVDD2 && !settings.decimalVDD2)
                                    width = renderer->getTextDimensions("100%@4444.44444 mV", false, fontsize).first;
                                else if (settings.showVDDQ)
                                    width = renderer->getTextDimensions("100%@4444.4444 mV", false, fontsize).first;
                            } else {
                                if (settings.decimalVDD2)
                                    width = renderer->getTextDimensions("100%@4444.44444.4 mV", false, fontsize).first;
                                else
                                    width = renderer->getTextDimensions("100%@4444.44444 mV", false, fontsize).first;
                            }
                        }
                    } else if (key == "SOC") {                // new block
                        if (!settings.realVolts)
                            if (settings.showFanPercentage)
                                width = renderer->getTextDimensions("88°C (100%)", false, fontsize).first;
                            else
                                width = renderer->getTextDimensions("88°C", false, fontsize).first;
                        else
                            if (settings.showSOCVoltage) {
                                if (settings.showFanPercentage)
                                    width = renderer->getTextDimensions("88°C 100%444 mV", false, fontsize).first;
                                else
                                    width = renderer->getTextDimensions("88°C444 mV", false, fontsize).first;
                            } else {
                                if (settings.showFanPercentage)
                                    width = renderer->getTextDimensions("88°C 100%", false, fontsize).first;
                                else
                                    width = renderer->getTextDimensions("88°C", false, fontsize).first;
                            }
                    } else if (key == "TMP") {
                        //dimensions = renderer->drawString("88.8\u00B0C88.8\u00B0C88.8\u00B0C (100%)", false, 0, 0, fontsize, renderer->a(0x0000));
                        if (!settings.realVolts) {
                            if (settings.showFanPercentage)
                                width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C 100%", false, fontsize).first;
                            else
                                width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C", false, fontsize).first;
                        } else {
                            if (settings.showSOCVoltage) {
                                if (settings.showFanPercentage)
                                    width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C 100%444 mV", false, fontsize).first;
                                else
                                    width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C444 mV", false, fontsize).first;
                            } else {
                                if (settings.showFanPercentage)
                                    width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C 100%", false, fontsize).first;
                                else
                                    width = renderer->getTextDimensions("88\u00B0C 88\u00B0C 88\u00B0C", false, fontsize).first;
                            }
                        }
                    } else if (key == "BAT") {
                        //dimensions = renderer->drawString("-44.44 W100.0% [44:44]", false, 0, 0, fontsize, renderer->a(0x0000));
                        width = renderer->getTextDimensions("-44.44 W100.0% [44:44]", false, fontsize).first;
                    } else if (key == "FPS") {
                        //dimensions = renderer->drawString("444.4", false, 0, 0, fontsize, renderer->a(0x0000));
                        width = renderer->getTextDimensions("444.4", false, fontsize).first;
                    } else if (key == "RES") {
                        //dimensions = renderer->drawString("3840x21603840x2160", false, 0, 0, fontsize, renderer->a(0x0000));
                        if (settings.showFullResolution)
                            width = renderer->getTextDimensions("3840x21603840x2160", false, fontsize).first;
                        else
                            width = renderer->getTextDimensions("2160p2160p", false, fontsize).first;
                    } else if (key == "READ") {
                        // Calculate width for read speed display
                        width = renderer->getTextDimensions("999.99 MiB/s", false, fontsize).first;
                    } else if (key == "MEM") {
                        // Calculate width for system memory display (System + divider + memory value)
                        const u32 memValueWidth = renderer->getTextDimensions("999.9 MB "+ult::FREE, false, fontsize).first;
                        const u32 dividerWidth = renderer->getTextDimensions(ult::DIVIDER_SYMBOL, false, fontsize).first;
                        const u32 systemWidth = renderer->getTextDimensions("System", false, fontsize).first;
                        width = memValueWidth + dividerWidth + systemWidth;
                    } else if (key == "DTC" && settings.showDTC) {
                        // Calculate width based on the datetime format
                        // Use multiple sample dates to ensure longest possible textual output

                        char sampleDateTime[64];
                        size_t maxWidth = 0;

                        // Representative dates that produce long names in most locales
                        constexpr int testDays[][3] = {
                            {2025, 11, 26}, // Wednesday – longest weekday
                            {2025, 9, 30},  // September – long month
                            {2025, 12, 31}, // December – another long month
                            {2025, 2, 28},  // February – to cover locales where it's long
                        };

                        struct tm t = {};
                        for (auto &d : testDays) {

                            t.tm_year = d[0] - 1900;
                            t.tm_mon  = d[1] - 1;
                            t.tm_mday = d[2];
                            t.tm_hour = 23;
                            t.tm_min  = 59;
                            t.tm_sec  = 59;
                            mktime(&t); // normalize (sets weekday, etc.)

                            strftime(sampleDateTime, sizeof(sampleDateTime), settings.dtcFormat.c_str(), &t);
                            const size_t w = renderer->getTextDimensions(std::string(sampleDateTime) + "  ", false, fontsize).first;
                            if (w > maxWidth)
                                maxWidth = w;
                        }

                        width = maxWidth;

                    } else {
                        continue;
                    }

                    if (rectangleWidth < width) {
                        rectangleWidth = width;
                    }
                }
                Initialized = true;
                needsRecalc = true;
            }

            // Recalculate layout when needed (including content changes)
            if (needsRecalc) {
                // Build label lines array for individual centering
                labelLines.clear();
                entryCount = 0;
                uint16_t flags = 0;

                bool shouldAdd;
                std::string labelText;
                for (const auto& key : showKeys) {
                    shouldAdd = false;
                    labelText = "";

                    if (key == "CPU" && !(flags & 1)) {
                        shouldAdd = true;
                        labelText = "CPU";
                        flags |= 1;
                    } else if (key == "GPU" && !(flags & 2)) {
                        shouldAdd = true;
                        labelText = "GPU";
                        flags |= 2;
                    } else if (key == "RAM" && !(flags & 4)) {
                        shouldAdd = true;
                        labelText = "RAM";
                        flags |= 4;
                    } else if (key == "SOC" && !(flags & 8)) {
                        shouldAdd = true;
                        labelText = "SOC";
                        flags |= 8;
                    } else if (key == "TMP" && !(flags & 16)) {
                        shouldAdd = true;
                        labelText = "TMP";
                        flags |= 16;
                    } else if ((key == "BAT" || key == "DRAW") && !(flags & 32)) {
                        shouldAdd = true;
                        labelText = "BAT";
                        flags |= 32;
                    } else if (key == "FPS" && !(flags & 64) && GameRunning) {
                        shouldAdd = true;
                        labelText = "FPS";
                        flags |= 64;
                    } else if (key == "RES" && !(flags & 128) && GameRunning) {
                        shouldAdd = true;
                        labelText = "RES";
                        flags |= 128;
                        resolutionShow = true;
                    } else if (key == "READ" && !(flags & 512) && GameRunning) {
                        shouldAdd = true;
                        labelText = "READ";
                        flags |= 512;
                    } else if (key == "DTC" && !(flags & 256) && settings.showDTC) {
                        shouldAdd = true;
                        labelText = settings.useDTCSymbol ? "\uE007" : "DTC";
                        flags |= 256;
                    } else if (key == "MEM" && !(flags & 1024)) {
                        shouldAdd = true;
                        labelText = "MEM";
                        flags |= 1024;
                    }


                    if (shouldAdd) {
                        labelLines.push_back(labelText);
                        entryCount++;

                        //if (settings.realVolts && key != "BAT" && key != "DRAW" && key != "FPS" && key != "RES") {
                        //    labelLines.push_back(""); // Empty line for voltage info
                        //    entryCount++;
                        //}
                    }
                }

                // Calculate actual entry count from Variables string
                size_t actualEntryCount = 1; // Start with 1 for the first line
                for (size_t i = 0; Variables[i] != '\0'; i++) {
                    if (Variables[i] == '\n') {
                        actualEntryCount++;
                    }
                }

                // Use the actual entry count for height calculation
                cachedHeight = ((fontsize + settings.spacing) * actualEntryCount) + (fontsize / 3) + settings.spacing + topPadding + bottomPadding;
                //const uint32_t margin = (fontsize * 4);

                cachedBaseX = 0;
                cachedBaseY = 0;

                needsRecalc = false;
            }

            // Fast rendering using cached values
            const uint32_t margin = (fontsize * 4);

            // Draw background
            const tsl::Color bgColor = !isDragging
                ? settings.backgroundColor // full opacity
                : settings.focusBackgroundColor;

            int clippingOffsetX = 0, clippingOffsetY = 0;

            int _frameOffsetX = ult::limitedMemory ? std::max(0, frameOffsetX - (1280-448)) : frameOffsetX;

            // Check X bounds and calculate clipping offset
            if (cachedBaseX + _frameOffsetX < int(framePadding)) {
                clippingOffsetX = framePadding - (cachedBaseX + _frameOffsetX);
            } else if ((cachedBaseX + _frameOffsetX + margin + rectangleWidth + (fontsize / 3)) > screenWidth - framePadding) {
                clippingOffsetX = (screenWidth - framePadding) - (cachedBaseX + _frameOffsetX + margin + rectangleWidth + (fontsize / 3));
            }

            // Check Y bounds and calculate clipping offset
            if (cachedBaseY + frameOffsetY < int(framePadding)) {
                clippingOffsetY = framePadding - (cachedBaseY + frameOffsetY);
            } else if ((cachedBaseY + frameOffsetY + cachedHeight) > screenHeight - framePadding) {
                clippingOffsetY = (screenHeight - framePadding) - (cachedBaseY + frameOffsetY + cachedHeight);
            }

            // Apply to all drawing calls
            renderer->drawRoundedRectSingleThreaded(
                cachedBaseX + _frameOffsetX + clippingOffsetX,
                cachedBaseY + frameOffsetY + clippingOffsetY,
                margin + rectangleWidth + (fontsize / 3),
                cachedHeight,
                16,
                a(bgColor)
            );


            // Split Variables into lines for individual positioning
            std::vector<std::string> variableLines;
            const std::string variablesStr(Variables);
            size_t start = 0;
            size_t pos = 0;
            while ((pos = variablesStr.find('\n', start)) != std::string::npos) {
                variableLines.push_back(variablesStr.substr(start, pos - start));
                start = pos + 1;
            }
            if (start < variablesStr.length()) {
                variableLines.push_back(variablesStr.substr(start));
            }

            // Draw each label and variable line individually
            uint32_t currentY = cachedBaseY + fontsize + settings.spacing + topPadding;
            size_t labelIndex = 0;

            static const std::vector<std::string> specialChars = {""};
            static uint32_t labelWidth, labelCenterX;
            static std::vector<std::string> _variableLines;
            if (!delayUpdate)
                _variableLines = variableLines;
            for (size_t i = 0; i < _variableLines.size() && labelIndex < labelLines.size(); i++) {
                // Check for RES label without corresponding RES data
                if (labelIndex < labelLines.size() && labelLines[labelIndex] == "RES") {
                    //const std::string& currentLine = _variableLines[i];
                    // Check if this data line looks like resolution data (contains 'x' or 'p')
                    bool isResolutionData = m_resolutionOutput[0].width;

                    if (!isResolutionData) {
                        // This is RES label but data isn't resolution data - skip the label
                        ++labelIndex;
                        // Don't increment i, so the current data line gets paired with the next label
                        --i;
                        continue;
                    }
                }

                // Draw label (centered in label region)
                if (!labelLines[labelIndex].empty()) {
                    labelWidth = renderer->getTextDimensions(labelLines[labelIndex], false, fontsize).first;
                    labelCenterX = cachedBaseX + (margin / 2) - (labelWidth / 2);
                    renderer->drawString(labelLines[labelIndex], false, labelCenterX + _frameOffsetX + clippingOffsetX, currentY + frameOffsetY + clippingOffsetY, fontsize, settings.catColor);
                }

                // Determine rendering method based on label type
                const std::string& currentLine = _variableLines[i];
                const int baseX = cachedBaseX + margin + _frameOffsetX + clippingOffsetX;
                const int baseY = currentY + frameOffsetY + clippingOffsetY;

                if (settings.useDynamicColors) {
					 if (labelIndex < labelLines.size() && labelLines[labelIndex] == "CPU") {
						std::string dataStr = currentLine;

						const size_t degreesPos = dataStr.find("°");
						if (degreesPos != std::string::npos && settings.realTemps && realCPU_Temp != 0) {
							size_t tempStart = dataStr.rfind(' ', degreesPos);
							if (tempStart != std::string::npos) {
							tempStart++;
							const size_t cPos = dataStr.find("C", degreesPos);
							if (cPos != std::string::npos) {
								const size_t tempEnd = cPos + 1;

								const std::string preTempPart = dataStr.substr(0, tempStart);
								const std::string tempPart = dataStr.substr(tempStart, tempEnd - tempStart);
								const std::string postTempPart = dataStr.substr(tempEnd);

								const float temp = realCPU_Temp / 1000.0f;
								const tsl::Color tempColor = tsl::GradientColor(temp);

								int currentX = baseX;
								if (!preTempPart.empty()) {
									renderer->drawStringWithColoredSections(preTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
									currentX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
								}

								renderer->drawStringWithColoredSections(tempPart, false, specialChars, currentX, baseY, fontsize, tempColor, settings.separatorColor);

								if (!postTempPart.empty()) {
									currentX += renderer->getTextDimensions(tempPart, false, fontsize).first;
									renderer->drawStringWithColoredSections(postTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
								}
							} else {
								renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
							}
						} else {
							renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
						}
					} else {
						renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
					}

				} else if (labelIndex < labelLines.size() && labelLines[labelIndex] == "GPU") {
				std::string dataStr = currentLine;

				const size_t degreesPos = dataStr.find("°");
				if (degreesPos != std::string::npos && settings.realTemps && realGPU_Temp != 0) {
					size_t tempStart = dataStr.rfind(' ', degreesPos);
					if (tempStart != std::string::npos) {
						tempStart++;
						const size_t cPos = dataStr.find("C", degreesPos);
						if (cPos != std::string::npos) {
							const size_t tempEnd = cPos + 1;

							const std::string preTempPart = dataStr.substr(0, tempStart);
							const std::string tempPart = dataStr.substr(tempStart, tempEnd - tempStart);
							const std::string postTempPart = dataStr.substr(tempEnd);

							const float temp = realGPU_Temp / 1000.0f;
							const tsl::Color tempColor = tsl::GradientColor(temp);

							int currentX = baseX;
							if (!preTempPart.empty()) {
								renderer->drawStringWithColoredSections(preTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
								currentX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
							}

							renderer->drawStringWithColoredSections(tempPart, false, specialChars, currentX, baseY, fontsize, tempColor, settings.separatorColor);

							if (!postTempPart.empty()) {
								currentX += renderer->getTextDimensions(tempPart, false, fontsize).first;
								renderer->drawStringWithColoredSections(postTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
							}
						} else {
							renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
						}
					} else {
						renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
					}
				} else {
					renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
				}

			} else if (labelIndex < labelLines.size() && labelLines[labelIndex] == "RAM") {
				std::string dataStr = currentLine;

				const size_t degreesPos = dataStr.find("°");
				if (degreesPos != std::string::npos && settings.realTemps && realRAM_Temp != 0) {
					size_t tempStart = dataStr.rfind(' ', degreesPos);
					if (tempStart != std::string::npos) {
						tempStart++;
						const size_t cPos = dataStr.find("C", degreesPos);
						if (cPos != std::string::npos) {
							const size_t tempEnd = cPos + 1;

							const std::string preTempPart = dataStr.substr(0, tempStart);
							const std::string tempPart = dataStr.substr(tempStart, tempEnd - tempStart);
							const std::string postTempPart = dataStr.substr(tempEnd);

							const float temp = realRAM_Temp / 1000.0f;
							const tsl::Color tempColor = tsl::GradientColor(temp);

							int currentX = baseX;
							if (!preTempPart.empty()) {
								renderer->drawStringWithColoredSections(preTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
								currentX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
							}

							renderer->drawStringWithColoredSections(tempPart, false, specialChars, currentX, baseY, fontsize, tempColor, settings.separatorColor);

							if (!postTempPart.empty()) {
								currentX += renderer->getTextDimensions(tempPart, false, fontsize).first;
								renderer->drawStringWithColoredSections(postTempPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
							}
						} else {
							renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
						}
					} else {
						renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
					}
				} else {
					renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
				}

			} else if (labelIndex < labelLines.size() && labelLines[labelIndex] == "SOC") {
                        // SOC temperature rendering with gradient
                        const size_t degreesPos = currentLine.find("°");
                        if (degreesPos != std::string::npos) {
                            // Find the 'C' after the degrees symbol
                            const size_t cPos = currentLine.find("C", degreesPos);
                            if (cPos != std::string::npos) {
                                const size_t tempEnd = cPos + 1; // Include the 'C'

                                // Extract temperature value and apply gradient
                                const int temp = atoi(currentLine.c_str());
                                const tsl::Color tempColor = tsl::GradientColor((float)temp);

                                // Split into temperature part and remaining part
                                const std::string tempPart = currentLine.substr(0, tempEnd);
                                const std::string restPart = currentLine.substr(tempEnd);

                                // Render temperature with gradient color
                                int currentX = baseX;
                                renderer->drawString(tempPart, false, currentX, baseY, fontsize, tempColor);

                                // Render remaining text with normal color
                                if (!restPart.empty()) {
                                    currentX += renderer->getTextDimensions(tempPart, false, fontsize).first;
                                    renderer->drawStringWithColoredSections(restPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
                                }
                            } else {
                                // Fallback: no C found after degrees, render normally
                                renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
                            }
                        } else {
                            // Fallback: no degrees symbol found, render normally
                            renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
                        }

                    } else if (labelIndex < labelLines.size() && labelLines[labelIndex] == "TMP") {
                        // TMP multiple temperatures rendering with gradients
                        int currentX = baseX;
                        size_t pos = 0;
                        bool parseSuccess = true;

                        // Parse up to 3 temperatures in the format "XX°C XX°C XX°C (XX%)"
                        for (int tempCount = 0; tempCount < 3 && parseSuccess && pos < currentLine.length(); tempCount++) {
                            // Find start of current temperature (skip any leading spaces)
                            while (pos < currentLine.length() && currentLine[pos] == ' ') {
                                renderer->drawString(" ", false, currentX, baseY, fontsize, settings.textColor);
                                currentX += renderer->getTextDimensions(" ", false, fontsize).first;
                                pos++;
                            }

                            if (pos >= currentLine.length()) break;

                            // Find degrees symbol
                            const size_t degreesPos = currentLine.find("°", pos);
                            if (degreesPos == std::string::npos) {
                                parseSuccess = false;
                                break;
                            }

                            // Find 'C' after degrees symbol
                            const size_t cPos = currentLine.find("C", degreesPos);
                            if (cPos == std::string::npos) {
                                parseSuccess = false;
                                break;
                            }

                            const size_t tempEnd = cPos + 1; // Include the 'C'

                            // Extract and render temperature with gradient
                            const std::string tempPart = currentLine.substr(pos, tempEnd - pos);
                            const int temp = atoi(tempPart.c_str());
                            const tsl::Color tempColor = tsl::GradientColor((float)temp);

                            renderer->drawString(tempPart, false, currentX, baseY, fontsize, tempColor);
                            currentX += renderer->getTextDimensions(tempPart, false, fontsize).first;

                            pos = tempEnd;
                        }

                        // Render any remaining text (like " (50%)" or voltage info)
                        if (pos < currentLine.length()) {
                            std::string restPart = currentLine.substr(pos);
                            renderer->drawStringWithColoredSections(restPart, false, specialChars, currentX, baseY, fontsize, settings.textColor, settings.separatorColor);
                        }

                        // If parsing failed, fall back to normal rendering
                        if (!parseSuccess) {
                            renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
                        }
                    } else if (labelIndex < labelLines.size() && labelLines[labelIndex] == "MEM") {
                        // MEM memory rendering with gradient color
                        // Extract numeric value for color determination
                        float sysValue = 0.0f;
                        sscanf(currentLine.c_str(), "%f", &sysValue);

                        // Determine unit from string
                        const bool isGB = (currentLine.find("GB") != std::string::npos);
                        const float sysValueMB = isGB ? (sysValue * 1024.0f) : sysValue;

                        // Apply same color scheme as in drawMemoryWidget
                        tsl::Color sysColor = settings.textColor;
                        if (sysValueMB >= 9.0f) {
                            sysColor = settings.useDynamicColors ? tsl::healthyRamTextColor : settings.textColor;
                        } else if (sysValueMB >= 3.0f) {
                            sysColor = settings.useDynamicColors ? tsl::neutralRamTextColor : settings.textColor;
                        } else {
                            sysColor = settings.useDynamicColors ? tsl::badRamTextColor : settings.textColor;
                        }

                        // Draw the memory value with color on the left
                        int currentX = baseX;
                        renderer->drawStringWithColoredSections(currentLine, false, specialChars, currentX, baseY, fontsize, sysColor, settings.separatorColor);
                        currentX += renderer->getTextDimensions(currentLine, false, fontsize).first;

                        // Draw separator " | "
                        renderer->drawString(ult::DIVIDER_SYMBOL, false, currentX, baseY, fontsize, settings.separatorColor);
                        currentX += renderer->getTextDimensions(ult::DIVIDER_SYMBOL, false, fontsize).first;

                        // Draw "System" on the right
                        renderer->drawString("System", false, currentX, baseY, fontsize, settings.textColor);
                    } else {
                        // Normal rendering for all other line types (CPU, GPU, RAM, BAT, FPS, RES, DTC)
                        renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
                    }
                } else {
                    // Normal rendering for all other line types (CPU, GPU, RAM, BAT, FPS, RES, DTC)
                    renderer->drawStringWithColoredSections(currentLine, false, specialChars, baseX, baseY, fontsize, settings.textColor, settings.separatorColor);
                }

                currentY += fontsize + settings.spacing;
                ++labelIndex;
            }
        });

        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("", "");
        rootFrame->setContent(Status);
        return rootFrame;
    }

    virtual void update() override {
        if (triggerExitNow)
            return;

        if (!SaltySD) {
            SaltySD = CheckPort();
            if (SaltySD) {
                LoadSharedMemory();
                threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
                threadStart(&t6);
            }
        }

        // Handle performance mode changes
        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            if (fontsize != settings.handheldFontSize) {
                Initialized = false;
                fontsize = settings.handheldFontSize;
            }
        }
        else if (performanceMode == ApmPerformanceMode_Boost) {
            if (fontsize != settings.dockedFontSize) {
                Initialized = false;
                fontsize = settings.dockedFontSize;
            }
        }

        // Parse show settings to determine what to calculate
        std::vector<std::string> showKeys;
        {
            size_t start = 0, end = 0;
            const std::string& show = settings.show;
            while ((end = show.find('+', start)) != std::string::npos) {
                showKeys.emplace_back(show.substr(start, end - start));
                start = end + 1;
            }
            if (start < show.length()) {
                showKeys.emplace_back(show.substr(start));
            }
        }

        // Helper to check if a key is active
        static auto isActive = [&showKeys](const std::string& key) {
            return std::find(showKeys.begin(), showKeys.end(), key) != showKeys.end();
        };

        mutexLock(&mutex_Misc);

        // Variables to store formatted strings
        char MINI_CPU_compressed_c[42] = "";
        char MINI_CPU_volt_c[16] = "";
        char MINI_GPU_Load_c[32] = "";
        char MINI_GPU_volt_c[20] = "";
        char MINI_RAM_var_compressed_c[35] = "";
        char MINI_RAM_volt_c[32] = "";
        char MINI_MEM_RAM_c[32] = "";
        char MINI_SOC_volt_c[16] = "";

        // Only process CPU if needed
        if (isActive("CPU")) {
            char MINI_CPU_Usage0[7], MINI_CPU_Usage1[7], MINI_CPU_Usage2[7], MINI_CPU_Usage3[7];

            const uint64_t idle0 = idletick0.load(std::memory_order_acquire);
            const uint64_t idle1 = idletick1.load(std::memory_order_acquire);
            const uint64_t idle2 = idletick2.load(std::memory_order_acquire);
            const uint64_t idle3 = idletick3.load(std::memory_order_acquire);

            auto formatMiniUsage = [this](char* buf, size_t size, uint64_t idletick) {
                if (idletick > systemtickfrequency_impl) {
                    strcpy(buf, "0%");
                } else {
                    snprintf(buf, size, "%.0f%%",
                             (1.0 - (static_cast<double>(idletick) / systemtickfrequency_impl)) * 100.0);
                }
            };

            formatMiniUsage(MINI_CPU_Usage0, sizeof(MINI_CPU_Usage0), idle0);
            formatMiniUsage(MINI_CPU_Usage1, sizeof(MINI_CPU_Usage1), idle1);
            formatMiniUsage(MINI_CPU_Usage2, sizeof(MINI_CPU_Usage2), idle2);
            formatMiniUsage(MINI_CPU_Usage3, sizeof(MINI_CPU_Usage3), idle3);

            if (settings.showFullCPU) {
                if (settings.realFrequencies && realCPU_Hz) {
                    snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c),
                        "[%s,%s,%s,%s]@%hu.%hhu",
                        MINI_CPU_Usage0, MINI_CPU_Usage1, MINI_CPU_Usage2, MINI_CPU_Usage3,
                        realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
                } else {
                    snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c),
                        "[%s,%s,%s,%s]@%hu.%hhu",
                        MINI_CPU_Usage0, MINI_CPU_Usage1, MINI_CPU_Usage2, MINI_CPU_Usage3,
                        CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
                }
            } else {
                double usage0 = 0, usage1 = 0, usage2 = 0, usage3 = 0;
                sscanf(MINI_CPU_Usage0, "%lf%%", &usage0);
                sscanf(MINI_CPU_Usage1, "%lf%%", &usage1);
                sscanf(MINI_CPU_Usage2, "%lf%%", &usage2);
                sscanf(MINI_CPU_Usage3, "%lf%%", &usage3);

                double maxUsage = std::max({usage0, usage1, usage2, usage3});

                if (settings.realFrequencies && realCPU_Hz) {
                    snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c),
                        "%.0f%%@%hu.%hhu", maxUsage,
                        realCPU_Hz / 1000000, (realCPU_Hz / 100000) % 10);
                } else {
                    snprintf(MINI_CPU_compressed_c, sizeof(MINI_CPU_compressed_c),
                        "%.0f%%@%hu.%hhu", maxUsage,
                        CPU_Hz / 1000000, (CPU_Hz / 100000) % 10);
                }
            }

            if (settings.realVolts) {
                const uint32_t mv = realCPU_mV / 1000;
                snprintf(MINI_CPU_volt_c, sizeof(MINI_CPU_volt_c), "%u mV", mv);
            }
        }

		if (settings.realTemps && realCPU_Temp != 0 && CPU_temp_c[0] != '\0') {
                char temp_buffer[48];
                snprintf(temp_buffer, sizeof(temp_buffer), " %s", CPU_temp_c);
                strncat(MINI_CPU_compressed_c, temp_buffer, sizeof(MINI_CPU_compressed_c) - strlen(MINI_CPU_compressed_c) - 1);
            }

        // Only process GPU if needed
        if (isActive("GPU")) {
            if (settings.realFrequencies && realGPU_Hz) {
                snprintf(MINI_GPU_Load_c, sizeof(MINI_GPU_Load_c),
                         "%hu%%%s%hu.%hhu",
                         GPU_Load_u / 10, "@",
                         realGPU_Hz / 1000000, (realGPU_Hz / 100000) % 10);
            } else {
                snprintf(MINI_GPU_Load_c, sizeof(MINI_GPU_Load_c),
                         "%hu%%%s%hu.%hhu",
                         GPU_Load_u / 10, "@",
                         GPU_Hz / 1000000, (GPU_Hz / 100000) % 10);
            }

            // Handle sleep exit
            if (settings.sleepExit) {
                const auto GPU_Hz_int = int(GPU_Hz / 1000000);
                static auto lastGPU_Hz_int = GPU_Hz_int;
                if (GPU_Hz_int == 0 && lastGPU_Hz_int != 0) {
                    isRendering = false;
                    leventSignal(&renderingStopEvent);
                    triggerExitNow = true;
                    mutexUnlock(&mutex_Misc);
                    return;
                }
                lastGPU_Hz_int = GPU_Hz_int;
            }

            if (settings.realVolts) {
                const uint32_t mv = realGPU_mV / 1000;
                snprintf(MINI_GPU_volt_c, sizeof(MINI_GPU_volt_c), "%u mV", mv);
            }
        }

        if (settings.realTemps && realGPU_Temp != 0 && GPU_temp_c[0] != '\0') {
            char temp_buffer[48];
            snprintf(temp_buffer, sizeof(temp_buffer), " %s", GPU_temp_c);
            strncat(MINI_GPU_Load_c, temp_buffer, sizeof(MINI_GPU_Load_c) - strlen(MINI_GPU_Load_c) - 1);
        }

        // Only process RAM if needed
        if (isActive("RAM")) {
            if (!settings.showpartLoad) {
                const float ramTotalGiB = (RAM_Total_application_u + RAM_Total_applet_u +
                                     RAM_Total_system_u + RAM_Total_systemunsafe_u) /
                                    (1024.0f * 1024.0f);
                const float ramUsedGiB  = (RAM_Used_application_u + RAM_Used_applet_u +
                                     RAM_Used_system_u + RAM_Used_systemunsafe_u) /
                                    (1024.0f * 1024.0f);

                if (settings.realFrequencies && realRAM_Hz) {
                    snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                             "%.0f/%.0fMB@%hu.%hhu",
                             ramUsedGiB, ramTotalGiB,
                             realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
                } else {
                    snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                             "%.0f/%.0fMB@%hu.%hhu",
                             ramUsedGiB, ramTotalGiB,
                             RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
                }
            } else {
                if (settings.ramInfoMode == "Bandwidth" && R_SUCCEEDED(hocclkCheck)) {
                    // Bandwidth mode: show GB/s from context (partLoad values are in MB/s)
                    const uint32_t ramFreq = settings.realFrequencies && realRAM_Hz ? realRAM_Hz : RAM_Hz;
                    const unsigned bwAll  = partLoad[HocClkPartLoad_RamBWAll]  / 1000;
                    const unsigned bwAllD = (partLoad[HocClkPartLoad_RamBWAll]  % 1000) / 100;
                    snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                             "%u.%uGB/s@%hu.%hhu",
                             bwAll, bwAllD,
                             ramFreq / 1000000, (ramFreq / 100000) % 10);
                } else {
                    unsigned partLoadInt;

                    if (R_SUCCEEDED(hocclkCheck)) {
                        partLoadInt = partLoad[HocClkPartLoad_EMC] / 10;

                        if (settings.showpartLoadCPUGPU) {
                            unsigned ramCpuLoadInt = partLoad[HocClkPartLoad_EMCCpu] / 10;
                            int RAM_GPU_Load = partLoad[HocClkPartLoad_EMC] - partLoad[HocClkPartLoad_EMCCpu];
                            unsigned ramGpuLoadInt = RAM_GPU_Load / 10;

                            if (settings.realFrequencies && realRAM_Hz) {
                                snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                         "%u%%[%u%%,%u%%]@%hu.%hhu",
                                         partLoadInt, ramCpuLoadInt, ramGpuLoadInt,
                                         realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
                            } else {
                                snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                         "%u%%[%u%%,%u%%]@%hu.%hhu",
                                         partLoadInt, ramCpuLoadInt, ramGpuLoadInt,
                                         RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
                            }
                        } else {
                            if (settings.realFrequencies && realRAM_Hz) {
                                snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                         "%u%%@%hu.%hhu", partLoadInt,
                                         realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
                            } else {
                                snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                         "%u%%@%hu.%hhu", partLoadInt,
                                         RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
                            }
                        }
                    } else {
                        const uint64_t RAM_Total_all = RAM_Total_application_u + RAM_Total_applet_u +
                                                       RAM_Total_system_u + RAM_Total_systemunsafe_u;
                        const uint64_t RAM_Used_all = RAM_Used_application_u + RAM_Used_applet_u +
                                                      RAM_Used_system_u + RAM_Used_systemunsafe_u;
                        partLoadInt = (RAM_Total_all > 0) ? (unsigned)((RAM_Used_all * 100) / RAM_Total_all) : 0;

                        if (settings.realFrequencies && realRAM_Hz) {
                            snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                     "%u%%@%hu.%hhu", partLoadInt,
                                     realRAM_Hz / 1000000, (realRAM_Hz / 100000) % 10);
                        } else {
                            snprintf(MINI_RAM_var_compressed_c, sizeof(MINI_RAM_var_compressed_c),
                                     "%u%%@%hu.%hhu", partLoadInt,
                                     RAM_Hz / 1000000, (RAM_Hz / 100000) % 10);
                        }
                    }
                }
            }

            if (settings.realVolts) {
                const float mv_vdd2_f = realVDD2_mV / 1000.0f;   // VDD2
                const uint32_t mv_vddq = realVDDQ_mV / 1000;   // VDDQ
                const uint32_t mv_vdd2_i = realVDD2_mV / 1000;

                if (isMariko) {
                    if (settings.showVDDQ && settings.showVDD2) {
                        if (settings.decimalVDD2)
                            snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c),
                                     "%.1f mV%u mV", mv_vdd2_f, mv_vddq);
                        else
                            snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c),
                                     "%u mV%u mV", mv_vdd2_i, mv_vddq);
                    } else if (settings.showVDDQ) {
                        snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c), "%u mV", mv_vddq);
                    } else if (settings.showVDD2) {
                        if (settings.decimalVDD2)
                            snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c), "%.1f mV", mv_vdd2_f);
                        else
                            snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c), "%u mV", mv_vdd2_i);
                    }
                } else {
                    if (settings.decimalVDD2)
                        snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c), "%.1f mV", mv_vdd2_f);
                    else
                        snprintf(MINI_RAM_volt_c, sizeof(MINI_RAM_volt_c), "%u mV", mv_vdd2_i);
                }
            }
        }

        if (settings.realTemps && realRAM_Temp != 0 && RAM_temp_c[0] != '\0') {
            char temp_buffer[48];
            snprintf(temp_buffer, sizeof(temp_buffer), " %s", RAM_temp_c);
            strncat(MINI_RAM_var_compressed_c, temp_buffer, sizeof(MINI_RAM_var_compressed_c) - strlen(MINI_RAM_var_compressed_c) - 1);
        }

        // Only process MEM if needed
        if (isActive("MEM")) {
            const u64 freeRamBytes = RAM_Total_system_u - RAM_Used_system_u;

            float value;
            const char* unit;

            if (freeRamBytes >= 1024ULL * 1024 * 1024) {
                value = static_cast<float>(freeRamBytes) / (1024.0f * 1024.0f * 1024.0f);
                unit = "GB";
            } else {
                value = static_cast<float>(freeRamBytes) / (1024.0f * 1024.0f);
                unit = "MB";
            }

            int decimalPlaces;
            if (value >= 1000.0f) {
                decimalPlaces = 0;
            } else if (value >= 100.0f) {
                decimalPlaces = 1;
            } else if (value >= 10.0f) {
                decimalPlaces = 2;
            } else {
                decimalPlaces = 3;
            }

            snprintf(MINI_MEM_RAM_c, sizeof(MINI_MEM_RAM_c), "%.*f %s %s",
                     decimalPlaces, value, unit, ult::FREE.c_str());
        }

        // Only process temperature/fan data if SOC or TMP is active
        if (isActive("TMP") || isActive("SOC")) {
            const int duty = safeFanDuty((int)Rotation_Duty);

            if (settings.realVolts && settings.showSOCVoltage) {
                const uint32_t mv = realSOC_mV / 1000;
                snprintf(MINI_SOC_volt_c, sizeof(MINI_SOC_volt_c), "%u mV", mv);
            }

            if (isActive("SOC")) {
                if (settings.showFanPercentage) {
                    snprintf(soc_temperature_c, sizeof(soc_temperature_c),
                             "%d°C %d%%", (int)SOC_temperatureF, duty);
                } else {
                    snprintf(soc_temperature_c, sizeof(soc_temperature_c),
                             "%d°C", (int)SOC_temperatureF);
                }
            }

            if (isActive("TMP")) {
                if (settings.showFanPercentage) {
                    snprintf(skin_temperature_c, sizeof(skin_temperature_c),
                        "%d\u00B0C %d\u00B0C %hu\u00B0C %d%%",
                        (int)SOC_temperatureF, (int)PCB_temperatureF,
                        skin_temperaturemiliC / 1000, duty);
                } else {
                    snprintf(skin_temperature_c, sizeof(skin_temperature_c),
                        "%d\u00B0C %d\u00B0C %hu\u00B0C",
                        (int)SOC_temperatureF, (int)PCB_temperatureF,
                        skin_temperaturemiliC / 1000);
                }
            }
        }

		if (settings.realTemps) {
            if (realCPU_Temp != 0) {
                snprintf(CPU_temp_c, sizeof(CPU_temp_c), "%u°C", static_cast<u32>(realCPU_Temp / 1000.0));
            }
            if (realGPU_Temp != 0) {
                snprintf(GPU_temp_c, sizeof(GPU_temp_c), "%u°C", static_cast<u32>(realGPU_Temp / 1000.0));
            }
            if (realRAM_Temp != 0) {
                snprintf(RAM_temp_c, sizeof(RAM_temp_c), "%u°C", static_cast<u32>(realRAM_Temp / 1000.0));
            }
        }

        // Only process resolution if RES is active and game is running
        if (isActive("RES") && GameRunning && NxFps) {
            if (!resolutionLookup) {
                (NxFps->renderCalls[0].calls) = 0xFFFF;
                resolutionLookup = 1;
            } else if (resolutionLookup == 1) {
                if ((NxFps->renderCalls[0].calls) != 0xFFFF) resolutionLookup = 2;
            } else {
                memcpy(&m_resolutionRenderCalls, &(NxFps->renderCalls), sizeof(m_resolutionRenderCalls));
                memcpy(&m_resolutionViewportCalls, &(NxFps->viewportCalls), sizeof(m_resolutionViewportCalls));
                qsort(m_resolutionRenderCalls, 8, sizeof(resolutionCalls), compare);
                qsort(m_resolutionViewportCalls, 8, sizeof(resolutionCalls), compare);
                memset(&m_resolutionOutput, 0, sizeof(m_resolutionOutput));
                size_t out_iter = 0;
                bool found = false;
                for (size_t i = 0; i < 8; i++) {
                    for (size_t x = 0; x < 8; x++) {
                        if (m_resolutionRenderCalls[i].width == 0) break;
                        if ((m_resolutionRenderCalls[i].width == m_resolutionViewportCalls[x].width) &&
                            (m_resolutionRenderCalls[i].height == m_resolutionViewportCalls[x].height)) {
                            m_resolutionOutput[out_iter].width = m_resolutionRenderCalls[i].width;
                            m_resolutionOutput[out_iter].height = m_resolutionRenderCalls[i].height;
                            m_resolutionOutput[out_iter].calls = (m_resolutionRenderCalls[i].calls > m_resolutionViewportCalls[x].calls)
                                ? m_resolutionRenderCalls[i].calls : m_resolutionViewportCalls[x].calls;
                            out_iter++;
                            found = true;
                            break;
                        }
                    }
                    if (!found && m_resolutionRenderCalls[i].width != 0) {
                        m_resolutionOutput[out_iter].width = m_resolutionRenderCalls[i].width;
                        m_resolutionOutput[out_iter].height = m_resolutionRenderCalls[i].height;
                        m_resolutionOutput[out_iter].calls = m_resolutionRenderCalls[i].calls;
                        out_iter++;
                    }
                    found = false;
                    if (out_iter == 8) break;
                }
                if (out_iter < 8) {
                    const size_t out_iter_s = out_iter;
                    for (size_t x = 0; x < 8; x++) {
                        for (size_t y = 0; y < out_iter_s; y++) {
                            if (m_resolutionViewportCalls[x].width == 0) break;
                            if ((m_resolutionViewportCalls[x].width == m_resolutionOutput[y].width) &&
                                (m_resolutionViewportCalls[x].height == m_resolutionOutput[y].height)) {
                                found = true;
                                break;
                            }
                        }
                        if (!found && m_resolutionViewportCalls[x].width != 0) {
                            m_resolutionOutput[out_iter].width = m_resolutionViewportCalls[x].width;
                            m_resolutionOutput[out_iter].height = m_resolutionViewportCalls[x].height;
                            m_resolutionOutput[out_iter].calls = m_resolutionViewportCalls[x].calls;
                            out_iter++;
                        }
                        found = false;
                        if (out_iter == 8) break;
                    }
                }
                qsort(m_resolutionOutput, 8, sizeof(resolutionCalls), compare);
            }
        } else if (!GameRunning && resolutionLookup != 0) {
            resolutionLookup = 0;
        }

        mutexUnlock(&mutex_Misc);

        // Battery/power draw - always update since BAT/DRAW might be displayed
        if (isActive("BAT") || isActive("DRAW")) {
            char remainingBatteryLife[8];
            const float drawW = (fabsf(PowerConsumption) < 0.01f) ? 0.0f : PowerConsumption;

            mutexLock(&mutex_BatteryChecker);

            if (batTimeEstimate >= 0 && !(drawW <= 0.01f && drawW >= -0.01f)) {
                snprintf(remainingBatteryLife, sizeof(remainingBatteryLife),
                         "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
            } else {
                strcpy(remainingBatteryLife, "--:--");
            }

            if (!settings.invertBatteryDisplay) {
                snprintf(Battery_c, sizeof(Battery_c),
                         "%.2f W%.1f%% [%s]",
                         drawW,
                         (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                         remainingBatteryLife);
            } else {
                snprintf(Battery_c, sizeof(Battery_c),
                         "%.1f%% [%s]%.2f W",
                         (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                         remainingBatteryLife,
                         drawW);
            }

            mutexUnlock(&mutex_BatteryChecker);
        }

        // Build Variables string
        char Temp[512] = "";
        uint16_t flags = 0;

        for (const auto& key : showKeys) {
            if (key == "CPU" && !(flags & 1)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, MINI_CPU_compressed_c);
                if (settings.realVolts && MINI_CPU_volt_c[0]) {
                    strcat(Temp, "");
                    strcat(Temp, MINI_CPU_volt_c);
                }
                flags |= 1;
            }
            else if (key == "GPU" && !(flags & 2)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, MINI_GPU_Load_c);
                if (settings.realVolts && MINI_GPU_volt_c[0]) {
                    strcat(Temp, "");
                    strcat(Temp, MINI_GPU_volt_c);
                }
                flags |= 2;
            }
            else if (key == "RAM" && !(flags & 4)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, MINI_RAM_var_compressed_c);
                if (settings.realVolts && MINI_RAM_volt_c[0]) {
                    strcat(Temp, "");
                    strcat(Temp, MINI_RAM_volt_c);
                }
                flags |= 4;
            }
            else if (key == "SOC" && !(flags & 8)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, soc_temperature_c);
                if (settings.realVolts && settings.showSOCVoltage && MINI_SOC_volt_c[0]) {
                    strcat(Temp, "");
                    strcat(Temp, MINI_SOC_volt_c);
                }
                flags |= 8;
            }
            else if (key == "TMP" && !(flags & 16)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, skin_temperature_c);
                if (settings.realVolts && settings.showSOCVoltage && MINI_SOC_volt_c[0]) {
                    strcat(Temp, "");
                    strcat(Temp, MINI_SOC_volt_c);
                }
                flags |= 16;
            }
            else if ((key == "BAT" || key == "DRAW") && !(flags & 32)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, Battery_c);
                flags |= 32;
            }
            else if (key == "FPS" && !(flags & 64) && GameRunning) {
                if (Temp[0]) strcat(Temp, "\n");
                char Temp_s[24];
                snprintf(Temp_s, sizeof(Temp_s), "%2.1f [%2.1f - %2.1f]", FPSavg, FPSmin, FPSmax);
                strcat(Temp, Temp_s);
                flags |= 64;
            }
            else if (key == "RES" && !(flags & 128) && GameRunning && m_resolutionOutput[0].width) {
                if (Temp[0]) strcat(Temp, "\n");
                char Temp_s[32] = "";
                static std::pair<uint16_t, uint16_t> old_res[2];

                uint16_t w0 = m_resolutionOutput[0].width;
                uint16_t h0 = m_resolutionOutput[0].height;
                uint16_t w1 = m_resolutionOutput[1].width;
                uint16_t h1 = m_resolutionOutput[1].height;

                if (w0 || w1) {
                    if (w0 && w1) {
                        if ((w0 == old_res[1].first && h0 == old_res[1].second) ||
                            (w1 == old_res[0].first && h1 == old_res[0].second)) {
                            std::swap(w0, w1);
                            std::swap(h0, h1);
                        }
                    }


                    // Format based on whether we show full resolution or just height (p)
                    if (settings.showFullResolution) {
                        if (!w1 || !h1)
                            snprintf(Temp_s, sizeof(Temp_s), "%dx%d", w0 ? w0 : w1, h0 ? h0 : h1);
                        else
                            snprintf(Temp_s, sizeof(Temp_s), "%dx%d%dx%d", w0, h0, w1, h1);
                    } else {
                        if (!w1 || !h1)
                            snprintf(Temp_s, sizeof(Temp_s), "%dp", h0 ? h0 : h1);
                        else
                            snprintf(Temp_s, sizeof(Temp_s), "%dp%dp", h0, h1);
                    }

                    // Update last-frame cache
                    old_res[0] = {m_resolutionOutput[0].width, m_resolutionOutput[0].height};
                    old_res[1] = {m_resolutionOutput[1].width, m_resolutionOutput[1].height};

                    strcat(Temp, Temp_s);
                    flags |= 128;
                }
            }
            else if (key == "READ" && !(flags & 512) && GameRunning && NxFps) {
                if (Temp[0]) strcat(Temp, "\n");
                char Temp_s[24];
                if ((NxFps->readSpeedPerSecond) != 0.f) {
                    snprintf(Temp_s, sizeof(Temp_s), "%.2f MiB/s", (NxFps->readSpeedPerSecond) / 1048576.f);
                } else {
                    strcpy(Temp_s, "n/d");
                }
                strcat(Temp, Temp_s);
                flags |= 512;
            }
            else if (key == "DTC" && !(flags & 256) && settings.showDTC) {
                if (Temp[0]) strcat(Temp, "\n");
                char dateTimeStr[64];
                time_t rawtime = time(NULL);
                struct tm *timeinfo = localtime(&rawtime);
                strftime(dateTimeStr, sizeof(dateTimeStr), settings.dtcFormat.c_str(), timeinfo);
                strcat(Temp, dateTimeStr);
                flags |= 256;
            }
            else if (key == "MEM" && !(flags & 1024)) {
                if (Temp[0]) strcat(Temp, "\n");
                strcat(Temp, MINI_MEM_RAM_c);
                flags |= 1024;
            }
        }

        strcpy(Variables, Temp);

        // Enable rendering if needed
        if (!skipOnce) {
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
        // Static variables to maintain drag state between function calls
        static bool oldTouchDetected = false;
        static bool oldMinusHeld = false;
        static bool oldPlusHeld = false;

        static HidTouchState initialTouchPos = {0};
        static int initialFrameOffsetX = 0;
        static int initialFrameOffsetY = 0;
        static constexpr int TOUCH_THRESHOLD = 8;
        static bool hasMoved = false;
        //static u64 lastTouchTime = 0;
        //static constexpr u64 TOUCH_DEBOUNCE_NS = 16000000ULL; // 16ms debounce

        // Get current time for debouncing
        //const u64 currentTime = armTicksToNs(armGetSystemTick());

        // Better touch detection - check if coordinates are within reasonable screen bounds
        const bool currentTouchDetected = (touchPos.x > 0 && touchPos.y > 0 &&
                                    touchPos.x < screenWidth && touchPos.y < screenHeight);

        // Debounce touch detection
        //if (currentTouchDetected && !oldTouchDetected) {
        //    if (currentTime - lastTouchTime < TOUCH_DEBOUNCE_NS) {
        //        currentTouchDetected = false; // Ignore rapid touch changes
        //    } else {
        //        lastTouchTime = currentTime;
        //    }
        //}

        static bool clearOnRelease = false;

        if (clearOnRelease && !isRendering) {
            clearOnRelease = false;
            isRendering = true;
            leventClear(&renderingStopEvent);
        }

        // Calculate overlay bounds
        const uint32_t margin = (fontsize * 4);

        // Cache bounds calculation
        static int cachedBaseX = 0;
        static int cachedBaseY = 0;
        static uint32_t cachedOverlayHeight = 0;
        static bool boundsNeedUpdate = true;
        static std::string lastVariables = "";

        // Only recalculate bounds when Variables change or first time
        if (boundsNeedUpdate || std::string(Variables) != lastVariables) {
            // Calculate actual entry count from Variables string
            size_t actualEntryCount = 1;
            for (size_t i = 0; Variables[i] != '\0'; i++) {
                if (Variables[i] == '\n') {
                    actualEntryCount++;
                }
            }

            cachedOverlayHeight = ((fontsize + settings.spacing) * actualEntryCount) +
                                 (fontsize / 3) + settings.spacing + topPadding + bottomPadding;

            // Position calculation based on settings
            cachedBaseX = 0;
            cachedBaseY = 0;

            boundsNeedUpdate = false;
            lastVariables = Variables;
        }

        const int overlayX = frameOffsetX;//ult::limitedMemory ? cachedBaseX + std::max(0, frameOffsetX - (1280-448)) : cachedBaseX + frameOffsetX;
        const int overlayY = cachedBaseY + frameOffsetY;
        const int overlayWidth = margin + rectangleWidth + (fontsize / 3);
        const int overlayHeight = cachedOverlayHeight;

        // Add padding to make touch detection more forgiving
        static constexpr int touchPadding = 4;
        const int touchableX = overlayX - touchPadding;
        const int touchableY = overlayY - touchPadding;
        const int touchableWidth = overlayWidth + (touchPadding * 2);
        const int touchableHeight = overlayHeight + (touchPadding * 2);

        // Screen boundaries for clamping
        const int minX = -cachedBaseX + framePadding;
        const int maxX = screenWidth - overlayWidth - cachedBaseX - framePadding;
        const int minY = -cachedBaseY + framePadding;
        const int maxY = screenHeight - overlayHeight - cachedBaseY - framePadding;

        const bool minusDragReady = buttonState.minusDragActive.load(std::memory_order_acquire);
        const bool plusDragReady = buttonState.plusDragActive.load(std::memory_order_acquire);

        // Check button states
        const bool currentMinusHeld = (keysHeld & KEY_MINUS) && !(keysHeld & ~KEY_MINUS & ALL_KEYS_MASK) && minusDragReady;
        const bool currentPlusHeld = (keysHeld & KEY_PLUS) && !(keysHeld & ~KEY_PLUS & ALL_KEYS_MASK) && plusDragReady;

        // Handle touch dragging
        if (currentTouchDetected && !isDragging) {
            // Touch detected and not currently dragging - check if we should start
            const int touchX = touchPos.x;
            const int touchY = touchPos.y;

            if (!oldTouchDetected) {
                // Touch just started - check if within overlay bounds
                if (touchX >= touchableX && touchX <= touchableX + touchableWidth &&
                    touchY >= touchableY && touchY <= touchableY + touchableHeight) {

                    // Start touch dragging
                    isDragging = true;
                    //isRendering = false;
                    //leventSignal(&renderingStopEvent);
                    triggerRumbleClick.store(true, std::memory_order_release);
                    triggerOnSound.store(true, std::memory_order_release);
                    hasMoved = false;
                    initialTouchPos = touchPos;
                    initialFrameOffsetX = frameOffsetX;
                    initialFrameOffsetY = frameOffsetY;
                }
            }
        } else if (currentTouchDetected && isDragging && !currentMinusHeld && !currentPlusHeld) {
            // Continue touch dragging (only if neither MINUS nor PLUS is held)
            const int touchX = touchPos.x;
            const int touchY = touchPos.y;
            const int deltaX = touchX - initialTouchPos.x;
            const int deltaY = touchY - initialTouchPos.y;

            // Check if we've moved enough to consider this a drag
            if (!hasMoved) {
                const int totalMovement = abs(deltaX) + abs(deltaY);
                if (totalMovement >= TOUCH_THRESHOLD) {
                    hasMoved = true;
                }
            }

            if (hasMoved) {
                // Update frame offsets with boundary checking
                const int newFrameOffsetX = std::max(minX, std::min(maxX, initialFrameOffsetX + deltaX));
                const int newFrameOffsetY = std::max(minY, std::min(maxY, initialFrameOffsetY + deltaY));

                frameOffsetX = newFrameOffsetX;
                frameOffsetY = newFrameOffsetY;

                if (ult::limitedMemory) {
                    tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
                }
                boundsNeedUpdate = true;
            }
        } else if (!currentTouchDetected && oldTouchDetected && isDragging && !currentMinusHeld && !currentPlusHeld) {
            // Touch just released and we were touch dragging
            if (hasMoved) {
                // Save position when touch drag ends
                auto iniData = ult::getParsedDataFromIniFile(configIniPath);
                iniData["mini"]["frame_offset_x"] = std::to_string(frameOffsetX);
                iniData["mini"]["frame_offset_y"] = std::to_string(frameOffsetY);
                ult::saveIniFileData(configIniPath, iniData);
            }

            // Reset touch drag state
            isDragging = false;
            hasMoved = false;
            //isRendering = true;
            //leventClear(&renderingStopEvent);
            clearOnRelease = true;
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerOffSound.store(true, std::memory_order_release);
        }


        // Handle joystick dragging (MINUS + right joystick OR PLUS + left joystick)
        if ((currentMinusHeld || currentPlusHeld) && !isDragging) {
            // Start joystick dragging
            isDragging = true;
            //isRendering = false;
            //leventSignal(&renderingStopEvent);
            triggerRumbleClick.store(true, std::memory_order_release);
            triggerOnSound.store(true, std::memory_order_release);
        } else if ((currentMinusHeld || currentPlusHeld) && isDragging) {
            // Continue joystick dragging
            static constexpr int JOYSTICK_DEADZONE = 20;

            // Choose the appropriate joystick based on which button is held
            const HidAnalogStickState& activeJoystick = currentMinusHeld ? joyStickPosRight : joyStickPosLeft;

            // Only move if joystick is outside deadzone
            if (abs(activeJoystick.x) > JOYSTICK_DEADZONE || abs(activeJoystick.y) > JOYSTICK_DEADZONE) {
                // Calculate joystick magnitude (distance from center)
                const float magnitude = sqrt((float)(activeJoystick.x * activeJoystick.x + activeJoystick.y * activeJoystick.y));
                const float normalizedMagnitude = magnitude / 32767.0f; // Normalize to 0-1 range

                // Single smooth curve: stays very slow for wide range, then accelerates
                static constexpr float baseSensitivity = 0.00008f;  // Higher so small movements register
                static constexpr float maxSensitivity = 0.0005f;

                // Use x^8 curve - stays very low until ~70% then curves up sharply
                const float curveValue = pow(normalizedMagnitude, 8.0f);
                const float currentSensitivity = baseSensitivity + (maxSensitivity - baseSensitivity) * curveValue;

                // Calculate movement delta with fractional accumulation
                static float accumulatedX = 0.0f;
                static float accumulatedY = 0.0f;

                accumulatedX += (float)activeJoystick.x * currentSensitivity;
                accumulatedY += -(float)activeJoystick.y * currentSensitivity;

                // Extract integer movement and keep fractional part
                const int deltaX = (int)accumulatedX;
                const int deltaY = (int)accumulatedY;
                accumulatedX -= deltaX;
                accumulatedY -= deltaY;

                // Update frame offsets with boundary checking
                const int newFrameOffsetX = std::max(minX, std::min(maxX, frameOffsetX + deltaX));
                const int newFrameOffsetY = std::max(minY, std::min(maxY, frameOffsetY + deltaY));

                frameOffsetX = newFrameOffsetX;
                frameOffsetY = newFrameOffsetY;

                if (ult::limitedMemory) {
                    tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
                }
                boundsNeedUpdate = true;
            }
        } else if (((!currentMinusHeld && oldMinusHeld) || (!currentPlusHeld && oldPlusHeld)) && isDragging) {
            // Button just released - stop joystick dragging
            auto iniData = ult::getParsedDataFromIniFile(configIniPath);
            iniData["mini"]["frame_offset_x"] = std::to_string(frameOffsetX);
            iniData["mini"]["frame_offset_y"] = std::to_string(frameOffsetY);
            ult::saveIniFileData(configIniPath, iniData);
            isDragging = false;
            //isRendering = true;
            //leventClear(&renderingStopEvent);
            clearOnRelease = true;
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerOffSound.store(true, std::memory_order_release);
        }

        // Update state for next frame
        oldTouchDetected = currentTouchDetected;
        oldMinusHeld = currentMinusHeld;
        oldPlusHeld = currentPlusHeld;

        // Handle existing key input logic (but don't interfere with dragging)
        if (!isDragging) {
            if (isKeyComboPressed(keysHeld, keysDown)) {
                isRendering = false;
                leventSignal(&renderingStopEvent);
                //triggerRumbleDoubleClick.store(true, std::memory_order_release);
                skipOnce = true;
                runOnce = true;
                //TeslaFPS = 60;
                if (skipMain) {
                    //lastSelectedItem = "Mini";
                    lastMode = "returning";
                    tsl::goBack();
                }
                else {
                    tsl::setNextOverlay(filepath.c_str(), "--lastSelectedItem Mini");
                    tsl::Overlay::get()->close();
                }
                return true;
            }
            else if (((keysDown & KEY_L) && (keysDown & KEY_ZL)) || ((keysDown & KEY_L) && (keysHeld & KEY_ZL)) || ((keysHeld & KEY_L) && (keysDown & KEY_ZL))) {
                FPSmin = 254;
                FPSmax = 0;
            }
        }

        // Return true if we handled the input (during dragging)
        return isDragging;
    }
};
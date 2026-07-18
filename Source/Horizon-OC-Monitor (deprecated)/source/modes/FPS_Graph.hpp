class MainMenu;

class com_FPSGraph : public tsl::Gui {
private:
    uint8_t refreshRate = 0;
    char FPSavg_c[8];
    FpsGraphSettings settings;
    uint64_t systemtickfrequency_impl = systemtickfrequency;
    uint32_t cnt = 0;
    char CPU_Load_c[12] = "    -";
    char GPU_Load_c[12] = "    -";
    char RAM_Load_c[12] = "    -";
    char SOC_TEMP_c[12] = "    -";
    char PCB_TEMP_c[12] = "    -";
    char SKIN_TEMP_c[12] = "    -";
	char CPU_TEMP_c[12] = "    -";
	char GPU_TEMP_c[12] = "    -";
	char RAM_TEMP_c[12] = "    -";
    bool skipOnce = true;
    bool runOnce = true;
    
    // Repositioning variables (matching Mini)
    int frameOffsetX = 0;
    int frameOffsetY = 0;
    bool isDragging = false;
    size_t framePadding = 10;
    static constexpr int screenWidth = 1280;
    static constexpr int screenHeight = 720;
    static constexpr int border = 8;

    bool originalUseRightAlignment = ult::useRightAlignment;

    struct ButtonState {
        std::atomic<bool> minusDragActive{false};
        std::atomic<bool> plusDragActive{false};
    } buttonState;

    Thread touchPollThread;
    std::atomic<bool> touchPollRunning{false};

    // Store actual rendered dimensions (including border)
    size_t actualTotalWidth = 0;
    size_t actualTotalHeight = 0;

public:
    bool isStarted = false;
    com_FPSGraph() { 
        tsl::hlp::requestForeground(false);
        disableJumpTo = true;
        GetConfigSettings(&settings);

        if (R_SUCCEEDED(SaltySD_Connect())) {
            if (R_FAILED(SaltySD_GetDisplayRefreshRate(&refreshRate)))
                refreshRate = 0;
            svcSleepThread(100'000);
            SaltySD_Term();
        }
        
        // Load saved frame offsets
        frameOffsetX = settings.frameOffsetX;
        frameOffsetY = settings.frameOffsetY;
        framePadding = settings.framePadding;
        
       
        if (ult::limitedMemory) {
            tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
        }

        FullMode = false;
        TeslaFPS = settings.refreshRate;
        systemtickfrequency_impl /= settings.refreshRate;
        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().removeScreenshotStacks();
        }
        deactivateOriginalFooter = true;
        mutexInit(&mutex_Misc);
        StartInfoThread();
        StartFPSCounterThread();

        // Start touch polling thread for instant response at low FPS
        touchPollRunning.store(true, std::memory_order_release);
        threadCreate(&touchPollThread, [](void* arg) -> void {
            com_FPSGraph* overlay = static_cast<com_FPSGraph*>(arg);
            
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
        
            while (overlay->touchPollRunning.load(std::memory_order_acquire)) {
                // Only poll when rendering and not dragging
                {
                    inputDetected = false;
                    
                    // Check touch in bounds
                    if (hidGetTouchScreenStates(&state, 1) && state.count > 0) {
                        const int touchX = state.touches[0].x;
                        const int touchY = state.touches[0].y;
                        
                        // Use actual dimensions, fallback to estimate if not yet rendered
                        size_t totalWidth = overlay->actualTotalWidth;
                        size_t totalHeight = overlay->actualTotalHeight;
                        
                        if (totalWidth == 0) {
                            // Fallback calculation
                            const s16 refresh_rate_offset = (overlay->refreshRate < 100) ? 21 : 28;
                            const s16 info_width = overlay->settings.showInfo ? (6 + overlay->rectangle_width/2 - 4) : 0;
                            const s16 content_width = overlay->rectangle_width + refresh_rate_offset + info_width;
                            const s16 content_height = overlay->rectangle_height + 12;
                            totalWidth = content_width + (2 * border);
                            totalHeight = content_height + (2 * border);
                        }
                        
                        // Apply frame offsets (base position already includes border offset)
                        const int overlayX = overlay->base_x + overlay->frameOffsetX - border;
                        const int overlayY = overlay->base_y + overlay->frameOffsetY - border;
                        
                        // Touch padding
                        const int touchPadding = 4;
                        const int touchableX = overlayX - touchPadding;
                        const int touchableY = overlayY - touchPadding;
                        const int touchableWidth = totalWidth + (touchPadding * 2);
                        const int touchableHeight = totalHeight + (touchPadding * 2);
                        
                        // Check if touch is within bounds
                        if (touchX >= touchableX && touchX <= touchableX + touchableWidth &&
                            touchY >= touchableY && touchY <= touchableY + touchableHeight) {
                            inputDetected = true;
                        }
                    }
                    
                    // Poll buttons from both controllers
                    padUpdate(&pad_p1);
                    padUpdate(&pad_handheld);
                    
                    // Combine input from both controllers
                    const u64 keysHeld = padGetButtons(&pad_p1) | padGetButtons(&pad_handheld);
                    const u64 now = armTicksToNs(armGetSystemTick());
                    
                    // Track MINUS hold duration
                    if ((keysHeld & KEY_MINUS) && !(keysHeld & ~KEY_MINUS & ALL_KEYS_MASK)) {
                        if (minusHoldStart == 0) {
                            minusHoldStart = now;
                        }
                        if (now - minusHoldStart >= HOLD_THRESHOLD_NS) {
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
                
                svcSleepThread(16000000ULL*2); // 16ms polling
            }
        }, this, NULL, 0x1000, 0x2B, -2);
        threadStart(&touchPollThread);
    }

    ~com_FPSGraph() {
        // Stop touch polling thread
        touchPollRunning.store(false, std::memory_order_release);
        threadWaitForExit(&touchPollThread);
        threadClose(&touchPollThread);

        EndInfoThread();
        EndFPSCounterThread();
        FullMode = true;
        fixForeground = true;
        ult::useRightAlignment = originalUseRightAlignment;
        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().addScreenshotStacks();
        }
        deactivateOriginalFooter = false;
    }

    struct stats {
        s16 value;
        bool zero_rounded;
    };

    std::vector<stats> readings;

    s16 base_y = 0;
    s16 base_x = 0;
    s16 rectangle_width = 180;
    s16 rectangle_height = 60;
    s16 rectangle_x = 15;
    s16 rectangle_y = 5;
    s16 rectangle_range_max = 60;
    s16 rectangle_range_min = 0;
    char legend_max[4] = "60";
    char legend_min[2] = "0";
    s32 range = std::abs(rectangle_range_max - rectangle_range_min) + 1;
    s16 x_end = rectangle_x + rectangle_width;
    s16 y_old = rectangle_y+rectangle_height;
    s16 y_30FPS = rectangle_y+(rectangle_height / 2);
    s16 y_60FPS = rectangle_y;
    bool isAbove = false;

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {

            // Calculate content dimensions (what goes inside the border)
            const s16 refresh_rate_offset = (refreshRate < 100) ? 21 : 28;
            const s16 info_width = settings.showInfo ? (6 + rectangle_width/2 - 4) : 6;
            const s16 content_width = rectangle_width + refresh_rate_offset + info_width;
            const s16 content_height = rectangle_height + 12;
            
            // Total dimensions including border
            const size_t totalWidth = content_width + (2 * border);
            const size_t totalHeight = content_height + (2 * border);
            
            // Store actual dimensions for input handling
            actualTotalWidth = totalWidth;
            actualTotalHeight = totalHeight;

            if (refreshRate && refreshRate < 240) {
                rectangle_height = refreshRate;
                rectangle_range_max = refreshRate;
                if (refreshRate < 100) {
                    rectangle_x = 15;
                    legend_max[0] = 0x30 + (refreshRate / 10);
                    legend_max[1] = 0x30 + (refreshRate % 10);
                    legend_max[2] = 0;
                }
                else {
                    rectangle_x = 22;
                    legend_max[0] = 0x30 + (refreshRate / 100);
                    legend_max[1] = 0x30 + ((refreshRate / 10) % 10);
                    legend_max[2] = 0x30 + (refreshRate % 10);
                }
                y_30FPS = rectangle_y+(rectangle_height / 2);
                range = std::abs(rectangle_range_max - rectangle_range_min) + 1;
            };

            int _frameOffsetX = ult::limitedMemory ? std::max(0, frameOffsetX - (1280-448)) : frameOffsetX;

            // Calculate position with frame offsets (for the rounded rect, which includes border)
            int posX = base_x + _frameOffsetX - border;
            int posY = base_y + frameOffsetY - border;
            
            // Clamp to screen bounds (accounting for total size including border)
            posX = std::max(int(framePadding), std::min(posX, static_cast<int>(screenWidth - totalWidth - framePadding)));
            posY = std::max(int(framePadding), std::min(posY, static_cast<int>(screenHeight - totalHeight - framePadding)));
            
            // Draw the rounded rectangle (background)
            const tsl::Color bgColor = !isDragging
                ? settings.backgroundColor
                : settings.focusBackgroundColor;
            
            renderer->drawRoundedRectSingleThreaded(
                posX, 
                posY, 
                totalWidth, 
                totalHeight,
                16, 
                aWithOpacity(bgColor)
            );
            
            posX += 4;

            // Content drawing position (inside the border)
            const int final_base_x = posX + border;
            const int final_base_y = posY + border;

            const s16 size = (refreshRate > 60 || !refreshRate) ? 63 : (s32)(63.0/(60.0/refreshRate));
            const auto width = renderer->getTextDimensions(FPSavg_c, false, size).first;

            const s16 pos_y = size + final_base_y + rectangle_y + ((rectangle_height - size) / 2);
            const s16 pos_x = final_base_x + rectangle_x + ((rectangle_width - width) / 2);

            if (FPSavg != 254.0)
                renderer->drawString(FPSavg_c, false, pos_x, pos_y-5, size, settings.fpsColor);
            renderer->drawEmptyRect(final_base_x+(rectangle_x - 1)+2, final_base_y+(rectangle_y - 1), rectangle_width + 2, rectangle_height + 4, aWithOpacity(settings.borderColor));
            renderer->drawDashedLine(final_base_x+rectangle_x+2, final_base_y+y_30FPS, final_base_x+rectangle_x+rectangle_width, final_base_y+y_30FPS, 6, aWithOpacity(settings.dashedLineColor));
            renderer->drawString(&legend_max[0], false, final_base_x+(rectangle_x-((refreshRate < 100) ? 15 : 22)), final_base_y+(rectangle_y+7), 10, (settings.maxFPSTextColor));
            renderer->drawString(&legend_min[0], false, final_base_x+(rectangle_x-10), final_base_y+(rectangle_y+rectangle_height+3), 10, settings.minFPSTextColor);

            size_t last_element = readings.size() - 1;

            s16 offset = 0;
            if (refreshRate >= 100) offset = 7;

            static s32 y_on_range;
            static tsl::Color color = {0};
            for (s16 x = x_end; x > static_cast<s16>(x_end-readings.size()); x--) {
                y_on_range = readings[last_element].value + std::abs(rectangle_range_min) + 1;
                if (y_on_range < 0) {
                    y_on_range = 0;
                }
                else if (y_on_range > range) {
                    isAbove = true;
                    y_on_range = range; 
                }
                
                const s16 y = rectangle_y + static_cast<s16>(std::lround((float)rectangle_height * ((float)(range - y_on_range) / (float)range)));
                color = (settings.mainLineColor);
                if (y == y_old && !isAbove && readings[last_element].zero_rounded) {
                    if ((y == y_30FPS || y == y_60FPS))
                        color = (settings.perfectLineColor);
                    else
                        color = (settings.dashedLineColor);
                }

                if (x == x_end) {
                    y_old = y;
                }

                renderer->drawLine(final_base_x+x+offset, final_base_y+y, final_base_x+x+offset, final_base_y+y_old, color);
                isAbove = false;
                y_old = y;
                last_element--;
            }

            if (settings.showInfo) {
                const s16 info_x = final_base_x+rectangle_width+rectangle_x + 6 +8;
                const s16 info_y = final_base_y + 3;
                const s16 fontSize = 11;
                
                // Get line height from font size (we'll use the actual rendered height)
                const auto testDimensions = renderer->getTextDimensions("A", false, fontSize);
                const s16 lineHeight = testDimensions.second;
                
                // Starting Y position for first line
                const s16 startY = info_y + lineHeight;
                
                // Value X position (offset from labels)
                const s16 value_x = info_x + 40;

                static constexpr s16 SPACING = 1;
                
                // Compute gradient colors for temperatures
                const tsl::Color socColor = settings.useDynamicColors ? tsl::GradientColor(SOC_temperatureF) : settings.textColor;
                const tsl::Color pcbColor = settings.useDynamicColors ? tsl::GradientColor(PCB_temperatureF) : settings.textColor;
                const tsl::Color skinColor = settings.useDynamicColors ? tsl::GradientColor(static_cast<float>(skin_temperaturemiliC) / 1000.0f) : settings.textColor;
                
                // Draw each label and value pair on the same baseline
                // Line 0: CPU
                renderer->drawString("CPU", false, info_x, startY, fontSize, settings.catColor);
                renderer->drawString(CPU_Load_c, false, value_x, startY, fontSize, settings.textColor);
                
                // Line 1: GPU
                renderer->drawString("GPU", false, info_x, startY + lineHeight+SPACING, fontSize, settings.catColor);
                renderer->drawString(GPU_Load_c, false, value_x, startY + lineHeight+SPACING, fontSize, settings.textColor);
                
                // Line 2: RAM
                renderer->drawString("RAM", false, info_x, startY + lineHeight * 2+2*SPACING, fontSize, settings.catColor);
                renderer->drawString(RAM_Load_c, false, value_x, startY + lineHeight * 2+2*SPACING, fontSize, settings.textColor);
                
                // Line 3: CPU or SOC (with gradient color)
				if (settings.realTemps && realCPU_Temp != 0) {
					const tsl::Color cpuTempColor = settings.useDynamicColors ? tsl::GradientColor(realCPU_Temp / 1000.0f) : settings.textColor;
					renderer->drawString("CPU", false, info_x, startY + lineHeight * 3+3*SPACING, fontSize, settings.catColor);
					renderer->drawString(CPU_TEMP_c, false, value_x, startY + lineHeight * 3+3*SPACING, fontSize, cpuTempColor);
				} else {
					renderer->drawString("SOC", false, info_x, startY + lineHeight * 3+3*SPACING, fontSize, settings.catColor);
					renderer->drawString(SOC_TEMP_c, false, value_x, startY + lineHeight * 3+3*SPACING, fontSize, socColor);
				}

				// Line 4: GPU or PCB (with gradient color)
				if (settings.realTemps && realGPU_Temp != 0) {
					const tsl::Color gpuTempColor = settings.useDynamicColors ? tsl::GradientColor(realGPU_Temp / 1000.0f) : settings.textColor;
					renderer->drawString("GPU", false, info_x, startY + lineHeight * 4+4*SPACING, fontSize, settings.catColor);
					renderer->drawString(GPU_TEMP_c, false, value_x, startY + lineHeight * 4+4*SPACING, fontSize, gpuTempColor);
				} else {
					renderer->drawString("PCB", false, info_x, startY + lineHeight * 4+4*SPACING, fontSize, settings.catColor);
					renderer->drawString(PCB_TEMP_c, false, value_x, startY + lineHeight * 4+4*SPACING, fontSize, pcbColor);
				}

				// Line 5: RAM or SKIN (with gradient color)
				if (settings.realTemps && realRAM_Temp != 0) {
					const tsl::Color ramTempColor = settings.useDynamicColors ? tsl::GradientColor(realRAM_Temp / 1000.0f) : settings.textColor;
					renderer->drawString("RAM", false, info_x, startY + lineHeight * 5+5*SPACING, fontSize, settings.catColor);
					renderer->drawString(RAM_TEMP_c, false, value_x, startY + lineHeight * 5+5*SPACING, fontSize, ramTempColor);
				} else {
					renderer->drawString("Skin", false, info_x, startY + lineHeight * 5+5*SPACING, fontSize, settings.catColor);
					renderer->drawString(SKIN_TEMP_c, false, value_x, startY + lineHeight * 5+5*SPACING, fontSize, skinColor);
				}
			}
		});

        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("", "");
        rootFrame->setContent(Status);

        return rootFrame;
    }

    virtual void update() override {
        cnt++;
        if (cnt >= TeslaFPS)
            cnt = 0;

        ///FPS
        stats temp = {0, false};
        static uint64_t lastFrame = 0;
        
        snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f",  FPSavg);
        const uint8_t SaltySharedDisplayRefreshRate = *(uint8_t*)((uintptr_t)shmemGetAddr(&_sharedmemory) + 1);
        if (SaltySharedDisplayRefreshRate) 
            refreshRate = SaltySharedDisplayRefreshRate;
        else refreshRate = 60;
        if (FPSavg < 254) {
            snprintf(FPSavg_c, sizeof(FPSavg_c), "%.1f", useOldFPSavg ? FPSavg_old : FPSavg);

            if (lastFrame == lastFrameNumber) return;
            else lastFrame = lastFrameNumber;
            if ((s16)(readings.size()) >= rectangle_width) {
                readings.erase(readings.begin());
            }
            const float whole = std::round(useOldFPSavg ? FPSavg_old : FPSavg);
            temp.value = static_cast<s16>(std::lround(useOldFPSavg ? FPSavg_old : FPSavg));
            if ((useOldFPSavg ? FPSavg_old : FPSavg) < whole+0.04 && (useOldFPSavg ? FPSavg_old : FPSavg) > whole-0.05) {
                temp.zero_rounded = true;
            }
            readings.push_back(temp);
        }
        else {
            if (readings.size()) {
                readings.clear();
                readings.shrink_to_fit();
                lastFrame = 0;
            }
            FPSavg_c[0] = 0;
        }

        if (cnt)
            return;

        mutexLock(&mutex_Misc);
        
        // Format temperature strings separately for proper alignment
        snprintf(SOC_TEMP_c, sizeof SOC_TEMP_c, "%2.1f\u00B0C", SOC_temperatureF);
        snprintf(PCB_TEMP_c, sizeof PCB_TEMP_c, "%2.1f\u00B0C", PCB_temperatureF);
        snprintf(SKIN_TEMP_c, sizeof SKIN_TEMP_c, "%2d.%d\u00B0C", 
                 skin_temperaturemiliC / 1000, (skin_temperaturemiliC / 100) % 10);
				 
		if (realCPU_Temp != 0) {
			snprintf(CPU_TEMP_c, sizeof(CPU_TEMP_c), "%.1f\u00B0C", realCPU_Temp / 1000.0f);
		}
		if (realGPU_Temp != 0) {
			snprintf(GPU_TEMP_c, sizeof(GPU_TEMP_c), "%.1f\u00B0C", realGPU_Temp / 1000.0f);
		}
		if (realRAM_Temp != 0) {
			snprintf(RAM_TEMP_c, sizeof(RAM_TEMP_c), "%.1f\u00B0C", realRAM_Temp / 1000.0f);
		}
        
        // Atomically snapshot each idle tick once
        const uint64_t idle0 = idletick0.load(std::memory_order_acquire);
        const uint64_t idle1 = idletick1.load(std::memory_order_acquire);
        const uint64_t idle2 = idletick2.load(std::memory_order_acquire);
        const uint64_t idle3 = idletick3.load(std::memory_order_acquire);
        
        // Clamp values to systemtickfrequency_impl (avoid div-by-zero / runaway)
        const uint64_t safe0 = std::min(idle0, systemtickfrequency_impl);
        const uint64_t safe1 = std::min(idle1, systemtickfrequency_impl);
        const uint64_t safe2 = std::min(idle2, systemtickfrequency_impl);
        const uint64_t safe3 = std::min(idle3, systemtickfrequency_impl);
        
        // Compute per-core CPU usage
        const double cpu_usage0 = (1.0 - (static_cast<double>(safe0) / systemtickfrequency_impl)) * 100.0;
        const double cpu_usage1 = (1.0 - (static_cast<double>(safe1) / systemtickfrequency_impl)) * 100.0;
        const double cpu_usage2 = (1.0 - (static_cast<double>(safe2) / systemtickfrequency_impl)) * 100.0;
        const double cpu_usage3 = (1.0 - (static_cast<double>(safe3) / systemtickfrequency_impl)) * 100.0;
        
        // Compute max core load (the highest usage)
        const double cpu_usageM = std::max({cpu_usage0, cpu_usage1, cpu_usage2, cpu_usage3});
        
        // Format output strings
        snprintf(CPU_Load_c, sizeof(CPU_Load_c), "%.1f%%", cpu_usageM);
        snprintf(GPU_Load_c, sizeof(GPU_Load_c), "%d.%d%%", GPU_Load_u / 10, GPU_Load_u % 10);
        snprintf(RAM_Load_c, sizeof(RAM_Load_c), "%hu.%hhu%%",
                 partLoad[HocClkPartLoad_EMC] / 10,
                 partLoad[HocClkPartLoad_EMC] % 10);
        
        mutexUnlock(&mutex_Misc);
    
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
    
        // Touch detection
        const bool currentTouchDetected = (touchPos.x > 0 && touchPos.y > 0 && 
                                    touchPos.x < screenWidth && touchPos.y < screenHeight);
        
        static bool clearOnRelease = false;
        
        if (clearOnRelease && !isRendering) {
            clearOnRelease = false;
            isRendering = true;
            leventClear(&renderingStopEvent);
        }
        
        // Use actual dimensions from last render, fallback to estimate if not available
        size_t totalWidth = actualTotalWidth;
        size_t totalHeight = actualTotalHeight;
        
        if (totalWidth == 0) {
            // Fallback calculation if not yet rendered
            const s16 refresh_rate_offset = (refreshRate < 100) ? 21 : 28;
            const s16 info_width = settings.showInfo ? (6 + rectangle_width/2 - 4) : 0;
            const s16 content_width = rectangle_width + refresh_rate_offset + info_width;
            const s16 content_height = rectangle_height + 12;
            totalWidth = content_width + (2 * border);
            totalHeight = content_height + (2 * border);
        }
        
        // Current overlay position (top-left of rounded rect)
        const int overlayX = base_x + frameOffsetX - border;
        const int overlayY = base_y + frameOffsetY - border;
        
        // Touch detection area (with padding for easier interaction)
        static constexpr int touchPadding = 4;
        const int touchableX = overlayX - touchPadding;
        const int touchableY = overlayY - touchPadding;
        const int touchableWidth = totalWidth + (touchPadding * 2);
        const int touchableHeight = totalHeight + (touchPadding * 2);
        
        // Screen boundaries for clamping (accounting for total size)
        const int minX = -(base_x - border) + framePadding;
        const int maxX = screenWidth - totalWidth - (base_x - border) - framePadding;
        const int minY = -(base_y - border) + framePadding;
        const int maxY = screenHeight - totalHeight - (base_y - border) - framePadding;
    
        const bool minusDragReady = buttonState.minusDragActive.load(std::memory_order_acquire);
        const bool plusDragReady = buttonState.plusDragActive.load(std::memory_order_acquire);

        // Check button states
        const bool currentMinusHeld = (keysHeld & KEY_MINUS) && !(keysHeld & ~KEY_MINUS & ALL_KEYS_MASK) && minusDragReady;
        const bool currentPlusHeld = (keysHeld & KEY_PLUS) && !(keysHeld & ~KEY_PLUS & ALL_KEYS_MASK) && plusDragReady;
    
        // Handle touch dragging
        if (currentTouchDetected && !isDragging) {
            const int touchX = touchPos.x;
            const int touchY = touchPos.y;
            
            if (!oldTouchDetected) {
                // Touch just started - check if within overlay bounds
                if (touchX >= touchableX && touchX <= touchableX + touchableWidth &&
                    touchY >= touchableY && touchY <= touchableY + touchableHeight) {
                    
                    // Start touch dragging
                    isDragging = true;
                    triggerRumbleClick.store(true, std::memory_order_release);
                    triggerOnSound.store(true, std::memory_order_release);
                    hasMoved = false;
                    initialTouchPos = touchPos;
                    initialFrameOffsetX = frameOffsetX;
                    initialFrameOffsetY = frameOffsetY;
                }
            }
        } else if (currentTouchDetected && isDragging && !currentMinusHeld && !currentPlusHeld) {
            // Continue touch dragging
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
                frameOffsetX = std::max(minX, std::min(maxX, initialFrameOffsetX + deltaX));
                frameOffsetY = std::max(minY, std::min(maxY, initialFrameOffsetY + deltaY));

                if (ult::limitedMemory) {
                    tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
                }
            }
        } else if (!currentTouchDetected && oldTouchDetected && isDragging && !currentMinusHeld && !currentPlusHeld) {
            // Touch just released
            if (hasMoved) {
                // Save position when touch drag ends
                auto iniData = ult::getParsedDataFromIniFile(configIniPath);
                iniData["fps-graph"]["frame_offset_x"] = std::to_string(frameOffsetX);
                iniData["fps-graph"]["frame_offset_y"] = std::to_string(frameOffsetY);
                ult::saveIniFileData(configIniPath, iniData);
            }
            
            // Reset touch drag state
            isDragging = false;
            hasMoved = false;
            clearOnRelease = true;
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerOffSound.store(true, std::memory_order_release);
        }
    
        // Handle joystick dragging (MINUS + right joystick OR PLUS + left joystick)
        if ((currentMinusHeld || currentPlusHeld) && !isDragging) {
            // Start joystick dragging
            isDragging = true;
            triggerRumbleClick.store(true, std::memory_order_release);
            triggerOnSound.store(true, std::memory_order_release);
        } else if ((currentMinusHeld || currentPlusHeld) && isDragging) {
            // Continue joystick dragging
            static constexpr int JOYSTICK_DEADZONE = 20;
            
            // Choose the appropriate joystick based on which button is held
            const HidAnalogStickState& activeJoystick = currentMinusHeld ? joyStickPosRight : joyStickPosLeft;
            
            // Only move if joystick is outside deadzone
            if (abs(activeJoystick.x) > JOYSTICK_DEADZONE || abs(activeJoystick.y) > JOYSTICK_DEADZONE) {
                // Calculate joystick magnitude
                const float magnitude = sqrt((float)(activeJoystick.x * activeJoystick.x + activeJoystick.y * activeJoystick.y));
                const float normalizedMagnitude = magnitude / 32767.0f;
                
                // Smooth curve for sensitivity
                static constexpr float baseSensitivity = 0.00008f;
                static constexpr float maxSensitivity = 0.0005f;
                
                const float curveValue = pow(normalizedMagnitude, 8.0f);
                const float currentSensitivity = baseSensitivity + (maxSensitivity - baseSensitivity) * curveValue;
                
                // Calculate movement delta with fractional accumulation
                static float accumulatedX = 0.0f;
                static float accumulatedY = 0.0f;
                
                accumulatedX += (float)activeJoystick.x * currentSensitivity;
                accumulatedY += -(float)activeJoystick.y * currentSensitivity;
                
                const int deltaX = (int)accumulatedX;
                const int deltaY = (int)accumulatedY;
                accumulatedX -= deltaX;
                accumulatedY -= deltaY;
                
                // Update frame offsets with boundary checking
                frameOffsetX = std::max(minX, std::min(maxX, frameOffsetX + deltaX));
                frameOffsetY = std::max(minY, std::min(maxY, frameOffsetY + deltaY));

                if (ult::limitedMemory) {
                    tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
                }
            }
        } else if (((!currentMinusHeld && oldMinusHeld) || (!currentPlusHeld && oldPlusHeld)) && isDragging) {
            // Button just released - stop joystick dragging
            auto iniData = ult::getParsedDataFromIniFile(configIniPath);
            iniData["fps-graph"]["frame_offset_x"] = std::to_string(frameOffsetX);
            iniData["fps-graph"]["frame_offset_y"] = std::to_string(frameOffsetY);
            ult::saveIniFileData(configIniPath, iniData);
            isDragging = false;
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
                runOnce = true;
                skipOnce = true;
                TeslaFPS = 60;
                lastSelectedItem = "FPS Graph";
                lastMode = "";
                if (skipMain) {
                    lastMode = "return";
                    tsl::goBack();
                }
                else {
                    tsl::setNextOverlay(filepath.c_str(), "--lastSelectedItem 'FPS Graph'");
                    tsl::Overlay::get()->close();
                }
                return true;
            }
        }
        
        // Return true if we handled the input (during dragging)
        return isDragging;
    }
};
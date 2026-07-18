class MainMenu;

class com_FPS : public tsl::Gui {
private:
    char FPSavg_c[8];
    FpsCounterSettings settings;
    size_t fontsize = 0;
    ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
    bool skipOnce = true;
    bool runOnce = true;

    // Repositioning variables
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

    // Store actual rendered dimensions
    size_t actualTextWidth = 0;
    size_t actualTotalWidth = 0;
    size_t actualTotalHeight = 0;

public:
    com_FPS() { 
        tsl::hlp::requestForeground(false);
        disableJumpTo = true;
        GetConfigSettings(&settings);
        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            fontsize = settings.handheldFontSize;
        }
        else if (performanceMode == ApmPerformanceMode_Boost) {
            fontsize = settings.dockedFontSize;
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
        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().removeScreenshotStacks();
        }
        deactivateOriginalFooter = true;
        StartFPSCounterThread();

        // Start touch polling thread for instant response at low FPS
        touchPollRunning.store(true, std::memory_order_release);
        threadCreate(&touchPollThread, [](void* arg) -> void {
            com_FPS* overlay = static_cast<com_FPS*>(arg);
            
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
                            size_t approxFontSize = overlay->fontsize;
                            if (approxFontSize == 0) approxFontSize = 50;
                            const size_t textWidth = approxFontSize * 4;
                            const size_t margin = (approxFontSize / 8);
                            const size_t innerWidth = textWidth + margin;
                            const size_t innerHeight = approxFontSize;
                            totalWidth = innerWidth + (2 * border);
                            totalHeight = innerHeight + (2 * border);
                        }
                        
                        // Apply frame offsets
                        const int overlayX = overlay->frameOffsetX;
                        const int overlayY = overlay->frameOffsetY;
                        
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

    ~com_FPS() {
        // Stop touch polling thread
        touchPollRunning.store(false, std::memory_order_release);
        threadWaitForExit(&touchPollThread);
        threadClose(&touchPollThread);

        TeslaFPS = 60;
        EndFPSCounterThread();
        FullMode = true;
        fixForeground = true;
        ult::useRightAlignment = originalUseRightAlignment;
        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().addScreenshotStacks();
        }
        deactivateOriginalFooter = false;
    }

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            // Calculate text dimensions
            const auto [textWidth, textHeight] = renderer->getTextDimensions(
                (FPSavg != 254.0) ? FPSavg_c : "--", false, fontsize
            );

            const size_t margin = (fontsize / 8);
            
            // Inner rectangle dimensions (content area)
            const size_t innerWidth = textWidth + margin;
            const size_t innerHeight = textHeight;
            
            // Total dimensions including border
            const size_t totalWidth = innerWidth + (2 * border);
            const size_t totalHeight = innerHeight + (2 * border);
            
            // Store actual dimensions for input handling
            actualTextWidth = textWidth;
            actualTotalWidth = totalWidth;
            actualTotalHeight = totalHeight;
            
            // Calculate position with frame offsets
            //int posX = frameOffsetX;
            //int posY = frameOffsetY;

            int _frameOffsetX = ult::limitedMemory ? std::max(0, frameOffsetX - (1280-448)) : frameOffsetX;
            
            // Clamp to screen bounds (accounting for total size including border)
            const int posX = std::max(int(framePadding), std::min(_frameOffsetX, static_cast<int>(screenWidth - totalWidth - framePadding)));
            const int posY = std::max(int(framePadding), std::min(frameOffsetY, static_cast<int>(screenHeight - totalHeight - framePadding)));
            
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
            
            // Calculate centered text position within the bordered area
            const int textX = posX + border + (margin / 2);
            const int textY = posY + border + (fontsize - margin);
            
            // Draw the text
            renderer->drawString(
                (FPSavg != 254.0) ? FPSavg_c : "--",
                false, 
                textX, 
                textY, 
                fontsize, 
                settings.textColor
            );
        });

        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("", "");
        rootFrame->setContent(Status);

        return rootFrame;
    }

    virtual void update() override {
        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            fontsize = settings.handheldFontSize;
        }
        else if (performanceMode == ApmPerformanceMode_Boost) {
            fontsize = settings.dockedFontSize;
        }
        if (settings.useIntegerCounter) {
            snprintf(FPSavg_c, sizeof FPSavg_c, "%d", (int)round(useOldFPSavg ? FPSavg_old : FPSavg));
        } else {
            snprintf(FPSavg_c, sizeof FPSavg_c, "%2.1f", useOldFPSavg ? FPSavg_old : FPSavg);
        }
    
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
            const size_t textWidth = fontsize * 4;
            const size_t margin = (fontsize / 8);
            const size_t innerWidth = textWidth + margin;
            const size_t innerHeight = fontsize + (margin / 2);
            totalWidth = innerWidth + (2 * border);
            totalHeight = innerHeight + (2 * border);
        }
        
        // Current overlay position
        const int overlayX = frameOffsetX;
        const int overlayY = frameOffsetY;
        
        // Touch detection area (with padding for easier interaction)
        static constexpr int touchPadding = 4;
        const int touchableX = overlayX - touchPadding;
        const int touchableY = overlayY - touchPadding;
        const int touchableWidth = totalWidth + (touchPadding * 2);
        const int touchableHeight = totalHeight + (touchPadding * 2);
        
        // Screen boundaries for clamping (accounting for total size)
        const int minX = framePadding;
        const int maxX = screenWidth - totalWidth - framePadding;
        const int minY = framePadding;
        const int maxY = screenHeight - totalHeight - framePadding;
    
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
                iniData["fps-counter"]["frame_offset_x"] = std::to_string(frameOffsetX);
                iniData["fps-counter"]["frame_offset_y"] = std::to_string(frameOffsetY);
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
            iniData["fps-counter"]["frame_offset_x"] = std::to_string(frameOffsetX);
            iniData["fps-counter"]["frame_offset_y"] = std::to_string(frameOffsetY);
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
                lastSelectedItem = "FPS Counter";
                lastMode = "";
                if (skipMain) {
                    lastMode = "return";
                    tsl::goBack();
                }
                else {
                    tsl::setNextOverlay(filepath.c_str(), "--lastSelectedItem 'FPS Counter'");
                    tsl::Overlay::get()->close();
                }
                return true;
            }
        }
        
        // Return true if we handled the input (during dragging)
        return isDragging;
    }
};
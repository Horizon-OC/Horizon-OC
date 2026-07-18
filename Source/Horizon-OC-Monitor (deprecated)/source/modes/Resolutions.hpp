class MainMenu;

class ResolutionsOverlay : public tsl::Gui {
private:
    char Resolutions_c[512] = {0};
    char Resolutions2_c[512] = {0};
    ResolutionSettings settings;
    bool skipOnce = true;
    bool runOnce = true;

    // Repositioning variables (matching Mini)
    int frameOffsetX = 0;
    int frameOffsetY = 0;
    bool isDragging = false;
    size_t framePadding = 10;
    static constexpr int screenWidth = 1280;
    static constexpr int screenHeight = 720;

    bool originalUseRightAlignment = ult::useRightAlignment;

    struct ButtonState {
        std::atomic<bool> minusDragActive{false};
        std::atomic<bool> plusDragActive{false};
    } buttonState;

    Thread touchPollThread;
    std::atomic<bool> touchPollRunning{false};

    std::atomic<bool> inputDetected{false};

public:
    ResolutionsOverlay() {
        tsl::hlp::requestForeground(false);
        disableJumpTo = true;
        GetConfigSettings(&settings);
        
        // Load saved frame offsets
        frameOffsetX = settings.frameOffsetX;
        frameOffsetY = settings.frameOffsetY;
        framePadding = settings.framePadding;
        
        if (ult::limitedMemory) {
            tsl::gfx::Renderer::get().setLayerPos(std::max(std::min((int)(frameOffsetX*1.5 + 0.5) - tsl::impl::currentUnderscanPixels.first, 1280-32 - tsl::impl::currentUnderscanPixels.first), 0), 0);
        }

        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().removeScreenshotStacks();
        }
        deactivateOriginalFooter = true;
        FullMode = false;
        TeslaFPS = settings.refreshRate;
        StartFPSCounterThread();

        // Start touch polling thread for instant response at low FPS
        touchPollRunning.store(true, std::memory_order_release);
        threadCreate(&touchPollThread, [](void* arg) -> void {
            ResolutionsOverlay* overlay = static_cast<ResolutionsOverlay*>(arg);
            
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
            
        
            while (overlay->touchPollRunning.load(std::memory_order_acquire)) {
                // Only poll when rendering and not dragging
                {
                    overlay->inputDetected.store(false, std::memory_order_release);
                    
                    // Check touch in bounds
                    if (hidGetTouchScreenStates(&state, 1) && state.count > 0) {
                        const int touchX = state.touches[0].x;
                        const int touchY = state.touches[0].y;
                        
                        // Calculate bounds (same logic as handleInput)
                        const int overlayX = overlay->frameOffsetX;
                        const int overlayY = overlay->frameOffsetY;
                        
                        // Overlay dimensions based on game state
                        int overlayWidth, overlayHeight;
                        overlayWidth = 360-20;
                        overlayHeight = 200;
                        
                        // Add touch padding
                        const int touchPadding = 4;
                        const int touchableX = overlayX - touchPadding;
                        const int touchableY = overlayY - touchPadding;
                        const int touchableWidth = overlayWidth + (touchPadding * 2);
                        const int touchableHeight = overlayHeight + (touchPadding * 2);
                        
                        // Check if touch is within bounds
                        if (touchX >= touchableX && touchX <= touchableX + touchableWidth &&
                            touchY >= touchableY && touchY <= touchableY + touchableHeight) {
                            overlay->inputDetected.store(true, std::memory_order_release);
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
                            // Long enough to start drag
                            overlay->inputDetected.store(true, std::memory_order_release);
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
                            overlay->inputDetected.store(true, std::memory_order_release);
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
                    if (overlay->inputDetected.load(std::memory_order_acquire)) {
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

    ~ResolutionsOverlay() {
        // Stop touch polling thread
        touchPollRunning.store(false, std::memory_order_release);
        threadWaitForExit(&touchPollThread);
        threadClose(&touchPollThread);

        EndFPSCounterThread();
        TeslaFPS = 60;
        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().addScreenshotStacks();
        }
        deactivateOriginalFooter = false;
        ult::useRightAlignment = originalUseRightAlignment;
        fixForeground = true;
        FullMode = true;
    }

    resolutionCalls m_resolutionRenderCalls[8] = {0};
    resolutionCalls m_resolutionViewportCalls[8] = {0};
    bool gameStart = false;
    uint8_t resolutionLookup = 0;
    u64 lastGameSeenTick = 0;
    bool waitingForGame = true;

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            int base_y = 0;
            int base_x = 0;
            int clippingOffsetX = 0, clippingOffsetY = 0;
        
            int total_width = 360 - 20;
            int total_height = 200;
        
            // Check clipping bounds (same as before)
            if (base_x + frameOffsetX < int(framePadding))
                clippingOffsetX = framePadding - (base_x + frameOffsetX);
            else if ((base_x + frameOffsetX + total_width) > static_cast<int>(screenWidth - framePadding))
                clippingOffsetX = (screenWidth - framePadding) - (base_x + frameOffsetX + total_width);
        
            if (base_y + frameOffsetY < int(framePadding))
                clippingOffsetY = framePadding - (base_y + frameOffsetY);
            else if ((base_y + frameOffsetY + total_height) > static_cast<int>(screenHeight - framePadding))
                clippingOffsetY = (screenHeight - framePadding) - (base_y + frameOffsetY + total_height);
            
            int _frameOffsetX = ult::limitedMemory ? std::max(0, frameOffsetX - (1280-448)) : frameOffsetX;

            const int final_base_x = base_x + _frameOffsetX + clippingOffsetX;
            const int final_base_y = base_y + frameOffsetY + clippingOffsetY;
        
            const tsl::Color bgColor = !isDragging ? settings.backgroundColor : settings.focusBackgroundColor;
        
            const u64 curTick = armGetSystemTick();

            // guard: ensure lastGameSeenTick is initialized to something reasonable
            if (lastGameSeenTick == 0) lastGameSeenTick = curTick;

            // threshold in ns (100 ms)
            static constexpr u64 CHECK_NS = 2000000000ULL;
        
            // Game detected
            if (gameStart && NxFps && NxFps->API >= 1 && (Resolutions_c[0] != '\0' && Resolutions2_c[0] != '\0')) {
                lastGameSeenTick = curTick;
                waitingForGame = true; // reset waiting state so next missing cycle shows "Checking..."
                renderer->drawRoundedRectSingleThreaded(final_base_x, final_base_y, total_width, total_height, 16, aWithOpacity(bgColor));
        
                int xOffset = 10;
                int yOffset = 10;
                renderer->drawString("Depth", false, xOffset + final_base_x + 20, yOffset + final_base_y + 20, 20, settings.catColor);
                renderer->drawString(Resolutions_c, false, xOffset + final_base_x + 20, yOffset + final_base_y + 55, 18, settings.textColor);
                renderer->drawString("Viewport", false, xOffset + final_base_x + 180, yOffset + final_base_y + 20, 20, settings.catColor);
                renderer->drawString(Resolutions2_c, false, xOffset + final_base_x + 180, yOffset + final_base_y + 55, 18, settings.textColor);
            }
            // Game not detected
            else {
                renderer->drawRoundedRectSingleThreaded(final_base_x, final_base_y, total_width, total_height, 16, aWithOpacity(bgColor));
        
                // Check elapsed time since last game detection
                u64 elapsed_ns = armTicksToNs(curTick - lastGameSeenTick);
                const bool under100ms = (elapsed_ns < CHECK_NS); // 100ms
        
                std::string msg;
                if (under100ms && waitingForGame)
                    msg = "Checking for game...";
                else {
                    msg = "Game is not running\nor is incompatible.";
                    waitingForGame = false;
                }
        
                const auto [textWidth, textHeight] = renderer->getTextDimensions(msg, false, 20);
                const int text_x = final_base_x + (total_width - textWidth) / 2;
                const int text_y = final_base_y + (total_height) / 2;
                
                renderer->drawString(msg, false, text_x, (under100ms && waitingForGame) ? text_y+textHeight/2 : text_y, 20, (under100ms && waitingForGame) ? 0xFFFF : 0xF00F);
            }
        });
        
        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("", "");
        rootFrame->setContent(Status);

        return rootFrame;
    }

    virtual void update() override {

        if (gameStart && NxFps) {
            if (!resolutionLookup) {
                NxFps -> renderCalls[0].calls = 0xFFFF;
                resolutionLookup = 1;
            }
            else if (resolutionLookup == 1) {
                if ((NxFps -> renderCalls[0].calls) != 0xFFFF) resolutionLookup = 2;
                else return;
            }
            memcpy(&m_resolutionRenderCalls, &(NxFps -> renderCalls), sizeof(m_resolutionRenderCalls));
            memcpy(&m_resolutionViewportCalls, &(NxFps -> viewportCalls), sizeof(m_resolutionViewportCalls));
            qsort(m_resolutionRenderCalls, 8, sizeof(resolutionCalls), compare);
            qsort(m_resolutionViewportCalls, 8, sizeof(resolutionCalls), compare);
            snprintf(Resolutions_c, sizeof Resolutions_c,
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d",
                m_resolutionRenderCalls[0].width, m_resolutionRenderCalls[0].height, m_resolutionRenderCalls[0].calls,
                m_resolutionRenderCalls[1].width, m_resolutionRenderCalls[1].height, m_resolutionRenderCalls[1].calls,
                m_resolutionRenderCalls[2].width, m_resolutionRenderCalls[2].height, m_resolutionRenderCalls[2].calls,
                m_resolutionRenderCalls[3].width, m_resolutionRenderCalls[3].height, m_resolutionRenderCalls[3].calls,
                m_resolutionRenderCalls[4].width, m_resolutionRenderCalls[4].height, m_resolutionRenderCalls[4].calls,
                m_resolutionRenderCalls[5].width, m_resolutionRenderCalls[5].height, m_resolutionRenderCalls[5].calls,
                m_resolutionRenderCalls[6].width, m_resolutionRenderCalls[6].height, m_resolutionRenderCalls[6].calls,
                m_resolutionRenderCalls[7].width, m_resolutionRenderCalls[7].height, m_resolutionRenderCalls[7].calls
            );
            snprintf(Resolutions2_c, sizeof Resolutions2_c,
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d\n"
                "%dx%d, %d",
                m_resolutionViewportCalls[0].width, m_resolutionViewportCalls[0].height, m_resolutionViewportCalls[0].calls,
                m_resolutionViewportCalls[1].width, m_resolutionViewportCalls[1].height, m_resolutionViewportCalls[1].calls,
                m_resolutionViewportCalls[2].width, m_resolutionViewportCalls[2].height, m_resolutionViewportCalls[2].calls,
                m_resolutionViewportCalls[3].width, m_resolutionViewportCalls[3].height, m_resolutionViewportCalls[3].calls,
                m_resolutionViewportCalls[4].width, m_resolutionViewportCalls[4].height, m_resolutionViewportCalls[4].calls,
                m_resolutionViewportCalls[5].width, m_resolutionViewportCalls[5].height, m_resolutionViewportCalls[5].calls,
                m_resolutionViewportCalls[6].width, m_resolutionViewportCalls[6].height, m_resolutionViewportCalls[6].calls,
                m_resolutionViewportCalls[7].width, m_resolutionViewportCalls[7].height, m_resolutionViewportCalls[7].calls
            );
        }
        if (FPSavg < 254) {
            gameStart = true;
        }
        else {
            gameStart = false;
            resolutionLookup = false;
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
    
        // Better touch detection - check if coordinates are within reasonable screen bounds
        const bool currentTouchDetected = (touchPos.x > 0 && touchPos.y > 0 && 
                                    touchPos.x < screenWidth && touchPos.y < screenHeight);
        
        static bool clearOnRelease = false;

        if (clearOnRelease && !isRendering) {
            clearOnRelease = false;
            isRendering = true;
            leventClear(&renderingStopEvent);
        }
        
        // Calculate overlay bounds
        // Cache bounds calculation
        static int cachedBaseX = 0;
        static int cachedBaseY = 0;
        static bool boundsNeedUpdate = true;
        
        // Only recalculate bounds when needed
        if (boundsNeedUpdate) {
            cachedBaseX = 0;
            cachedBaseY = 0;
            boundsNeedUpdate = false;
        }
        
        const int overlayX = cachedBaseX + frameOffsetX;
        const int overlayY = cachedBaseY + frameOffsetY;
        
        // Overlay dimensions based on game state
        int overlayWidth, overlayHeight;
        overlayWidth = 360-20;
        overlayHeight = 200;
        
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
            // Touch just released
            if (hasMoved) {
                // Save position when touch drag ends
                auto iniData = ult::getParsedDataFromIniFile(configIniPath);
                iniData["game_resolutions"]["frame_offset_x"] = std::to_string(frameOffsetX);
                iniData["game_resolutions"]["frame_offset_y"] = std::to_string(frameOffsetY);
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
            iniData["game_resolutions"]["frame_offset_x"] = std::to_string(frameOffsetX);
            iniData["game_resolutions"]["frame_offset_y"] = std::to_string(frameOffsetY);
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
                lastSelectedItem = "Game Resolutions";
                lastMode = "";
                if (skipMain) {
                    lastMode = "return";
                    tsl::goBack();
                }
                else {
                    tsl::setNextOverlay(filepath.c_str(), "--lastSelectedItem 'Game Resolutions'");
                    tsl::Overlay::get()->close();
                }
                return true;
            }
        }
        
        // Return true if we handled the input (during dragging)
        return isDragging;
    }
};
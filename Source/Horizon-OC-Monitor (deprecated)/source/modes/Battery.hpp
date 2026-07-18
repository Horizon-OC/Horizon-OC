class OtherMenu;

class BatteryOverlay : public tsl::Gui {
private:
    // Separated value buffers
    char actualCapacity_c[32] = "";
    char designedCapacity_c[32] = "";
    char batteryTemp_c[32] = "";
    char rawCharge_c[32] = "";
    char batteryAge_c[32] = "";
    char voltageAvg_c[64] = "";
    char currentFlow_c[64] = "";
    char powerFlow_c[64] = "";
    char remainingTime_c[16] = "";
    char inputCurrentLimit_c[32] = "";
    char vbusCurrentLimit_c[32] = "";
    char chargeVoltageLimit_c[32] = "";
    char chargeCurrentLimit_c[32] = "";
    char chargerType_c[32] = "";
    char chargerMaxVoltage_c[32] = "";
    char chargerMaxCurrent_c[32] = "";
    
    bool skipOnce = true;
    bool runOnce = true;
    bool isChargerConnected = false;
    FullSettings settings;
public:
    BatteryOverlay() {
        GetConfigSettings(&settings);
        disableJumpTo = true;
        mutexInit(&mutex_BatteryChecker);
        StartBatteryThread();
        // tsl::elm::g_disableMenuCacheOnReturn.store(true, std::memory_order_release);
    }
    ~BatteryOverlay() {
        CloseBatteryThread();
        fixForeground = true;
    }

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            static constexpr u16 Y_OFFSET = 40;
            static constexpr u16 X_OFFSET = 20;
            static const u16 LABEL_X = 20 + X_OFFSET;
            static const u16 VALUE_X = 240+ X_OFFSET;
            static const u16 START_Y = 155 + Y_OFFSET;
            static constexpr u16 LINE_HEIGHT = 18;
            static constexpr u8 FONT_SIZE = 18;
            static const tsl::Color LABEL_COLOR_1= settings.catColor1;
            static const tsl::Color LABEL_COLOR_2 = settings.catColor2;
            static const tsl::Color VALUE_COLOR = settings.textColor;
            
            renderer->drawString("Battery Stats", false, LABEL_X, 120 + Y_OFFSET, 20, LABEL_COLOR_1);
            
            u16 currentY = START_Y;
            
            // Actual Capacity
            renderer->drawString("Actual Capacity", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(actualCapacity_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Designed Capacity
            renderer->drawString("Designed Capacity", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(designedCapacity_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Temperature
            renderer->drawString("Temperature", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(batteryTemp_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Raw Charge
            renderer->drawString("Raw Charge", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(rawCharge_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Age
            renderer->drawString("Age", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(batteryAge_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Voltage
            renderer->drawString("Voltage", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(voltageAvg_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Current Flow
            renderer->drawString("Current Flow", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(currentFlow_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Power Flow
            renderer->drawString("Power Flow", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(powerFlow_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Remaining Time
            renderer->drawString("Remaining Time", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
            renderer->drawString(remainingTime_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            currentY += LINE_HEIGHT;
            
            // Charger-specific fields (only shown when charger is connected)
            if (isChargerConnected) {
                currentY += 3*LINE_HEIGHT;
                renderer->drawString("Charger Stats", false, LABEL_X, currentY, 20, LABEL_COLOR_1);
                currentY += 2*LINE_HEIGHT;
                // Input Current Limit
                renderer->drawString("Input Current Limit", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(inputCurrentLimit_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // VBUS Current Limit
                renderer->drawString("VBUS Current Limit", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(vbusCurrentLimit_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // Charge Voltage Limit
                renderer->drawString("Voltage Limit", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(chargeVoltageLimit_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // Charge Current Limit
                renderer->drawString("Current Limit", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(chargeCurrentLimit_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // Charger Type
                renderer->drawString("Type", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(chargerType_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // Charger Max Voltage
                renderer->drawString("Max Voltage", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(chargerMaxVoltage_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
                currentY += LINE_HEIGHT;
                
                // Charger Max Current
                renderer->drawString("Max Current", false, LABEL_X, currentY, FONT_SIZE, LABEL_COLOR_2);
                renderer->drawString(chargerMaxCurrent_c, false, VALUE_X, currentY, FONT_SIZE, VALUE_COLOR);
            }
        });

        // tsl::elm::g_disableMenuCacheOnReturn.store(true, std::memory_order_release);
        tsl::elm::HeaderOverlayFrame* rootFrame = new tsl::elm::HeaderOverlayFrame("Horizon OC Monitor", APP_VERSION, true);
        rootFrame->setContent(Status);

        return rootFrame;
    }

    virtual void update() override {
        mutexLock(&mutex_BatteryChecker);
        
        char tempBatTimeEstimate[8] = "--:--";
        if (batTimeEstimate >= 0) {
            snprintf(tempBatTimeEstimate, sizeof(tempBatTimeEstimate), "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
        }

        const BatteryChargeInfoFieldsChargerType ChargerConnected = hosversionAtLeast(17,0,0) ? 
            ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields)->ChargerType : 
            _batteryChargeInfoFields.ChargerType;
        const int32_t ChargerVoltageLimit = hosversionAtLeast(17,0,0) ? 
            ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields)->ChargerVoltageLimit : 
            _batteryChargeInfoFields.ChargerVoltageLimit;
        const int32_t ChargerCurrentLimit = hosversionAtLeast(17,0,0) ? 
            ((BatteryChargeInfoFields17*)&_batteryChargeInfoFields)->ChargerCurrentLimit : 
            _batteryChargeInfoFields.ChargerCurrentLimit;

        isChargerConnected = (ChargerConnected != 0);

        // Format all values
        snprintf(actualCapacity_c, sizeof(actualCapacity_c), "%.0f mAh", actualFullBatCapacity);
        snprintf(designedCapacity_c, sizeof(designedCapacity_c), "%.0f mAh", designedFullBatCapacity);
        snprintf(batteryTemp_c, sizeof(batteryTemp_c), "%.1f\u00B0C", (float)_batteryChargeInfoFields.BatteryTemperature / 1000);
        snprintf(rawCharge_c, sizeof(rawCharge_c), "%.1f%%", (float)_batteryChargeInfoFields.RawBatteryCharge / 1000);
        snprintf(batteryAge_c, sizeof(batteryAge_c), "%.1f%%", (float)_batteryChargeInfoFields.BatteryAge / 1000);
        snprintf(voltageAvg_c, sizeof(voltageAvg_c), "%.0f mV (%ds)", batVoltageAvg, batteryFiltered ? 45 : 5);
        snprintf(currentFlow_c, sizeof(currentFlow_c), "%+.0f mA (%ss)", batCurrentAvg, batteryFiltered ? "11.25" : "5");
        snprintf(powerFlow_c, sizeof(powerFlow_c), "%+.3f W%s", PowerConsumption, batteryFiltered ? "" : " (5s)");
        snprintf(remainingTime_c, sizeof(remainingTime_c), "%s", tempBatTimeEstimate);

        if (isChargerConnected) {
            snprintf(inputCurrentLimit_c, sizeof(inputCurrentLimit_c), "%d mA", _batteryChargeInfoFields.InputCurrentLimit);
            snprintf(vbusCurrentLimit_c, sizeof(vbusCurrentLimit_c), "%d mA", _batteryChargeInfoFields.VBUSCurrentLimit);
            snprintf(chargeVoltageLimit_c, sizeof(chargeVoltageLimit_c), "%d mV", _batteryChargeInfoFields.ChargeVoltageLimit);
            snprintf(chargeCurrentLimit_c, sizeof(chargeCurrentLimit_c), "%d mA", _batteryChargeInfoFields.ChargeCurrentLimit);
            snprintf(chargerType_c, sizeof(chargerType_c), "%u", ChargerConnected);
            snprintf(chargerMaxVoltage_c, sizeof(chargerMaxVoltage_c), "%u mV", ChargerVoltageLimit);
            snprintf(chargerMaxCurrent_c, sizeof(chargerMaxCurrent_c), "%u mA", ChargerCurrentLimit);
        }
        
        mutexUnlock(&mutex_BatteryChecker);
        
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
    
    virtual bool handleInput(uint64_t keysDown, uint64_t keysHeld, touchPosition touchInput, JoystickPosition leftJoyStick, JoystickPosition rightJoyStick) override {
        if (keysDown & KEY_B) {
            isRendering = false;
            leventSignal(&renderingStopEvent);
            triggerRumbleDoubleClick.store(true, std::memory_order_release);
            triggerExitSound.store(true, std::memory_order_release);
            skipOnce = true;
            runOnce = true;
            lastSelectedItem = "Battery/Charger";
            lastMode = "";
            tsl::swapTo<OtherMenu>();
            return true;
        }
        return false;
    }
};
class MainMenu;

class MicroOverlay : public tsl::Gui {
private:
    char GPU_Load_c[32];
    char RAM_var_compressed_c[128];
    char CPU_compressed_c[160];
    char CPU_Usage0[32];
    char CPU_Usage1[32];
    char CPU_Usage2[32];
    char CPU_Usage3[32];
    char CPU_UsageM[32];
    char soc_temperature_c[32];
    char skin_temperature_c[32];
    char FPS_var_compressed_c[64];
    char Battery_c[32];
    char CPU_volt_c[16];
    char GPU_volt_c[16];
    char RAM_volt_c[32];
    char SOC_volt_c[16];
	char CPU_temp_c[32];
    char GPU_temp_c[32];
    char RAM_temp_c[32];
    char RES_var_compressed_c[32];
    char READ_var_compressed_c[32];
    char DTC_c[32];

    static constexpr uint32_t margin = 4;

    // Performance optimization members
    bool Initialized = false;
    MicroSettings settings;
    size_t text_width = 0;
    size_t fps_width = 0;
    ApmPerformanceMode performanceMode = ApmPerformanceMode_Invalid;
    size_t fontsize = 0;
    bool showFPS = false;
    uint64_t systemtickfrequency_impl = systemtickfrequency;

    // Pre-compiled render data structures
    struct RenderItem {
        uint8_t type;
        const char* label;
        const char* data_ptr;
        const char* volt_ptr;
        bool has_voltage;
    };

    // Resolution tracking
    resolutionCalls m_resolutionRenderCalls[8] = {0};
    resolutionCalls m_resolutionViewportCalls[8] = {0};
    resolutionCalls m_resolutionOutput[8] = {0};
    uint8_t resolutionLookup = 0;
    bool resolutionShow = false;

    std::vector<RenderItem> renderItems;
    uint32_t cachedMargin = 0;
    tsl::Color catColorA = 0;
    tsl::Color textColorA = 0;
    uint32_t base_y = 0;
    bool renderDataDirty = true;

    bool skipOnce = true;
    bool runOnce = true;

    // Fixed spacing system - calculate actual widths at render time
    struct LayoutMetrics {
        uint32_t label_data_gap = 8;      // Fixed gap between label and data
        uint32_t volt_separator_gap = 0;   // Fixed gap before voltage separator
        uint32_t volt_data_gap = 0;        // Fixed gap after voltage separator
        uint32_t item_spacing = 16;        // Minimum spacing between complete items
        uint32_t side_margin = 3;          // Margins on left and right
        bool calculated = false;
    } layout;

    // Lookup table for difference symbols
    static constexpr const char* diffSymbols[4] = {"△", "@", "▽", "≠"};

    inline const char* getDifferenceSymbol(int32_t delta) {
        if (delta > 20000) return diffSymbols[0];      // △
        if (delta > -20000) return diffSymbols[1];     // @
        if (delta < -50000) return diffSymbols[3];     // ≠
        return diffSymbols[2];                         // ▽
    }

    void calculateLayoutMetrics(tsl::gfx::Renderer *renderer) {
        if (layout.calculated) return;

        // Use font size to determine appropriate spacing
        if (fontsize <= 16) {
            layout.label_data_gap = 6;
            layout.volt_separator_gap = 0;
            layout.volt_data_gap = 0;
            layout.item_spacing = 12;
        } else if (fontsize <= 20) {
            layout.label_data_gap = 8;
            layout.volt_separator_gap = 0;
            layout.volt_data_gap = 0;
            layout.item_spacing = 16;
        } else {
            layout.label_data_gap = 10;
            layout.volt_separator_gap = 0;
            layout.volt_data_gap = 0;
            layout.item_spacing = 20;
        }

        layout.calculated = true;
    }

public:
    MicroOverlay() {
        CPU_temp_c[0] = '\0';
        GPU_temp_c[0] = '\0';
        RAM_temp_c[0] = '\0';

        tsl::hlp::requestForeground(false);
        disableJumpTo = true;
        //tsl::initializeUltrahandSettings();
        GetConfigSettings(&settings);
        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            fontsize = settings.handheldFontSize;
        }
        else fontsize = settings.dockedFontSize;

        if (ult::limitedMemory && settings.setPosBottom) {
            const auto [horizontalUnderscanPixels, verticalUnderscanPixels] = tsl::gfx::getUnderscanPixels();
            tsl::gfx::Renderer::get().setLayerPos(0, !verticalUnderscanPixels ? 1038 : 1038- (tsl::cfg::ScreenHeight/720. * verticalUnderscanPixels) +0.5);
        }

        if (settings.disableScreenshots) {
            tsl::gfx::Renderer::get().removeScreenshotStacks();
        }
        mutexInit(&mutex_BatteryChecker);
        mutexInit(&mutex_Misc);
        TeslaFPS = settings.refreshRate;
        systemtickfrequency_impl /= settings.refreshRate;
        FullMode = false;
        //alphabackground = 0x0;
        deactivateOriginalFooter = true;
        StartThreads();

        // Pre-allocate render items vector
        //renderItems.reserve(8);
        realVoltsPolling = settings.realVolts;

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


    }

    ~MicroOverlay() {
        CloseThreads();
        fixForeground = true;
        FullMode = true;
    }

    // Fast parsing and render item preparation
    void prepareRenderItems() {
        if (!renderDataDirty) return;

        renderItems.clear();

        // Fast manual parsing of settings.show
        const std::string& show = settings.show;
        size_t start = 0, end = 0;
        uint8_t seen_flags = 0;

        static size_t len;
        static uint32_t key3;
        while (start < show.length()) {
            end = show.find('+', start);
            if (end == std::string::npos) end = show.length();

            len = end - start;
            if (len >= 3) {
                const char* key = &show[start];

                // Use first 3 chars for fast comparison
                key3 = (key[0] << 16) | (key[1] << 8) | key[2];

                switch (key3) {
                    case 0x435055: // "CPU"
                        if (!(seen_flags & 1)) {
                            renderItems.push_back({0, "CPU", CPU_compressed_c, CPU_volt_c, settings.realVolts});
                            seen_flags |= 1;
                        }
                        break;
                    case 0x475055: // "GPU"
                        if (!(seen_flags & 2)) {
                            renderItems.push_back({1, "GPU", GPU_Load_c, GPU_volt_c, settings.realVolts});
                            seen_flags |= 2;
                        }
                        break;
                    case 0x52414D: // "RAM"
                        if (!(seen_flags & 4)) {
                            renderItems.push_back({2, "RAM", RAM_var_compressed_c, RAM_volt_c, settings.realVolts});
                            seen_flags |= 4;
                        }
                        break;
                    case 0x534F43: // "SOC"
                        if (!(seen_flags & 8)) {
                            renderItems.push_back({3, "SOC", soc_temperature_c, SOC_volt_c, settings.realVolts && settings.showSOCVoltage});
                            seen_flags |= 8;
                        }
                        break;
                    case 0x544D50: // "TMP"
                        if (!(seen_flags & 16)) {
                            renderItems.push_back({4, "TMP", skin_temperature_c, SOC_volt_c, settings.realVolts && settings.showSOCVoltage});
                            seen_flags |= 16;
                        }
                        break;
                    case 0x524553: // "RES"
                        if (!(seen_flags & 128)) {
                            renderItems.push_back({7, "RES", RES_var_compressed_c, nullptr, false}); // We'll reuse FPS buffer temporarily
                            seen_flags |= 128;
                            resolutionShow = true;
                        }
                        break;
                    case 0x465053: // "FPS"
                        if (!(seen_flags & 32)) {
                            renderItems.push_back({5, "FPS", FPS_var_compressed_c, nullptr, false});
                            seen_flags |= 32;
                        }
                        break;
                    case 0x424154: // "BAT"
                        if (!(seen_flags & 64)) {
                            renderItems.push_back({6, "BAT", Battery_c, nullptr, false});
                            seen_flags |= 64;
                        }
                        break;
                    case 0x524541: // "REA" (for READ)
                        if (len >= 4 && key[3] == 'D' && !(seen_flags & 512)) {
                            renderItems.push_back({9, "READ", READ_var_compressed_c, nullptr, false});
                            seen_flags |= 512;
                        }
                        break;
                    case 0x445443: // "DTC"
                        if (!(seen_flags & 256) && settings.showDTC) {
                            renderItems.push_back({8, settings.useDTCSymbol ? "\uE007" : "DTC", DTC_c, nullptr, false});
                            seen_flags |= 256;
                        }
                        break;
                }
            }
            start = end + 1;
        }

        renderDataDirty = false;
    }

    virtual tsl::elm::Element* createUI() override {

        auto* Status = new tsl::elm::CustomDrawer([this](tsl::gfx::Renderer *renderer, u16 x, u16 y, u16 w, u16 h) {
            cachedMargin = renderer->getTextDimensions("CPUGPURAMSOCBAT[]", false, fontsize).second;
            if (!Initialized) {
                //cachedMargin = renderer->drawString(" ", false, 0, 0, fontsize, renderer->a(0x0000)).first;

                catColorA = settings.catColor;
                textColorA = settings.textColor;
                base_y = settings.setPosBottom ?
                    tsl::cfg::FramebufferHeight - (fontsize + (fontsize / 4)) +1: 0;
                Initialized = true;
                renderDataDirty = true;
                layout.calculated = false; // Force recalculation
                tsl::hlp::requestForeground(false);
            }

            //renderer->drawRect(0, 0, tsl::cfg::FramebufferWidth, cachedMargin + 4, a(settings.backgroundColor));
            renderer->drawRect(0, settings.setPosBottom ? base_y-1 : 0, tsl::cfg::FramebufferWidth, cachedMargin + 4, a(settings.backgroundColor));

            // Prepare render items if settings changed
            prepareRenderItems();
            calculateLayoutMetrics(renderer);

            // Separate battery from other items
            std::vector<RenderItem> main_items;
            //RenderItem* battery_item = nullptr;

            for (auto& item : renderItems) {
                //if (item.type == 6) { // BAT
                //    battery_item = &item;
                if (item.type == 5 && (!GameRunning || (strcmp(FPS_var_compressed_c, "254.0") == 0))) {
                    // Skip FPS if no game running
                    continue;
                } else if (item.type == 7 && (!GameRunning || !m_resolutionOutput[0].width)) {
                    // Skip RES if no game running or no resolution data yet
                    continue;
                } else if (item.type == 8 && !settings.showDTC) {
                    // Skip DTC if disabled in settings
                    continue;
                } else if (item.type == 9 && (!GameRunning || !NxFps)) {
                    // Skip READ if no game running or no NxFps available
                    continue;
                } else {
                    main_items.push_back(item);
                }
            }

            // Calculate actual widths for all main items
            struct ItemLayout {
                uint32_t label_width;
                uint32_t data_width;
                uint32_t volt_width;
                uint32_t total_width;
            };

            std::vector<ItemLayout> item_layouts;
            uint32_t total_main_width = 0;

            static const auto sep_width = renderer->getTextDimensions("", false, fontsize).first;

            static ItemLayout item_layout;
            for (const auto& item : main_items) {
                item_layout = {};

                // Calculate actual label width
                //auto label_dim = renderer->drawString(item.label, false, 0, 0, fontsize, renderer->a(0x0000));
                //auto label_dim = renderer->getTextDimensions(item.label, fontsize);
                item_layout.label_width = renderer->getTextDimensions(item.label, false, fontsize).first;

                // Calculate actual data width
                //auto data_dim = renderer->drawString(item.data_ptr, false, 0, 0, fontsize, renderer->a(0x0000));
                //auto data_dim = renderer->getTextDimensions(item.data_ptr, fontsize);
                item_layout.data_width = renderer->getTextDimensions(item.data_ptr, false, fontsize).first;

                // Calculate voltage width if present
                if (item.has_voltage && item.volt_ptr) {
                    //uto volt_dim = renderer->drawString(item.volt_ptr, false, 0, 0, fontsize, renderer->a(0x0000));
                    //auto volt_dim = renderer->getTextDimensions(item.volt_ptr, fontsize);
                    item_layout.volt_width = renderer->getTextDimensions(item.volt_ptr, false, fontsize).first;

                    // Total: label + gap + data + gap + "|" + gap + voltage
                    //auto sep_width = renderer->drawString("", false, 0, 0, fontsize, renderer->a(0x0000));
                    item_layout.total_width = item_layout.label_width + layout.label_data_gap +
                                            item_layout.data_width + layout.volt_separator_gap +
                                            sep_width + layout.volt_data_gap + item_layout.volt_width;
                } else {
                    // Total: label + gap + data
                    item_layout.total_width = item_layout.label_width + layout.label_data_gap + item_layout.data_width;
                }

                item_layouts.push_back(item_layout);
                total_main_width += item_layout.total_width;
            }



            // Determine if we have battery and handle it as the rightmost item
            std::vector<RenderItem> all_items_ordered;
            std::vector<ItemLayout> all_layouts_ordered;

            // Add main items first
            for (size_t i = 0; i < main_items.size(); i++) {
                all_items_ordered.push_back(main_items[i]);
                all_layouts_ordered.push_back(item_layouts[i]);
            }

            // Add battery as the last item if present
            //if (battery_item) {
            //    //auto bat_label_dim = renderer->drawString("BAT", false, 0, 0, fontsize, renderer->a(0x0000));
            //    //auto bat_label_dim = renderer->getTextDimensions("BAT", fontsize);
            //    //auto bat_data_dim = renderer->drawString(battery_item->data_ptr, false, 0, 0, fontsize, renderer->a(0x0000));
            //    //auto bat_data_dim = renderer->getTextDimensions(battery_item->data_ptr, fontsize);
            //
            //    ItemLayout battery_layout = {};
            //    battery_layout.label_width = renderer->getTextDimensions("BAT", false, fontsize).first;
            //    battery_layout.data_width = renderer->getTextDimensions(battery_item->data_ptr, false, fontsize).first;
            //    battery_layout.volt_width = 0;
            //    battery_layout.total_width = battery_layout.label_width + layout.label_data_gap + battery_layout.data_width;
            //
            //    all_items_ordered.push_back(*battery_item);
            //    all_layouts_ordered.push_back(battery_layout);
            //}

            // Calculate total width of all items
            uint32_t total_all_width = 0;
            for (const auto& item_layout : all_layouts_ordered) {
                total_all_width += item_layout.total_width;
            }

            // Calculate available space for distribution
            //uint32_t available_width = tsl::cfg::FramebufferWidth - (2 * layout.side_margin);
            //uint32_t remaining_space = available_width - total_all_width;

            // Calculate positions based on alignment mode
            std::vector<uint32_t> item_positions;
            const size_t N = all_items_ordered.size();
            if (N == 0) return;

            if (N == 1) {
                // Single item positioning based on alignment
                if (settings.alignTo == 2) { // RIGHT
                    const uint32_t total_width = all_layouts_ordered[0].total_width;
                    item_positions.push_back(tsl::cfg::FramebufferWidth - layout.side_margin - total_width);
                } else { // LEFT or CENTER
                    item_positions.push_back(layout.side_margin);
                }
            } else {
                uint32_t total_widths = 0;
                for (const auto& layout : all_layouts_ordered) {
                    total_widths += layout.total_width;
                }

                if (settings.alignTo == 0) { // LEFT alignment
                    // All items except last positioned from left with small gaps
                    // Last item (battery if present) positioned at far right
                    const uint32_t small_gap = layout.item_spacing;

                    // Position items from left
                    uint32_t current_x = layout.side_margin;
                    for (size_t i = 0; i < N - 1; ++i) {
                        item_positions.push_back(current_x);
                        current_x += all_layouts_ordered[i].total_width + small_gap;
                    }

                    // Position last item at far right
                    const uint32_t last_width = all_layouts_ordered[N-1].total_width;
                    item_positions.push_back(tsl::cfg::FramebufferWidth - layout.side_margin - last_width);

                } else if (settings.alignTo == 2) { // RIGHT alignment
                    // First item at far left, remaining items packed at right
                    const uint32_t small_gap = layout.item_spacing;

                    // Resize vector to hold all positions
                    item_positions.resize(N);

                    // Position first item at far left
                    item_positions[0] = layout.side_margin;

                    // Calculate total width of items 1 to N-1 plus gaps between them
                    uint32_t right_group_width = 0;
                    for (size_t i = 1; i < N; ++i) {
                        right_group_width += all_layouts_ordered[i].total_width;
                        if (i < N - 1) right_group_width += small_gap; // Gap after each item except the last
                    }

                    // Start positioning from right margin minus total width of right group
                    uint32_t current_x = tsl::cfg::FramebufferWidth - layout.side_margin - right_group_width;

                    // Position items 1 to N-1 sequentially from left to right within the right group
                    for (size_t i = 1; i < N; ++i) {
                        item_positions[i] = current_x;
                        current_x += all_layouts_ordered[i].total_width + small_gap;
                    }
                } else { // CENTER alignment (default behavior)
                    // Total available width for spacing = framebuffer width minus total item widths minus margins
                    const int32_t total_spacing = (int32_t)tsl::cfg::FramebufferWidth - (2 * (int32_t)layout.side_margin) - (int32_t)total_widths;

                    // Number of gaps between items is N-1
                    const uint32_t gap = total_spacing > 0 ? (uint32_t)(total_spacing / (N - 1)) : 0;

                    // Position first item flush left
                    item_positions.push_back(layout.side_margin);
                    static uint32_t prev_pos, prev_width;
                    // Position subsequent items
                    for (size_t i = 1; i < N; ++i) {
                        prev_pos = item_positions[i - 1];
                        prev_width = all_layouts_ordered[i - 1].total_width;
                        item_positions.push_back(prev_pos + prev_width + gap);
                    }

                    // Fix any rounding error for center alignment
                    const int32_t last_item_end = item_positions.back() + all_layouts_ordered.back().total_width;
                    const int32_t overflow = (int32_t)tsl::cfg::FramebufferWidth - layout.side_margin - last_item_end;

                    if (overflow != 0) {
                        for (size_t i = 1; i < item_positions.size(); ++i) {
                            item_positions[i] += overflow;
                        }
                    }
                }
            }


            uint32_t current_x;
            static std::vector<std::string> specialChars = {""};

            // Render all items at calculated positions
            for (size_t i = 0; i < all_items_ordered.size(); i++) {
                const auto& item = all_items_ordered[i];
                const auto& item_layout = all_layouts_ordered[i];
                current_x = item_positions[i];

                // Draw label
                renderer->drawString(item.label, false, current_x, base_y + cachedMargin, fontsize, catColorA);
                current_x += item_layout.label_width + layout.label_data_gap;

                // Draw data
                //renderer->drawString(item.data_ptr, false, current_x, base_y + fontsize, fontsize, textColorA);

                if (settings.useDynamicColors) {
                    // Draw data with temperature gradient support
					if (item.type == 0) { // CPU
						std::string dataStr(item.data_ptr);
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

								uint32_t renderX = current_x;
								if (!preTempPart.empty()) {
								renderer->drawStringWithColoredSections(preTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
								renderX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
								}

								renderer->drawStringWithColoredSections(tempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, tempColor, a(settings.separatorColor));

								if (!postTempPart.empty()) {
								renderX += renderer->getTextDimensions(tempPart, false, fontsize).first;
								renderer->drawStringWithColoredSections(postTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
							}
						} else {
							renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
						}
					} else {
						renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
					}
				} else {
					renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
				}

					} else if (item.type == 1) { // GPU
						std::string dataStr(item.data_ptr);
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

								uint32_t renderX = current_x;
								if (!preTempPart.empty()) {
								renderer->drawStringWithColoredSections(preTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
								renderX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
								}

								renderer->drawStringWithColoredSections(tempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, tempColor, a(settings.separatorColor));

								if (!postTempPart.empty()) {
								renderX += renderer->getTextDimensions(tempPart, false, fontsize).first;
								renderer->drawStringWithColoredSections(postTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
							}
						} else {
							renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
						}
					} else {
						renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
					}
				} else {
					renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
				}

					} else if (item.type == 2) { // RAM
					std::string dataStr(item.data_ptr);
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

							uint32_t renderX = current_x;
							if (!preTempPart.empty()) {
							renderer->drawStringWithColoredSections(preTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
							renderX += renderer->getTextDimensions(preTempPart, false, fontsize).first;
							}

							renderer->drawStringWithColoredSections(tempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, tempColor, a(settings.separatorColor));

							if (!postTempPart.empty()) {
							renderX += renderer->getTextDimensions(tempPart, false, fontsize).first;
							renderer->drawStringWithColoredSections(postTempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
						}
					} else {
						renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
					}
				} else {
					renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
					}
			} else {
					renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
			}

					} else  if (item.type == 3) { // SOC temperature
                        // Parse SOC temperature: "XX°C (XX%)"
                        std::string dataStr(item.data_ptr);
                        const size_t degreesPos = dataStr.find("°");
                        if (degreesPos != std::string::npos) {
                            const size_t cPos = dataStr.find("C", degreesPos);
                            if (cPos != std::string::npos) {
                                const size_t tempEnd = cPos + 1; // Include the 'C'

                                // Extract temperature value and apply gradient
                                const int temp = atoi(item.data_ptr);
                                const tsl::Color tempColor = tsl::GradientColor((float)temp);

                                // Split into temperature part and remaining part
                                const std::string tempPart = dataStr.substr(0, tempEnd);
                                const std::string restPart = dataStr.substr(tempEnd);

                                // Render temperature with gradient color
                                renderer->drawString(tempPart, false, current_x, base_y + cachedMargin, fontsize, tempColor);

                                // Render remaining text with normal color
                                if (!restPart.empty()) {
                                    const uint32_t tempPartWidth = renderer->getTextDimensions(tempPart, false, fontsize).first;
                                    renderer->drawStringWithColoredSections(restPart, false, specialChars, current_x + tempPartWidth, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                                }
                            } else {
                                // Fallback: render normally
                                renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                            }
                        } else {
                            // Fallback: render normally
                            renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                        }

                    } else if (item.type == 4) { // TMP multiple temperatures
                        // Parse TMP temperatures: "XX°C XX°C XX°C (XX%)"
                        std::string dataStr(item.data_ptr);
                        uint32_t renderX = current_x;
                        size_t pos = 0;
                        bool parseSuccess = true;

                        // Parse up to 3 temperatures
                        for (int tempCount = 0; tempCount < 3 && parseSuccess && pos < dataStr.length(); tempCount++) {
                            // Skip any leading spaces
                            while (pos < dataStr.length() && dataStr[pos] == ' ') {
                                renderer->drawString(" ", false, renderX, base_y + cachedMargin, fontsize, textColorA);
                                renderX += renderer->getTextDimensions(" ", false, fontsize).first;
                                pos++;
                            }

                            if (pos >= dataStr.length()) break;

                            // Find degrees symbol
                            const size_t degreesPos = dataStr.find("°", pos);
                            if (degreesPos == std::string::npos) {
                                parseSuccess = false;
                                break;
                            }

                            // Find 'C' after degrees symbol
                            const size_t cPos = dataStr.find("C", degreesPos);
                            if (cPos == std::string::npos) {
                                parseSuccess = false;
                                break;
                            }

                            const size_t tempEnd = cPos + 1; // Include the 'C'

                            // Extract and render temperature with gradient
                            const std::string tempPart = dataStr.substr(pos, tempEnd - pos);
                            const int temp = atoi(tempPart.c_str());
                            const tsl::Color tempColor = tsl::GradientColor((float)temp);

                            renderer->drawStringWithColoredSections(tempPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, tempColor, a(settings.separatorColor));
                            renderX += renderer->getTextDimensions(tempPart, false, fontsize).first;

                            pos = tempEnd;
                        }

                        // Render any remaining text (like " (50%)")
                        if (pos < dataStr.length()) {
                            const std::string restPart = dataStr.substr(pos);
                            renderer->drawStringWithColoredSections(restPart, false, specialChars, renderX, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                        }

                        // If parsing failed, fall back to normal rendering
                        if (!parseSuccess) {
                            renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                        }

                    } else {
                        // Normal rendering for all other item types
                        renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                    }
                } else {
                    // Normal rendering for all other item types
                    renderer->drawStringWithColoredSections(item.data_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA, a(settings.separatorColor));
                }
                current_x += item_layout.data_width;

                // Draw voltage if present
                if (item.has_voltage && item.volt_ptr) {
                    current_x += layout.volt_separator_gap;
                    renderer->drawString("", false, current_x, base_y + cachedMargin, fontsize, a(settings.separatorColor));
                    //auto sep_width = renderer->drawString("", false, 0, 0, fontsize, renderer->a(0x0000));
                    //auto sep_width = renderer->getTextDimensions("", fontsize);
                    current_x += sep_width + layout.volt_data_gap;
                    renderer->drawStringWithColoredSections(item.volt_ptr, false, specialChars, current_x, base_y + cachedMargin, fontsize, textColorA,  a(settings.separatorColor));
                }
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
                //Assign NX-FPS to default core
                threadCreate(&t6, CheckIfGameRunning, NULL, NULL, 0x1000, 0x38, -2);
                threadStart(&t6);
            }
        }

        //static bool triggerExit = false;
        //if (triggerExit) {
        //    ult::setIniFileValue(
        //        ult::ULTRAHAND_CONFIG_INI_PATH,
        //        ult::ULTRAHAND_PROJECT_NAME,
        //        ult::IN_OVERLAY_STR,
        //        ult::FALSE_STR
        //    );
        //    tsl::setNextOverlay(
        //        ult::OVERLAY_PATH + "ovlmenu.ovl"
        //    );
        //    tsl::Overlay::get()->close();
        //    return;
        //}

        apmGetPerformanceMode(&performanceMode);
        if (performanceMode == ApmPerformanceMode_Normal) {
            if (fontsize != settings.handheldFontSize) {
                Initialized = false;
                layout.calculated = false; // Recalculate layout for new font size
                fontsize = settings.handheldFontSize;
            }
        }
        else if (performanceMode == ApmPerformanceMode_Boost) {
            if (fontsize != settings.dockedFontSize) {
                Initialized = false;
                layout.calculated = false; // Recalculate layout for new font size
                fontsize = settings.dockedFontSize;
            }
        }

        // CPU usage calculations - optimized with fewer conditionals
        const double inv_freq = 1.0 / systemtickfrequency_impl;

        // Capture systemtickfrequency_impl and inv_freq safely
        const auto formatUsage = [this](char* buf, size_t size, uint64_t idletick, double inv_freq) {
            if (idletick > systemtickfrequency_impl) {
                strcpy(buf, "0%");
            } else {
                snprintf(buf, size, "%.0f%%", (1.0 - (idletick * inv_freq)) * 100.0);
            }
        };

        // Atomically load idle ticks before using them
        const uint64_t idle0 = idletick0.load(std::memory_order_acquire);
        const uint64_t idle1 = idletick1.load(std::memory_order_acquire);
        const uint64_t idle2 = idletick2.load(std::memory_order_acquire);
        const uint64_t idle3 = idletick3.load(std::memory_order_acquire);

        formatUsage(CPU_Usage0, sizeof(CPU_Usage0), idle0, inv_freq);
        formatUsage(CPU_Usage1, sizeof(CPU_Usage1), idle1, inv_freq);
        formatUsage(CPU_Usage2, sizeof(CPU_Usage2), idle2, inv_freq);
        formatUsage(CPU_Usage3, sizeof(CPU_Usage3), idle3, inv_freq);

        mutexLock(&mutex_Misc);

        // CPU frequency and voltage
        const char* cpuDiff = "@";
        if (realCPU_Hz) {
            const int32_t deltaCPU = (int32_t)(realCPU_Hz / 1000) - (CPU_Hz / 1000);
            cpuDiff = getDifferenceSymbol(deltaCPU);
        }

        const uint32_t cpuFreq = settings.realFrequencies && realCPU_Hz ? realCPU_Hz : CPU_Hz;

        if (settings.showFullCPU) {
            snprintf(CPU_compressed_c, sizeof(CPU_compressed_c),
                "[%s,%s,%s,%s]%s%u.%u",
                CPU_Usage0, CPU_Usage1, CPU_Usage2, CPU_Usage3,
                cpuDiff, cpuFreq / 1000000, (cpuFreq / 100000) % 10);
        } else {
            // Find max CPU usage across all cores
            const auto extractUsage = [](const char* usage_str) -> double {
                return strtod(usage_str, nullptr);
            };

            const double usage0 = extractUsage(CPU_Usage0);
            const double usage1 = extractUsage(CPU_Usage1);
            const double usage2 = extractUsage(CPU_Usage2);
            const double usage3 = extractUsage(CPU_Usage3);

            const double maxUsage = std::max({usage0, usage1, usage2, usage3});

            snprintf(CPU_compressed_c, sizeof(CPU_compressed_c),
                "%.0f%%%s%u.%u",
                maxUsage, cpuDiff, cpuFreq / 1000000, (cpuFreq / 100000) % 10);
        }

		 if (settings.realTemps && realCPU_Temp != 0 && CPU_temp_c[0] != '\0') {
            char temp_buffer[48];
            snprintf(temp_buffer, sizeof(temp_buffer), " %s", CPU_temp_c);
            strncat(CPU_compressed_c, temp_buffer, sizeof(CPU_compressed_c) - strlen(CPU_compressed_c) - 1);
        }

        //if (settings.realVolts) {
        //    snprintf(CPU_volt_c, sizeof(CPU_volt_c), "%u.%u mV",
        //        realCPU_mV/1000, (isMariko ? (realCPU_mV/100)%10 : (realCPU_mV/10)%100));
        //}

        /* ── CPU voltage ───────────────────────────── */
        if (settings.realVolts) {
            const uint32_t mv = realCPU_mV / 1000;                 // µV → mV
            snprintf(CPU_volt_c, sizeof(CPU_volt_c), "%u mV", mv);
        }

        // GPU frequency and voltage
        const char* gpuDiff = "@";
        if (realGPU_Hz) {
            const int32_t deltaGPU = (int32_t)(realGPU_Hz / 1000) - (GPU_Hz / 1000);
            gpuDiff = getDifferenceSymbol(deltaGPU);
        }

        const uint32_t gpuFreq = settings.realFrequencies && realGPU_Hz ? realGPU_Hz : GPU_Hz;
        snprintf(GPU_Load_c, sizeof(GPU_Load_c),
                 "%u%%%s%u.%u",
                 GPU_Load_u / 10,
                 gpuDiff, gpuFreq / 1000000, (gpuFreq / 100000) % 10);

		if (settings.realTemps && realGPU_Temp != 0 && GPU_temp_c[0] != '\0') {
            char temp_buffer[48];
            snprintf(temp_buffer, sizeof(temp_buffer), " %s", GPU_temp_c);
            strncat(GPU_Load_c, temp_buffer, sizeof(GPU_Load_c) - strlen(GPU_Load_c) - 1);
        }

        //if (settings.realVolts) {
        //    snprintf(GPU_volt_c, sizeof(GPU_volt_c), "%u.%u mV",
        //        realGPU_mV/1000, (isMariko ? (realGPU_mV/100)%10 : (realGPU_mV/10)%100));
        //}

        // For properly handling sleep exit
        if (settings.sleepExit) {
            const auto GPU_Hz_int = int(GPU_Hz / 1000000);
            static auto lastGPU_Hz_int = GPU_Hz_int;
            if (GPU_Hz_int == 0 && lastGPU_Hz_int != 0) {
                isRendering = false;
                leventSignal(&renderingStopEvent);

                triggerExitNow = true;
                return;
            }
            lastGPU_Hz_int = GPU_Hz_int;
        }


        /* ── GPU voltage ───────────────────────────── */
        if (settings.realVolts) {
            const uint32_t mv = realGPU_mV / 1000;
            snprintf(GPU_volt_c, sizeof(GPU_volt_c), "%u mV", mv);
        }

        // RAM usage and frequency
        char MICRO_RAM_all_c[16];
        if (!settings.showpartLoad) {
            // User wants GB display
            const float RAM_Total_all_f = (RAM_Total_application_u + RAM_Total_applet_u + RAM_Total_system_u + RAM_Total_systemunsafe_u) / (1024.0f * 1024.0f * 1024.0f);
            const float RAM_Used_all_f = (RAM_Used_application_u + RAM_Used_applet_u + RAM_Used_system_u + RAM_Used_systemunsafe_u) / (1024.0f * 1024.0f * 1024.0f);
            snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%.0f%.0fGB", RAM_Used_all_f, RAM_Total_all_f);
        } else if (settings.ramInfoMode == "Bandwidth" && R_SUCCEEDED(hocclkCheck)) {
            // Bandwidth mode: show GB/s from context (partLoad values are in MB/s)
            const unsigned bwAll  = partLoad[HocClkPartLoad_RamBWAll] / 1000;
            const unsigned bwAllD = (partLoad[HocClkPartLoad_RamBWAll] % 1000) / 100;
            snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%u.%uGB/s", bwAll, bwAllD);
        } else {
            // User wants percentage display
            if (R_SUCCEEDED(hocclkCheck)) {
                // Use sys-clk's RAM load if available
                snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%hu%%",
                         partLoad[HocClkPartLoad_EMC] / 10);
            } else {
                // Calculate percentage manually when sys-clk isn't available
                const uint64_t RAM_Total_all = RAM_Total_application_u + RAM_Total_applet_u + RAM_Total_system_u + RAM_Total_systemunsafe_u;
                const uint64_t RAM_Used_all = RAM_Used_application_u + RAM_Used_applet_u + RAM_Used_system_u + RAM_Used_systemunsafe_u;
                const unsigned partLoadPercent = (RAM_Total_all > 0) ? (unsigned)((RAM_Used_all * 100) / RAM_Total_all) : 0;
                snprintf(MICRO_RAM_all_c, sizeof(MICRO_RAM_all_c), "%u%%", partLoadPercent);
            }
        }

        const char* ramDiff = "@";
        if (realRAM_Hz) {
            const int32_t deltaRAM = (int32_t)(realRAM_Hz / 1000) - (RAM_Hz / 1000);
            ramDiff = getDifferenceSymbol(deltaRAM);
        }

        const uint32_t ramFreq = settings.realFrequencies && realRAM_Hz ? realRAM_Hz : RAM_Hz;
        snprintf(RAM_var_compressed_c, sizeof(RAM_var_compressed_c),
            "%s%s%u.%u", MICRO_RAM_all_c, ramDiff,
            ramFreq / 1000000, (ramFreq / 100000) % 10);

		if (settings.realTemps && realRAM_Temp != 0 && RAM_temp_c[0] != '\0') {
            char temp_buffer[48];
            snprintf(temp_buffer, sizeof(temp_buffer), " %s", RAM_temp_c);
            strncat(RAM_var_compressed_c, temp_buffer, sizeof(RAM_var_compressed_c) - strlen(RAM_var_compressed_c) - 1);
        }

        //if (settings.realVolts) {
        //    uint32_t vdd2 = realRAM_mV / 10000;
        //    uint32_t vddq = realRAM_mV % 10000;
        //    if (isMariko) {
        //        snprintf(RAM_volt_c, sizeof(RAM_volt_c), "%u.%u%u.%u mV",
        //            vdd2/10, vdd2%10, vddq/10, vddq%10);
        //    } else {
        //        snprintf(RAM_volt_c, sizeof(RAM_volt_c), "%u.%u mV", vdd2/10, vdd2%10);
        //    }
        //}

        /* ── RAM voltage ───────────────────────────── */
        if (settings.realVolts && (settings.showVDD2 || settings.showVDDQ)) {
            /* realRAM_mV packs VDD2 | VDDQ in 10-µV units        *
             * → split, convert to mV
                                    */
            const float mv_vdd2 = realVDD2_mV / 1000.0f;   // VDD2
            const uint32_t mv_vddq = realVDDQ_mV / 1000;   // VDDQ

            // Build voltage string based on settings
            RAM_volt_c[0] = '\0'; // Start with empty string
            char temp_buffer[16];

            if (settings.showVDD2) {
                if (settings.decimalVDD2) {
                    snprintf(temp_buffer, sizeof(temp_buffer), "%.1f mV", mv_vdd2);
                } else {
                    snprintf(temp_buffer, sizeof(temp_buffer), "%u mV", (uint32_t)mv_vdd2);
                }
                strcat(RAM_volt_c, temp_buffer);
            }

            if (settings.showVDDQ && isMariko) {
                if (RAM_volt_c[0] != '\0') {
                    strcat(RAM_volt_c, "");
                }
                snprintf(temp_buffer, sizeof(temp_buffer), "%u mV", mv_vddq);
                strcat(RAM_volt_c, temp_buffer);
            }
        } else {
            RAM_volt_c[0] = '\0'; // Empty if voltages disabled
        }

        /* ── Battery / power draw ───────────────────────────── */
        char remainingBatteryLife[8];

        /* Normalise “-0.00” → “0.00” W */
        const float drawW = (fabsf(PowerConsumption) < 0.01f) ? 0.0f
                                                         : PowerConsumption;

        mutexLock(&mutex_BatteryChecker);

        /* show a time only when the estimate is valid **and** draw ≥ 0.01 W */
        if (batTimeEstimate >= 0 && !(drawW <= 0.01f && drawW >= -0.01f)) {
            snprintf(remainingBatteryLife, sizeof remainingBatteryLife,
                     "%d:%02d", batTimeEstimate / 60, batTimeEstimate % 60);
        } else {
            strcpy(remainingBatteryLife, "--:--");
        }

        if (!settings.invertBatteryDisplay) {
            snprintf(Battery_c, sizeof Battery_c,
                     "%.2f W%.1f%% [%s]",
                     drawW,
                     (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                     remainingBatteryLife);
        } else {
            snprintf(Battery_c, sizeof Battery_c,
                     "%.1f%% [%s]%.2f W",
                     (float)_batteryChargeInfoFields.RawBatteryCharge / 1000.0f,
                     remainingBatteryLife,
                     drawW);
        }

        mutexUnlock(&mutex_BatteryChecker);


        // Format current datetime for DTC
        if (settings.showDTC) {
            time_t rawtime = time(NULL);
            struct tm *timeinfo = localtime(&rawtime);
            strftime(DTC_c, sizeof(DTC_c), settings.dtcFormat.c_str(), timeinfo);
        }

        // Thermal info
        const int duty = safeFanDuty((int)Rotation_Duty);

        /* Integer SoC temperature + duty */
        snprintf(soc_temperature_c, sizeof soc_temperature_c,
                 "%d°C %d%%",
                 (int)SOC_temperatureF,          // SoC °C, no decimals
                 duty);                          // fan %

        /* Integer SOC, PCB and skin temperatures + duty                    *
         *  skin_temperaturemiliC is in milli-degrees C → divide by 1000     */
        snprintf(skin_temperature_c, sizeof skin_temperature_c,
                 "%d°C %d°C %hu°C %d%%",
                 (int)SOC_temperatureF,          // SoC
                 (int)PCB_temperatureF,          // PCB
                 (uint16_t)(skin_temperaturemiliC / 1000), // skin
                 duty);

        //if (settings.realVolts) {
        //    snprintf(SOC_volt_c, sizeof(SOC_volt_c), "%u.%u mV",
        //        realSOC_mV/1000, (realSOC_mV/100)%10);
        //}

        /* ── SoC voltage ───────────────────────────── */
        if (settings.realVolts && settings.showSOCVoltage) {
            const uint32_t mv = realSOC_mV / 1000;
            snprintf(SOC_volt_c, sizeof(SOC_volt_c), "%u mV", mv);
        } else {
            SOC_volt_c[0] = '\0'; // Clear the buffer when disabled
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

        // Resolution processing
        //char RES_var_compressed_c[32] = "";
        if (GameRunning && NxFps && resolutionShow) {
            if (!resolutionLookup) {
                if (NxFps && SharedMemoryUsed) {
                    (NxFps -> renderCalls[0].calls) = 0xFFFF;
                    resolutionLookup = 1;
                }
            }
            else if (resolutionLookup == 1) {
                if (NxFps && SharedMemoryUsed && (NxFps -> renderCalls[0].calls) != 0xFFFF) {
                    resolutionLookup = 2;
                }
            }
            else {
                if (NxFps && SharedMemoryUsed) {
                    memcpy(&m_resolutionRenderCalls, &(NxFps -> renderCalls), sizeof(m_resolutionRenderCalls));
                    memcpy(&m_resolutionViewportCalls, &(NxFps -> viewportCalls), sizeof(m_resolutionViewportCalls));
                } else {
                    memset(&m_resolutionRenderCalls, 0, sizeof(m_resolutionRenderCalls));
                    memset(&m_resolutionViewportCalls, 0, sizeof(m_resolutionViewportCalls));
                }
                qsort(m_resolutionRenderCalls, 8, sizeof(resolutionCalls), compare);
                qsort(m_resolutionViewportCalls, 8, sizeof(resolutionCalls), compare);
                memset(&m_resolutionOutput, 0, sizeof(m_resolutionOutput));
                size_t out_iter = 0;
                bool found = false;
                for (size_t i = 0; i < 8; i++) {
                    for (size_t x = 0; x < 8; x++) {
                        if (m_resolutionRenderCalls[i].width == 0) {
                            break;
                        }
                        if ((m_resolutionRenderCalls[i].width == m_resolutionViewportCalls[x].width) && (m_resolutionRenderCalls[i].height == m_resolutionViewportCalls[x].height)) {
                            m_resolutionOutput[out_iter].width = m_resolutionRenderCalls[i].width;
                            m_resolutionOutput[out_iter].height = m_resolutionRenderCalls[i].height;
                            m_resolutionOutput[out_iter].calls = (m_resolutionRenderCalls[i].calls > m_resolutionViewportCalls[x].calls) ? m_resolutionRenderCalls[i].calls : m_resolutionViewportCalls[x].calls;
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
                            if (m_resolutionViewportCalls[x].width == 0) {
                                break;
                            }
                            if ((m_resolutionViewportCalls[x].width == m_resolutionOutput[y].width) && (m_resolutionViewportCalls[x].height == m_resolutionOutput[y].height)) {
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

                // Anti-flicker swap logic
                static std::pair<uint16_t, uint16_t> old_res[2];

                // Only swap if BOTH resolutions exist (prevent swapping with empty slot)
                if (m_resolutionOutput[0].width && m_resolutionOutput[1].width) {
                    if ((m_resolutionOutput[0].width == old_res[1].first && m_resolutionOutput[0].height == old_res[1].second) ||
                        (m_resolutionOutput[1].width == old_res[0].first && m_resolutionOutput[1].height == old_res[0].second)) {
                        const uint16_t swap_width = m_resolutionOutput[0].width;
                        const uint16_t swap_height = m_resolutionOutput[0].height;
                        m_resolutionOutput[0].width = m_resolutionOutput[1].width;
                        m_resolutionOutput[0].height = m_resolutionOutput[1].height;
                        m_resolutionOutput[1].width = swap_width;
                        m_resolutionOutput[1].height = swap_height;
                    }
                }

                // Format resolution string
                if (m_resolutionOutput[0].width) {
                    if (settings.showFullResolution) {
                        if (!m_resolutionOutput[1].width) {
                            snprintf(RES_var_compressed_c, sizeof(RES_var_compressed_c), "%dx%d",
                                m_resolutionOutput[0].width, m_resolutionOutput[0].height);
                        }
                        else {
                            snprintf(RES_var_compressed_c, sizeof(RES_var_compressed_c), "%dx%d%dx%d",
                                m_resolutionOutput[0].width, m_resolutionOutput[0].height,
                                m_resolutionOutput[1].width, m_resolutionOutput[1].height);
                        }
                    } else {
                        if (!m_resolutionOutput[1].width) {
                            snprintf(RES_var_compressed_c, sizeof(RES_var_compressed_c), "%dp",
                                m_resolutionOutput[0].height);
                        }
                        else {
                            snprintf(RES_var_compressed_c, sizeof(RES_var_compressed_c), "%dp%dp",
                                m_resolutionOutput[0].height, m_resolutionOutput[1].height);
                        }
                    }
                }

                // Always store current resolutions for next frame comparison
                old_res[0] = std::make_pair(m_resolutionOutput[0].width, m_resolutionOutput[0].height);
                old_res[1] = std::make_pair(m_resolutionOutput[1].width, m_resolutionOutput[1].height);
            }
        }
        else if (!GameRunning && resolutionLookup != 0) {
            resolutionLookup = 0;
        }

        // FPS
        snprintf(FPS_var_compressed_c, sizeof FPS_var_compressed_c, "%2.1f", useOldFPSavg ? FPSavg_old : FPSavg);


        // Read Speed
        if (GameRunning && NxFps && SharedMemoryUsed) {
            float readSpeed = 0.0f;
            memcpy(&readSpeed, &(NxFps->readSpeedPerSecond), sizeof(float));
            if (readSpeed != 0.f) {
                snprintf(READ_var_compressed_c, sizeof(READ_var_compressed_c), "%.2f MiB/s", readSpeed / 1048576.f);
            } else {
                strcpy(READ_var_compressed_c, "n/d");
            }
        } else {
            strcpy(READ_var_compressed_c, "n/d");
        }

        mutexUnlock(&mutex_Misc);

        //static bool skipOnce = true;

        if (!skipOnce) {
            //static bool runOnce = true;
            if (runOnce) {
                isRendering = true;
                leventClear(&renderingStopEvent);
                runOnce = false;  // Add this to prevent repeated calls
            }
        } else {
            skipOnce = false;
        }
    }

    virtual bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState joyStickPosLeft, HidAnalogStickState joyStickPosRight) override {
        if (isKeyComboPressed(keysHeld, keysDown)) {
            isRendering = false;
            leventSignal(&renderingStopEvent);
            //triggerRumbleDoubleClick.store(true, std::memory_order_release);
            skipOnce = true;
            runOnce = true;
            //TeslaFPS = 60;
            if (skipMain) {
                //lastSelectedItem = "Micro";
                lastMode = "returning";
                //tsl::swapTo<MainMenu>();
                tsl::goBack();
            }
            else {
                tsl::setNextOverlay(filepath.c_str(), "--lastSelectedItem Micro");
                tsl::Overlay::get()->close();
            }
            return true;
        }
        return false;
    }
};
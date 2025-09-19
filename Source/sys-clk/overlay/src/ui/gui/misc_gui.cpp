#include "misc_gui.h"
#include "fatal_gui.h"
#include "../format.h"
#include <cstdio>
#include <cstring>
//#include <sstream>

MiscGui::MiscGui()
{
    
    // Load current config values
    configValues["uncapped_clocks"] = getConfigValue("uncapped_clocks");
    configValues["override_boost_mode"] = getConfigValue("override_boost_mode");
    configValues["auto_cpu_boost"] = getConfigValue("auto_cpu_boost");
    configValues["sync_reversenx"] = getConfigValue("sync_reversenx");
    // gpu_dvfs is handled separately as it's now a trackbar with integer values
}

MiscGui::~MiscGui()
{
    this->configToggles.clear();
}

bool MiscGui::getConfigValue(const std::string& iniKey)
{
    FILE* file = fopen("/config/sys-clk/config.ini", "r");
    if (!file) {
        // Return default values if file doesn't exist
        return (iniKey == "gpu_dvfs"); // gpu_dvfs defaults to true, others default to false
    }
    
    char line[512];
    bool inValuesSection = false;

    size_t len;
    while (fgets(line, sizeof(line), file)) {
        // Remove newline if present
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Trim whitespace
        char* start = line;
        while (*start == ' ' || *start == '\t') start++;
        char* end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        // Check for [values] section
        if (strcmp(start, "[values]") == 0) {
            inValuesSection = true;
            continue;
        }
        
        // Check for new section
        if (strlen(start) > 0 && start[0] == '[') {
            inValuesSection = false;
            continue;
        }
        
        // Parse key=value in values section
        if (inValuesSection) {
            char* equalPos = strchr(start, '=');
            if (equalPos != nullptr) {
                *equalPos = '\0'; // Split the string
                char* key = start;
                char* value = equalPos + 1;
                
                if(iniKey == "uncapped_clocks") {
                    Result rc = sysclkIpcSetEnabled(value);
                }

                // Trim key
                char* keyEnd = key + strlen(key) - 1;
                while (keyEnd > key && (*keyEnd == ' ' || *keyEnd == '\t')) {
                    *keyEnd = '\0';
                    keyEnd--;
                }
                
                // Trim value
                while (*value == ' ' || *value == '\t') value++;
                char* valueEnd = value + strlen(value) - 1;
                while (valueEnd > value && (*valueEnd == ' ' || *valueEnd == '\t')) {
                    *valueEnd = '\0';
                    valueEnd--;
                }
                
                if (iniKey == key) {
                    bool result = (strcmp(value, "1") == 0);
                    fclose(file);
                    return result;
                }
            }
        }
    }
    
    fclose(file);
    
    // Return default values if key not found
    return (iniKey == "gpu_dvfs"); // gpu_dvfs defaults to true, others default to false
}

int MiscGui::getConfigIntValue(const std::string& iniKey, int defaultValue)
{
    FILE* file = fopen("/config/sys-clk/config.ini", "r");
    if (!file) {
        return defaultValue;
    }
    
    char line[512];
    bool inValuesSection = false;

    size_t len;

    while (fgets(line, sizeof(line), file)) {
        // Remove newline if present
        len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }
        
        // Trim whitespace
        char* start = line;
        while (*start == ' ' || *start == '\t') start++;
        char* end = start + strlen(start) - 1;
        while (end > start && (*end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        // Check for [values] section
        if (strcmp(start, "[values]") == 0) {
            inValuesSection = true;
            continue;
        }
        
        // Check for new section
        if (strlen(start) > 0 && start[0] == '[') {
            inValuesSection = false;
            continue;
        }
        
        // Parse key=value in values section
        if (inValuesSection) {
            char* equalPos = strchr(start, '=');
            if (equalPos != nullptr) {
                *equalPos = '\0'; // Split the string
                char* key = start;
                char* value = equalPos + 1;
                
                // Trim key
                char* keyEnd = key + strlen(key) - 1;
                while (keyEnd > key && (*keyEnd == ' ' || *keyEnd == '\t')) {
                    *keyEnd = '\0';
                    keyEnd--;
                }
                
                // Trim value
                while (*value == ' ' || *value == '\t') value++;
                char* valueEnd = value + strlen(value) - 1;
                while (valueEnd > value && (*valueEnd == ' ' || *valueEnd == '\t')) {
                    *valueEnd = '\0';
                    valueEnd--;
                }
                
                if (iniKey == key) {
                    int result = atoi(value);
                    fclose(file);
                    return result;
                }
            }
        }
    }
    
    fclose(file);
    return defaultValue;
}

void MiscGui::setConfigValue(const std::string& iniKey, bool value)
{
    if(iniKey == "uncapped_clocks") {
        Result rc = sysclkIpcSetEnabled(value);
    }
    // Read the entire file
    FILE* file = fopen("/config/sys-clk/config.ini", "r");
    std::vector<std::string> lines;
    
    if (file) {
        char line[512];
        size_t len;
        while (fgets(line, sizeof(line), file)) {
            // Remove newline if present
            len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }
            lines.push_back(std::string(line));
        }
        fclose(file);
    }
    
    // Find and update the value
    bool inValuesSection = false;
    bool keyFound = false;
    int valuesSectionIndex = -1;

    std::string trimmedLine;
    size_t equalPos;
    std::string key;
    for (size_t i = 0; i < lines.size(); i++) {
        trimmedLine = lines[i];
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t") + 1);
        
        if (trimmedLine == "[values]") {
            inValuesSection = true;
            valuesSectionIndex = i;
            continue;
        }
        
        if (trimmedLine.length() > 0 && trimmedLine[0] == '[') {
            inValuesSection = false;
            continue;
        }
        
        if (inValuesSection) {
            equalPos = trimmedLine.find('=');
            if (equalPos != std::string::npos) {
                key = trimmedLine.substr(0, equalPos);
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                if (key == iniKey) {
                    lines[i] = iniKey + "=" + (value ? "1" : "0");
                    keyFound = true;
                    break;
                }
            }
        }
    }
    
    // If key wasn't found, add it to the values section
    if (!keyFound) {
        if (valuesSectionIndex == -1) {
            // Add [values] section if it doesn't exist
            lines.push_back("[values]");
            lines.push_back(iniKey + "=" + (value ? "1" : "0"));
        } else {
            // Add to existing values section
            lines.insert(lines.begin() + valuesSectionIndex + 1, iniKey + "=" + (value ? "1" : "0"));
        }
    }
    
    // Write the file back
    FILE* outFile = fopen("/config/sys-clk/config.ini", "w");
    if (outFile) {
        for (const auto& fileLine : lines) {
            fprintf(outFile, "%s\n", fileLine.c_str());
        }
        fclose(outFile);
    }
}

void MiscGui::setConfigIntValue(const std::string& iniKey, int value)
{
    // Read the entire file
    FILE* file = fopen("/config/sys-clk/config.ini", "r");
    std::vector<std::string> lines;
    
    if (file) {
        char line[512];
        size_t len;
        while (fgets(line, sizeof(line), file)) {
            // Remove newline if present
            len = strlen(line);
            if (len > 0 && line[len - 1] == '\n') {
                line[len - 1] = '\0';
            }
            lines.push_back(std::string(line));
        }
        fclose(file);
    }
    
    // Find and update the value
    bool inValuesSection = false;
    bool keyFound = false;
    int valuesSectionIndex = -1;
    
    std::string trimmedLine;
    size_t equalPos;
    std::string key;

    for (size_t i = 0; i < lines.size(); i++) {
        trimmedLine = lines[i];
        trimmedLine.erase(0, trimmedLine.find_first_not_of(" \t"));
        trimmedLine.erase(trimmedLine.find_last_not_of(" \t") + 1);
        
        if (trimmedLine == "[values]") {
            inValuesSection = true;
            valuesSectionIndex = i;
            continue;
        }
        
        if (trimmedLine.length() > 0 && trimmedLine[0] == '[') {
            inValuesSection = false;
            continue;
        }
        
        if (inValuesSection) {
            equalPos = trimmedLine.find('=');
            if (equalPos != std::string::npos) {
                key = trimmedLine.substr(0, equalPos);
                key.erase(0, key.find_first_not_of(" \t"));
                key.erase(key.find_last_not_of(" \t") + 1);
                
                if (key == iniKey) {
                    lines[i] = iniKey + "=" + std::to_string(value);
                    keyFound = true;
                    break;
                }
            }
        }
    }
    
    // If key wasn't found, add it to the values section
    if (!keyFound) {
        if (valuesSectionIndex == -1) {
            // Add [values] section if it doesn't exist
            lines.push_back("[values]");
            lines.push_back(iniKey + "=" + std::to_string(value));
        } else {
            // Add to existing values section
            lines.insert(lines.begin() + valuesSectionIndex + 1, iniKey + "=" + std::to_string(value));
        }
    }
    
    // Write the file back
    FILE* outFile = fopen("/config/sys-clk/config.ini", "w");
    if (outFile) {
        for (const auto& fileLine : lines) {
            fprintf(outFile, "%s\n", fileLine.c_str());
        }
        fclose(outFile);
    }
}

void MiscGui::addConfigToggle(const std::string& iniKey, const char* displayName) {
    tsl::elm::ToggleListItem* toggle = new tsl::elm::ToggleListItem(displayName, configValues[iniKey]);
    toggle->setStateChangedListener([this, iniKey](bool state) {
        configValues[iniKey] = state;
        setConfigValue(iniKey, state);
        this->lastContextUpdate = armGetSystemTick();
    });
    this->listElement->addItem(toggle);
    this->configToggles[iniKey] = toggle;
}

void MiscGui::updateConfigToggles() {
    for (const auto& [key, toggle] : this->configToggles) {
        if (toggle != nullptr) {
            bool currentValue = getConfigValue(key);
            configValues[key] = currentValue;
            toggle->setState(currentValue);
        }
    }
}

void MiscGui::listUI()
{

    this->listElement->addItem(new tsl::elm::CategoryHeader("Settings"));

    this->enabledToggle = new tsl::elm::ToggleListItem("Enable", false);
    enabledToggle->setStateChangedListener([this](bool state) {
        Result rc = sysclkIpcSetEnabled(state);
        if(R_FAILED(rc))
        {
            FatalGui::openWithResultCode("sysclkIpcSetEnabled", rc);
        }

        this->lastContextUpdate = armGetSystemTick();
        this->context->enabled = state;
    });
    this->listElement->addItem(this->enabledToggle);

    // Add the 4 boolean config toggles using INI keys
    addConfigToggle("uncapped_clocks", "Uncapped Clocks");
    addConfigToggle("override_boost_mode", "Override Boost Mode");
    addConfigToggle("auto_cpu_boost", "Auto CPU Boost");
    addConfigToggle("reversenx_sync", "Sync ReverseNX");
    
    // Add GPU DVFS as a NamedStepTrackBar with V2 style
    this->gpuDvfsTrackbar = new tsl::elm::NamedStepTrackBar("", {
        "Off",
        "Official Service Method", 
        "Hijack Method"
    }, true, "GPU DVFS");
    
    // Set initial value (default is 0 if not set)
    int currentDvfsValue = getConfigIntValue("gpu_dvfs", 1);
    // Ensure the value is within valid range (0-2)
    currentDvfsValue = std::max(0, std::min(2, currentDvfsValue));
    this->gpuDvfsTrackbar->setProgress(static_cast<u8>(currentDvfsValue));
    
    // Set up the value change listener to update the INI file
    this->gpuDvfsTrackbar->setValueChangedListener([this](u8 value) {
        // Ensure value is within expected range
        const int intValue = static_cast<int>(std::min(static_cast<u8>(2), value));
        setConfigIntValue("gpu_dvfs", intValue);
        this->lastContextUpdate = armGetSystemTick();
    });
    
    this->listElement->addItem(this->gpuDvfsTrackbar);
}

void MiscGui::refresh() {
    BaseMenuGui::refresh();

    // Update the enabled toggle state
    if(this->context)
    {
        this->enabledToggle->setState(this->context->enabled);
    }

    // Update config values and toggle states every 60 frames (once per second at 60fps)
    if (this->context && ++frameCounter >= 60)
    {
        frameCounter = 0;
        updateConfigToggles();
        
        // Update GPU DVFS trackbar
        if (this->gpuDvfsTrackbar != nullptr) {
            int currentDvfsValue = getConfigIntValue("gpu_dvfs", 1);
            // Ensure the value is within valid range (0-2)
            currentDvfsValue = std::max(0, std::min(2, currentDvfsValue));
            this->gpuDvfsTrackbar->setProgress(static_cast<u8>(currentDvfsValue));
        }
    }
}
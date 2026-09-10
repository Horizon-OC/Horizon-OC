/*
 * Copyright (c) Souldbminer, Lightos_ and Horizon OC Contributors
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "../../ipc.h"

#include <atomic>
#include <string>
#include <vector>

#include "base_menu_gui.h"

class UpdateStatusPanel;

class UpdateGui : public BaseMenuGui {

public:
    UpdateGui();
    ~UpdateGui();

    void listUI() override;
    void update() override;
    bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos,
                     HidAnalogStickState leftJoy, HidAnalogStickState rightJoy) override;

    void startJob(int packageIndex, bool extractOnly);

private:
    enum class UpdateStage {
        Idle,
        Downloading,
        Extracting,
        Done,
        Failed,
        Cancelled,
        FetchingChangelog,
        ChangelogReady,
    };

    struct PackageInfo {
        const char *displayName;
        const char *url;
        const char *zipPath;
    };

    static constexpr PackageInfo kPackages[2] = {
        {"Horizon OC",     "https://github.com/Horizon-OC/Horizon-OC/releases/latest/download/dist.zip", "sdmc:/config/horizon-oc/dist.zip"},
        {"Horizon OC + Extensions", "https://github.com/Horizon-OC/Horizon-OC/releases/latest/download/dist_ext.zip", "sdmc:/config/horizon-oc/dist_ext.zip"},
    };

    static constexpr const char *kReleaseApiUrl = "https://api.github.com/repos/Horizon-OC/Horizon-OC/releases/latest";
    static constexpr const char *kReleaseInfoPath = "sdmc:/config/horizon-oc/release_info.json";

    bool isBusy() const {
        return m_stage.load(std::memory_order_acquire) != UpdateStage::Idle;
    }

    void startChangelogFetch(int packageIndex);
    void requestCancel();
    void reapThread();
    void pollJob();

    static void jobEntry(void *arg);
    void jobBody();

    static void fetchChangelogEntry(void *arg);
    void fetchChangelogBody();

    std::atomic<UpdateStage> m_stage{UpdateStage::Idle};
    int m_activePackage = -1;
    bool m_extractOnly = false;
    bool m_threadActive = false;
    Thread m_thread{};
    std::string m_resultMessage;

    std::string m_changelogTag;
    std::vector<std::string> m_changelogLines;

    bool m_pendingChangelogConfirm = false;

    tsl::elm::ListItem *m_items[2] = {nullptr, nullptr};
    UpdateStatusPanel *m_status = nullptr;

protected:
    std::string getJumpToItemName() override;
};

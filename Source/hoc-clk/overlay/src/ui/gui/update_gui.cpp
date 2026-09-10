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

#include "update_gui.h"
#include "ult_ext.h"
#include "base_menu_gui.h"
#include "update_confirm_gui.h"
#include "../format.h"
#include "../style.h"

#include <tesla.hpp>
#include <ultra.hpp>
#include <cstdio>
#include <memory>
#include <string>


static constexpr u16 PANEL_H = 80;

class UpdateStatusPanel : public tsl::elm::Element {
public:
    void setState(int stage, int progress,
                  const std::string &pkg, const std::string &msg) {
        m_stage    = stage;
        m_progress = progress;
        m_pkg      = pkg;
        m_msg      = msg;
    }

    void draw(tsl::gfx::Renderer *renderer) override {
        if (this->getHeight() == 0) return;

        const s32 PAD = 20;
        const s32 x   = this->getX() + PAD;
        const s32 y   = this->getY();
        const s32 w   = this->getWidth() - PAD * 2;

        if (m_stage == 0) {
            return;
        }

        if (m_stage == 1 || m_stage == 2) {
            const char *opLabel = (m_stage == 1) ? "Downloading" : "Extracting";
            tsl::Color accent   = (m_stage == 1)
                ? tsl::Color{0x4, 0x8, 0xF, 0xF}   // blue
                : tsl::Color{0x3, 0xA, 0x8, 0xF};  // teal

            renderer->drawString(opLabel,       false, x,       y + 20, 15, accent);
            renderer->drawString(m_pkg.c_str(), false, x + 104, y + 20, 15,
                                 tsl::sectionTextColor);

            const s32 barY = y + 48;
            renderer->drawRect(x, barY, w, 7, tsl::Color{0x3, 0x3, 0x3, 0xF});

            int pct   = (m_progress < 0) ? 0 : (m_progress > 100 ? 100 : m_progress);
            s32 fillW = (s32)((long)w * pct / 100);
            if (fillW > 0)
                renderer->drawRect(x, barY, fillW, 7, accent);

            char buf[8];
            if (m_progress < 0)
                std::snprintf(buf, sizeof(buf), "...");
            else
                std::snprintf(buf, sizeof(buf), "%d%%", pct);

            u32 tw = renderer->getTextDimensions(buf, false, 13).first;
            renderer->drawString(buf, false,
                x + w - (s32)tw, barY - 14, 13, tsl::infoTextColor);

        } else {
            tsl::Color col;
            switch (m_stage) {
                case 3:  col = {0x3, 0xB, 0x5, 0xF}; break;  // Done    → green
                case 4:  col = {0xE, 0x3, 0x3, 0xF}; break;  // Failed  → red
                default: col = {0x7, 0x7, 0x7, 0xF}; break;  // Cancel  → grey
            }
            renderer->drawString(m_msg.c_str(), false, x, y + 30, 14, col);
        }
    }

    void layout(u16, u16, u16, u16) override {
        this->setBoundaries(this->getX(), this->getY(),
                            this->getWidth(), PANEL_H);
    }

    Element *requestFocus(Element *, tsl::FocusDirection) override {
        return nullptr;
    }

private:
    int         m_stage    = 0;
    int         m_progress = 0;
    std::string m_pkg;
    std::string m_msg;
};

UpdateGui::UpdateGui()  = default;
UpdateGui::~UpdateGui() { reapThread(); }

void UpdateGui::startJob(int packageIndex, bool extractOnly) {
    if (isBusy()) return;

    m_activePackage = packageIndex;
    m_extractOnly   = extractOnly;
    m_resultMessage.clear();

    ult::abortDownload.store(false, std::memory_order_release);
    ult::abortUnzip.store(false, std::memory_order_release);

    m_stage.store(UpdateStage::Downloading, std::memory_order_release);

    if (R_SUCCEEDED(threadCreate(&m_thread, jobEntry, this,
                                 nullptr, 0x8000, 0x20, -2))) {
        m_threadActive = true;
        threadStart(&m_thread);
    } else {
        m_resultMessage = "Failed to create thread";
        m_stage.store(UpdateStage::Failed, std::memory_order_release);
        m_threadActive = false;
    }
}

void UpdateGui::startChangelogFetch(int packageIndex) {
    if (isBusy()) return;

    m_activePackage = packageIndex;
    m_resultMessage.clear();
    m_changelogTag.clear();
    m_changelogLines.clear();

    ult::abortDownload.store(false, std::memory_order_release);

    m_stage.store(UpdateStage::FetchingChangelog, std::memory_order_release);

    if (R_SUCCEEDED(threadCreate(&m_thread, fetchChangelogEntry, this,
                                 nullptr, 0x8000, 0x20, -2))) {
        m_threadActive = true;
        threadStart(&m_thread);
    } else {
        m_resultMessage = "Failed to create thread";
        m_stage.store(UpdateStage::Failed, std::memory_order_release);
        m_threadActive = false;
    }
}

void UpdateGui::requestCancel() {
    ult::abortDownload.store(true, std::memory_order_release);
    ult::abortUnzip.store(true, std::memory_order_release);
}

void UpdateGui::reapThread() {
    if (m_threadActive) {
        threadWaitForExit(&m_thread);
        threadClose(&m_thread);
        m_threadActive = false;
    }
}

/* static */ void UpdateGui::jobEntry(void *arg) {
    static_cast<UpdateGui *>(arg)->jobBody();
}

void UpdateGui::jobBody() {
    const PackageInfo &pkg = kPackages[m_activePackage];

    if (!m_extractOnly) {
        m_stage.store(UpdateStage::Downloading, std::memory_order_release);
        ult::createDirectory("sdmc:/config/horizon-oc/");

        bool ok = ult::downloadFile(pkg.url, pkg.zipPath, false, false);
        if (!ok || ult::abortDownload.load(std::memory_order_acquire)) {
            m_resultMessage = ult::abortDownload.load(std::memory_order_acquire)
                ? "Download cancelled"
                : "Download failed -- check network";
            m_stage.store(
                ult::abortDownload.load() ? UpdateStage::Cancelled : UpdateStage::Failed,
                std::memory_order_release);
            return;
        }
    }

    m_stage.store(UpdateStage::Extracting, std::memory_order_release);

    bool ok = ult::unzipFile(pkg.zipPath, "sdmc:/");
    if (!ok || ult::abortUnzip.load(std::memory_order_acquire)) {
        m_resultMessage = ult::abortUnzip.load(std::memory_order_acquire)
            ? "Extraction cancelled"
            : "Extraction failed -- archive may be corrupt";
        m_stage.store(
            ult::abortUnzip.load() ? UpdateStage::Cancelled : UpdateStage::Failed,
            std::memory_order_release);
        return;
    }

    std::remove(pkg.zipPath);

    m_resultMessage = "Update complete! Restart required.";
    m_stage.store(UpdateStage::Done, std::memory_order_release);
}

/* static */ void UpdateGui::fetchChangelogEntry(void *arg) {
    static_cast<UpdateGui *>(arg)->fetchChangelogBody();
}

void UpdateGui::fetchChangelogBody() {
    ult::createDirectory("sdmc:/config/horizon-oc/");

    bool ok = ult::downloadFile(kReleaseApiUrl, kReleaseInfoPath, false, false);
    if (!ok || ult::abortDownload.load(std::memory_order_acquire)) {
        m_resultMessage = ult::abortDownload.load(std::memory_order_acquire)
            ? "Changelog fetch cancelled"
            : "Could not fetch changelog -- check network";
        m_stage.store(
            ult::abortDownload.load() ? UpdateStage::Cancelled : UpdateStage::Failed,
            std::memory_order_release);
        return;
    }

    std::unique_ptr<ult::json_t, ult::JsonDeleter> root(ult::readJsonFromFile(kReleaseInfoPath));
    std::remove(kReleaseInfoPath);

    if (!root) {
        m_resultMessage = "Could not parse changelog";
        m_stage.store(UpdateStage::Failed, std::memory_order_release);
        return;
    }

    m_changelogTag = ult::getStringFromJson(root.get(), "tag_name");
    const std::string body = ult::getStringFromJson(root.get(), "body");

    m_changelogLines.clear();
    size_t pos = 0;
    while (pos <= body.size()) {
        size_t nl = body.find('\n', pos);
        std::string line = (nl == std::string::npos) ? body.substr(pos) : body.substr(pos, nl - pos);
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        if (!line.empty())
            m_changelogLines.push_back(line);
        if (nl == std::string::npos)
            break;
        pos = nl + 1;
    }
    if (m_changelogLines.empty())
        m_changelogLines.push_back("No changelog available for this release.");

    m_stage.store(UpdateStage::ChangelogReady, std::memory_order_release);
}


void UpdateGui::listUI() {
    BaseMenuGui::refresh();
    if (!this->context) return;

    this->listElement->addItem(new tsl::elm::CategoryHeader("Releases"));

    for (int i = 0; i < 2; ++i) {
        auto *item = new tsl::elm::ListItem(kPackages[i].displayName);
        item->setValue("\uE0E0");

        const int idx = i;
        item->setClickListener([this, idx](u64 keys) -> bool {
            if (isBusy()) return false;
            if (keys & HidNpadButton_A) {
                if (ult::limitedMemory) {
                    m_status->setState(4, 100, kPackages[idx].displayName,
                                       "Updates cannot be performed on a 4MB overlay heap");
                    return true;
                }
                startChangelogFetch(idx);
                return true;
            }
            return false;
        });

        m_items[i] = item;
        this->listElement->addItem(item);
    }

    this->listElement->addItem(new tsl::elm::CategoryHeader("Status"));

    auto *panel = new UpdateStatusPanel();
    panel->setState(0, 0, "", "");
    m_status = panel;
    this->listElement->addItem(panel);
}

void UpdateGui::pollJob() {
    auto stage = m_stage.load(std::memory_order_acquire);
    if (stage == UpdateStage::Idle || !m_status) return;

    const char *pkgName = (m_activePackage >= 0 && m_activePackage < 2)
                          ? kPackages[m_activePackage].displayName : "";

    if (stage == UpdateStage::Downloading) {
        int pct = ult::downloadPercentage.load(std::memory_order_relaxed);
        m_status->setState(1, pct, pkgName, "");
        if (m_items[m_activePackage])
            m_items[m_activePackage]->setValue("...");
        return;
    }

    if (stage == UpdateStage::Extracting) {
        int pct = ult::unzipPercentage.load(std::memory_order_relaxed);
        m_status->setState(2, pct, pkgName, "");
        if (m_items[m_activePackage])
            m_items[m_activePackage]->setValue("...");
        return;
    }

    if (stage == UpdateStage::FetchingChangelog) {
        m_status->setState(6, 0, pkgName, "Fetching changelog...");
        if (m_items[m_activePackage])
            m_items[m_activePackage]->setValue("...");
        return;
    }

    if (stage == UpdateStage::ChangelogReady) {
        reapThread();
        m_stage.store(UpdateStage::Idle, std::memory_order_release);
        m_status->setState(0, 0, "", "");
        if (m_items[m_activePackage])
            m_items[m_activePackage]->setValue("");

        m_pendingChangelogConfirm = true;
        return;
    }

    int panelStage = (stage == UpdateStage::Done)   ? 3
                   : (stage == UpdateStage::Failed)  ? 4 : 5;

    m_status->setState(panelStage, 100, pkgName, m_resultMessage);

    for (int i = 0; i < 2; ++i)
        if (m_items[i]) m_items[i]->setValue("\uE0E0");

    reapThread();
    m_stage.store(UpdateStage::Idle, std::memory_order_release);
}

void UpdateGui::update() {
    BaseMenuGui::update();
    pollJob();
}

bool UpdateGui::handleInput(u64 keysDown, u64 keysHeld,
                            const HidTouchState &touchPos,
                            HidAnalogStickState leftJoy,
                            HidAnalogStickState rightJoy) {
    if (m_pendingChangelogConfirm) {
        m_pendingChangelogConfirm = false;
        tsl::changeTo<UpdateConfirmGui>(this, m_activePackage, kPackages[m_activePackage].displayName,
                                        m_changelogTag, m_changelogLines);
        return true;
    }

    if ((keysDown & HidNpadButton_R) && isBusy()) {
        requestCancel();
        return true;
    }

    return BaseMenuGui::handleInput(keysDown, keysHeld, touchPos, leftJoy, rightJoy);
}

std::string UpdateGui::getJumpToItemName() {
    return "Updates";
}

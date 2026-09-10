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

#include "update_confirm_gui.h"
#include "info_gui.h"
#include "update_gui.h"

#include <tesla.hpp>

UpdateConfirmGui::UpdateConfirmGui(UpdateGui *parent, int packageIndex, std::string packageName,
                                   std::string tag, std::vector<std::string> changelog)
    : m_parent(parent), m_packageIndex(packageIndex), m_packageName(std::move(packageName)),
      m_tag(std::move(tag)), m_changelog(std::move(changelog)) {
}

void UpdateConfirmGui::listUI() {
    std::string header = m_packageName;
    if (!m_tag.empty())
        header += "  " + m_tag;
    this->listElement->addItem(new tsl::elm::CategoryHeader(header));

    addWrappedTextItems(this->listElement, m_changelog);

    this->listElement->addItem(new tsl::elm::CategoryHeader("Confirm"));

    auto *installItem = new tsl::elm::ListItem("Install Update");
    installItem->setValue("");
    installItem->setClickListener([this](u64 keys) -> bool {
        if ((keys & HidNpadButton_A) == HidNpadButton_A) {
            m_parent->startJob(m_packageIndex, false);
            tsl::goBack();
            return true;
        }
        return false;
    });
    this->listElement->addItem(installItem);
}

bool UpdateConfirmGui::handleInput(u64, u64, const HidTouchState &,
                                   HidAnalogStickState, HidAnalogStickState) {
    return false;
}

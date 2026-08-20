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
#include <string>
#include <vector>

#include "base_menu_gui.h"

class UpdateGui;

class UpdateConfirmGui : public BaseMenuGui {
public:
    UpdateConfirmGui(UpdateGui *parent, int packageIndex, std::string packageName,
                      std::string tag, std::vector<std::string> changelog);
    ~UpdateConfirmGui() = default;

    void listUI() override;
    bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos,
                      HidAnalogStickState leftJoy, HidAnalogStickState rightJoy) override;

private:
    UpdateGui *m_parent;
    int m_packageIndex;
    std::string m_packageName;
    std::string m_tag;
    std::vector<std::string> m_changelog;
};

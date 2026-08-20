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
#include "base_menu_gui.h"
#include "freq_choice_gui.h"
#include "value_choice_gui.h"
#include "fatal_gui.h"
#include <map>
#include <vector>

class AboutGui : public BaseMenuGui
{
protected:
    char strings[32][32];
    std::string getJumpToItemName() override;

public:
    AboutGui();
    ~AboutGui();

    void listUI() override;
    void update() override;
    void refresh() override;
};
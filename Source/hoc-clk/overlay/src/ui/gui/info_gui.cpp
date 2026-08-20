/*
 *
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
 *
 */

#include <sstream>

#include "info_gui.h"
#include "ult_ext.h"

InfoGui::InfoGui(std::string title, std::vector<std::string> strings) : m_title(std::move(title)), m_strings(std::move(strings)) {
}

static constexpr s32 TEXT_SIZE = 16;
static constexpr s32 LINE_H = 22;
static constexpr s32 PARA_GAP = 10;
static constexpr s32 MARGIN_L = 20;
static constexpr s32 MARGIN_R = 35;

static std::vector<std::string> wrapText(const std::string &text, s32 maxWidth) {
    constexpr float CHAR_W = 10.0f;

    // Preserve leading whitespace as an indent prefix for wrapped continuation lines.
    std::string indent;
    for (char c : text) {
        if (c == ' ')
            indent += ' ';
        else
            break;
    }

    std::vector<std::string> lines;
    std::istringstream ss(text);
    std::string word, line = indent;  // seed with indent so first word inherits it
    bool first = true;
    while (ss >> word) {
        std::string candidate = (first && !indent.empty()) ? indent + word : line.empty() ? word : line + " " + word;
        first = false;
        if (static_cast<s32>(candidate.size() * CHAR_W) <= maxWidth)
            line = std::move(candidate);
        else {
            if (!line.empty() && line != indent)
                lines.push_back(line);
            line = indent + word;
        }
    }
    if (!line.empty() && line != indent)
        lines.push_back(line);
    if (lines.empty())
        lines.emplace_back("");
    return lines;
}

void addWrappedTextItems(tsl::elm::List *list, const std::vector<std::string> &paragraphs) {
    const s32 maxWidth = tsl::cfg::FramebufferWidth - MARGIN_L - MARGIN_R;

    for (const auto &para : paragraphs) {
        for (const auto &lineText : wrapText(para, maxWidth)) {
            auto *d = new FocusableDrawer([lineText](tsl::gfx::Renderer *r, s32 x, s32 y, s32 w, s32 h) {
                r->drawString((lineText + "\n").c_str(), false, x + MARGIN_L, y + LINE_H - 5, TEXT_SIZE, tsl::style::color::ColorText);
            });
            d->setBoundaries(0, 0, tsl::cfg::FramebufferWidth, LINE_H);
            list->addItem(d, LINE_H);
        }

        // paragraph gap
        auto *gap = new tsl::elm::CustomDrawer([](tsl::gfx::Renderer *, s32, s32, s32, s32) {});
        gap->setBoundaries(0, 0, tsl::cfg::FramebufferWidth, PARA_GAP);
        list->addItem(gap, PARA_GAP);
    }
}

void InfoGui::listUI() {
    this->listElement->addItem(new tsl::elm::CategoryHeader(m_title));
    addWrappedTextItems(this->listElement, m_strings);
}

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


#include "about_gui.h"
#include "../format.h"
#include <tesla.hpp>
#include <string>
#include "cat.h"
#include "main_gui.h"
#include "ult_ext.h"

// tsl::elm::ListItem* custRevItem = NULL;
tsl::elm::ListItem* kipVersionItem = NULL;
tsl::elm::ListItem* kipLoadedItem = NULL;
tsl::elm::ListItem* SpeedoItem = NULL;
tsl::elm::ListItem* IddqItem = NULL;
tsl::elm::ListItem* DramModule = NULL;
tsl::elm::ListItem* sysdockStatusItem = NULL;
tsl::elm::ListItem* saltyNXStatusItem = NULL;
tsl::elm::ListItem* waferCordsItem = NULL;
tsl::elm::ListItem* ramVoltItem = NULL;
tsl::elm::ListItem* eristaPLLXItem = NULL;
tsl::elm::ListItem* dispVoltItem = NULL;
tsl::elm::ListItem* ramBWItemAll = NULL;
tsl::elm::ListItem* ramBWItemCpu = NULL;
tsl::elm::ListItem* ramBWItemGpu = NULL;
tsl::elm::ListItem* ramBWItemMax = NULL;
tsl::elm::ListItem* bqtempitem = NULL;
tsl::elm::ListItem* aotagTempItem = NULL;
tsl::elm::ListItem* cTypeItem = NULL;
tsl::elm::ListItem* creditsItem = NULL;
tsl::elm::ListItem* bpmpStatusItem = NULL;

#define R_ARROW "\u2192"

class CreditsSubMenu;

AboutGui::AboutGui()
{
    memset(strings, 0, sizeof(strings));
}

AboutGui::~AboutGui()
{
}

void AboutGui::listUI()
{
    BaseMenuGui::refresh();

    if (!this->context)
        return;

    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Voltages")
    );

    ramVoltItem =
        new tsl::elm::ListItem("RAM Voltage:");

    if(IsMariko()) {
        this->listElement->addItem(ramVoltItem);
    }
    dispVoltItem =
        new tsl::elm::ListItem("Display Voltage:");
    this->listElement->addItem(dispVoltItem);

    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Temperatures")
    );
    eristaPLLXItem =
        new tsl::elm::ListItem("PLLX Temp:");
    if(this->context->temps[HocClkThermalSensor_AO] > 0) { // Only show if the value is valid (not -126, which means not patched)
        this->listElement->addItem(eristaPLLXItem);
    }

    aotagTempItem =
        new tsl::elm::ListItem("AOTAG Temp:");
    this->listElement->addItem(aotagTempItem);

    bqtempitem =
        new tsl::elm::ListItem("BQ24193 Temp:");
    this->listElement->addItem(bqtempitem);

    this->listElement->addItem(
        new tsl::elm::CategoryHeader("RAM Bandwidth")
    );

    ramBWItemMax =
        new tsl::elm::ListItem("RAM BW (Peak):");
    this->listElement->addItem(ramBWItemMax);

    ramBWItemAll =
        new tsl::elm::ListItem("RAM BW (All):");
    this->listElement->addItem(ramBWItemAll);

    ramBWItemCpu =
        new tsl::elm::ListItem("RAM BW (CPU):");
    this->listElement->addItem(ramBWItemCpu);

    ramBWItemGpu =
        new tsl::elm::ListItem("RAM BW (GPU):");
    this->listElement->addItem(ramBWItemGpu);


    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Hardware Info")
    );

    cTypeItem =
        new tsl::elm::ListItem("Console Type:");
    this->listElement->addItem(cTypeItem);

    SpeedoItem =
        new tsl::elm::ListItem("Speedo:");
    this->listElement->addItem(SpeedoItem);

    IddqItem =
        new tsl::elm::ListItem("IDDQ:");
    this->listElement->addItem(IddqItem);

    DramModule =
        new tsl::elm::ListItem("DRAM Module: ");
    this->listElement->addItem(DramModule);

    waferCordsItem =
        new tsl::elm::ListItem("Wafer Position:");
    this->listElement->addItem(waferCordsItem);

    this->listElement->addItem(
        new tsl::elm::CategoryHeader("Software Info")
    );

    // custRevItem = new tsl::elm::ListItem("CUST revision:");
    // this->listElement->addItem(custRevItem);

    bpmpStatusItem = new tsl::elm::ListItem("BPMP Status:");
    this->listElement->addItem(bpmpStatusItem);

    kipVersionItem = new tsl::elm::ListItem("KIP version:");
    this->listElement->addItem(kipVersionItem);

    kipLoadedItem = new tsl::elm::ListItem("KIP status:");
    this->listElement->addItem(kipLoadedItem);

    if(!IsHoag()) {
        sysdockStatusItem =
            new tsl::elm::ListItem("sys-dock status:");
        this->listElement->addItem(sysdockStatusItem);
    }

    saltyNXStatusItem =
        new tsl::elm::ListItem("SaltyNX status:");
    this->listElement->addItem(saltyNXStatusItem);

    this->listElement->addItem(
        new tsl::elm::CategoryHeader("General Info")
    );

    creditsItem = new tsl::elm::ListItem("Credits");
    creditsItem->setClickListener([](u64 keys) {
        if (keys & HidNpadButton_A) {
            tsl::swapTo<CreditsSubMenu>();
            return true;
        }
        return false;
    });
    creditsItem->setValue(R_ARROW);
    this->listElement->addItem(creditsItem);

    if (!lastItemName.empty()) {
        this->listElement->jumpToItem(lastItemName);
    }
    lastItemName = "";
}

void AboutGui::update()
{
    BaseMenuGui::update();
}

std::string AboutGui::getJumpToItemName() {
    return "About";
}

void AboutGui::refresh()
{
    BaseMenuGui::refresh();

    if (!this->context)
        return;
    // Format strings once per refresh
    sprintf(strings[0], "%u/%u/%u", this->context->speedos[HocClkSpeedo_CPU], this->context->speedos[HocClkSpeedo_GPU], this->context->speedos[HocClkSpeedo_SOC]);
    // This is how hekate does it
    sprintf(strings[1], "%u/%u/%u", this->context->iddq[HocClkSpeedo_CPU], this->context->iddq[HocClkSpeedo_GPU], this->context->iddq[HocClkSpeedo_SOC]);
    SpeedoItem->setValue(strings[0]);
    IddqItem->setValue(strings[1]);
    DramModule->setValue(formatRamModule(this->context->dramID));

    // custRevItem->setValue(std::to_string(this->context->custRev));

    kipVersionItem->setValue(std::to_string((this->context->kipVersion / 100) % 10) + "." + std::to_string((this->context->kipVersion / 10) % 10) + "." + std::to_string( this->context->kipVersion % 10) + " (Cust Rev " + std::to_string(this->context->custRev) + ")");
    kipLoadedItem->setValue(this->context->isKipLoaded ? "Loaded" : "Not Loaded");

    if(!IsHoag())
        sysdockStatusItem->setValue(this->context->isSysDockInstalled ? "Installed" : "Not Installed");

    saltyNXStatusItem->setValue(this->context->isSaltyNXInstalled ? "Installed" : "Not Installed");

    sprintf(strings[2], "X: %d Y: %d", this->context->waferX, this->context->waferY);
    waferCordsItem->setValue(strings[2]);

    s32 millis = context->temps[HocClkThermalSensor_PLLX];
    sprintf(strings[3], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    eristaPLLXItem->setValue(strings[3]);

    millis = context->temps[HocClkThermalSensor_AO];
    if(millis > 0) {
        sprintf(strings[11], "%u.%u °C", millis / 1000U, (millis % 1000U) / 100U);
    } else if (millis == -125) {
        sprintf(strings[11], "Invalid");
    } else if (millis == -126) {
        sprintf(strings[11], "Not Patched");
    }
    aotagTempItem->setValue(strings[11]);

    sprintf(strings[4], "%u.%u / %u mV", context->voltages[HocClkVoltage_EMCVDD2] / 1000U, (context->voltages[HocClkVoltage_EMCVDD2] % 1000U) / 100U, context->voltages[HocClkVoltage_EMCVDDQ] / 1000);
    ramVoltItem->setValue(strings[4]);

    sprintf(strings[5], "%u.%u mV", context->voltages[HocClkVoltage_Display] / 1000U, (context->voltages[HocClkVoltage_Display] % 1000U) / 100U);
    dispVoltItem->setValue(strings[5]);

    sprintf(strings[6], "%u MB/s", context->partLoad[HocClkPartLoad_RamBWAll]);
    ramBWItemAll->setValue(strings[6]);

    sprintf(strings[7], "%u MB/s", context->partLoad[HocClkPartLoad_RamBWCpu]);
    ramBWItemCpu->setValue(strings[7]);

    sprintf(strings[8], "%u MB/s", context->partLoad[HocClkPartLoad_RamBWGpu]);
    ramBWItemGpu->setValue(strings[8]);

    sprintf(strings[9], "%u MB/s", context->partLoad[HocClkPartLoad_RamBWPeak]);
    ramBWItemMax->setValue(strings[9]);

    switch(context->temps[HocClkThermalSensor_BQ24193]) {
        case BQ24193Temp_Normal:
            strcpy(strings[10], "Normal");
            break;
        case BQ24193Temp_Warm:
            strcpy(strings[10], "Warm");
            break;
        case BQ24193Temp_Hot:
            strcpy(strings[10], "Hot");
            break;
        case BQ24193Temp_Overheat:
            strcpy(strings[10], "Overheat");
            break;
        default:
            strcpy(strings[10], "Unknown");
    }

    bqtempitem->setValue(strings[10]);

    cTypeItem->setValue(hocClkFormatConsoleType(this->context->consoleType, true));
    bpmpStatusItem->setValue(this->context->bpmpInfo.magic == HOCCLK_BPMP_MAGIC ? "Enabled" : "Halted");
}

class CreditsSubMenu : public AboutGui {
    public:
        CreditsSubMenu() { }

        bool handleInput(u64 keysDown, u64 keysHeld, const HidTouchState &touchPos, HidAnalogStickState leftJoyStick,
                          HidAnalogStickState rightJoyStick) override {
            if (keysDown & KEY_B) {
                triggerExitFeedback();
                lastItemName = "Credits";
                tsl::swapTo<AboutGui>();
                return true;
            }
            return false;
        }

        /* This is needed because AboutGui::Refresh() dereferences pointers that get freed after swapTo is called. */
        void refresh() override {
            BaseMenuGui::refresh();
        }

    protected:
        ImageElement* CatImage = NULL;
        HideableCategoryHeader* CatHeader = NULL;
        HideableCustomDrawer* CatSpacer = NULL;
        int lightosClickCount = 0;

        void listUI() override {
            this->listElement->addItem(
                new tsl::elm::CategoryHeader("Developers")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Souldbminer")
            );

            // Create special clickable item for Lightos
            auto lightosItem = new tsl::elm::ListItem("Lightos_");
            lightosItem->setClickListener([this](u64 keys) -> bool {
                if (keys & HidNpadButton_A) {
                    lightosClickCount++;
                    if (lightosClickCount >= 10) {
                        if (CatImage != NULL) CatImage->setVisible(true);
                        if (CatHeader != NULL) CatHeader->setVisible(true);
                        if (CatSpacer != NULL) CatSpacer->setVisible(true);
                    }
                    return true;
                }
                return false;
            });
            this->listElement->addItem(lightosItem);

            // ---- Contributors ----
            this->listElement->addItem(
                new tsl::elm::CategoryHeader("Contributors")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Dom")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Blaise25")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("tetetete-ctrl")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("B3711")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("TDRR")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("MasaGratoR")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Miki1305")
            );

            // ---- Testers ----
            this->listElement->addItem(
                new tsl::elm::CategoryHeader("Testers")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Samybigio2011")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("arcdelta")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Happy")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Winnerboi77")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Blaise25")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("WE1ZARD")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Alvise")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("agjeococh")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("Frost")
            );

            // ---- Special Thanks ----
            this->listElement->addItem(
                new tsl::elm::CategoryHeader("Special Thanks")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("SciresM, hexkyz and Alula - Atmosphere CFW")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("KazushiMe - Switch OC Suite")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("hanai3Bi - Switch OC Suite & EOS")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("RetroNX - sys-clk")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("ppkantorski - Ultrahand")
            );

            this->listElement->addItem(
                new tsl::elm::ListItem("CtCaer - Hekate, L4T and Proper Timings")
            );

            // Create cat elements but hide them initially
            CatHeader = new HideableCategoryHeader("Cat");
            CatHeader->setVisible(false);
            this->listElement->addItem(CatHeader);

            CatImage = new ImageElement(CAT_DATA, CAT_WIDTH, CAT_HEIGHT);
            CatImage->setVisible(false);
            this->listElement->addItem(CatImage);

            CatSpacer = new HideableCustomDrawer(75);
            CatSpacer->setVisible(false);
            this->listElement->addItem(CatSpacer);
        }

};

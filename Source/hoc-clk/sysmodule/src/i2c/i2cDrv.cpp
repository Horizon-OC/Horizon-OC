/*
 * Copyright (c) KazushiMe
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
#include "i2cDrv.h"

Result I2cSet_U8(I2cDevice dev, u8 reg, u8 val) {
    // ams::fatal::srv::StopSoundTask::StopSound()
    // I2C Bus Communication Reference: https://www.ti.com/lit/an/slva704/slva704.pdf
    struct {
        u8 reg;
        u8 val;
    } __attribute__((packed)) cmd;

    I2cSession _session;
    Result res = i2cOpenSession(&_session, dev);
    if (res)
        return res;

    cmd.reg = reg;
    cmd.val = val;
    res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
    i2csessionClose(&_session);
    return res;
}

Result I2cRead_OutU8(I2cDevice dev, u8 reg, u8 *out) {
    struct {
        u8 reg;
    } __attribute__((packed)) cmd;
    struct {
        u8 val;
    } __attribute__((packed)) rec;

    I2cSession _session;
    Result res = i2cOpenSession(&_session, dev);
    if (res)
        return res;

    cmd.reg = reg;
    res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
    if (res) {
        i2csessionClose(&_session);
        return res;
    }

    res = i2csessionReceiveAuto(&_session, &rec, sizeof(rec), I2cTransactionOption_All);
    i2csessionClose(&_session);
    if (res) {
        return res;
    }

    *out = rec.val;
    return 0;
}

Result I2cRead_OutU16(I2cDevice dev, u8 reg, u16 *out) {
    struct {
        u8 reg;
    } __attribute__((packed)) cmd;
    struct {
        u16 val;
    } __attribute__((packed)) rec;

    I2cSession _session;
    Result res = i2cOpenSession(&_session, dev);
    if (res)
        return res;

    cmd.reg = reg;
    res = i2csessionSendAuto(&_session, &cmd, sizeof(cmd), I2cTransactionOption_All);
    if (res) {
        i2csessionClose(&_session);
        return res;
    }

    res = i2csessionReceiveAuto(&_session, &rec, sizeof(rec), I2cTransactionOption_All);
    i2csessionClose(&_session);
    if (res) {
        return res;
    }

    *out = rec.val;
    return 0;
}

float I2c_Max17050_GetBatteryCurrent() {
    u16 val;
    Result res = I2cRead_OutU16(I2cDevice_Max17050, MAX17050_CURRENT_REG, &val);
    if (res)
        return 0.f;

    const float SenseResistor = 5.;  // in uOhm
    const float CGain = 1.99993;
    return (s16)val * (1.5625 / (SenseResistor * CGain));
}

u32 I2c_BuckConverter_MultiplierToMvOut(const I2c_BuckConverter_Domain *domain, u8 multiplier) {
    return (domain->uv_min + domain->uv_step * multiplier) / 1000;
}

u32 I2c_BuckConverter_MultiplierToUvOut(const I2c_BuckConverter_Domain *domain, u8 multiplier) {
    return domain->uv_min + domain->uv_step * multiplier;
}

u8 I2c_BuckConverter_MvOutToMultiplier(const I2c_BuckConverter_Domain *domain, u32 mvolt) {
    u32 uvolt = mvolt * 1000;
    if (uvolt < domain->uv_min)
        uvolt = domain->uv_min;
    if (uvolt > domain->uv_max)
        uvolt = domain->uv_max;

    return (uvolt - domain->uv_min) / domain->uv_step;
}

u32 I2c_BuckConverter_GetMvOut(const I2c_BuckConverter_Domain *domain) {
    u8 val;
    // Retry 5 times if received POR value
    for (int i = 0; i < 5; i++) {
        if (R_FAILED(I2cRead_OutU8(domain->device, domain->reg, &val)))
            return 0u;

        // Wait 1us
        svcSleepThread(1E3);

        if (!domain->por_val || val != domain->por_val)
            break;
    }
    return I2c_BuckConverter_MultiplierToMvOut(domain, val & domain->volt_mask);
}

u32 I2c_BuckConverter_GetUvOut(const I2c_BuckConverter_Domain *domain) {
    u8 val;
    // Retry 5 times if received POR value
    for (int i = 0; i < 5; i++) {
        if (R_FAILED(I2cRead_OutU8(domain->device, domain->reg, &val)))
            return 0u;

        // Wait 1us
        svcSleepThread(1E3);

        if (!domain->por_val || val != domain->por_val)
            break;
    }
    return I2c_BuckConverter_MultiplierToUvOut(domain, val & domain->volt_mask);
}

Result I2c_BuckConverter_SetMvOut(const I2c_BuckConverter_Domain *domain, u32 mvolt) {
    u8 val;
    Result res = I2cRead_OutU8(domain->device, domain->reg, &val);
    if (R_FAILED(res))
        return res;

    u8 multiplier = I2c_BuckConverter_MvOutToMultiplier(domain, mvolt);
    val &= ~domain->volt_mask;
    val |= multiplier & domain->volt_mask;

    res = I2cSet_U8(domain->device, domain->reg, val);
    if (R_FAILED(res))
        return res;

    // 5ms Ramp delay
    svcSleepThread(5E6);
    u8 new_val;
    res = I2cRead_OutU8(domain->device, domain->reg, &new_val);
    if (R_FAILED(res))
        return res;
    if (new_val != val)
        return -1;

    return 0;
}

u8 I2c_Bq24193_Convert_mA_Raw(u32 ma) {
    // Adjustment is required
    u8 raw = 0;

    if (ma > MA_RANGE_MAX)  // capping
        ma = MA_RANGE_MAX;

    bool pct20 = ma <= (MA_RANGE_MIN - 64);
    if (pct20) {
        ma = ma * 5;
        raw |= 0x1;
    }

    ma -= ma % 100;             // round to 100
    ma -= (MA_RANGE_MIN - 64);  // ceiling
    raw |= (ma >> 6) << 2;

    return raw;
};

u32 I2c_Bq24193_Convert_Raw_mA(u8 raw) {
    // No adjustment is allowed
    u32 ma = (((raw >> 2)) << 6) + MA_RANGE_MIN;

    bool pct20 = raw & 1;
    if (pct20)
        ma = ma * 20 / 100;

    return ma;
};

Result I2c_Bq24193_GetFastChargeCurrentLimit(u32 *ma) {
    u8 raw;
    Result res = I2cRead_OutU8(I2cDevice_Bq24193, BQ24193_CHARGE_CURRENT_CONTROL_REG, &raw);
    if (res)
        return res;

    *ma = I2c_Bq24193_Convert_Raw_mA(raw);
    return 0;
}

Result I2c_Bq24193_SetFastChargeCurrentLimit(u32 ma) {
    u8 raw = I2c_Bq24193_Convert_mA_Raw(ma);
    return I2cSet_U8(I2cDevice_Bq24193, BQ24193_CHARGE_CURRENT_CONTROL_REG, raw);
}

// Converts mA to the raw value for bits [2:0] of REG00
static u8 I2c_Bq24193_Convert_InputmA_Raw(u32 ma) {
    if (ma <= 100)
        return 0b000;
    if (ma <= 150)
        return 0b001;
    if (ma <= 500)
        return 0b010;
    if (ma <= 900)
        return 0b011;
    if (ma <= 1200)
        return 0b100;
    if (ma <= 1500)
        return 0b101;
    if (ma <= 2000)
        return 0b110;
    return 0b111;  // 3000mA max
}

Result I2c_Bq24193_SetInputCurrentLimit(u32 ma) {
    // don't do anything if it's disabled
    if (ma == 0)
        return 0;

    u8 raw;
    Result res = I2cRead_OutU8(I2cDevice_Bq24193, BQ24193_INPUT_SOURCE_CONTROL_REG, &raw);
    if (R_FAILED(res))
        return res;

    raw &= ~0x07;
    raw |= I2c_Bq24193_Convert_InputmA_Raw(ma);

    return I2cSet_U8(I2cDevice_Bq24193, BQ24193_INPUT_SOURCE_CONTROL_REG, raw);
}

// not used
Result I2c_Bq24193_GetInputCurrentLimit(u32 *ma) {
    u8 raw;
    Result res = I2cRead_OutU8(I2cDevice_Bq24193, BQ24193_INPUT_SOURCE_CONTROL_REG, &raw);
    if (R_FAILED(res))
        return res;

    switch (raw & 0x07) {
        case 0b000:
            *ma = 100;
            break;
        case 0b001:
            *ma = 150;
            break;
        case 0b010:
            *ma = 500;
            break;
        case 0b011:
            *ma = 900;
            break;
        case 0b100:
            *ma = 1200;
            break;
        case 0b101:
            *ma = 1500;
            break;
        case 0b110:
            *ma = 2000;
            break;
        case 0b111:
            *ma = 3000;
            break;
        default:
            *ma = 0;
            break;
    }
    return 0;
}

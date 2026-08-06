# Clock table

### MEM clocks (mhz)
* 3400 → max on mariko.
* 3366
* 3333
* 3300
* 3266
* 3233
* 3200 → JEDEC.
* 3166
* 3133
* 3100
* 3066
* 3033
* 3000
* 2966
* 2933 → JEDEC.
* 2900
* 2866
* 2833
* 2800
* 2766
* 2733
* 2700
* 2666 → JEDEC.
* 2633
* 2600
* 2566
* 2533
* 2500
* 2466
* 2433
* 2400 → max on erista, JEDEC.
* 2366
* 2333
* 2300
* 2266
* 2233
* 2200
* 2166
* 2133 → Mariko JEDEC standard max (4266 Modules)
* 2100
* 2066
* 2033
* 2000
* 1996 → JEDEC standard
* 1966
* 1933
* 1900
* 1866 → Mariko JEDEC standard max (3733 Modules)
* 1833
* 1800
* 1766
* 1733
* 1700
* 1666
* 1633
* 1600 → official docked, boost mode, Erista JEDEC standard max (3200 Modules), JEDEC.
* 1331 → official handheld, JEDEC.
* 1065 → Erista only
* 800  → Erista only
* 665  → Erista only
* 408  → Erista only
* 204  → sleep mode
* 102  → Erista only
* 68   → Erista only
* 40   → Erista only

### CPU clocks (mhz)
* 2703 → mariko absolute max, dangerous
* 2601 → unsafe
* 2499
* 2397 → mariko safe max with UV (low speedo)
* 2295
* 2193
* 2091
* 1963 → mariko no UV max clock
* 1887
* 1785 → erista no UV max clock, boost mode
* 1683
* 1581
* 1428
* 1326
* 1224 → sdev oc
* 1122
* 1020 → official docked & handheld
* 918
* 816
* 714
* 612 → sleep mode

### GPU clocks (mhz)
* 1536 → absolute max clock on mariko. Dangerous
* 1497 → max mariko unsafe clock
* 1459
* 1420
* 1382 → Max mariko "safe" clock
* 1344
* 1305
* 1267 → NVIDIA T214(mariko) rating
* 1228 → mariko High UV safe clock
* 1152 → mariko hiOpt-15mV max clock
* 1075 → mariko hiOpt max clock. absolute max clock on erista. very dangerous
* 998 → NVIDIA T210 (erista) rating
* 960 (erista only) → erista high uv/hiOpt-15mV safe max clock
* 921 → erista no UV max clock
* 844
* 768 → official docked
* 691
* 614
* 537
* 460 → max handheld
* 384 → official handheld
* 307 → official handheld
* 230
* 153
* 76 → boost mode

**Notes:**
1. On Erista, CPU in handheld is capped to 1581MHz
2. GPU overclock is capped at 460MHz on erista in handheld
3. On Mariko, cap with hiOpt is 614MHz, with hiOpt-15mV it is 691MHz and with High UV it's 768MHz
4. Clocks higher than 768MHz on erista need the official charger is plugged in.

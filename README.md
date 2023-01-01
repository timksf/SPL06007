# SPL06007
Single header library for SPL06007 (Goertek) operation via I2C, uses arduino Wire library <br>
Sample output of example code: <br>
```
sensor id: 1.0
pressure registers: PB2: 11001011, PB1: 01110101, PB0: 10111000
Pressure raw: 11111111110010110111010110111000	=	-3443272
temp registers: TB2: 00010000, TB1: 00101000, TB0: 01110010
Temperature raw: 00000000000100000010100001110010	=	1058930
pressure_cfg: 00110011
temperature_cfg: 10100010
meas_cfg: 11000111
coeffs ready: true
c0: 197
c1: -255
c00: 80023
c10: -52203
c01: -3131
c11: 1607
c20: -9991
c21: -38
c30: -1340
temperature: 24.92 C
Pressure Pa: 99967.96
Altitude: 152.91
```
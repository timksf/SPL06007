#include <Wire.h>

#include "SPL06007.hpp"

SPL06_007 baro(0x76);

//used to calculate elevation
double sea_level_air_pressure = 101800;

void setup(){

    baro.begin();
    baro.set_pressure_rate(SPL06_007::measurement_rate::RATE_8HZ);
    baro.set_pressure_oversampling(SPL06_007::oversampling_rate::OVERSAMPLING_8);

    baro.set_temperature_rate(SPL06_007::measurement_rate::RATE_4HZ);
    baro.set_temperature_oversampling(SPL06_007::oversampling_rate::OVERSAMPLING_4);
    delay(20);
    baro.set_measurement_mode(SPL06_007::mode::MODE_ALL_BACKGR);

    test_calibration_values();

    Serial.begin(115200);
    Serial.println("Setup complete");
}

void loop(){
    test_baro();
    delay(2000);
}

void printBits(uint8_t val){ 
    for(uint8_t mask = 0x80; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void printBits(uint16_t val){ 
    for(uint16_t mask = 0x8000; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void printBits(uint32_t val){
    for(uint32_t mask = 0x80000000; mask; mask >>= 1){
        if(mask & val) Serial.print('1');
        else Serial.print('0'); 
    }
}

void test_baro(){
    Serial.print("sensor id: ");
    Serial.print(baro.get_prod_id());
    Serial.print(".");
    Serial.println(baro.get_rev_id());
    Serial.print("pressure registers: PB2: ");  printBits(baro.read8(0x00));
    Serial.print(", PB1: ");                    printBits(baro.read8(0x01));
    Serial.print(", PB0: ");                    printBits(baro.read8(0x02));
    Serial.println();
    Serial.print("Pressure raw: ");             printBits(baro.get_pressure_raw());
    Serial.print("\t=\t");
    Serial.print((int32_t)baro.get_pressure_raw());
    Serial.println();

    Serial.print("temp registers: TB2: ");      printBits(baro.read8(0x03));
    Serial.print(", TB1: ");                    printBits(baro.read8(0x04));
    Serial.print(", TB0: ");                    printBits(baro.read8(0x05));
    Serial.println();
    Serial.print("Temperature raw: ");          printBits(baro.get_temp_raw()); 
    Serial.print("\t=\t");
    Serial.print((int32_t)baro.get_temp_raw());
    Serial.println();

    Serial.print("pressure_cfg: ");             printBits(baro.read8(0x06));
    Serial.println();
    Serial.print("temperature_cfg: ");          printBits(baro.read8(0x07));
    Serial.println();
    Serial.print("meas_cfg: ");                 printBits(baro.read8(0x08));
    Serial.println();
    Serial.print("coeffs ready: "); Serial.println(baro.is_coeffs_rdy() ? "true" : "false");

    Serial.print("c0: "); Serial.println((int16_t)baro._calib_coeffs.c0);
    Serial.print("c1: "); Serial.println((int16_t)baro._calib_coeffs.c1);
    Serial.print("c00: "); Serial.println((int32_t)baro._calib_coeffs.c00);
    Serial.print("c10: "); Serial.println((int32_t)baro._calib_coeffs.c10);
    Serial.print("c01: "); Serial.println((int16_t)baro._calib_coeffs.c01);
    Serial.print("c11: "); Serial.println((int16_t)baro._calib_coeffs.c11);
    Serial.print("c20: "); Serial.println((int16_t)baro._calib_coeffs.c20);
    Serial.print("c21: "); Serial.println((int16_t)baro._calib_coeffs.c21);
    Serial.print("c30: "); Serial.println((int16_t)baro._calib_coeffs.c30);
    Serial.print("temperature: "); Serial.print(baro.get_temp_c()); Serial.println(" C");

    Serial.print("Pressure Pa: "); Serial.println(baro.get_pressure_pa());
    Serial.print("Altitude: "); Serial.println(baro.get_altitude(sea_level_air_pressure));
    //  test_calibration_values();
}

void test_calibration_values(){
    //Print coefficients
    Serial.print("coeff0: "); printBits((uint16_t)baro._calib_coeffs.c0);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c0);

    Serial.print("check: "); printBits(baro.read8(0x10)); Serial.print(" :: ");
                             printBits(baro.read8(0x11));
    Serial.println();

    Serial.print("coeff1: "); printBits((uint16_t)baro._calib_coeffs.c1);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c1);

    Serial.print("check: "); printBits(baro.read8(0x12));
    Serial.println();

    Serial.print("coeff00: "); printBits((uint32_t)baro._calib_coeffs.c00);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c00);

    Serial.print("check: "); printBits(baro.read8(0x13)); Serial.print(" :: "); printBits(baro.read8(0x14)); Serial.print(" :: ");
                             printBits(baro.read8(0x15));
    Serial.println();

    Serial.print("coeff10: "); printBits((uint32_t)baro._calib_coeffs.c10);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c10);

    Serial.print("check: "); printBits(baro.read8(0x16)); Serial.print(" :: "); printBits(baro.read8(0x17));
    Serial.println();

    Serial.print("coeff01: "); printBits((uint16_t)baro._calib_coeffs.c01);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c01);

    Serial.print("check: "); printBits(baro.read8(0x18)); Serial.print(" :: "); printBits(baro.read8(0x19));
    Serial.println();

    Serial.print("coeff11: "); printBits((uint16_t)baro._calib_coeffs.c11);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c11);
    Serial.print("check: "); printBits(baro.read8(0x1A)); Serial.print(" :: "); printBits(baro.read8(0x1B));
    Serial.println();

    Serial.print("coeff20: "); printBits((uint16_t)baro._calib_coeffs.c20);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c20);
    Serial.print("check: "); printBits(baro.read8(0x1C)); Serial.print(" :: "); printBits(baro.read8(0x1D));
    Serial.println();

    Serial.print("coeff21: "); printBits((uint16_t)baro._calib_coeffs.c21);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c21);
    Serial.print("check: "); printBits(baro.read8(0x1E)); Serial.print(" :: "); printBits(baro.read8(0x1F));
    Serial.println();

    Serial.print("coeff30: "); printBits((uint16_t)baro._calib_coeffs.c30);
    Serial.print("\t=\t");
    Serial.println(baro._calib_coeffs.c30);
    Serial.print("check: "); printBits(baro.read8(0x20)); Serial.print(" :: "); printBits(baro.read8(0x21));
    Serial.println();
}
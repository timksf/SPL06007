#pragma once

#ifndef __SPL06_007_HPP__
#define __SPL06_007_HPP__

class SPL06_007{
public:

    typedef struct{
        int16_t c0  { 0 };  //12 bit
        int16_t c1  { 0 };  //12 bit
        int32_t c00 { 0 };  //24 bit 
        int32_t c10 { 0 };  //24 bit    
        int16_t c01 { 0 };  //16 bit
        int16_t c11 { 0 };  //16 bit
        int16_t c20 { 0 };  //16 bit
        int16_t c21 { 0 };  //16 bit
        int16_t c30 { 0 };  //16 bit
    } spl06_007_calibration_coeffs;

private:

    //24 bit pressure data |data2|data1|data0
    constexpr static uint8_t _psr_data0 =       0x02;
    constexpr static uint8_t _psr_data1 =       0x01;
    constexpr static uint8_t _psr_data2 =       0x00;
    //24 bit temperature data |data2|data1|data0
    constexpr static uint8_t _tmp_data0 =       0x05;
    constexpr static uint8_t _tmp_data1 =       0x04;
    constexpr static uint8_t _tmp_data2 =       0x03;
    //config registers
    constexpr static uint8_t _psr_cfg_reg =     0x06;
    constexpr static uint8_t _tmp_cfg_reg =     0x07;
    constexpr static uint8_t _meas_cfg_reg =    0x08;
    //interrupt and fifo cfgs
    constexpr static uint8_t _reg_cfg =         0x09;
    //interrupt status register
    constexpr static uint8_t _int_sts =         0x0A;
    constexpr static uint8_t _fifo_sts =        0x0B;
 
    constexpr static uint8_t _rst_reg =         0x0C;
    constexpr static uint8_t _id_reg =          0x0D;
 
    //calibration registers
    constexpr static uint8_t _cal_coef0 =       0x10;
    constexpr static uint8_t _cal_coef0_1 =     0x11; //separated at bit 4 to bit 3
    constexpr static uint8_t _cal_coef1 =       0x12;
    constexpr static uint8_t _cal_coef00H =     0x13;
    constexpr static uint8_t _cal_coef00L =     0x14;
    constexpr static uint8_t _cal_coef00XL_10 = 0x15; //separated at bit 4 to bit 3
    constexpr static uint8_t _cal_coef10H =     0x16;
    constexpr static uint8_t _cal_coef10L =     0x17;
    constexpr static uint8_t _cal_coef01H =     0x18;
    constexpr static uint8_t _cal_coef01L =     0x19;
    constexpr static uint8_t _cal_coef11H =     0x1A;
    constexpr static uint8_t _cal_coef11L =     0x1B;
    constexpr static uint8_t _cal_coef20H =     0x1C;
    constexpr static uint8_t _cal_coef20L =     0x1D;
    constexpr static uint8_t _cal_coef21H =     0x1E;
    constexpr static uint8_t _cal_coef21L =     0x1F;
    constexpr static uint8_t _cal_coef30H =     0x20;
    constexpr static uint8_t _cal_coef30L =     0x21;
 
    constexpr static uint8_t _n_coefs = 18;

    constexpr static uint8_t _def_base_address = 0x77;

    constexpr static inline uint32_t _sampling_scale_factors[] = {
        524288u,
        1572864,
        3670016,
        7864320,
        253952,
        516096,
        1040384,
        2088960
    };

    const uint8_t _i2c_address;

    //spl06_007_calibration_coeffs _calib_coeffs;
    uint8_t _prod_id;
    uint8_t _rev_id;
    uint8_t _psr_cfg;
    uint8_t _tmp_cfg{ 0x80 }; //init with MSB set to 1 to use external temp sensor as noted in datasheet
    uint8_t _meas_cfg;

    void write_register(uint8_t addr, uint8_t val){
        Wire.beginTransmission(_i2c_address);
        Wire.write(addr);
        Wire.write(val);
        Wire.endTransmission();
    }

    void update_meas_cfg(){
        uint8_t val = read8(_meas_cfg_reg);
        _meas_cfg = val;
    }

public:
    spl06_007_calibration_coeffs _calib_coeffs;

    enum class measurement_rate : uint8_t{
        RATE_1HZ =      0b000,
        RATE_2HZ =      0b001,
        RATE_4HZ =      0b010,
        RATE_8HZ =      0b011,
        RATE_16HZ =     0b100,
        RATE_32HZ =     0b101,
        RATE_64HZ =     0b110,
        RATE_128HZ =    0b111,
    };

    enum class oversampling_rate : uint8_t {
        OVERSAMPLING_1 = 0b0000,
        OVERSAMPLING_2 = 0b0001,
        OVERSAMPLING_4 = 0b0010,
        OVERSAMPLING_8 = 0b0011
        // SAMPLE_RATE_16 = 0b0000
        // SAMPLE_RATE_32 = 0b0000
        // SAMPLE_RATE_64 = 0b0000
        // SAMPLE_RATE_128 = 0b0000
    };

    enum class mode : uint8_t {
        MODE_IDLE       =   0b000,
        MODE_PSR_FORCED =   0b001,
        MODE_TMP_FORCED =   0b010,
        MODE_PSR_BACKGR =   0b101,
        MODE_TMP_BACKGR =   0b110,
        MODE_ALL_BACKGR =   0b111
    };

    SPL06_007(uint8_t i2c_address) : _i2c_address(i2c_address){
    };

    SPL06_007() : SPL06_007(_def_base_address){};

    void begin(){
        Wire.begin();
        uint8_t id = read8(_id_reg);
        _prod_id = (id & 0b11110000) >> 4;
        _rev_id = (id & 0b00001111);
        _meas_cfg = read8(_meas_cfg_reg);
        while(!is_coeffs_rdy()) 
            delay(5);
        load_coefficients();
        delay(10);
    }

    uint8_t read8(uint8_t addr){
        uint8_t val = 0x00;
        Wire.beginTransmission(_i2c_address);
        Wire.write(addr);
        Wire.endTransmission();
        Wire.requestFrom(_i2c_address, (uint8_t) 1u);
        val = Wire.read();
        return val;
    }

    uint16_t read16(uint8_t addr){
        uint16_t val;
        Wire.beginTransmission(_i2c_address);
        Wire.write(addr);
        Wire.endTransmission();
        Wire.requestFrom(_i2c_address, (uint8_t) 2u);
        val = (Wire.read() << 8) | Wire.read();
        return val;
    }

    uint16_t read16_little_endian(uint8_t addr){
        uint16_t val = read16(addr);
        return (val >> 8) | (val << 8);
    }

    uint32_t read24(uint8_t addr){
        uint32_t val;
        Wire.beginTransmission(_i2c_address);
        Wire.write(addr);
        Wire.endTransmission();
        Wire.requestFrom(_i2c_address, (uint8_t)3u);
        val = Wire.read();
        val <<= 8;
        val |= Wire.read();
        val <<= 8;
        val |= Wire.read();
        return val;
    }

    uint8_t get_prod_id(){
        return _prod_id;
    }

    uint8_t get_rev_id(){
        return _rev_id;
    }

    uint32_t get_pressure_raw(){
        uint32_t tmp = read24(_psr_data2);
        if(tmp & (1UL << 23))
            tmp |= 0xFF000000UL;
        return tmp;
    }

    uint32_t get_temp_raw(){
        uint32_t tmp = read24(_tmp_data2);
        if(tmp & (1UL << 23))
            tmp |= 0xFF000000;
        return tmp;
    }

    void set_pressure_rate(measurement_rate rate){
        constexpr uint8_t rate_mask = 0b01110000;
        _psr_cfg = (_psr_cfg & ~rate_mask) | ((uint8_t) rate << 4);
        write_register(_psr_cfg_reg, _psr_cfg);
    }

    void set_pressure_oversampling(oversampling_rate rate){
        constexpr uint8_t rate_mask = 0b00001111;
        _psr_cfg = (_psr_cfg &~ rate_mask) | ((uint8_t) rate);
        write_register(_psr_cfg_reg, _psr_cfg);
    }

    void set_temperature_rate(measurement_rate rate){
        constexpr uint8_t rate_mask = 0b01110000;
        _tmp_cfg = (_tmp_cfg & ~rate_mask) | ((uint8_t) rate << 4);
        write_register(_tmp_cfg_reg, _tmp_cfg);
    }

    void set_temperature_oversampling(oversampling_rate rate){
        constexpr uint8_t rate_mask = 0b00000111;
        _tmp_cfg = (_tmp_cfg & ~rate_mask) | ((uint8_t) rate);
        write_register(_tmp_cfg_reg, _tmp_cfg);
    }

    void set_measurement_mode(mode m){
        constexpr uint8_t mode_mask = 0b00000111;
        _meas_cfg = (_meas_cfg & ~mode_mask) | ((uint8_t) m);
        write_register(_meas_cfg_reg, _meas_cfg);
    }

    bool is_coeffs_rdy(){
        update_meas_cfg();
        return (_meas_cfg & 0x80) >> 7;
    }

    bool is_sensor_rdy(){
        update_meas_cfg();
        return (_meas_cfg & 0x40) >> 6;
    }

    bool is_pressure_rdy(){
        update_meas_cfg();
        return (_meas_cfg & 0x10) >> 4;
    }
    
    bool is_temp_rdy(){
        update_meas_cfg();
        return (_meas_cfg & 0x20) >> 5;
    }

    void reset(){
        write_register(_rst_reg, 0b1001);
    }

    float get_temp_c(){
        return (float)_calib_coeffs.c0 * 0.5f 
                    + (float)_calib_coeffs.c1 * get_temp_scaled();
    }

    float get_temp_scaled(){
        return (float) get_temp_raw() / (float) get_scale_factor_temp();
    }

    uint32_t get_scale_factor_temp(){
        uint8_t scale_index = _tmp_cfg & 0x07;
        return _sampling_scale_factors[scale_index];
    }

    double get_pressure_pa(){
        double p_sc = get_pressure_scaled();
        double t_sc = get_temp_scaled();

        double p_calib =  (double)_calib_coeffs.c00 + 
                p_sc * (_calib_coeffs.c10 + p_sc * (_calib_coeffs.c20 + p_sc * _calib_coeffs.c30));
        double t_comp = t_sc * (_calib_coeffs.c01 + p_sc * (_calib_coeffs.c11 + p_sc * _calib_coeffs.c21));

        return p_calib + t_comp;
    }

    double get_altitude(double pressure_sealevel){
        return 44330 * (1 - pow((get_pressure_pa() / pressure_sealevel), 0.190263));
    }

    double get_pressure_scaled(){
        return (int32_t) get_pressure_raw() / (double) get_scale_factor_pressure();
    }

    uint32_t get_scale_factor_pressure(){
        uint8_t scale_index = _psr_cfg & 0x07;
        return _sampling_scale_factors[scale_index];
    }

    void load_coefficients(){
        uint8_t tmp;
        //c0
        _calib_coeffs.c0 = 0x000 | (read8(_cal_coef0) << 4);
        tmp = read8(_cal_coef0_1);
        //upper 4 bits belong to c0
        _calib_coeffs.c0 |= ((0xF0 & tmp) >> 4);
        //lower 4 bits belong to c1
        //c1
        _calib_coeffs.c1 = 0x000 | ((0x0F & tmp) << 8);
        _calib_coeffs.c1 |= read8(_cal_coef1);
        //c00
        _calib_coeffs.c00 = read8(_cal_coef00H);
        _calib_coeffs.c00 <<= 8;
        _calib_coeffs.c00 |= read8(_cal_coef00L);
        _calib_coeffs.c00 <<= 4;
        tmp = read8(_cal_coef00XL_10);
        //upper 4 bits belong to c00
        _calib_coeffs.c00 |= ((0xF0 & tmp) >> 4);

        //lower 4 bits belong to c10
        //c10
        tmp &= 0x0F;
        _calib_coeffs.c10 = tmp;
        _calib_coeffs.c10 <<= 8;
        _calib_coeffs.c10 |= read8(_cal_coef10H);
        _calib_coeffs.c10 <<= 8;
        _calib_coeffs.c10 |= read8(_cal_coef10L);
        //c01
        _calib_coeffs.c01 = ((uint16_t)read8(_cal_coef01H) << 8) | (uint16_t)(read8(_cal_coef01L));
        //c11
        _calib_coeffs.c11 = ((uint16_t)read8(_cal_coef11H) << 8) | (uint16_t)(read8(_cal_coef11L));
        //c20
        _calib_coeffs.c20 = ((uint16_t)read8(_cal_coef20H) << 8) | (uint16_t)(read8(_cal_coef20L));
        //c21
        _calib_coeffs.c21 = ((uint16_t)read8(_cal_coef21H) << 8) | (uint16_t)(read8(_cal_coef21L));
        //c30
        _calib_coeffs.c30 = ((uint16_t)read8(_cal_coef30H) << 8) | (uint16_t)(read8(_cal_coef30L));
        //Fill with 1's if number is negative
        if(_calib_coeffs.c0  &  (1 << 11)) _calib_coeffs.c0     |= 0xF000; 
        if(_calib_coeffs.c1  &  (1 << 11)) _calib_coeffs.c1     |= 0xF000; 
        if(_calib_coeffs.c00 &  (1UL << 19)) _calib_coeffs.c00  |= 0xFFF00000UL; 
        if(_calib_coeffs.c10 &  (1UL << 19)) _calib_coeffs.c10  |= 0xFFF00000UL; 
    }

};


#endif
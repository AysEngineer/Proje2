#include "display_frs_m3.h"

const uint8_t __segment_map_table[DSP_SUPPORTED_CHARS] = {    /**< Map table for alphanumeric chars */
    0x3F,   //0                          0
    0x06,   //1                          1
    0x5B,   //2                          2
    0x4F,   //3                          3
    0x66,   //4                          4
    0x6D,   //5                          5
    0x7D,   //6                          6
    0x07,   //7                          7
    0x7F,   //8                          8
    0x6F,   //9                          9
    0x77,   //A                          10
    0x7C,   //b                          11
    0x39,   //C                          12
    0x3D,   //D                          13
    0x79,   //E                          14
    0xF1,   //F                          15
    0x00,   //SPACE (BLANK DIGIT)        16
    0x38,   //L                          17
    0x08,   //_                          18
    0x50,   //r                          19
    0x5E,   //d                          20
    0x1C,   //u                          21
    0x74,   //h                          22
    0x63,   //derece sembolu             23
    0x54,   //n                          24
    0x40,   //_                          25
};

void __update_chars_segments(uint8_t data) {
    if ( ( data & 0x40 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_G, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_G, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x20 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_F, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_F, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x10 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_E, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_E, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x08 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_D, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_D, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x04 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_C, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_C, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x02 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_B, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_B, BSP_IO_LEVEL_LOW);
    if ( ( data & 0x01 ) != 0 )
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_A, BSP_IO_LEVEL_HIGH);
    else
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_A, BSP_IO_LEVEL_LOW);
}

void __set_com_status(enum DisplayCOM com, bool status) {
    if ( com == COM_1 ) {
        if ( status )       R_IOPORT_PinWrite(&g_ioport_ctrl, DIG1, BSP_IO_LEVEL_HIGH);
        else                R_IOPORT_PinWrite(&g_ioport_ctrl, DIG1, BSP_IO_LEVEL_LOW);
    } else {
        if ( status )       R_IOPORT_PinWrite(&g_ioport_ctrl, DIG2, BSP_IO_LEVEL_HIGH);
        else                R_IOPORT_PinWrite(&g_ioport_ctrl, DIG2, BSP_IO_LEVEL_LOW);        
    }
}

void __update_degree_icon(enum DisplayCOM com, bool isOn) {
    if ( com != COM_1 ) {
        return;
    }
    if ( isOn ) {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_H, BSP_IO_LEVEL_HIGH);
    } else {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_H, BSP_IO_LEVEL_LOW);
    }    
}

void __update_warning_icon(enum DisplayCOM com, bool isOn) {
    if ( com != COM_2 ) {
        return;
    }
    if ( isOn ) {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_H, BSP_IO_LEVEL_HIGH);
    } else {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_H, BSP_IO_LEVEL_LOW);
    }
}

void __update_on_off_icon(enum DisplayCOM com, bool isOn) {
    if ( isOn ) {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_K, BSP_IO_LEVEL_HIGH);
    } else {
        R_IOPORT_PinWrite(&g_ioport_ctrl, SEG_K, BSP_IO_LEVEL_LOW);
    }
}


void setDisplayFrsM3(enum DisplayCOM com, DisplayDigit* displayDigit, const DisplayIcons* displayIcons) {
    //Ortak katot
    __set_com_status(COM_1, true);
    __set_com_status(COM_2, true);

    if ( displayDigit->isOn == false ) {
        __update_chars_segments(__segment_map_table[DSP_CHAR_SPACE]);
    } else {
        __update_chars_segments(__segment_map_table[displayDigit->value]);
    }
    __update_degree_icon(com, displayIcons->degreeStatus.isOn);
    __update_warning_icon(com, displayIcons->warningStatus.isOn);
    __update_on_off_icon(com, displayIcons->onOffStatus.isOn);

    //Ortak katot
    if ( com == COM_1 )                     __set_com_status(COM_1, false);
    else if ( com == COM_2 )                __set_com_status(COM_2, false);
}

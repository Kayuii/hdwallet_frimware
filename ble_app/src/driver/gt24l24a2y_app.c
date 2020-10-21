#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "driver/gt24l24a2y_app.h"
#include "driver/gt24l24a2y.h"
#include "driver/gt24l24a2y_hal.h"

static const uint8_t lenforzktype[] = {
    0, 8, 8, 12, 26, 26, 16, 34, 34, 48, 74, 64, 130,  // ASCII_5X7-ASCII_32_B
};

uint8_t zk_getbmp(const uint32_t unicode, uint8_t zktype, uint8_t* buf) {
    uint8_t bmplen = 0;
    uint8_t asciicode = 0;
    unsigned int tmpcode;
    asciicode = (uint8_t)unicode;

    switch (zktype) {
        case ASCII_5X7:
        case ASCII_7X8:
        case ASCII_6X12:
        case ASCII_12_B_A:
        case ASCII_12_B_T:
        case ASCII_8X16:
        case ASCII_16_A:
        case ASCII_16_T:
        case ASCII_12X24:
        case ASCII_24_B:
        case ASCII_16X32:
        case ASCII_32_B:
            ASCII_GetData(asciicode, zktype, buf);
            bmplen = lenforzktype[zktype];
            break;
        case SEL_GB:  // GB18030	16x16
            //			unsigned int GB_CODE;
            //			GB_CODE = U2G(unicode);
            //			hzbmp16(SEL_GB, GB_CODE, 0, 16, buf);
            tmpcode = U2G(unicode);
            hzbmp16(SEL_GB, tmpcode, 0, 16, buf);
            bmplen = 32;
            break;
        case SEL_JIS:
            //			unsigned int JIS_CODE;
            //    		JIS_CODE = U2J(unicode);
            //			hzbmp16(SEL_JIS, JIS_CODE, 0, 16, buf);
            tmpcode = U2J(unicode);
            hzbmp16(SEL_JIS, tmpcode, 0, 16, buf);
            bmplen = 32;
            break;
        case SEL_KSC:
            //			unsigned int KSC_CODE;
            //    		KSC_CODE = U2K(unicode);
            //			hzbmp16(SEL_KSC, KSC_CODE, 0, 16, buf);
            tmpcode = U2K(unicode);
            hzbmp16(SEL_KSC, tmpcode, 0, 16, buf);
            bmplen = 32;
            break;
        case SEL_GB_24X24:  // GB2312 24x24
            //			unsigned int GB_CODE;
            //			GB_CODE = U2G(unicode);
            //			hzbmp24(SEL_GB, GB_CODE, 0, 24, buf);
            tmpcode = U2G(unicode);
            hzbmp16(SEL_GB, tmpcode, 0, 16, buf);
            bmplen = 72;
            break;
        default:
            bmplen = 0;
            break;
    }

    return bmplen;
}

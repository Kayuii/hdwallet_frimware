
/* This file was automatically generated by nrfutil on 2019-02-14 (YY-MM-DD) at 14:06:19 */

#include "sdk_config.h"
#include "stdint.h"
#include "compiler_abstraction.h"

#if NRF_CRYPTO_BACKEND_OBERON_ENABLED
/* Oberon backend is changing endianness thus public key must be kept in RAM. */
#define _PK_CONST
#else
#define _PK_CONST const
#endif


/* This file was generated with a throwaway private key, that is only inteded for a debug version of the DFU project.
  Please see https://github.com/NordicSemiconductor/pc-nrfutil/blob/master/README.md to generate a valid public key. */

#ifdef NRF_DFU_DEBUG_VERSION 

/** @brief Public key used to verify DFU images */
__ALIGN(4) const uint8_t pk[64] =
{
    0x74, 0xa3, 0x86, 0xca, 0xba, 0x53, 0xd7, 0x80, 0x91, 0x72, 0xbf, 0x5c, 0xe6, 0x0f, 0x0f, 0x18, 0x12, 0x29, 0x26, 0x20, 0xf4, 0x7d, 0x8f, 0xc2, 0x0a, 0x4f, 0xce, 0xf3, 0xd2, 0x14, 0xcb, 0x0e,
    0x29, 0x30, 0x46, 0xfd, 0x1c, 0x24, 0xed, 0x49, 0x6b, 0x86, 0xe7, 0x83, 0xe2, 0x1a, 0x25, 0xdc, 0xed, 0xa2, 0x55, 0xa4, 0xcf, 0xbe, 0x64, 0x6d, 0x2b, 0x59, 0xc8, 0x58, 0x30, 0x50, 0x91, 0x84
};
#else
/** @brief Public key used to verify DFU images */
__ALIGN(4) const uint8_t pk[64] =
{
    0x8c, 0x63, 0xe5, 0x8b, 0xe1, 0x87, 0x8b, 0x00, 0x6b, 0x7f, 0x28, 0x94, 0x49, 0x68, 0xea, 0x73, 0xdc, 0x7f, 0x1c, 0xd8, 0x09, 0x6d, 0xeb, 0xc2, 0x21, 0x89, 0x4d, 0x60, 0xe6, 0xa2, 0xa9, 0x0c,
    0xd4, 0x96, 0x07, 0x13, 0xf4, 0xf4, 0x1a, 0x44, 0xec, 0x33, 0x79, 0x60, 0x20, 0x7b, 0xfa, 0x07, 0x11, 0xcc, 0xd2, 0x6f, 0x81, 0x5b, 0x6d, 0xab, 0x9f, 0x01, 0x1c, 0xeb, 0x58, 0xfb, 0xa6, 0x1a
};
#endif

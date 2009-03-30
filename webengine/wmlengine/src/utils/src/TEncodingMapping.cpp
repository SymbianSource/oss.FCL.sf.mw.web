/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/


#include "TEncodingMapping.h"
#include "nwx_http_defs.h"
#include "nwx_defs.h"
#include <charconv.h>

const SupportedCharset supportedCharset[] =
    {
    {KCharacterSetIdentifierIso88591, HTTP_iso_8859_1, EISO8859_1, (NW_Uint8*)NW_Iso88591_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88591, HTTP_iso_8859_1, EISO8859_1, (NW_Uint8*)NW_Windows1252_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierUtf8,     HTTP_utf_8,      EUTF8,      (NW_Uint8*)NW_Utf8_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierGb2312,   HTTP_gb2312,     EGb2312,    (NW_Uint8*)NW_Gb2312_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierBig5,     HTTP_big5,       EBig5,      (NW_Uint8*)NW_Big5_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88592, HTTP_iso_8859_2, EISO8859_2, (NW_Uint8*)NW_Iso88592_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88594, HTTP_iso_8859_4, EISO8859_4, (NW_Uint8*)NW_Iso88594_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88595, HTTP_iso_8859_5, EISO8859_5, (NW_Uint8*)NW_Iso88595_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88596, HTTP_iso_8859_6, EISO8859_6, (NW_Uint8*)NW_Iso88596_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88597, HTTP_iso_8859_7, EISO8859_7, (NW_Uint8*)NW_Iso88597_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88598, HTTP_iso_8859_8, EISO8859_8, (NW_Uint8*)NW_Iso88598_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88599, HTTP_iso_8859_9, EISO8859_9, (NW_Uint8*)NW_Iso88599_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso88599, HTTP_iso_8859_9, EISO8859_9, (NW_Uint8*)NW_Iso885915_CharsetStr, NW_TRUE},
    {0x100059d6, HTTP_windows_1250, EWindows1250, (NW_Uint8*)NW_Windows1250_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1250
    {0x100059D7, HTTP_windows_1251, EWindows1251, (NW_Uint8*)NW_Windows1251_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1251
    {0x100059D8, HTTP_windows_1253, EWindows1253, (NW_Uint8*)NW_Windows1253_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1253
    {0x100059D9, HTTP_windows_1254, EWindows1254, (NW_Uint8*)NW_Windows1254_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1254
    {0x101F8547, HTTP_windows_1255, EWindows1255, (NW_Uint8*)NW_Windows1255_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1255
    {0x101F8547, HTTP_windows_1255, EWindows1255, (NW_Uint8*)NW_Iso88598i_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1255
    {0x101F8548, HTTP_windows_1256, EWindows1256, (NW_Uint8*)NW_Windows1256_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1256
    {0x100059DA, HTTP_windows_1257, EWindows1257, (NW_Uint8*)NW_Windows1257_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-1257
    {0x101F8549, HTTP_tis_620,      ETis620,      (NW_Uint8*)NW_Tis_620_CharsetStr, NW_TRUE}, // no header file is provided with the converter tis-620
    {KCharacterSetIdentifierShiftJis,    HTTP_shift_JIS,      EShiftJis,      (NW_Uint8*)NW_ShiftJis_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierShiftJis,    HTTP_shift_JIS,      EShiftJis,      (NW_Uint8*)NW_ShiftJis_CharsetStr2, NW_TRUE},
    {KCharacterSetIdentifierShiftJis,    HTTP_shift_JIS,      EShiftJis,      (NW_Uint8*)NW_X_SJis_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierJis,         HTTP_jis_x0201_1997, EJisX0201_1997, (NW_Uint8*)NW_JisX0201_1997_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierJis,         HTTP_jis_x0208_1997, EJisX0208_1997, (NW_Uint8*)NW_JisX0208_1997_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierEucJpPacked, HTTP_euc_jp,         EEucJp,         (NW_Uint8*)NW_EucJp_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierEucJpPacked, HTTP_euc_jp,         EEucJp,         (NW_Uint8*)NW_X_EucJp_CharsetStr, NW_TRUE},
    {KCharacterSetIdentifierIso2022Jp,   HTTP_iso_2022_jp,    EIso2022Jp,     (NW_Uint8*)NW_Iso2022Jp_CharsetStr, NW_TRUE},
    {0x101F854A, HTTP_windows_874, EWindows874, (NW_Uint8*)NW_Windows874_CharsetStr, NW_TRUE}, // no header file is provided with the converter windows-874
    {0x101F8778, HTTP_Koi8_r,      EKoi8_r,     (NW_Uint8*)NW_Koi8r_CharsetStr, NW_TRUE}, // no header file is provided with the converter Russian
    {0x101F8761, HTTP_Koi8_u,      EKoi8_u,     (NW_Uint8*)NW_Koi8u_CharsetStr, NW_TRUE}, // no header file is provided with the converter Ukrainian
    {KCharacterSetIdentifierUcs2, HTTP_iso_10646_ucs_2, EISO10646_UCS_2, (NW_Uint8*)NW_Iso10646Ucs2_CharsetStr, NW_FALSE},
    {KCharacterSetIdentifierUcs2, HTTP_iso_10646_ucs_2, EISO10646_UCS_2, (NW_Uint8*)NW_Unicode_CharsetStr, NW_FALSE},
    {0, 0, EAutomatic, (NW_Uint8*)NULL, NW_TRUE},
    {0, 0, EDummyLast, (NW_Uint8*)NULL, NW_TRUE}
};

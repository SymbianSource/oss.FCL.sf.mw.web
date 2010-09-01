/*
* Copyright (c) 2000 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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

#include "nw_browser_browsermimetablei.h"

#include "nw_imagech_epoc32contenthandler.h"
#include "nw_xhtml_epoc32contenthandler.h"
#include "nw_wml_core.h"  /* needed for the wml content handler */
#include "nw_wmlscriptch_wmlscriptcontenthandler.h"
#include "nw_wml1x_epoc32contenthandler.h"
#include "nwx_http_defs.h"

#ifdef __TEST_TESTHARNESS_ENABLED
#include "nw_bth_testharnesscontenthandler.h"
#endif
 
/* ------------------------------------------------------------------------- */
static
const NW_HED_MimeTable_Entry_t _NW_Browser_MimeTable_Array[] = {
  { HTTP_image_vnd_wap_wbmp_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  {	HTTP_image_gif_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  {	HTTP_image_jpeg_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  {	HTTP_image_tiff_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  {	HTTP_image_png_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  {	HTTP_image_bmp_string,
    (NW_HED_ContentHandler_Class_t*) &NW_ImageCH_Epoc32ContentHandler_Class, NW_FALSE },
  { HTTP_application_wml_xml_string,  /* WML 2.0 */
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_application_vnd_wap_wml_plus_xml_string,
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_application_xhtml_xml_string,  /* XHTML */
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  /* TODO: remove when xhtml is no longer handled via server wbxml encoding */
  { HTTP_application_vnd_wap_xhtml_xml_string,  /* XHTML - MP*/
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_application_vnd_wap_wbxml_string,
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  /* TODO: remove text_html when content delivered as above types */
  { HTTP_text_html_string,
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_text_vnd_wap_wml_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Wml1x_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_application_vnd_wap_wmlc_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Wml1x_Epoc32ContentHandler_Class, NW_TRUE },
/*//R
  { HTTP_application_x_javascript_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Ecma_ContentHandler_Class, NW_FALSE },
  { HTTP_text_javascript_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Ecma_ContentHandler_Class, NW_FALSE },
  { HTTP_text_ecmascript_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Ecma_ContentHandler_Class, NW_FALSE },
*/
  { HTTP_application_xhtml_xml_saved_string,
    (NW_HED_ContentHandler_Class_t*) &NW_XHTML_Epoc32ContentHandler_Class, NW_TRUE },
  { HTTP_application_wmlc_saved_string,
    (NW_HED_ContentHandler_Class_t*) &NW_Wml1x_Epoc32ContentHandler_Class, NW_TRUE },
#ifdef __TEST_TESTHARNESS_ENABLED
  { HTTP_text_plain_string,
    (NW_HED_ContentHandler_Class_t*) &NW_BTH_TestHarness_ContentHandler_Class, NW_TRUE },
  { HTTP_application_x_nokia_test_harness_string,
    (NW_HED_ContentHandler_Class_t*) &NW_BTH_TestHarness_ContentHandler_Class, NW_TRUE  },
#endif
  { HTTP_application_vnd_wap_wmlscriptc_string,
    (NW_HED_ContentHandler_Class_t*) &NW_WmlScript_ContentHandler_Class, NW_TRUE },
};

/* ------------------------------------------------------------------------- */
const
NW_Browser_MimeTable_Class_t NW_Browser_MimeTable_Class = {
  { /* NW_Object_Core       */
    /* superclass           */ &NW_HED_MimeTable_Class,
    /* queryInterface       */ _NW_Object_Core_QueryInterface
  },
  { /* NW_HED_MimeTable     */
    /* numEntries           */ sizeof (_NW_Browser_MimeTable_Array)
                                 / sizeof (_NW_Browser_MimeTable_Array[0]),
    /* entries              */ &_NW_Browser_MimeTable_Array[0],
    /* createContentHandler */ _NW_HED_MimeTable_CreateContentHandler
  },
  { /* NW_Browser_MimeTable */
    /* unused               */ 0
  }
};

/* ------------------------------------------------------------------------- */
const NW_Browser_MimeTable_t NW_Browser_MimeTable = {
  { { &NW_Browser_MimeTable_Class } }
};

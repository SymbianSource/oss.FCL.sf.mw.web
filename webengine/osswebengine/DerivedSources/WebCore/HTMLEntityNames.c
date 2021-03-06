/* ANSI-C code produced by gperf version 2.7.2 */
/* Command-line: gperf -a -L ANSI-C -C -G -c -o -t -k '*' -N findEntity -D -s 2 /cygdrive/x/tot/WebCore/html/HTMLEntityNames.gperf  */
/*   This file is part of the KDE libraries
  
     Copyright (C) 1999 Lars Knoll (knoll@mpi-hd.mpg.de)
  
     This library is free software; you can redistribute it and/or
     modify it under the terms of the GNU Library General Public
     License as published by the Free Software Foundation; either
     version 2 of the License, or (at your option) any later version.
  
     This library is distributed in the hope that it will be useful,
     but WITHOUT ANY WARRANTY; without even the implied warranty of
     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
     Library General Public License for more details.
  
     You should have received a copy of the GNU Library General Public License
     along with this library; see the file COPYING.LIB.  If not, write to
     the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
     Boston, MA 02110-1301, USA.
  
  ----------------------------------------------------------------------------
  
    HTMLEntityNames.gperf: input file to generate a hash table for entities
    HTMLEntityNames.c: DO NOT EDIT! generated by the command
    "gperf -a -L "ANSI-C" -C -G -c -o -t -k '*' -NfindEntity -D -s 2 HTMLEntityNames.gperf > entities.c"   
    from HTMLEntityNames.gperf 

*/
struct Entity {
    const char *name;
    int code;
};

#define TOTAL_KEYWORDS 262
#define MIN_WORD_LENGTH 2
#define MAX_WORD_LENGTH 8
#define MIN_HASH_VALUE 4
#define MAX_HASH_VALUE 1556
/* maximum key range = 1553, duplicates = 1 */

#ifdef __GNUC__
__inline
#else
#ifdef __cplusplus
inline
#endif
#endif
static unsigned int
hash (register const char *str, register unsigned int len)
{
  static const unsigned short asso_values[] =
    {
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,   15,
        25,   20,   10, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557,   70,   10,    0,    0,  130,
      1557,   10,    0,  140, 1557,   10,    5,    0,    5,   75,
        15,    0,    5,   10,    0,  100, 1557, 1557,    5,   35,
         0, 1557, 1557, 1557, 1557, 1557, 1557,    0,   30,   95,
        30,    5,   95,  100,  305,   15,    0,    0,   10,  350,
       400,  170,  290,  145,    0,  390,   30,  110,   90,    5,
        20,  105,    5, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557, 1557,
      1557, 1557, 1557, 1557, 1557, 1557
    };
  register int hval = len;

  switch (hval)
    {
      default:
      case 8:
        hval += asso_values[(unsigned char)str[7]];
      case 7:
        hval += asso_values[(unsigned char)str[6]];
      case 6:
        hval += asso_values[(unsigned char)str[5]];
      case 5:
        hval += asso_values[(unsigned char)str[4]];
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

static const struct Entity wordlist[] =
  {
    {"rarr", 0x2192},
    {"LT", 60},
    {"GT", 62},
    {"zwj", 0x200d},
    {"larr", 0x2190},
    {"le", 0x2264},
    {"real", 0x211c},
    {"Xi", 0x039e},
    {"Pi", 0x03a0},
    {"darr", 0x2193},
    {"xi", 0x03be},
    {"eta", 0x03b7},
    {"Zeta", 0x0396},
    {"lt", 60},
    {"zeta", 0x03b6},
    {"Beta", 0x0392},
    {"Delta", 0x0394},
    {"beta", 0x03b2},
    {"trade", 0x2122},
    {"rArr", 0x21d2},
    {"delta", 0x03b4},
    {"lArr", 0x21d0},
    {"AMP", 38},
    {"THORN", 0x00de},
    {"tilde", 0x02dc},
    {"atilde", 0x00e3},
    {"crarr", 0x21b5},
    {"Ntilde", 0x00d1},
    {"dArr", 0x21d3},
    {"ge", 0x2265},
    {"reg", 0x00ae},
    {"Mu", 0x039c},
    {"Tau", 0x03a4},
    {"uarr", 0x2191},
    {"Nu", 0x039d},
    {"COPY", 0x00a9},
    {"rceil", 0x2309},
    {"gt", 62},
    {"ETH", 0x00d0},
    {"aelig", 0x00e6},
    {"deg", 0x00b0},
    {"lceil", 0x2308},
    {"tau", 0x03c4},
    {"radic", 0x221a},
    {"REG", 0x00ae},
    {"iexcl", 0x00a1},
    {"brvbar", 0x00a6},
    {"cedil", 0x00b8},
    {"Ccedil", 0x00c7},
    {"Eta", 0x0397},
    {"bull", 0x2022},
    {"Atilde", 0x00c3},
    {"Otilde", 0x00d5},
    {"or", 0x2228},
    {"QUOT", 34},
    {"uArr", 0x21d1},
    {"loz", 0x25ca},
    {"divide", 0x00f7},
    {"agrave", 0x00e0},
    {"egrave", 0x00e8},
    {"circ", 0x02c6},
    {"acirc", 0x00e2},
    {"Dagger", 0x2021},
    {"ecirc", 0x00ea},
    {"igrave", 0x00ec},
    {"iota", 0x03b9},
    {"frac14", 0x00bc},
    {"icirc", 0x00ee},
    {"frac34", 0x00be},
    {"frac12", 0x00bd},
    {"dagger", 0x2020},
    {"acute", 0x00b4},
    {"aacute", 0x00e1},
    {"eacute", 0x00e9},
    {"ccedil", 0x00e7},
    {"iacute", 0x00ed},
    {"otilde", 0x00f5},
    {"Agrave", 0x00c0},
    {"Ograve", 0x00d2},
    {"Acirc", 0x00c2},
    {"Yacute", 0x00dd},
    {"Ocirc", 0x00d4},
    {"euro", 0x20ac},
    {"forall", 0x2200},
    {"para", 0x00b6},
    {"ordf", 0x00aa},
    {"Ugrave", 0x00d9},
    {"oelig", 0x0153},
    {"pi", 0x03c0},
    {"harr", 0x2194},
    {"Ucirc", 0x00db},
    {"ugrave", 0x00f9},
    {"Aacute", 0x00c1},
    {"ucirc", 0x00fb},
    {"Oacute", 0x00d3},
    {"Chi", 0x03a7},
    {"part", 0x2202},
    {"weierp", 0x2118},
    {"AElig", 0x00c6},
    {"Egrave", 0x00c8},
    {"OElig", 0x0152},
    {"Phi", 0x03a6},
    {"Ecirc", 0x00ca},
    {"Igrave", 0x00cc},
    {"eth", 0x00f0},
    {"Iota", 0x0399},
    {"Theta", 0x0398},
    {"Uacute", 0x00da},
    {"Icirc", 0x00ce},
    {"yacute", 0x00fd},
    {"uacute", 0x00fa},
    {"there4", 0x2234},
    {"lrm", 0x200e},
    {"rlm", 0x200f},
    {"equiv", 0x2261},
    {"ograve", 0x00f2},
    {"theta", 0x03b8},
    {"Eacute", 0x00c9},
    {"hArr", 0x21d4},
    {"ocirc", 0x00f4},
    {"Iacute", 0x00cd},
    {"cap", 0x2229},
    {"Prime", 0x2033},
    {"piv", 0x03d6},
    {"ne", 0x2260},
    {"zwnj", 0x200c},
    {"oacute", 0x00f3},
    {"ni", 0x220b},
    {"chi", 0x03c7},
    {"Lambda", 0x039b},
    {"Psi", 0x03a8},
    {"lambda", 0x03bb},
    {"raquo", 0x00bb},
    {"and", 0x2227},
    {"laquo", 0x00ab},
    {"nabla", 0x2207},
    {"int", 0x222b},
    {"macr", 0x00af},
    {"rfloor", 0x230b},
    {"quot", 34},
    {"rdquo", 0x201d},
    {"lfloor", 0x230a},
    {"mu", 0x03bc},
    {"exist", 0x2203},
    {"ldquo", 0x201c},
    {"uml", 0x00a8},
    {"auml", 0x00e4},
    {"image", 0x2111},
    {"rho", 0x03c1},
    {"euml", 0x00eb},
    {"Sigma", 0x03a3},
    {"Rho", 0x03a1},
    {"iuml", 0x00ef},
    {"bdquo", 0x201e},
    {"prod", 0x220f},
    {"ntilde", 0x00f1},
    {"cup", 0x222a},
    {"frasl", 0x2044},
    {"ang", 0x2220},
    {"rang", 0x3009},
    {"Yuml", 0x0178},
    {"nu", 0x03bd},
    {"yen", 0x00a5},
    {"lang", 0x3008},
    {"aring", 0x00e5},
    {"sect", 0x00a7},
    {"szlig", 0x00df},
    {"sub", 0x2282},
    {"cent", 0x00a2},
    {"Omega", 0x03a9},
    {"sube", 0x2286},
    {"Auml", 0x00c4},
    {"Ouml", 0x00d6},
    {"ordm", 0x00ba},
    {"Uuml", 0x00dc},
    {"yuml", 0x00ff},
    {"uuml", 0x00fc},
    {"kappa", 0x03ba},
    {"perp", 0x22a5},
    {"Aring", 0x00c5},
    {"Kappa", 0x039a},
    {"not", 0x00ac},
    {"Euml", 0x00cb},
    {"oline", 0x203e},
    {"alpha", 0x03b1},
    {"lowast", 0x2217},
    {"phi", 0x03c6},
    {"Iuml", 0x00cf},
    {"curren", 0x00a4},
    {"sdot", 0x22c5},
    {"omega", 0x03c9},
    {"middot", 0x00b7},
    {"micro", 0x00b5},
    {"clubs", 0x2663},
    {"hellip", 0x2026},
    {"amp", 38},
    {"ouml", 0x00f6},
    {"copy", 0x00a9},
    {"prime", 0x2032},
    {"permil", 0x2030},
    {"Alpha", 0x0391},
    {"Scaron", 0x0160},
    {"psi", 0x03c8},
    {"iquest", 0x00bf},
    {"Gamma", 0x0393},
    {"hearts", 0x2665},
    {"prop", 0x221d},
    {"sim", 0x223c},
    {"fnof", 0x0192},
    {"cong", 0x2245},
    {"empty", 0x2205},
    {"diams", 0x2666},
    {"sup", 0x2283},
    {"times", 0x00d7},
    {"supe", 0x2287},
    {"shy", 0x00ad},
    {"supl", 0x00b9},
    {"gamma", 0x03b3},
    {"sup1", 0x00b9},
    {"sup3", 0x00b3},
    {"sup2", 0x00b2},
    {"rsquo", 0x2019},
    {"rsaquo", 0x203a},
    {"isin", 0x2208},
    {"percnt", 0x0025},
    {"lsquo", 0x2018},
    {"lsaquo", 0x2039},
    {"sbquo", 0x201a},
    {"sum", 0x2211},
    {"apos", 0x0027},
    {"sigma", 0x03c3},
    {"thorn", 0x00fe},
    {"infin", 0x221e},
    {"nsub", 0x2284},
    {"sigmaf", 0x03c2},
    {"alefsym", 0x2135},
    {"otimes", 0x2297},
    {"oplus", 0x2295},
    {"pound", 0x00a3},
    {"notin", 0x2209},
    {"emsp", 0x2003},
    {"scaron", 0x0161},
    {"mdash", 0x2014},
    {"ensp", 0x2002},
    {"spades", 0x2660},
    {"Omicron", 0x039f},
    {"nbsp", 0x00a0},
    {"upsih", 0x03d2},
    {"ndash", 0x2013},
    {"asymp", 0x2248},
    {"Oslash", 0x00d8},
    {"nsup", 0x2285},
    {"omicron", 0x03bf},
    {"thetasym", 0x03d1},
    {"minus", 0x2212},
    {"oslash", 0x00f8},
    {"epsilon", 0x03b5},
    {"Upsilon", 0x03a5},
    {"upsilon", 0x03c5},
    {"Epsilon", 0x0395},
    {"thinsp", 0x2009},
    {"plusmn", 0x00b1}
  };

static const short lookup[] =
  {
      -1,   -1,   -1,   -1,    0,   -1,   -1,    1,
      -1,   -1,   -1,   -1,    2,    3,    4,   -1,
      -1,    5,   -1,    6,   -1,   -1,    7,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
       8,   -1,    9,   -1,   -1,   10,   11,   12,
      -1,   -1,   13,   -1,   14,   -1,   -1,   -1,
      -1,   15,   16,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   17,   18,   -1,
      -1,   -1,   19,   -1,   -1,   -1,   -1,   -1,
      20,   -1,   -1,   -1,   21,   -1,   -1,   -1,
      22,   -1,   23,   -1,   -1,   -1,   -1,   24,
      25,   -1,   -1,   -1,   26,   27,   -1,   -1,
      28,   -1,   -1,   29,   30,   -1,   -1,   -1,
      31,   32,   33,   -1,   -1,   34,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   35,   36,   -1,   37,   38,   -1,   39,
      -1,   -1,   40,   -1,   41,   -1,   -1,   42,
      -1,   43,   -1,   -1,   44,   -1,   45,   -1,
      -1,   -1,   -1,   -1,   46,   -1,   -1,   -1,
      47,   48,   -1,   49,   50,   -1,   51,   -1,
      -1,   -1,   -1,   52,   53,   -1,   -1,   -1,
      -1,   -1,   -1,   54,   -1,   -1,   -1,   -1,
      55,   -1,   -1,   -1,   56,   -1,   -1,   57,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   58,   -1,   -1,   -1,   -1,   59,   -1,
      -1,   60,   61,   62,   -1,   -1,   -1,   63,
      64,   -1,   -1,   65,   -1,   66,   -1,   -1,
      -1,   67,   68,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   69,   -1,   -1,   -1,
      -1,   70,   -1,   -1,   -1,   71,   72,   -1,
      -1,   -1,   -1,   73,   -1,   -1,   -1,   -1,
      74,   -1,   -1,   -1,   -1,   75,   -1,   -1,
      -1,   -1,   76,   -1,   -1,   -1,   -1,   77,
      -1,   -1,   -1,   -1,   78,   -1,   -1,   -1,
      79,   80,   -1,   -1,   -1,   81,   -1,   -1,
      -1,   82,   -1,   83,   -1,   -1,   84,   -1,
      -1,   -1,   -1,   85,   -1,   86,   -1,   -1,
      -1,   87,   -1,   88,   -1,   89,   90,   91,
      -1,   -1,   -1,   -1,   92,   -1,   -1,   -1,
      93,   94,   -1,   95,   96,   -1,   97,   -1,
      -1,   -1,   98,   99,   -1,   -1,   -1,  100,
      -1,   -1,  101,   -1,  102,  103,   -1,  104,
     105,  106,  107,   -1,   -1,   -1,  108,  109,
      -1,   -1,   -1,   -1,  110,   -1,   -1,   -1,
      -1,  111,   -1, -627, -150,   -2,   -1,   -1,
      -1,   -1,  114,  115,   -1,   -1,   -1,  116,
     117,   -1,   -1,  118,  119,   -1,   -1,   -1,
      -1,   -1,  120,   -1,  121,   -1,  122,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  123,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  124,
      -1,   -1,   -1,   -1,   -1,   -1,  125,   -1,
     126,  127,  128,   -1,   -1,  129,   -1,  130,
      -1,   -1,  131,   -1,   -1,   -1,  132,   -1,
      -1,  133,   -1,   -1,   -1,   -1,   -1,   -1,
     134,   -1,   -1,   -1,   -1,  135,   -1,   -1,
     136,  137,   -1,  138,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  139,  140,  141,  142,   -1,
      -1,  143,   -1,   -1,   -1,   -1,  144,   -1,
      -1,  145,  146,  147,   -1,   -1,  148,  149,
     150,   -1,   -1,  151,   -1,   -1,   -1,   -1,
      -1,  152,  153,   -1,   -1,   -1,  154,   -1,
     155,   -1,  156,   -1,  157,   -1,   -1,  158,
     159,   -1,   -1,   -1,   -1,  160,   -1,   -1,
     161,  162,  163,   -1,   -1,   -1,   -1,   -1,
     164,   -1,   -1,   -1,  165,  166,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  167,  168,  169,
      -1,   -1,   -1,  170,   -1,   -1,   -1,   -1,
     171,   -1,   -1,   -1,   -1,  172,   -1,   -1,
      -1,   -1,  173,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  174,   -1,
      -1,   -1,   -1,  175,   -1,   -1,   -1,   -1,
     176,  177,   -1,   -1,   -1,  178,  179,   -1,
      -1,   -1,   -1,  180,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  181,  182,  183,   -1,   -1,
      -1,   -1,  184,  185,   -1,  186,  187,   -1,
     188,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     189,   -1,   -1,   -1,   -1,   -1,  190,  191,
      -1,   -1,   -1,  192,   -1,   -1,   -1,   -1,
     193,  194,   -1,  195,  196,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     197,  198,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  199,   -1,   -1,   -1,
     200,  201,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  202,   -1,   -1,  203,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,  204,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     205,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  206,   -1,   -1,   -1,  207,   -1,
      -1,   -1,   -1,   -1,  208,   -1,   -1,   -1,
      -1,  209,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  210,   -1,   -1,   -1,   -1,  211,   -1,
      -1,  212,   -1,  213,   -1,   -1,   -1,  214,
      -1,   -1,   -1,  215,  216,  217,   -1,   -1,
      -1,  218,   -1,   -1,   -1,   -1,  219,   -1,
      -1,   -1,   -1,  220,  221,  222,   -1,   -1,
     223,   -1,  224,   -1,   -1,   -1,  225,  226,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  227,   -1,   -1,  228,  229,   -1,
      -1,   -1,   -1,   -1,  230,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  231,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  232,   -1,   -1,   -1,  233,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  234,   -1,   -1,   -1,
      -1,   -1,  235,   -1,   -1,   -1,  236,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  237,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  238,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  239,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  240,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,  241,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     242,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,  243,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  244,
     245,   -1,  246,  247,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  248,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  249,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     250,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,  251,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  252,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  253,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  254,  255,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,  256,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,  257,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
     258,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  259,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  260,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
      -1,   -1,   -1,   -1,  261
  };

#ifdef __GNUC__
__inline
#endif
const struct Entity *
findEntity (register const char *str, register unsigned int len)
{
  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].name;

              if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                return &wordlist[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct Entity *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register const struct Entity *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->name;

                  if (*str == *s && !strncmp (str + 1, s + 1, len - 1) && s[len] == '\0')
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}

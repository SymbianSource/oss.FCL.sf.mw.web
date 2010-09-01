/*
* Copyright (c) 1999 - 2001 Nokia Corporation and/or its subsidiary(-ies).
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


 /*
    $Workfile: scr_valexpr.c $

     Purpose:
        
          This file implements the evaluation of expression
 */

#include "scr_valexpr.h"
#include "scr_opcodes.h"
#include "nwx_defs.h"
#include "nwx_float.h"
#include "nwx_math.h"

/*Known also as bang*/
val_t l_not_val(val_t v) 
{
  NW_Bool b;

  if (val2bool(v, &b)) {
    return new_bool_val(NW_BOOL_CAST(!b));
  }
  return new_invalid_val(); 
}

/*known as tilde ~*/
val_t b_not_val(val_t v)
{
  NW_Int32  i;

  if (val2int(v, &i)) {
    return new_int_val(~i);
  } else {
    return new_invalid_val();
  }
}

val_t uminus_val(val_t v)
{
  NW_Int32  i;
  NW_Float32 f;

  if (val2int(v, &i)) {
    return new_int_val(-i);
  } else if (val2float(v, &f)) {
    return new_float_val(- f);  
  } else {
    return new_invalid_val();
  }
}

val_t incr_val(val_t v)
{
  NW_Int32  i;
  NW_Float32 f;

  if (val2int(v, &i)) {
    if (i < (NW_Int32)SCR_MAX_INT) {
      return new_int_val(++i);
    } else {
       return new_invalid_val();
    }
  } else if (val2float(v, &f)) {
    if (is_finite_float64((NW_Float64)f+1)) {
      return new_float_val(f+1);
    } else {
      return new_invalid_val();
    }
  } else {
    return new_invalid_val();
  }
}

val_t decr_val(val_t v)
{
  NW_Int32  i;
  NW_Float32 f;

  if (val2int(v, &i)) {
    if (i > (NW_Int32)SCR_MIN_INT) {
      return new_int_val(--i);
    } else {
      return new_invalid_val();
    }
  } else if (val2float(v, &f)) {
    if (is_finite_float64((NW_Float64)f-1)) {
      return new_float_val(f - 1);
    } else {
       return new_invalid_val();
    }
  } else {
    return new_invalid_val();
  }
}

NW_Bool comp_string_vals(str_t s1, str_t s2, NW_Byte op) 
{
  NW_Int32  i;

  NW_Bool eqOp = NW_BOOL_CAST((op == GE) || (op == EQ) || (op == LE));

  if ((s1 == 0) && (s2 == 0)) {
    return eqOp;
  } else if ((s1 == 0) || (s2 == 0)) {
    return NW_BOOL_CAST(!eqOp); 
  }
  
  i = str_cmp(s1, s2);

  if (i < 0) {
    if ( (op == LE) || (op == NE) || (op == LT)) {
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
  } else if (i > 0) {
    if ( (op == GE) || (op == NE) || (op == GT)) {
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
  } else { 
    NW_ASSERT ( i == 0 );
    if ( (op == GE) || (op == EQ) || (op == LE)) {
      return NW_TRUE;
    } else {
      return NW_FALSE;
    }
  }
}

/* assume v1 and v2 can be coverted to ints */
NW_Bool comp_int_vals(NW_Int32  i1, NW_Int32  i2, NW_Byte op)
{

  if (op == EQ)      return NW_BOOL_CAST(i1 == i2);
  else if (op == NE) return NW_BOOL_CAST(i1 != i2);
  else if (op == LT) return NW_BOOL_CAST(i1 <  i2);
  else if (op == LE) return NW_BOOL_CAST(i1 <= i2);
  else if (op == GT) return NW_BOOL_CAST(i1 >  i2);
  else if (op == GE) return NW_BOOL_CAST(i1 >= i2);

  NW_ASSERT(NW_FALSE); /* should never be here*/
  return NW_FALSE;

}

NW_Bool comp_float_vals(NW_Float32 f1, NW_Float32 f2, NW_Byte op)
{
  NW_Float32 diff;
  NW_Float32 absDiff;
  NW_Bool floatEq;

  diff = (f1 - f2)*NW_Math_powint(10.0f, NW_FLOAT32_DIG);
  absDiff = NW_Math_fabs(diff);
  floatEq = (NW_Bool)((absDiff <= NW_Math_fabs(f1)) && (absDiff <= NW_Math_fabs(f2)));
             
  if (op == EQ)      return NW_BOOL_CAST(floatEq);
  else if (op == NE) return NW_BOOL_CAST(!floatEq);
  else if (op == LT) return NW_BOOL_CAST(f1 <  f2);
  else if (op == LE) return NW_BOOL_CAST(floatEq || (f1 < f2));
  else if (op == GT) return NW_BOOL_CAST(f1 >  f2);
  else if (op == GE) return NW_BOOL_CAST(floatEq || (f1 > f2));

  NW_ASSERT(NW_FALSE); /* should never be here*/
  return NW_FALSE;

}


val_t comp_vals(val_t v1, val_t v2, NW_Byte op) 
{
  NW_Int32  i1, i2;
  NW_Bool retval1, retval2;
  val_t res = uninitialize_val();
  
  if ( (ScriptVarType(v1) == VAL_TYPE_STR) || (ScriptVarType(v2) == VAL_TYPE_STR) ) {
    str_t s1 = 0;
    str_t s2 = 0;
    retval1 = val2str(v1, &s1);
    retval2 = val2str(v2, &s2);
    if (retval1 &&  retval2) {
      res = new_bool_val(comp_string_vals(s1, s2, op));
    } else {
      res = new_invalid_val();
    }
    free_str(s1);
    free_str(s2);
    return res;
  }
  
  
  if ( (ScriptVarType(v1) == VAL_TYPE_FLOAT) || (ScriptVarType(v2) == VAL_TYPE_FLOAT) ) {
    NW_Float32 f1, f2;
    retval1 = val2float(v1, &f1);
    retval2 = val2float(v2, &f2);
    if (retval1 &&  retval2) {
      return new_bool_val(comp_float_vals(f1, f2, op));
    } else {
      return new_invalid_val();
    }
  }

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    return new_bool_val(comp_int_vals(i1, i2, op));
  }
  
  return new_invalid_val();
}

val_t rem_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;
  
  if(val2int(v1, &i1) && val2int(v2, &i2))
  {
    if (i2 == 0) {
      return new_invalid_val();
    } else if (i1 >= 0) {
      return new_int_val(NW_Math_abs(i1 % i2)); 
    } else {
      NW_ASSERT ( i1 < 0 );
      return new_int_val(- NW_Math_abs(i1 % i2)); 
    }
  } else {
    return new_invalid_val();
  }
}

val_t sub_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;
  NW_Float32 f1, f2;
  
  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    if (i1 > 0) {
      if (i2 > 0) {
        /* i1 & i2 are positive, valid values, so diff will be valid */
        return new_int_val(i1 - i2);
      } else {
        if ( ((NW_Int32)SCR_MAX_INT - i1) >= (-i2) ) {
          /* i1 is pos, i2 neg, but i1 + abs(i2) < SCR_MAX_INT */
          return new_int_val(i1 - i2);
        } else {
          return new_invalid_val();
        }
      }
    } else {
      if (i2 < 0) {
        /* i1 & i2 are neg, valid vals, so diff will be valid */
        return new_int_val(i1 - i2);
      } else {
        if ( ((NW_Int32)SCR_MIN_INT + i2) <= i1) {
          /* i1 is neg, i2 pos, but i1 - i2 >= SCR_MIN_INT */
          return new_int_val(i1 - i2);
        } else {
          return new_invalid_val();
        }
      }
    }
  } else if (val2float(v1, &f1) && val2float(v2, &f2)) {
    if (is_finite_float64((NW_Float64)f1 - (NW_Float64)f2)) {
      return new_float_val(f1 - f2);
    } else {
      return new_invalid_val();
    }
  } else {
    return new_invalid_val();
  }
}

val_t add_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;
  NW_Bool retval1, retval2;
  val_t res = uninitialize_val();
  
  if ( (ScriptVarType(v1) == VAL_TYPE_STR) || (ScriptVarType(v2) == VAL_TYPE_STR) )
  {
    str_t s1 = 0;
    str_t s2 = 0;
    retval1 = val2str(v1, &s1);
    retval2 = val2str(v2, &s2);
    if (retval1 &&  retval2)
    {
      str_t res_str = str_cat(s1, s2);
      res = new_str_val(res_str);
      free_str(res_str);
    }
    else res = new_invalid_val();
    free_str(s1);
    free_str(s2);
    return res;
  }

  if ( (ScriptVarType(v1) == VAL_TYPE_FLOAT) || (ScriptVarType(v2) == VAL_TYPE_FLOAT) )
  {
    NW_Float32 f1, f2;
    retval1 = val2float(v1, &f1);
    retval2 = val2float(v2, &f2);
    if (retval1 &&  retval2 && is_finite_float64((NW_Float64)f1 + (NW_Float64)f2)) {
      return new_float_val(f1 + f2);
    } else {
      return new_invalid_val();
    }
  }
  
  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    if (i1 > 0) {
      if (i2 > 0) {
        if ( ((NW_Int32)SCR_MAX_INT - i1) >= i2) {
          return new_int_val(i1 + i2);
        } else {
          return new_invalid_val();
        }
      } else {
        return new_int_val(i1 + i2);
      }
    } else {
      if (i2 > 0) {
        return new_int_val(i1 + i2);
      } else {
        if ( ((NW_Int32)SCR_MIN_INT - i1) <= i2) {
          return new_int_val(i1 + i2);
        } else {
          return new_int_val(i1 + i2);
        }
      }
    }
  } else {
    return new_invalid_val();
  }
}

val_t lshift_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if ( val2int(v1, &i1) && val2int(v2, &i2) ) {
    /* Mask 2nd operand so value is 0 to 31. */
    return new_int_val(i1 << ( i2 & 0x1F ));
  } else {
    return new_invalid_val();
  }
}

val_t rsshift_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    /* Mask 2nd operand so value is 0 to 31. */
    /*lint -e{704} Shift right of signed quantity*/
    return new_int_val( i1 >> (i2 & 0x1F) );  
  } else {
    return new_invalid_val();
  }
}

val_t rszshift_vals(val_t v1, val_t v2)
{
  NW_Int32   i1, i2;
  NW_Uint32 ui1;

  if ( val2int(v1, &i1) && val2int(v2, &i2) ) {
    /* Shift unsigned value so C will fill with 0's.
       Mask 2nd operand so value is 0 to 31. */
    ui1 = (NW_Uint32) i1;
    return new_int_val( (NW_Int32 ) (ui1 >> (i2 & 0x1F)) );
  } else {
    return new_invalid_val();
  }
}

val_t mul_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;
  NW_Float32 f1, f2;
  
  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    if (i1 > 0) {
      if (i2 > 0) {
        if ( ((NW_Int32)SCR_MAX_INT / i2) >= i1) {
          return new_int_val(i1 * i2);
        } else {
          return new_invalid_val();
        }
      } else {
        if ( (i2 == 0) || (i2 == -1) || (((NW_Int32)SCR_MIN_INT / i2) >= i1) ) {
          return new_int_val(i1 * i2);
        } else {
          return new_invalid_val();
        }
      }
    } else {
      if (i2 > 0) {
        if ( (i1 == 0) || (i1 == -1) || (((NW_Int32)SCR_MIN_INT / i1) >= i2) ) {
          return new_int_val(i1 * i2);
        } else {
          return new_invalid_val();
        }
      } else {
        if ( (i2 == 0) || (((NW_Int32)SCR_MAX_INT / i2) <= i1) ) {
          return new_int_val(i1 * i2);
        } else {
          return new_invalid_val();
        }
      }
    }
  } else if (val2float(v1, &f1) && val2float(v2, &f2)) {
    NW_Float64 res64 = (NW_Float64)f1 * (NW_Float64)f2;
    if (is_zero_float64(res64)) {
      /*lint -e{747} Significant prototype coercion*/
      return new_float_val(0.0);
    } else if (is_finite_float64(res64)) {
      return new_float_val(f1 * f2);
    } else {
      return new_invalid_val();
    }
  } else {
    return new_invalid_val();
  }
}

val_t bit_and_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    return new_int_val(i1 & i2);
  } else {
    return new_invalid_val();
  }
}

val_t bit_or_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    return new_int_val(i1 | i2);
  } else {
    return new_invalid_val();
  }
}

val_t bit_xor_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    return new_int_val(i1 ^ i2);
  } else {
    return new_invalid_val();
  }
}

val_t idiv_vals(val_t v1, val_t v2)
{
  NW_Int32  i1, i2;

  if (val2int(v1, &i1) && val2int(v2, &i2)) {
    if (i2 != 0) {
      return new_int_val(i1 / i2);
    } else {
      return new_invalid_val();
    }
  } else {
    return new_invalid_val();
  }
}


val_t div_vals(val_t v1, val_t v2)
{
  NW_Float32 f1, f2;
  
  if (val2float(v1, &f1) && val2float(v2, &f2)) {
    if (f2 != 0.0) {
      NW_Float64 resd = (NW_Float64)f1 / (NW_Float64)f2;
      if (is_zero_float64(resd)) {
        /*lint -e{747} Significant prototype coercion*/
        return new_float_val(0.0);
      } else if (is_finite_float64(resd)) {
        return new_float_val(f1/f2);
      } else {
        return new_invalid_val();
      }
    } else {
      return new_invalid_val(); /* validation divission by 0 */
    }
  } else {
    return new_invalid_val();
  }
}



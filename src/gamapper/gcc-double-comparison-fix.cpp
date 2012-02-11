/** 
  * Copyright (c) 2012, Universita' della Svizzera Italiana,
  * All rights reserved.
  * 
  * Redistribution and use in source and binary forms, with or without modification, 
  * are permitted provided that the following conditions are met:
  * 
  * 1. Redistributions of source code must retain the above copyright notice, this list 
  * of conditions and the following disclaimer.
  * 
  * 2. Redistributions in binary form must reproduce the above copyright notice, this 
  * list of conditions and the following disclaimer in the documentation and/or other 
  * materials provided with the distribution.
  * 
  * 3. Neither the name of Universita' della Svizzera Italiana nor the names of its 
  * contributors may be used to endorse or promote products derived from this software 
  * without specific prior written permission.
  * 
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY 
  * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES 
  * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT 
  * SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
  * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
  * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, 
  * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
  * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  * 
  * The licenses for the third party libraries used in this software can be found in 
  * the folders of the third party libraries.
  *
  * Author(s): Onur Derin <oderin@users.sourceforge.net>
  * 
  */
#include "gcc-double-comparison-fix.h"

void set_fpu_control_word(unsigned int mode)
{
  asm ("fldcw %0" : : "m" (*&mode));
}

unsigned int get_fpu_control_word()
{
  volatile unsigned int mode = 0;
  asm ("fstcw %0" : "=m" (*&mode));
  return mode;
}

bool fpu_set_extended_precision_is_on(bool state)
{
  unsigned int old_cw = get_fpu_control_word();
  unsigned int masked = old_cw & ~PRECISION_BIT_MASK;
  unsigned int new_cw;
  if(state)
    new_cw = masked + EXTENDED_PRECISION_BITS;
  else
    new_cw = masked + STANDARD_PRECISION_BITS;
  set_fpu_control_word(new_cw);
  return true;
}

bool fpu_get_extended_precision_is_on()
{
  unsigned int old_cw = get_fpu_control_word();
  return  ((old_cw & PRECISION_BIT_MASK) == 0x300);
}

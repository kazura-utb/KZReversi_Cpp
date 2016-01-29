//#include "stdafx.h"
//#include "Reversi.h"

/*
Copyright (c) 2003, Gunnar Andersson
Copyright (c) 2005, Toshihiko Okuhara
All rights reserved.

Redistribution and use in source and binary forms, with or
without modification, are permitted provided that the
following conditions are met:

* Redistributions of source code must retain the above copyright notice,
  this list of conditions and the following disclaimer. 

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution. 

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF
THE POSSIBILITY OF SUCH DAMAGE.
*/



typedef struct {
  unsigned long high;
  unsigned long low;
} bit_low_high;



static const unsigned long long mmx_c7e = 0x7e7e7e7e7e7e7e7eULL;
static const unsigned long long mmx_c0f = 0x0f0f0f0f0f0f0f0fULL;
static const unsigned long long mmx_c33 = 0x3333333333333333ULL;
static const unsigned long long mmx_c55 = 0x5555555555555555ULL;



void
init_mmx( void ) {
  /* obsolete */
}



int
bitboard_mobility( const bit_low_high my_bits,
		   const bit_low_high opp_bits, 
		   unsigned long long *p_moves) {
  unsigned int count;
  unsigned long long moves;
  //int a,b,d,e,f;

  __asm 
  {

	mov			a, esi		
	movd		b, mm7		/* AMD 22007 pp.171 */
	mov			c, edi		
	movd		d, mm6
	punpckldq	mm7, a
	punpckldq	mm6, c
			/* shift=+1 */			/* shift=+8 */
	mov			esi, eax   	
	movq		mm7, mm0				
	movq		5, mm5
	shr			1, eax				psrlq	8, mm0
	and			2122219134, edi		pand	mm6, mm5
	and			edi, eax	     	pand	mm6, mm0	/* 0 m7&o6 m6&o5 .. m1&o0 */
	mov			eax, edx     	movq	mm0, mm1
	shr			1, eax		psrlq	8, mm0
	mov			edi, ecx     	movq	mm6, mm3
	and	edi, eax     	pand	mm6, mm0	/* 0 0 m7&o6&o5 .. m2&o1&o0 */
	shr	1, ecx		psrlq	8, mm3
	or	edx, eax     	por	mm1, mm0	/* 0 m7&o6 (m6&o5)|(m7&o6&o5) .. (m1&o0) */
	and	edi, ecx     	pand	mm6, mm3	/* 0 o7&o6 o6&o5 o5&o4 o4&o3 .. */
	mov	eax, edx     	movq	mm0, mm4
	shr	2, eax		psrlq	16, mm0
	and	ecx, eax     	pand	mm3, mm0	/* 0 0 0 m7&o6&o5&o4 (m6&o5&o4&o3)|(m7&o6&o5&o4&o3) .. */
	or	eax, edx     	por	mm0, mm4
	shr	2, eax		psrlq	16, mm0
	and	ecx, eax     	pand	mm3, mm0	/* 0 0 0 0 0 m7&o6&..&o2 (m6&o5&..&o1)|(m7&o6&..&o1) .. */
	or	edx, eax     	por	mm0, mm4
	shr	1, eax		psrlq	8, mm4		/* result of +8 */
	/* shift=-1 */			/* shift=-8 */
					movq	mm7, mm0
	add	esi, esi     	psllq	8, mm0
	and	edi, esi     	pand	mm6, mm0
	mov	esi, edx     	movq	mm0, mm1
	add	esi, esi     	psllq	8, mm0
	and	edi, esi     	pand	mm6, mm0
	or	edx, esi     	por	mm1, mm0
	add	ecx, ecx     	psllq	8, mm3
	mov	esi, edx     	movq	mm0, mm1
	shl	2, esi		psllq	16, mm0
	and	ecx, esi     	pand	mm3, mm0
	or	esi, edx     	por	mm0, mm1
	shl	2, esi		psllq	16, mm0
	and	ecx, esi     	pand	mm3, mm0
	or	edx, esi     	por	mm1, mm0
	add	esi, esi     	psllq	8, mm0
	or	eax, esi     	por	mm0, mm4
			/* Serialize */			/* shift=+7 */
					movq	mm7, mm0
	movd	esi, mm1
					psrlq	7, mm0
	psllq	32, mm1
					pand	mm5, mm0
	por	mm1, mm4
					movq	mm0, mm1
					psrlq	7, mm0
					pand	mm5, mm0
					movq	mm5, mm3
					por	mm1, mm0
					psrlq	7, mm3
					movq	mm0, mm1
					pand	mm5, mm3
					psrlq	14, mm0
					pand	mm3, mm0
					por	mm0, mm1
	movd	mm5, edi
					psrlq	14, mm0
	movd	mm7, esi
					pand	mm3, mm0
	mov	edi, ecx     	por	mm1, mm0
	shr	1, ecx		psrlq	7, mm0
	and	edi, ecx     	por	mm0, mm4
			/* shift=+1 */			/* shift=-7 */
	mov	esi, eax     	movq	mm7, mm0
	shr	1, eax		psllq	7, mm0
	and	edi, eax     	pand	mm5, mm0
	mov	eax, edx     	movq	mm0, mm1
	shr	1, eax		psllq	7, mm0
	and	edi, eax     	pand	mm5, mm0
	or	edx, eax     	por	mm1, mm0
					psllq	7, mm3
	mov	eax, edx     	movq	mm0, mm1
	shr	2, eax		psllq	14, mm0
	and	ecx, eax     	pand	mm3, mm0
	or	eax, edx     	por	mm0, mm1
	shr	2, eax		psllq	14, mm0
	and	ecx, eax   	pand	mm3, mm0
	or	edx, eax   	por	mm1, mm0
	shr	1, eax		psllq	7, mm0
					por	mm0, mm4
			/* shift=-1 */			/* shift=+9 */
					movq	mm7, mm0
	add	esi, esi     	psrlq	9, mm0
	and	edi, esi     	pand	mm5, mm0
	mov	esi, edx     	movq	mm0, mm1
	add	esi, esi     	psrlq	9, mm0
	and	edi, esi     	pand	mm5, mm0
					movq	mm5, mm3
	or	edx, esi     	por	mm1, mm0
					psrlq	9, mm3
	mov	esi, edx     	movq	mm0, mm1
	add	ecx, ecx     	pand	mm5, mm3
	shl	2, esi		psrlq	18, mm0
	and	ecx, esi     	pand	mm3, mm0
	or	esi, edx     	por	mm0, mm1
	shl	2, esi		psrlq	18, mm0
	and	ecx, esi     	pand	mm3, mm0
	or	edx, esi     	por	mm1, mm0
	add	esi, esi     	psrlq	9, mm0
	or	eax, esi     	por	mm0, mm4
			/* Serialize */			/* shift=-9 */
					movq	mm7, mm0
	movd	esi, mm1
					psllq	9, mm0
	por	mm1, mm4
					pand	mm5, mm0
					movq	mm0, mm1
					psllq	9, mm0
					pand	mm5, mm0
					por	mm1, mm0
					psllq	9, mm3
					movq	mm0, mm1
					psllq	18, mm0
					pand	mm3, mm0
					por	mm0, mm1
					psllq	18, mm0
					pand	mm3, mm0
					por	mm1, mm0
					psllq	9, mm0
					por	mm0, mm4
			/* mm4 is the pseudo-feasible moves at this point. */
			/* Let mm7 be the feasible moves, i.e., mm4 restricted to empty squares. */
					por	mm6, mm7
					pandn	mm4, mm7
	movq	moves, mm7 /* let moves */
			/* Count the moves, i.e., the number of bits set in mm7. */
	movq	mm7, mm1
	psrld	1, mm7
	pand	6, mm7
	psubd	mm7, mm1
	movq	mm1, mm7
	psrld	2, mm1
	pand	7, mm7
	pand	7, mm1
	paddd	mm1, mm7
	movq	mm7, mm1
	psrld	4, mm7
	paddd	mm1, mm7
	pand	8, mm7
	movq	mm7, mm1
	psrlq	32, mm7
	paddd	mm1, mm7
	movd	mm7, 0
	imul	16843009, 0, 0	/* AMD 22007 pp.138 */
	shr 	24, 0
	emms		/* Reset the FP/MMX unit. */
    : =a (count)
    : m (my_bits.high), m (my_bits.low), m (opp_bits.high), m (opp_bits.low),
      m (mmx_c7e), m (mmx_c55), m (mmx_c33), m (mmx_c0f)
    : ecx, edx, esi, edi

  }

  *p_moves = moves;

  return count;
}
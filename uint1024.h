/***************************************************************************    
*   Knapsack problem solving encryption - asymetric encryption based on
*   NP-complete problem (Knapsack)
*   Copyright (C) 2001 Miroslav 'Mirco' Bajtos
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA,
*   or try <http://www.gnu.org>
***************************************************************************/

#ifndef __UINT1024_H__
#define __UINT1024_H__


#include "config.h"

#include <stdint.h>
#include <stdio.h>

#define __SZ1024 33

typedef 
	uint32_t uint1024[__SZ1024];

int 	read1024	( FILE *stream, uint1024 x );
int	write1024	( FILE *stream, const uint1024 x );
		/*
		 * reads/writes x from/to stream
		 * return non-zero if failed
		 */

	
int8_t	add1024 	( uint1024 A, const uint1024 B );  
		/*
		*  A += B 
		*  returns 1 if A>2^1024 
		*/


int8_t	sub1024 	( uint1024 A, const uint1024 B );  
		/*
		*  A -= B 
		*  returns 1 if A<0 
		*/

void mul1024modN	( uint1024 A, const uint1024 B , const uint1024 N);
		/*
		 * A *= B (mod N)
		 */


void  	shr1024 	( uint1024 A, uint16_t s );
		/*  
		*  A = A >> s 
		*/


void  	shl1024 	( uint1024 A, uint16_t s );
		/*  
		*  A = A << s 
		*/


int8_t	zero1024	( const uint1024 A );
		/*
		*  returns 1 if A=0, 0 otherwise
		*/

#define non_zero1024(A) (!zero1024(A))

void	cpy1024		( uint1024 A, const uint1024 B );
		/*
		 * A = B
		 */

void
	uint_to_1024	( uint1024 D, const  unsigned int A );
		/* 
		 * returns A as uint1024 
		 */

int
	cmp1024		( const uint1024 A, const uint1024 B );
		/*
		 * compares A and B, returns -1 if A<B, 0 if equal and +1 if
		 * A>B
		 */

void	dump1024	( const uint1024 A );
		/*
		 * dumps A as array of uints
		 */

void	GCD		( const uint1024 A, const uint1024 B, uint1024 G );
		/* 
		 * finds the GCD of A and B and result stores in G
		 */

#endif

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


#include "uint1024.h"

#include <math.h>

short verbose = 0;	/* from config.h */

/*
 * representation of uint1024: u[0] is the lowest 'digit',
 * u[__SZ1024-2] is the highest 'digit', u[__SZ1024-1] is
 * used to catch overflows
 */


/*
 *  A += B 
 *  returns 1 if A>2^1024 
 */
int8_t	add1024 	( uint1024 A, const uint1024 B )  
{
	int8_t i;
	uint64_t b;

	for (i=0; i<__SZ1024-1; i++) {
		b = B[i];
		*((uint64_t *) (A+i)) += b;
	}
 
	i = A[__SZ1024-1]?1:0;
	A[__SZ1024-1] = 0;
	
	return (i);
	
}
	

/*
 *  A -= B 
 *  returns 1 if A<0 
 */
int8_t	sub1024 	( uint1024 A, const uint1024 B )  
{
	int8_t i;
	uint64_t a,b;
	uint32_t r;

	
	r=0;
	for (i=0; i<__SZ1024-1; i++) {
		a = A[i];
		b = B[i];
		if (b+r) {
			b+=r; r=0;
			if (a<b) {
				r = 1;
				a += ((uint64_t) 1) << 32;
			}
			a-=b;
			A[i]=a;
			
		}
	}

	return (r);
	
}

void mod_n(uint1024 X, const uint1024 N) {
	uint1024 m;
	
	cpy1024(m,N);
	while (cmp1024(m,X)<0) { shl1024(m,1); }

	while (cmp1024(X,N)>=0) {
		while (cmp1024(m,X)>0) { shr1024(m,1); }
		sub1024(X,m);
	}
}

/*
 * A *= B (mod N)
 */
void mul1024modN	( uint1024 A, const uint1024 B , const uint1024 N) {
	uint1024 C,R,m;
	uint64_t t;
	uint8_t i,j,k;
	
	uint_to_1024(R,0); 
	for (i=0; i<__SZ1024-1; i++) {
		uint_to_1024(C,0);
		for (j=0; j<__SZ1024-1; j++) {
			t = A[j];
			t *= B[i];
		        *((uint64_t *) (C+j)) += t;
		}
		cpy1024(m,N);
		mod_n(C,m);
		
		for (k=0; k<=i; k++) {
			if (k) { shl1024(C, 32); }
			/*if (C[__SZ1024-2])*/ mod_n(C,m);
		}
		add1024(R,C);
		mod_n(R,m);
		
	}
	cpy1024(A,R);

}
	




/*  
 *  A = A >> s 
 */
void  	shr1024 	( uint1024 A, uint16_t s )
{
	int8_t i;
	uint32_t r,r2;
	
	r=s/32;
	if (r) {
		for (i=0; i<__SZ1024-r; i++) A[i]=A[i+r];
		for (i=__SZ1024-r; i<__SZ1024; i++) A[i]=0;
		s%=32;
	}
	
	if (s) {
		r=0;
		for (i=__SZ1024-2; i>=0; i--) {
			r2 = A[i] & ((1 << s) -1);
			A[i] = (A[i] >> s) + (r << (32-s));
			r = r2;
		}
	}
}


/*  
 *  A = A << s 
 */
void  	shl1024 	( uint1024 A, uint16_t s )
{
	int8_t i;
	uint32_t r,r2;

#if DSHL1024
	printf("Debugging shl1024(s=%hu)\n",s);
	printf("A="); dump1024(A);
#endif
	
	r=s/32;
	if (r) {
		for (i=__SZ1024-1; i>=r; i--) A[i]=A[i-r];
		for (i=0; i<r; i++) A[i]=0;
		s%=32;
	}
	
	#if DSHL1024
		printf("s=%hu",s);
	#endif

	if (s) {
		r=0;
		for (i=0; i<__SZ1024-1; i++) {
			r2 = A[i] >> (32-s);
			A[i] = (A[i] << s) + r;
			r = r2;
			#if DSHL1024
				dump1024(A);
			#endif
		}
	}
	
	#if DSHL1024
		puts("leaving shl1024");
	#endif
}

int
	cmp1024		( const uint1024 A, const uint1024 B )
{
	int8_t i=__SZ1024-1;

	while (i>=0) 
		if (A[i]<B[i]) return(-1);
		else if (A[i]>B[i]) return (1);
		else i--;
	return(0);
}


/*
 *  returns 1 if A=0, 0 otherwise
 */
int8_t	zero1024	( const uint1024 A )
{
	uint8_t i;
	
	for (i=0; i<__SZ1024; i++)
		if ( A[i] ) return (0);

	return (1);

}

/* 
 * converts A as uint32_t to D as uint1024 uint1024
 */
void  uint_to_1024	( uint1024 D, const uint32_t A ) {
	uint8_t i;
		
	for (i=0; i<__SZ1024; i++)
		D[i] = 0;
	D[0] = A;
	

}


/*
 * A = B
 */
void	cpy1024		( uint1024 A, const uint1024 B ) {
	uint8_t i;

	for (i=0; i<__SZ1024; i++)
		A[i]=B[i];

}


#include <strings.h>
#include <stdio.h>

/*
 * dumps A as array of uints
 */
void dump1024	(const uint1024 A)
{
	uint8_t i;
	for (i=0; i<__SZ1024; i++) 
		printf("%u ",A[i]);
	puts("\n");
}


/* 
 * finds the GCD of A and B and result stores in G
 */

void GCD(const uint1024 A, const uint1024 B, uint1024 G) {
	uint1024 C,D;
	if (cmp1024(A,B)>0) { cpy1024(C,A); cpy1024(D,B); }
	else { cpy1024(C,B); cpy1024(D,A); }
	
	do {
		cpy1024(G,C);
		mod_n(G,D);
		
		cpy1024(C,D);
		cpy1024(D,G);
	} while (non_zero1024(G));

	cpy1024(G,C);
}


/*
 * reads/writes x from/to stream
 * return non-zero if failed
 */
int 	read1024	( FILE *stream, uint1024 x ) {
	return ( fread(x, 32/8, __SZ1024, stream) != __SZ1024 );
}
int	write1024	( FILE *stream, const uint1024 x ) {
	return ( fwrite(x, 32/8, __SZ1024, stream) != __SZ1024 );
}




/*
 * ***********************************************************
 */

#if DEBUG1024
/*
 * testing functionality
 */
int testAdd(uint32_t x0, uint32_t dx, uint32_t loop)
{
	uint1024 A, B;

	uint_to_1024 (A,x0);
	uint_to_1024 (B,dx);

	while (loop--) { add1024 (A,B); dump1024(A); }
	
}


int testSub(uint32_t x0, uint32_t dx, uint32_t loop)
{
	uint1024 A,B;

	uint_to_1024 (A, 1);
	shl1024(A,x0);
	
	dump1024(A);
	uint_to_1024(B, dx);
	while (loop--) {
		if (sub1024(A,B)) puts("Negative:"); else
		dump1024(A);
	}
	
}

int testShr(uint32_t loop)
{
	uint1024 A,B;

	uint_to_1024 (A, 16777216);

	while (1) { 
		memcpy(B, A, __SZ1024*sizeof(uint32_t));
		if (add1024(A,B)) break;
	}
	
	memcpy(A,B, __SZ1024*sizeof(uint32_t));
	dump1024(A);
	while (loop--) {
		/*if (sub1024(A,B)) puts("Negative:");*/
		shr1024(A,1);
		dump1024(A);
	}
	
}

void test_mulmod(int ArgC, char *ArgV[]) {
	unsigned int n,a,b;
	uint1024 N,A,B,one;

	n=63; a=40; b=50;
	if (ArgC>2) {
		sscanf(ArgV[1], "%u", &n);
		sscanf(ArgV[2], "%u", &a);
		sscanf(ArgV[3], "%u", &b);
	}

	uint_to_1024(one,1); 
#define cr(N,n)	cpy1024(N,one); shl1024(N,n); sub1024(N,one)
	cr(N,n); cr(A,a); cr(B,b);

	printf("Testing: (2^%u-1)*(2^%u-1)\%(2^%u-1)\n",a,b,n);
/*	printf("N="); dump1024(N);
	printf("A="); dump1024(A);
	printf("B="); dump1024(B);
*/
	mul1024modN(A,B,N);
	dump1024(A);
}

void test_GCD(int ArgC, char *ArgV[]) {
	int e1,d1,e2,d2;
	uint1024 A,B, one;  uint_to_1024(one,1);
	
	e1=97; e2=96; d1=0; d2=0;
	if (ArgC>3) {
		sscanf(ArgV[1], "%d", &e1);
		sscanf(ArgV[2], "%d", &d1);
		sscanf(ArgV[3], "%d", &e2);
		sscanf(ArgV[4], "%d", &d2);
	}

	printf("Testing gcd(2^%d%+d, 2^%d%+d)\n",e1,d1,e2,d2);
	
	cpy1024(A,one); shl1024(A,e1);
	cpy1024(B,one); shl1024(B,e2);
	
	if (d1>0) {
		uint_to_1024(one,d1); 
		add1024(A,one);
	} else {
		uint_to_1024(one,-d1);
		sub1024(A,one);
	}
	if (d2>0) {
		uint_to_1024(one,d2); 
		add1024(B,one);
	} else {
		uint_to_1024(one,-d2);
		sub1024(B,one);
	}

	dump1024(A); 
	dump1024(B);
	puts("Launching GCD ... ");
	GCD(A,B,one);
	dump1024(one);
	
}

int main(int ArgC, char *ArgV[]) {


	uint1024 one, x; int i; uint_to_1024(one,1); //shl1024(one,40);
	puts("testing shl:");
	
	for (i=128; i<900; i+=16) {
		cpy1024(x,one);
		shl1024(x,i);
		printf("i=%d: ",i); dump1024(x);
	}
		
//	test_GCD(ArgC, ArgV);
//	test_mulmod(ArgC, ArgV);
/*
	unsigned int x0, dx, l;
	x0=10; dx=1; l=100;
	if (ArgC>2) { 
		sscanf(ArgV[1], "%u", &x0);
		sscanf(ArgV[2], "%u", &dx);
		sscanf(ArgV[3], "%u", &l);
	}

	printf("Testing Sub: (x0=%u) dx=%u loops: %u\n",x0, dx, l);
	testSub(x0,dx,l);
*/
	printf("Done\n");
		
}

#endif

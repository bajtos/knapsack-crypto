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

#include "config.h"
#include "uint1024.h"
#include "ks_crypt.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/******************************************************************
 *         PRIVATE KEY
 *****************************************************************/

uint1024 priv_sum;	/* sum of all items in private key */

/*
 * finds random number m, m>priv_sum 
 */

void find_m(void) {
	uint1024 q;
	int i;
	unsigned long s;
	
	uint_to_1024(m,0);
	
	s=RAND_MAX; i=0;
	while (s) { s>>=1; i++; }
	s=i;

	for (i=0; (i<=1024) && (cmp1024(m,priv_sum)<=0); i+=s) {
		uint_to_1024(q,random());
		if (i) shl1024(m,s);
		add1024(m,q);
	}

	while (cmp1024(m,priv_sum)<0) {
		uint_to_1024(q, 1.0*(1<<(1022+s-i))*random()/RAND_MAX);
		add1024(m,q);
	}
}

/*
 * finds number v, v<m which is relatively prime to m 
 * (sharing no factors with m besides 1)
 */
void find_v(void) {
	uint1024 one,g; uint_to_1024(one,1);
	
	cpy1024(v,m);
	shr1024(v,1);
	do {
	  add1024(v,one);
	  GCD(v,m,g); 
	} while (cmp1024(g,one));
}

/*
 * finds such u, so u*v=1 (mod m)
 */
void find_u(void) {
	uint1024 A[3][2];
	uint1024 N;


	uint_to_1024(A[0][0],0);
	uint_to_1024(A[1][0],1);
	cpy1024(A[0][1], m);
	cpy1024(A[1][1], v);

	while (non_zero1024(A[1][1])) {
		cpy1024(A[2][0], A[1][0]);
		cpy1024(A[2][1], A[1][1]);
		
		cpy1024(A[1][0], A[0][0]);
		cpy1024(A[1][1], A[0][1]);
		cpy1024(A[0][1], A[2][1]);
		cpy1024(A[0][0], A[2][0]);


/*
 * X <- X%mm
 * Y <- [mm/X]*nn
 */
#define mod(X,mm,Y,nn) \
	cpy1024(N,mm); \
	while (cmp1024(mm,X)<0) { shl1024(mm,1); shl1024(nn,1); }\
\
	while (cmp1024(X,N)>=0) {\
		while (cmp1024(mm,X)>0) { shr1024(mm,1); shr1024(nn,1); }\
		sub1024(X,mm);\
		if (cmp1024(Y,nn)<0) add1024(Y,m); sub1024(Y,nn);\
	}
		
		mod(A[1][1], A[2][1], A[1][0], A[2][0]);
//		puts("dumping v:");
//		dump1024(A[1][0]); dump1024(A[1][1]);
	}	

	cpy1024(u, A[0][0]);
}




/*
 * gen_priv_key()
 */
void gen_priv_key(const unsigned int seed) {
  uint1024 a,one;
  int i;

  if (verbose>1)  puts("  Generating items");
  srandom(seed);

  uint_to_1024(priv_sum,0);
  
  for (i=0; i<ITEMS; i++) {
    uint_to_1024(private_key[i],0);
    while (cmp1024(private_key[i],priv_sum)<=0) {
      shl1024(private_key[i],4);
      uint_to_1024(a,(uint) (16.0*random()/RAND_MAX));
      add1024(private_key[i],a);
    }
    add1024(priv_sum,private_key[i]);

  }
  
  if (verbose>1) puts("  Counting 'm'"); 
  find_m();
  
  if (verbose>1) puts("  Counting 'v'");
  find_v();
  
  if (verbose>1) puts("  Counting 'u'");
  find_u();

  if (verbose>1) puts("  Checking u*v=1 (mod m)");
  cpy1024(a, u);
  mul1024modN(a,v,m);
  uint_to_1024(one,1);
  if (cmp1024(one,a)) {
	  puts("!! WARNING !! u*v != 1 (mod m) !! WARNING !!");
	  puts("This key will provide only one-way trip !");
	  if (verbose>1) 
		  puts("E.g. it's usable only for encryption, not decryption !");
  }
}


/*
 * writes/reads {private_key, u, m} to/from specified file
 */
int		store_priv_key	( const char *file_name ){
	FILE *f; int r=0;
	f = fopen(file_name, "w");
	if (f) {
		if (fwrite(private_key,sizeof(uint1024),ITEMS, f)!=ITEMS)
			r=1;
		else
		if (fwrite(m, sizeof(uint1024), 1, f)!=1)
			r=1;
		else
		if (fwrite(u, sizeof(uint1024), 1, f)!=1)
			r=1;
	} else return(-1);
	fclose(f);
	return(r);
}

int		load_priv_key	( const char *file_name ){
	FILE *f; int r=0;
	f = fopen(file_name, "r");
	if (f) {
		if (fread(private_key,sizeof(uint1024),ITEMS, f)!=ITEMS)
			r=1;
		else
		if (fread(m, sizeof(uint1024), 1, f)!=1)
			r=2;
		else
		if (fread(u, sizeof(uint1024), 1, f)!=1)
			r=3;
	} else return(-1);
	fclose(f);
	return(r);
}




/******************************************************************
 *                 PUBLIC KEY
 ******************************************************************/

void gen_pub_key(void) {
	uint16_t i;
#if SHAKE_PUB_KEY
	uint1024 c;
	uint1024 t;
#define swap(A,B) { cpy1024(t,A); cpy1024(A,B); cpy1024(B,t); }
#endif
	if (verbose>1) puts("  Changing values [ *v mod m ]");
	for (i=0; i<ITEMS; i++) {
		cpy1024(public_key[i], private_key[i]);
		mul1024modN(public_key[i], v, m);
	}
#if SHAKE_PUB_KEY
/*
 * well, the values should be yet distributed 'randomly'. If we will shake them,
 * we'll need a 'map of shaking' for decryption routine. Or count it 
 * every time we'll decrypt.
 * 
	if (verbose>1) puts("  Shaking");
	c = ITEMS*(1.0*random()/RAND_MAX+0.5);
	for (i=0; i<c; i++)  
		swap(	public_key[(int) (1.0*ITEMS*random()/RAND_MAX)],
			public_key[(int) (1.0*ITEMS*random()/RAND_MAX)]);
*/
#endif
}

/*
 * writes/reads public_key to/from specified file
 */
int		store_pub_key	( const char *file_name ){
  	FILE *f; int r=0;
	f = fopen(file_name, "w");
	if (f) {
		if (fwrite(public_key, sizeof(uint1024), ITEMS, f)!=ITEMS)
			r=1;
	} else return(-1);
	fclose(f);
	return(0);
}

int		load_pub_key	( const char *file_name ){
  	FILE *f; int r=0;
	f = fopen(file_name, "r");
	if (f) {
		if (fread(public_key, sizeof(uint1024), ITEMS, f) != ITEMS)
			r=-1;
		fclose(f);
	} else return(1);
	return(r);
}



/************************************************************
 * 			Encryption
 ************************************************************/

/*
 * returns encrypted first ITEMS bites of data
 */
void encrypt	( const void *data, uint1024 dest ) {
	uint8_t  t=0;
	int16_t i,o;

	o=-1; uint_to_1024(dest,0);

	for (i=0; i<ITEMS; i++) {
		if (!(i%8)) t=((uint8_t *)data)[++o];
		if (t&1) add1024(dest, public_key[i]); 
		t>>=1;
	}
}




/***********************************************************
 *			Decryption
 ***********************************************************/


/*
 * returns decrypted first ITEMS bites of data
 */
void  	decrypt	( const uint1024 data, void *dest) {
	uint8_t  t, buff[ITEMS/8];
	int16_t i,o;
	uint1024 dat;

	if (zero1024(u) || zero1024(m)) { dest=0; return; }
	
	cpy1024(dat,data);
	mul1024modN(dat,u,m);

	o=ITEMS/8; t=0;

	for (i=ITEMS-1; i>=0; i--) {
		t<<=1;
		if (cmp1024(dat,private_key[i])>=0) {
			t|=1;
			sub1024(dat, private_key[i]);
		}
		
		if (i%8==0) {
			buff[--o]=t;
			t=0;
		}
	}

	bcopy(buff, dest, ITEMS/8);
}


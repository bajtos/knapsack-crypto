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

#ifndef __KS_CRYPT_H__ 
#define __KS_CRYPT_H__

#include  "uint1024.h"

#define ITEMS 256
typedef 	uint1024 	kskey_t[ITEMS];

kskey_t 	private_key, 
		public_key;
uint1024	u,v,m;

/*
 * generates {private_key, u, v, m}
 */
void 		gen_priv_key	( const unsigned int seed );

/*
 * writes/reads {private_key, u, m} to/from specified file
 */
int		store_priv_key	( const char *file_name );
int		load_priv_key	( const char *file_name );

/*
 * generates public_key from {private_key, v, m}
 */
void		gen_pub_key	( void );

/*
 * writes/reads public_key to/from specified file
 */
int		store_pub_key	( const char *file_name );
int		load_pub_key	( const char *file_name );


/*
 * returns encrypted first ITEMS bites of data
 */
void		encrypt		( const void *data, uint1024 dest );

/*
 * returns decrypted first ITEMS bites of data
 */
void		decrypt	( const uint1024 data, void *dest );


#endif /* ks_crypt.h */

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

#define APP_NAME "encryptor"

#include "ks_crypt.h"
#include "uint1024.h"
#include "config.h"
#include <stdint.h>
#include <stdio.h>
#include <getopt.h>

char *in_fn, *out_fn, *key_fn="public-key";
	/* files with key, input file & output file */

void init(char *pn);			/* prints stuff about prog&author */
void warranty(void);			/* warranty - cut&pasted from GPL */
void help(void);			/* prints help */

int parse_args(int argc, char *argv[]) {
	int c;
	int opt_ix;
	static struct option long_options[] = 
	{
		{ "warranty", 0, 0, 'w'},
		{ "help", 0, 0, 0},
		{ "key-file", 1, 0, 'k'},
		{ 0, 0, 0, 0}
	};

	while (1) {
		c = getopt_long (argc, argv, "wk:", 
				 long_options, &opt_ix);

		if (c==-1) break;

		switch (c) {
		  case 0:
			switch (opt_ix) {  
			  case 1: init(argv[0]); help(); return(1);
			  default: 
			    return(1);
			}
			break;
			
		  case 'w': warranty(); return(1);

		  case 'v': 
		    if (optarg) 
		      if (sscanf(optarg,"%hd", &verbose)!=1) {
			      fprintf(stderr,"Invalid argument for %s: %s\n",
					      argv[optind], optarg);
			      return(1);
		      } else;
		    else verbose=1;
		    break;

		  case 'q':
		    verbose = -1;
		    break;
		    
		  case 'k': key_fn=optarg; break;

		  case '?':
		    return(1);
	  	  default:
          	    fprintf(stderr,
			"?? getopt returned character code 0%o ??\n", c);
        }
  }
  if (optind<argc) out_fn = argv[optind++]; else out_fn = 0;

  if (optind<argc) in_fn = argv[optind++]; else in_fn=0;
		
/*  init(argv[0]);	*/
   
  if (optind < argc) 
    {
      fputs ("Unused parameter(s): ",stderr);
      while (optind < argc)
        fputs (argv[optind++],stderr);
      fputs ("\n",stderr);
    }
  return(0);
}


/************
 *   MAIN   *
 ***********/
int main(int argc, char *argv[]) {

	FILE *fi,*fo, *ft;
	uint8_t data[ITEMS/8];
	uint1024 d;
	int e;
	uint8_t r;
	
	verbose = 1;
	if (parse_args(argc,argv)) return(1);
	
	switch (load_pub_key(key_fn)) {
		case 0: break;
		case -1:
			fprintf(stderr,"Could not open %s.\n",key_fn);
			return(2);
		default:
			fprintf(stderr,"Incorrect format of public key.\n");
			return(3);
	}

	if (in_fn && strcmp(in_fn,"-"))
		if (!(fi=fopen(in_fn, "r"))) {
			fprintf(stderr,"Could not open file %s.\n",in_fn);
			return(4);
		} else;
	else fi=stdin;

	if (out_fn && strcmp(out_fn,"-"))
		if (!(fo=fopen(out_fn, "w"))) {
			fprintf(stderr,"Could not create file %s.\n",out_fn);
			return(5);
		} else;
	else fo=stdout;
	
	if (!(ft=tmpfile() )) {
		fputs("Could not create temp file.\n",stderr);
		return(6);
	}
	
	e=0;
	while ( !feof(fi) && !ferror(fi) && !ferror(ft) ) {
		r=fread(data, 1, ITEMS/8, fi);
		if (r) {
  			for (e=r; e<ITEMS/8; e++) data[e]=0;
			encrypt(data,d);
			write1024(ft, d);
			e=1;
		}
	}	
	
	if (ferror(fi)) {
		fprintf(stderr, "Error encountered during reading from %s\n",
				in_fn);
		return(7);
	}
	fclose(fi); 	

	if (ferror(ft)) {
		fprintf(stderr, "Could not write to temp file\n"); 
		return(8);
	}
	
	if (e && !r) r=ITEMS/8;
	fwrite( &r, 1, 1, fo);
	rewind(ft);
	while (!ferror(ft) && !ferror(fo)) {
		read1024(ft,d);
		if (!feof(ft)) write1024(fo,d); else break;
	}
	
	if (ferror(ft)) {
		fprintf(stderr, 
			"Error encountered during reading from temp file\n");
		return(7);
	}
	if (ferror(fo)) {
		fprintf(stderr, "Could not write to %s\n", 
			(out_fn)?out_fn:"stdout");
		return(8);
	}
	fclose(fo); fclose(ft);

	return(0);	
} /* main */



/**** STUFF ****/


void help(void) {
printf("
Syntax: 
  %s [options] [output file] [input file]

If some file is not specified or is -, standard input/output will be used.

Options:
	-w	--warranty		show warranty information 
		--help			show this

	-k	--key-file		specifies file containing public key
",APP_NAME);
}

void init(char *pn) {
if (verbose<0) return;
printf("\
Knapsack solving problem encryption - %s (version %s)
Copyright (C) 2001 Miroslav 'Mirco' Bajtos
%s comes with ABSOLUTELY NO WARRANTY; for details run
	%s --warranty
This is free software, and you are welcome to redistribute it
under certain conditions; see GPL for details.

",APP_NAME,VERSION,APP_NAME,pn);
}

void warranty(void) {
puts("\
BECAUSE THE PROGRAM IS LICENSED FREE OF CHARGE, THERE IS NO WARRANTY
FOR THE PROGRAM, TO THE EXTENT PERMITTED BY APPLICABLE LAW.  EXCEPT WHEN
OTHERWISE STATED IN WRITING THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES
PROVIDE THE PROGRAM \"AS IS\" WITHOUT WARRANTY OF ANY KIND, EITHER EXPRESSED
OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.  THE ENTIRE RISK AS
TO THE QUALITY AND PERFORMANCE OF THE PROGRAM IS WITH YOU.  SHOULD THE
PROGRAM PROVE DEFECTIVE, YOU ASSUME THE COST OF ALL NECESSARY SERVICING,
REPAIR OR CORRECTION.

IN NO EVENT UNLESS REQUIRED BY APPLICABLE LAW OR AGREED TO IN WRITING
WILL ANY COPYRIGHT HOLDER, OR ANY OTHER PARTY WHO MAY MODIFY AND/OR
REDISTRIBUTE THE PROGRAM AS PERMITTED ABOVE, BE LIABLE TO YOU FOR DAMAGES,
INCLUDING ANY GENERAL, SPECIAL, INCIDENTAL OR CONSEQUENTIAL DAMAGES ARISING
OUT OF THE USE OR INABILITY TO USE THE PROGRAM (INCLUDING BUT NOT LIMITED
TO LOSS OF DATA OR DATA BEING RENDERED INACCURATE OR LOSSES SUSTAINED BY
YOU OR THIRD PARTIES OR A FAILURE OF THE PROGRAM TO OPERATE WITH ANY OTHER
PROGRAMS), EVEN IF SUCH HOLDER OR OTHER PARTY HAS BEEN ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.
");
}


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

#define APP_NAME "key generator"

#include "config.h"
#include "uint1024.h"
#include "ks_crypt.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

char *pub_key_file = "public-key";	/* file name for public key */
char *priv_key_file = "private-key";	/* file name for private key */
unsigned int seed;			/* seed for priv_key generator */

void init(char *pn);			/* prints stuff about prog&author */
void warranty(void);			/* warranty - cut&pasted from GPL */
void help(void);			/* prints help */

int parse_args(int argc, char *argv[]) {
	int c;
	int opt_ix;
	static struct option long_options[] = 
	{
		{ "warranty", 0, 0, 'w'},
		{ "verbose", 2, 0, 'v'},
		{ "priv-key-file", 1, 0, 0},
		{ "pub-key-file", 1, 0, 0},
		{ "help", 0, 0, 0},
		{ "seed", 1, 0, 's'},
		{ "quite", 0, 0, 'q'},
		{ 0, 0, 0, 0}
	};

	while (1) {
		c = getopt_long (argc, argv, "qwv::s:", long_options, &opt_ix);

		if (c==-1) break;

		switch (c) {
		  case 0:
			switch (opt_ix) {  
			  case 2: priv_key_file=optarg; break;
			  case 3: pub_key_file=optarg; break;
			  case 4: init(argv[0]); help(); return(1);
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
		    
		  case 's': 
		    if (sscanf(optarg,"%u", &seed)!=1) {
			    fprintf(stderr, "Invalid argument for %s: %s\n",
					    argv[optind], optarg);
			    return(1);
		    } 
		    break;
		  case '?':
		    return(1);
	  	  default:
          	    fprintf(stderr,
			"?? getopt returned character code 0%o ??\n", c);
        }
  } 
  
  init(argv[0]);	
   
  if (optind < argc) 
    {
      fputs ("Unused parameter(s): ",stderr);
      while (optind < argc)
        fputs (argv[optind++],stderr);
      fputs ("\n\n",stderr);
    }
  return(0);
}

int main(int argc, char *argv[]) {
  unsigned long tm;
  
  
  time(&tm);
  seed = tm%(1U<<(8U*(sizeof(int)-1)));
  verbose = 1;
  
  if (parse_args(argc, argv)) return(-1);
  
  if (verbose>0) puts("Generating private key ...");
  if (verbose>1) printf("  Seed: %u\n",seed);
  gen_priv_key(seed);  
  
  if (verbose>0) printf("Writing >> %s\n",priv_key_file);
  switch (store_priv_key(priv_key_file)) {
	case 0: break;
	case -1: fputs("Could not open file private-key",stderr);
		 break;
	default: fputs("Could not write private key",stderr);
  }


  if (verbose>0) puts("Generating public key ...");
  gen_pub_key();
    
  if (verbose>0) printf("Writing >> %s\n",pub_key_file);
  switch (store_pub_key(pub_key_file)) {
	  case 0: break;
	  case -1: fputs("Could not open file public-key",stderr);
		   break;
	  default: fputs("Could not write public key",stderr);
  }
  
  if (verbose>0) puts("Done.");

  return(0);

} /* main */



void help(void) {
puts("\
Options:
	-v[l]	--verbose [l]		sets the verbosity of application,
					parameter is optional (default: 1)
	-q	--quite			don't write anything to stdout
	-w	--warranty		show warranty information 
		--help			show this

	-s	--seed			set seed for random number generator

	--priv-key-file <file>		
	--priv-key-file=<file>		specify filename for private key

	--pub-key-file <file>
	--pub-key-file=<file>		specify filename for public key

");
}

void init(char *pn) {
if (verbose>0) 
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


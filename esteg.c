#include <stdio.h>

#define ncolors 28 +2		/* para ignorar o par CR+LF */

int
main (void)
{
  const char *acolors[] =
    { "M@", "H", "mRW#", "BE", "gKQS", "pqGU", "DX8$", "bdkwAFP", "ehNTZ0&",
"Y4569", "V", "ansyzC*", "oxLO3%", "fuJ2", "crI", "tv[]", "l17{}", "j", "I", "()+|", "=", ";",
"~!_", "\\/:", "^,", ".", "`", " ", "\r", "\n" };
  unsigned int palbits[ncolors], palopts[ncolors];
  unsigned int i, j, k, c, aux, bitp, bits;
  FILE *fpi = stdin, *fpo = stdout;
  for (i = 0; i < ncolors; i++)
    {
      for (j = 0; acolors[i][j]; j++);
      palopts[i] = j;
      for (k = -1; j; j >>= 1, k++);
      palbits[i] = k;
      fprintf (stderr, "%d\topts = %d\tbits = %d\n", i, palopts[i],
	       palbits[i]);
    }
  for (aux = bitp = 0; !feof (fpi);)
    {
      c = fgetc (fpi);

      //fprintf(stderr, "%c", c);

      for (i = 0; i < ncolors; i++)
	{
	  for (j = 0; acolors[i][j] && acolors[i][j] != c; j++);
	  if (acolors[i][j])
	    break;
	}

      fprintf (stderr, "[%c] maps to %d\n", c, j);

      if (palbits[i])
	{
	  aux <<= palbits[i];
	  aux |= j;
	  bitp += palbits[i];
	  fprintf (stderr, "BITP = %2d, AUX = %04X, BITSt = %01X, BITS = %d\n", bitp, aux,
		   j, palbits[i]);
	  if (bitp >= 8)
	    {
	      bitp -= 8;
              fprintf (fpo, "%02X ", aux & 0xFF);
	      aux >>= 8;
	    }
	}

    }

}

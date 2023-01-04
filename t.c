#include <stdio.h>
#include "tfx_fontdata.h"

#define IS_BIT(c, x, y) (TFX_AsciiFontdata[c * 12 + y] & (1 << x) ? 1 : 0)
#define FY(x) (x < 12? x : 12 )
#define BLUR_BITS(c, x, y) ((double)IS_BIT(c, x, y) + (double) IS_BIT(c, x, FY(y)) )/ 2.0 

void
main (void)
{
  int i, j, k, n = 0, l;
  double aux, aux2;
  char acolors[256][40];
  double pal1[256], pal2[256];
  for (i = 0; i < 256; i++)
    {
      if (isprint (i))
	{
	  //printf ("\nMAPA de [%c]\n", isprint (i) ? i : '.');
	  for (j = 0, aux = 0; j < 6; j++)
	    {
	      for (k = 0; k < 8; k++)

		aux += BLUR_BITS (i, k, j);

	    }
	  for (aux2 = 0; j < 12; j++)
	    {
	      for (k = 0; k < 8; k++)
		aux2 += BLUR_BITS (i, k, j);

	    }
	  for (j = 0; j < n; j++)
	    {
	      if (pal1[j] == aux && pal2[j] == aux2)
		{
		  l = strlen (acolors[j]);
		  acolors[j][l] = i;
		  acolors[j][l + 1] = '\0';
		  break;
		}
	    }

	  if (j == n)
	    {
	      acolors[n][0] = i;
	      acolors[n][1] = '\0';
	      pal1[n] = aux;
	      pal2[n] = aux2;
	      n++;
	    }
	}

    }
  for (i = 0; i < n; i++)
    printf ("%3.1f, %3.1f: \"%s\"\n", pal1[i], pal2[i], acolors[i]);


}

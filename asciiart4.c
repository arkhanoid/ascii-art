/*
  Implementa‡Æo do algoritmo de Floyd Steinberg para dithering
  Utiliza paleta e gamma para converter um bitmap em ascii art
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tfx_fontdata.h"

#define DITHER 1		/* Enable dithering? */
#define SHARPEN 0		/* Enable sharpening? */

#define KS 15			/* kernel = KS * I - BLUR */
#include <math.h>

#define GAMMA 0.45454545
#define GAMMAR 2.2
#define gamma(x, g) (int) ( powf( (float) (x) / 256, g) * 256 )

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a > b ? a : b)
#define PIX(x, y) image[(x) + (y) * h1.W]
#define DIST(x, y) ((x-y)*(x-y))	/* utiliza NORMA L2 */
//#define DIST(x, y) ((x-y)>0?x-y:y-x) /* utiliza NORMA L1 */
#define DIST2(x, y, p1, p2) DIST(PIX(x, y), p1) + DIST(PIX(x, y+1), p2 /*MAX(255, p2 + 0.1875 * (PIX(x, y) - p1)) */)
#define ADJ(x, y, c, e) PIX(x, y) = MIN( MAX( PIX(x, y) + c * e, 1), 255)
#define IS_BIT(c, x, y) ( TFX_AsciiFontdata[c * 12 + y ] & (1 << x) ? 1 : 0)
#define FY(x) (x < 12? x : 12 )
#define BLUR_BITS(c, x, y) ((double)IS_BIT(c, x, y) + (double) IS_BIT(c, x, FY(y)) )/ 2.0

typedef struct bmph
{
  unsigned long flen;		// 4604 0000
  unsigned long reserved;	// 0000 0000
  unsigned long offs;		// 3604 0000
  unsigned long H_SIZE;		// 2800 0000
  unsigned long W;		// 0400 0000
  unsigned long H;		// 0400 0000
  unsigned short PLANES;	// 0100
  unsigned short bpp;		// 0800
  unsigned long COMPRESSION;	// 0000
  unsigned long IMAGE_SIZE;	// 1000 0000
  unsigned long V_RES;		// 0B6D 0000
  unsigned long H_RES;		// 0B6D 0000
  unsigned long COLORINDEX;	// 0100 0000
  unsigned long IMPORTINDEX;	// 0100 0000
} bmph;

int ncolors;
char acolors[256][40];
int
index_color (int x, int y, int *pal1, int *pal2, int *image, bmph h1)
{
  unsigned int i, d = 65535, k;
  for (i = 0; i < ncolors; i++)

    {
      if (d > DIST2 (x, y, pal1[i], pal2[i]))
	{
	  d = DIST2 (x, y, pal1[i], pal2[i]);
	  k = i;
	}
    }
  return k;
}

void
initialize_font (int *p1, int *p2)
{
  int i, j, k, n = 0, l;
  double aux, aux2, pal1[256], pal2[256];
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

  ncolors = n;
  for (i = 0; i < n; i++)
    aux = MAX (MAX (aux, pal1[i]), pal2[i]);

  for (i = 0; i < n; i++)
    {
#ifdef LINEAR_PAL
   p1[i] = (int) pal1[i] * 255 / aux;
   p2[i] = (int) pal2[i] * 255 / aux;
#else
      p1[i] = (int) (gamma (pal1[i], GAMMA) * 255 / gamma (aux, GAMMA));
      p2[i] = (int) (gamma (pal2[i], GAMMA) * 255 / gamma (aux, GAMMA));
#endif
  	fprintf (stderr, "%3d, %3d: \"%s\"\n", p1[i], p2[i], acolors[i]);
    }
}

int
main (int argc, char **argv)
{
  bmph h1;
  FILE *fpi, *fpo, *fpr;
  int x, y, i, j, k, r, g, b, aux, s;
  double np, op, err, np2, op2, err2;
  unsigned int *image;
  unsigned int pal1[256], pal2[256];
  unsigned int palopts[256];
  if (argc < 2)
    {
      fprintf (stderr, "%s file.bmp [out.txt] [subtitles]\n", argv[0]);
      return -1;
    }
  fpi = fopen (argv[1], "rb");
  fpo = argc > 2 && strcmp ("-", argv[2]) ? fopen (argv[2], "wt") : stdout;
  if (fpi == NULL || fpo == NULL)

    {
      fprintf (stderr, "fopen()\n");
      return -1;
    }
  fpr = fopen ("/dev/urandom", "rb");
  if (fpr == NULL)
    {
      fprintf (stderr, "urandom\n");
      return -1;
    }
  fprintf (stderr, "sizeof(bmph) = %d\n", sizeof (bmph));
  fgetc (fpi);
  fgetc (fpi);
  fread (&h1, sizeof (bmph), 1, fpi);
  image = (int *) malloc (h1.W * h1.H * sizeof (int));
  initialize_font (pal1, pal2);
  if (image == NULL)

    {
      fprintf (stderr, "malloc()\n");
      return -1;
    }
  aux = (h1.IMAGE_SIZE - h1.W * h1.H * h1.bpp / 8) / h1.H;
  if (h1.bpp != 24)

    {
      fprintf (stderr, "bits per pixel != 24\n");
      return -1;
    }

  for (y = h1.H - 1; y >= 0; y--)

    {
      for (x = 0; x < h1.W; x++)

	{
	  b = fgetc (fpi);
	  g = fgetc (fpi);
	  r = fgetc (fpi);

	  PIX (x, y) =
	    (30 * gamma (r, GAMMA) + 59 * gamma (g, GAMMA) +
	     11 * gamma (b, GAMMA)) / 100;
	}
      for (i = 0; i < aux; i++)
	fgetc (fpi);
    }
  for (i = 0; i < ncolors; i++)

    {
      for (j = 0; acolors[i][j]; j++);
      palopts[i] = j;
    }

  for (y = aux = 0; y < h1.H - 1; y += 2)

    {
      for (x = 0; x < h1.W; x++)

	{
	  op = PIX (x, y);
	  op2 = PIX (x, y + 1);
	  j = index_color (x, y, pal1, pal2, image, h1);

#if DITHER
	  np = pal1[j];
	  np2 = pal2[j];
	  err = op - np;
	  err2 = op2 - np2 /*- err * 0.1875*/;
	  if (x + 1 < h1.W)
	    {
	      ADJ (x + 1, y, 0.4375, err);
	      ADJ (x + 1, y + 1, 0.4375, err2);
	    }
	  if (x)
	    {
	      ADJ (x, y + 1, 0.1875, err);
	      ADJ (x, y + 2, 0.1875, err2);
	    }
	  if (y + 2 < h1.H)
	    {
	      ADJ (x, y + 1, 0.3125, err);
	      ADJ (x, y + 2, 0.3125, err2);
	    }
	  if (x + 1 < h1.W && y + 2 < h1.H)
	    {
	      ADJ (x + 1, y + 1, 0.0625, err);
	      ADJ (x + 1, y + 2, 0.0625, err2);
	    }
#endif /*  */

	  k = fgetc (fpr) % palopts[j];	/* randomize palete bits */
	  if(!strchr("<>&", acolors[j][k]) ) fputc (acolors[j][k], fpo);
	  else fprintf(fpo, "%s", acolors[j][k] == '>' ? "&gt;" :  acolors[j][k] == '<' ? "&lt;" : "&amp;");
	  /* if(feof(fpr)) { goto abort; } */
	}
      fprintf (fpo, "\n");
    }
abort:
  fclose (fpi);
  fclose (fpo);
  fclose (fpr);
}

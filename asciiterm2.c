/*
  Implementa‡Æo do algoritmo de Floyd Steinberg para dithering
  Utiliza paleta e gamma para converter um bitmap em ascii art
*/

#include <stdio.h>
#include <stdlib.h>

#define DITHER 1		/* Enable dithering? */
#define SHARPEN 0		/* Enable sharpening? */

#define KS 15			/* kernel = KS * I - BLUR */
#include <math.h>

#define GAMMA 0.45454545
#define GAMMAR 2.2
#define gamma(x, g) (int) ( powf( (float) (x) / 256, g) * 256 )

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a > b ? a : b)
#define DIST(x, y) ((x-y)*(x-y))	/* utiliza NORMA L2 */
//#define DIST(x, y) ((x-y)>0?x-y:y-x) /* utiliza NORMA L1 */
#define PIX(x, y) image[(x) + (y) * h1.W]
#define ADJ(x, y, c) PIX(x, y) = MIN( MAX( PIX(x, y) + c * err, 1), 255)

const char *acolors[] =
  { "M", "H", "R", "E", "K", "U", "X", "k", "0",
  "Y", "V", "C", "x", "f", "I", "v", "l", "j", "I",
  "+", "=", ";",
  "!", ":", "^", ".", "`", " "
};

#define ncolors 28
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
int
index (int c, int *pal)
{
  unsigned int i, d = 65535, k;
  for (i = 0; i < ncolors; i++)

    {
      if (d > DIST (c, pal[i]))

	{
	  d = DIST (c, pal[i]);
	  k = i;
	}
    }
  return k;
}

int
main (int argc, char **argv)
{
  bmph h1;
  FILE *fpi, *fpo, *fpr, *fpl;
  int x, y, i, j, k, r, g, b, aux, s;
  double np, op, err;
  unsigned int *image;
  char buffer[200];
/*  unsigned int pal[ncolors] = 
  { 0, 42, 50, 58, 66, 74, 81, 89, 96, 104, 111, 118, 125, 132, 139, 146, 153, 160, 167, 173, 180, 187, 199, 206, 212, 231, 243, 255 }; */
  unsigned int pal[ncolors] =
    { 255, 238, 235, 231, 227, 224, 220, 216, 212, 208, 204, 200, 195, 191,
    186, 181, 176, 171, 166, 160, 155, 149, 136, 129, 121, 94, 69, 0
  };
  unsigned int palopts[ncolors], palbits[ncolors], bits, bitp;

  fpl = fopen ("lista2.txt", "rt");

  if (fpl == NULL)
    {
#ifdef DEBUG
      fprintf (stderr, "fopen(fpl)\n");
     #endif
      return -1;
    }

  fpo = stdout;
  fpr = fopen ("/dev/urandom", "rb");
  if (fpr == NULL)
    {
#ifdef DEBUG
      fprintf (stderr, "urandom\n");
#endif
      return -1;
    }

  while (!feof (fpl))
    {
      fgets (buffer, 199, fpl);
      for(i=0; buffer[i]; i++); 
      buffer[i-1] = '\0';
      fprintf (fpo, "\x1b[H");
      fpi = fopen (buffer, "rb");

      if (fpi == NULL || fpo == NULL)

	{
	  fprintf (stderr, "fopen()\n");
	  return -1;
	}
      //fprintf (stderr, "sizeof(bmph) = %d\n", sizeof (bmph));
      fgetc (fpi);
      fgetc (fpi);
      fread (&h1, sizeof (bmph), 1, fpi);
      image = (int *) malloc (h1.W * h1.H * sizeof (int));
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
	  for (k = -1; j; j >>= 1, k++);
	  palbits[i] = k;
//	  fprintf (stderr, "pal[%d] = %d,\topts = %d\tbits = %d\n", i, pal[i],
//		   palopts[i], palbits[i]);
	}

      /* sharpen filter */

#if SHARPEN

#define PIX2(x,y) (x + 1 < h1.W && y + 1 < h1.H ? MIN( MAX( (KS * PIX(x,y) - PIX(x+1,y) - PIX(x, y+1) - PIX(x+1, y+1) ) / (KS - 3), 1), 255): PIX(x,y) )
      for (y = 0; y < h1.H - 1; y++)
	for (x = 0; x < h1.W; x++)
	  PIX (x, y) = PIX2 (x, y);

#endif /*  */
      for (y = bits = bitp = aux = 0; y < h1.H - 1; y++)

	{
	  for (x = 0; x < h1.W; x++)

	    {
	      op = PIX (x, y);
	      j = index (PIX (x, y), pal);

#if DITHER
	      np = pal[j];
	      err = op - np;
	      if (x + 1 < h1.W)
		ADJ (x + 1, y, 0.4375);
	      if (x)
		ADJ (x, y + 1, 0.1875);
	      if (y + 1 < h1.H)
		ADJ (x, y + 1, 0.3125);
	      if (x + 1 < h1.W && y + 1 < h1.H)
		ADJ (x + 1, y + 1, 0.0625);

#endif /*  */

	      k = fgetc (fpr) % palopts[j];	/* randomize palete bits */
	      fputc (acolors[j][k], fpo);
	      /* if(feof(fpr)) { goto abort; } */
	    }
	  fprintf (fpo, "\n");
	}
    abort:
      fclose (fpi);
      usleep(82000); 
    }
  fclose (fpo);
  fclose (fpr);
  fclose (fpl);
}

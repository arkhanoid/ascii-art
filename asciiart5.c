/*
  Implementa‡Æo do algoritmo de Floyd Steinberg para dithering
  Utiliza paleta e gamma para converter um bitmap em ascii art
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
//#include "tfx_fontdata.h"

#ifndef DITHER
#define DITHER 1		/* Enable dithering? */
#endif

#include <math.h>

#define FONT_H font_y
#define FONT_W font_x

#ifndef FONTFILE
#define FONTFILE "courier10neg.bmp"
#endif

#ifndef GAMMA
#define GAMMA 0.65454545
#endif

#define gamma(x, g) (int) ( pow( (double) (x) / 256, g) * 256 )
//#define gamma(x, g) (x)

#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a > b ? a : b)
#define PIX(x, y) image[(x) + (y) * h1.W]
#define DIST(x, y) ((x-y)*(x-y))	/* utiliza NORMA L2 */
//#define DIST(x, y) ((x-y)>0?x-y:y-x) /* utiliza NORMA L1 */
#define DIST2(x, y, p1, p2) DIST(PIX(x, y), p1) + DIST(PIX(x, y+1), p2 )
#define ADJ(x, y, c, e) if(x<h1.W && y<h1.H) PIX(x, y) = MIN( MAX( PIX(x, y) + c * e, 1), 255)
//#define IS_BIT(c, x, y) ( TFX_AsciiFontdata[c * 12 + y ] & (1 << x) ? 1 : 0)
#define IS_BIT(c, x, y) ( c >= ' ' && c <= '?' ? fontbody[x + (c-' ') * FONT_W + y * f1.W] : \
	                  c >= '@' && c <= '_' ? fontbody[x + (c-'@') * FONT_W + (y + FONT_H) * f1.W] : \
	                  c >= '`' && c <= '~' ? fontbody[x + (c-'`') * FONT_W + (y + FONT_H*2) * f1.W] :  0 ) / 256
//#define FY(x) (x < 12? x : 12 )
//#define BLUR_BITS(c, x, y) ((double)IS_BIT(c, x, y) + (double) IS_BIT(c, x, FY(y)) )/ 2.0
#define BLUR_BITS(c, x, y) IS_BIT(c, x, y)

#ifdef WIN32
#define RANDOM rand()
#else
#define RANDOM fgetc(fpr)
//#error "NOT WIN32"
#endif

typedef struct bmph
{
  unsigned int flen;		// 4604 0000
  unsigned int reserved;	// 0000 0000
  unsigned int  offs;		// 3604 0000
  unsigned int H_SIZE;		// 2800 0000
  unsigned int  W;		// 0400 0000
  unsigned int  H;		// 0400 0000
  unsigned short PLANES;	// 0100
  unsigned short bpp;		// 0800
  unsigned short COMPRESSION;	// 0000
  unsigned int  IMAGE_SIZE;	// 1000 0000
  unsigned int V_RES;		// 0B6D 0000
  unsigned int  H_RES;		// 0B6D 0000
  unsigned int  COLORINDEX;	// 0100 0000
  unsigned int  IMPORTINDEX;	// 0100 0000
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

int font_x, font_y;
void
initialize_font (int *p1, int *p2)
{
  int i, j, k, x, y, n = 0, l, m;
  bmph f1;
  double aux, aux2, pal1[256], pal2[256], max, min;
  FILE *fpf;
  double *fontbody;

  fpf = fopen (FONTFILE, "rb");
  if (fpf == NULL)
    {
      fprintf (stderr, "fopen(fpf)");
      exit (-1);
    }
  fgetc (fpf);
  fgetc (fpf);
  fread (&f1, sizeof (bmph), 1, fpf);
  font_x = f1.W / 32;
  font_y = f1.H / 3;
#if DEBUG
  fprintf (stderr, "font is "FONTFILE", BMPH size is %d\n", (int) sizeof(bmph));
  fprintf (stderr, "Detected FX=%d, FY=%d, bpp=%d\n", font_x, font_y, f1.bpp );
#endif
  if(font_x != FONT_W || font_y != FONT_H)
    fprintf(stderr, "Warning: font chars appears to be %dx%d, see Makefile\n", font_x, font_y);


  fontbody = (double *) malloc (f1.W * f1.H * sizeof (double));
  if (fontbody == NULL)
    {
      fprintf (stderr, "malloc()\n");
      exit (-1);
    }
  k = (f1.IMAGE_SIZE - f1.W * f1.H * f1.bpp / 8) / f1.H;
  if (f1.bpp != 24)

    {
      fprintf (stderr, "fnt bits per pixel != 24\n");
      exit (-1);
    }

  for (y = f1.H - 1; y >= 0; y--)

    {
      for (x = 0; x < f1.W; x++)
	/*fontbody[x + y * f1.W] =
	  (11 * gamma (fgetc (fpf), GAMMA) + 59 * gamma (fgetc (fpf), GAMMA) +
	   30 * gamma (fgetc (fpf), GAMMA)) / 100;*/
	fontbody[x+y*f1.W] = gamma( ( 11 * fgetc(fpf) + 59 * fgetc(fpf) + 30 * fgetc(fpf) ) / 100  , GAMMA);
      for (i = 0; i < k; i++)
	fgetc (fpf);
    }

  fclose (fpf);

  for (i = 0; i < 256; i++)
    {
#ifndef PROIBIDOS
      if (isprint (i))
#else
      if (isprint (i) && !strchr(PROIBIDOS, i))
#endif
	{
#ifdef DEBUG
/*	  fprintf (stderr, "\nMAPA de [%c]\n", isprint (i) ? i : '.');
	  for (j = 0; j < FONT_H; j++, putc (10, stderr))
	    for (k = 0; k < FONT_W; k++)
	      {
		aux = IS_BIT (i, k, j);
		if (aux < 0.25)
		  putc (' ', stderr);
		else if (aux < 0.5)
		  putc ('.', stderr);
		else if (aux < 0.75)
		  putc ('+', stderr);
		else
		  putc ('#', stderr);
	      } */
#endif

	  for (j = 0, aux = 0; j < FONT_H / 2; j++)
	    {
	      for (k = 0; k < FONT_W; k++)
		aux += BLUR_BITS (i, k, j);
	    }
	  for (aux2 = 0; j < FONT_H; j++)
	    {
	      for (k = 0; k < FONT_W; k++)
		aux2 += BLUR_BITS (i, k, j);
	    }
	  pal1[i] = aux;
	  pal2[i] = aux2;
	}
    }
  for (i = 0, max = min = aux; i < 256; i++)
#ifndef PROIBIDOS
      if (isprint (i))
#else
      if (isprint (i) && !strchr(PROIBIDOS, i))
#endif
      {
	max = MAX (MAX (max, pal1[i]), pal2[i]);
	min = MIN (MIN (min, pal1[i]), pal2[i]);
      }
#if DEBUG

fprintf(stderr, "MAX = %f, MIN = %f\n", max, min);

#endif

  for (i = 0, n = 0; i < 256; i++)
    {
#ifndef PROIBIDOS
      if (isprint (i))
#else
      if (isprint (i) && !strchr(PROIBIDOS, i))
#endif
	{
	  for (j = 0; j < n; j++)
	    {
	      if (p1[j] ==
		  (int) (gamma (pal1[i] - min, GAMMA) * 255 /
			 gamma (max - min, GAMMA))
		  && p2[j] ==
		  (int) (gamma (pal2[i] - min, GAMMA) * 255 /
			 gamma (max - min, GAMMA)))
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
	      p1[n] =
		(int) (gamma (pal1[i] - min, GAMMA) * 255 /
		       gamma (max - min, GAMMA));
	      p2[n] =
		(int) (gamma (pal2[i] - min, GAMMA) * 255 /
		       gamma (max - min, GAMMA));
	      n++;
	    }
	}
    }

  free (fontbody);
  ncolors = n;

#ifdef DEBUG
  for (i = 0; i < n; i++)
    {
      fprintf (stderr, "%3d, %3d: \"%s\"\n", p1[i], p2[i], acolors[i]);
    }
#endif

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
      fprintf (stderr, "%s file.bmp [out.txt]\n", argv[0]);
      return -1;
    }
  fpi = fopen (argv[1], "rb");
  fpo = argc > 2 && strcmp ("-", argv[2]) ? fopen (argv[2], "wb") : stdout;
  initialize_font (pal1, pal2);
  if (fpi == NULL || fpo == NULL)

    {
      fprintf (stderr, "fopen(fpi)\n");
      return -1;
    }
#ifndef WIN32
  fpr = fopen ("/dev/urandom", "rb");
  if (fpr == NULL)
    {
      fprintf (stderr, "urandom\n");
      return -1;
    }
#else
  if(argc > 3) srand(atoi(argv[3])); /* initializes PRNG */
#endif
//  fprintf(stdout, "Figura 1. %-72s\n", argv[1]);
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

	  /*PIX (x, y) =
	    (30 * gamma (r, GAMMA) + 59 * gamma (g, GAMMA) +
	     11 * gamma (b, GAMMA)) / 100; */
	  PIX(x, y) = gamma((30 * r + 59 * g + 11 * b) / 100, GAMMA);
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
#ifdef C_SOURCE
      fputc('\"', fpo);
#endif
      for (x = 0; x < h1.W; x++)

	{
	  op = PIX (x, y);
	  op2 = PIX (x, y + 1);
	  j = index_color (x, y, pal1, pal2, image, h1);

#if DITHER
	  np = pal1[j];
	  np2 = pal2[j];
	  err = op - np;
	  err2 = op2 - np2 /*- err * 0.1875*/ ;
	  //if (x + 1 < h1.W)
	  //  {
	      ADJ (x + 1, y, 0.4375, err);
	      ADJ (x + 1, y + 1, 0.4375, err2);
	  //  }
	  //if (x)
	  //  {
	      ADJ (x, y + 1, 0.1875, err);
	      ADJ (x, y + 2, 0.1875, err2);
	  //  }
	  //if (y + 2 < h1.H)
	  //  {
	      ADJ (x, y + 1, 0.3125, err);
	      ADJ (x, y + 2, 0.3125, err2);
	  //  }
	  //if (x + 1 < h1.W && y + 2 < h1.H)
	  //  {
	      ADJ (x + 1, y + 1, 0.2500 /* 0.0625*/, err);
	      ADJ (x + 1, y + 2, 0.0625, err2);
	   // }
#endif /*  */

          k = RANDOM % palopts[j]; /* randomize palete bits */
#if HTML_OUT
	  if (!strchr ("<>&", acolors[j][k]))	/* html special chars */
#endif

#if C_SOURCE /* for use inside printf() */
          if(strchr("\\\"", acolors[j][k])) fputc('\\', fpo);
          if(acolors[j][k] == '%') fputc('%', fpo);
#endif
	    fputc (acolors[j][k], fpo);


#if HTML_OUT
	  else
	    fprintf (fpo, "%s",
		     acolors[j][k] == '>' ? "&gt;" : acolors[j][k] ==
		     '<' ? "&lt;" : "&amp;");
#endif
	  /* if(feof(fpr)) { goto abort; } */
	}
#if HTML_OUT
      fprintf (fpo, "<br>");
#else
# ifdef C_SOURCE
      fprintf(fpo, "\\n\"");
# endif

#  ifdef WIN32
      fprintf (fpo, "\r\n");
#  else
      fprintf (fpo, "\n");
#  endif
#endif
    }
abort:
  fclose (fpi);
  fclose (fpo);
#ifndef WIN32
  fclose (fpr);
#endif
}

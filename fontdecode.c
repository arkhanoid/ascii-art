#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GAMMA 0.45454545
#define GAMMAR 2.2
//#define gamma(x, g) (int) ( powf( (float) (x) / 256, g) * 256 )
#define gamma(x, g) (x)
#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a > b ? a : b)
#define DIST(x, y) ((x-y)*(x-y))	/* utiliza NORMA L2 */
#define PIX(x, y) image[(x) + (y) * h1.W]
#define ADJ(x, y, c) PIX(x, y) = MIN( MAX( PIX(x, y) + c * err, 1), 255)

#define STEP 6

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
main (int argc, char **argv)
{
  bmph h1;
  FILE *fpi, *fpo, *fpt;
  int x, y, i, j, k, r, g, b, aux, c;
  unsigned int *image;
  fpi = fopen ("font.bmp", "rb");
  fpo = fopen ("saida.txt", "wt");
  fpt = fopen ("t.txt", "rb");
  if (fpi == NULL || fpo == NULL || fpt == NULL)

    {
      fprintf (stderr, "fopen()\n");
      return -1;
    }
  fprintf (stderr, "sizeof(bmph) = %d\n", sizeof (bmph));
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


  for (j = 0; j < h1.W; j += STEP)
    {
      c = fgetc (fpt); fprintf(stdout, "\n---> %c\n", c);
      for (k = 0, aux = 0; k < STEP; k++, fputc('\n', stdout))
	for (i = 0; i < h1.H; i++)
	{
	  aux += PIX (j + k, i) < 230 ? 1 : 0;
	  fputc( PIX (j +k, i) < 230 ? 'x' : ' ', stdout);
	}
      fprintf (fpo, "%c : %2d\n", c, aux);
    }

  fclose (fpi);
  fclose (fpo);
  fclose (fpt);
}

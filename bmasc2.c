/*
  este programa eh free software bla bla
  by m4n14c
*/

/*
  todo: eleiminar espacos em branco no final das linhas..
*/

#include <stdio.h>
#include "bmp.h"

#define USEFILE 0		// usa stdout pra saida

/*#define BITS 6
  #define filter (1 << BITS)
  #define mask (filter -1)*/

#define MAXX (argc > 2 ? atoi(argv[2]) : 30)
#define MAXY (argc > 3 ? atoi(argv[3]) : 63)
#define MINX (argc > 4 ? atoi(argv[4]) : 0)
#define MINY (argc > 5 ? atoi(argv[5]) : 0)

#define ENDLINE "\r\n"

/* luma */
#define GRAY(c) ((11 * pal[c][0] + 59 * pal[c][1] + 30 * pal[c][2]) / 100 )
/* luminance */
//#define GRAY(c) ((7 * pal[c][0] + 71 * pal[c][1] + 21 * pal[c][2]) / 100 )
/* averagr */
//#define GRAY(c) ( pal[c][0] + pal[c][1] +  pal[c][2] )

#define compressed bmph.COMPRESSION

int
main (int argc, char **argv)
{
  // const char acolors[] = "RSACsc, ";
  const char acolors[] = "WNEHDPZY652sc;:. ";
  //const char acolors[] = " .:;cs256YWZPDHEN";
  struct bmph bmph;
  unsigned int pal[257][3];
  FILE *fp, *O;
  unsigned int x, y, i, c, p, cold = -1, a1, a2, ctr = 0, pc;
  if (argc < 2)
    {
      fprintf (stderr, "Usage\n%s file.bmp [MAXX] [MAXY] [MINX] [MINY]\n");
      return;
    }
  fp = fopen (argv[1], "rb");
  if (fp == NULL)
    {
      fprintf (stderr, "Erro %s!\n", argv[1]);
      return;
    }

  // fread(&bmph, sizeof(bmph), 1, fp);

  if (fgetc (fp) != 'B' || fgetc (fp) != 'M')
    {
      fprintf (stderr, "File is not a bitmap\n");
      return;
    }

  fread (&bmph, sizeof (bmph), 1, fp);
  if (bmph.BITPERPIXEL != 8)
    {
      fprintf (stderr, "Erro: BITESPER PIXEL = %d\n");
      return;
    }
  fprintf (stderr, "%d\n", bmph.IMAGE_WIDTH);

  fseek (fp, 54, SEEK_SET);
  fprintf (stderr, "OFFS = %ld\n", bmph.offs);
  for (i = 0; i < (bmph.offs - 54) / 4; i++)
    {

      //   printf("#%02x%02x%02x\n", pal[i][0], pal[i][1], pal[i][2]);
      pal[i][2] = fgetc (fp);
      if (pal[i][2])
	pal[i][2]--;
      pal[i][1] = fgetc (fp);
      if (pal[i][1])
	pal[i][1]--;
      pal[i][0] = fgetc (fp);
      if (pal[i][0])
	pal[i][0]--;
      fgetc (fp);
    }
  for (a1 = GRAY (0), a2 = GRAY (0), i = 1; i < (bmph.offs - 54) / 4; i++)
    {
      if (GRAY (i) < a1)
	a1 = GRAY (i);
      if (GRAY (i) > a2)
	a2 = GRAY (i);
    }
  if (a1 == a2)
    {
      fprintf (stderr, "PANIC: all colors have the same gray level!\n");
      return -1;
    }
#if USEFILE
  O = fopen ("o.txt", "wt");
#else
  O = stdout;
#endif

  fseek (fp, bmph.offs, SEEK_SET);
  for (y = 0; y < bmph.IMAGE_HEIGHT; y++)
    {
      for (x = 0; x < (bmph.flen - bmph.offs) / bmph.IMAGE_HEIGHT; x++)
	{
	  if (compressed)
	    {
	      if (ctr > 0)
		{
		  ctr--;
		}
	      else
		{
		  ctr = fgetc (fp);
		  c = fgetc (fp);
		  if (!ctr)
		    {
		      // fprintf(stderr,"(%d, %d)\n", c,x);
		      x = bmph.IMAGE_WIDTH;
		    }
		  else
		    ctr--;
		}
	    }
	  else
	    {
	      c = fgetc (fp);
	    }
	  if (x < bmph.IMAGE_WIDTH)
	    {
	      pc = ((GRAY (c) - a1) * (strlen (acolors) - 1)) / (a2 - a1);
	      if (pc >= strlen (acolors))
		{
		  fprintf (stderr, "PANIC: pc > sizeof(acolors)\n");
		  return -1;
		}
	      if (x < MAXX && y < MAXY && x >= MINX && y >= MINY)
		{
		  //if(pal[c][0] + pal[c][1] + pal[c][2] > 128 * 3) fprintf(O,"\e[5m");
		  fprintf (O, "%c", acolors[pc]);
		  fprintf (O, "%c", acolors[pc]);
		}
	    }
	}
      //  for(i=0;i<6;i++) fgetc(fp);
      if (y < MAXY && y >= MINY)
	fprintf (O, ENDLINE);
    }
  fclose (fp);
  fclose (O);
}

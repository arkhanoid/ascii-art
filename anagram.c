/*
 * Anagram.c: cria um anagrama de todas as palavras, mantendo a posicao das maiúsculas, da primeira e da ultima letra de cada 
 * palavra. Desta maneira, cria-se um filtro para que o texto seja legível apenas para indivíduos com inteligência superior.
 *
 * gcc anagram.c -o anagram
 */


#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#ifndef MAXBUFFER
#define MAXBUFFER 200
#endif
#ifndef MAXWORD
#define MAXWORD 13
#endif

int
main (int argc, char *argv[])
{
  FILE *fp, *fp2, *aux;
  char buffer[MAXBUFFER] /*, indexes[MAXBUFFER] */ ;
  const char letras[] = "aáãâbcçdeéêfghiíjklmnoóõôpqrstuúvwxyz";
  int i, j, k, l, c;

  /* Inicia o gerador de numeros aleatórios */

  aux = fopen ("/dev/urandom", "rb");
  fread (&i, sizeof (int), 1, aux);
  fclose (aux);
  srand (i);

  /* le o arquivo de entrada, se nao le da entrada padrão */

  if (argc == 1)
    {
      fprintf (stderr, "<< stdin\n");
      fp = stdin;
    }
  else
    {
      fprintf (stderr, "<< %s...\n", argv[1]);
      fp = fopen (argv[1], "rt");
      if (fp == NULL)
	{
	  fprintf (stderr, "Cannot open %s\n", argv[1]);
	  return -1;
	}
    }

  /* le o arquivo de saída, se nao escreve na saída padrão */

  if (argc > 2)
    {
      fprintf (stderr, ">> %s...\n", argv[2]);
      fp2 = fopen (argv[2], "wt");
      if (fp2 == NULL)
	{
	  fprintf (stderr, "Cannot open %s\n", argv[2]);
	  return -1;
	}
    }
  else
    {
      fprintf (stderr, ">> stdout\n");
      fp2 = stdout;
    }
//  printf ("Abertura do arquivo\n");
  for (i = 0;;)
    {
      c = fgetc (fp);
      if (feof (fp))
	break;
/* if (!isalpha (c))  */
      if (!strchr (letras,  c))
	{
	  if (i)
	    {
	      //   fputc ('[', fp2);
	      if (i < MAXWORD)
		for (j = 1; j < i - 1; j++)
		  {
		    k = rand () % (i - 2) + 1;
		    l = buffer[k];
		    buffer[k] = buffer[j];
		    buffer[j] = l;
		  }

	      for (j = 0; j < i; j++)
		fputc (buffer[j], fp2);
	      // fputc (']', fp2);
	      i = 0;
	    }
	  fputc (c, fp2);
	  // continue;
	}
      else if (i < MAXBUFFER)
	{
#ifdef L337
	  switch (c)
	    {
	    case 'a':
	      c = '4';
	      break;
	    case 'e':
	      c = '3';
	      break;
	    case 'i':
	      c = '1';
	      break;
	    case 'o':
	      c = '0';
	      break;
	    case 't':
	      c = '7';
	      break;
	    case 'g':
	      c = '9';
	      break;
	    case 'b':
	      c = '8';
	      break;
	    }
#endif
	  buffer[i++] = c;
	}
    }
  if (i)
    {
      if (i < MAXWORD)
	for (j = 1; j < i - 1; j++)
	  {
	    k = rand () % (i - 2) + 1;
	    l = buffer[k];
	    buffer[k] = buffer[j];
	    buffer[j] = l;
	  }
      for (j = 0; j < i; j++)
	fputc (buffer[j], fp2);
    }
  fclose (fp);
  fclose (fp2);
}

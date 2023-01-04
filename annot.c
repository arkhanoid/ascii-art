#include <stdio.h>
#include <strings.h>
#define MLINE 400
#define PADD "  "
#define testchr(x) ( x != '\r' && x != '\n')
int main(int argc, char ** argv)
{
  int i, j, k, l, x, y;
  char buffer[MLINE], buffer2[MLINE];
  FILE *fpi, *fpt, *fpo;

  if(argc < 3)
  {
   printf("%s image text x y [out]\n", argv[0]);
   return (-1);
  } 
  fpt = fopen(argv[2], "rt");
  if(fpt == NULL) {
   printf("fopen %s (text) falhou\n", argv[2]);
   return (-1);
  }
  fpi = strcmp(argv[1], "-") ? fopen(argv[1], "rt") : stdin;
  if(fpi == NULL) {
   printf("fopen %s (image) falhou\n", argv[1]);
   return (-1);
  }
  x = (argc >= 3? atoi(argv[3]) : 0);
  y = (argc >= 4? atoi(argv[4]) : 0);
  if(argc >= 6) 
  {
   fpo = fopen(argv[5], "wt");
   if(fpo == NULL) {
    printf("fopen %s (out) falhou\n", argv[5]);
    return (-1);
   }
  }
  else fpo = stdout;
  /* x = 8; y = 10; */ 
  
  /* printf("x = %d, y = %d\n",  x, y); */
  
  for(j=0; fgets(buffer, MLINE, fpi); j++)
  {
    *buffer2 = '\0';
    if(j >= y && !feof(fpt)) {
      strcpy(buffer2, PADD);  
      fgets(buffer2+2, MLINE, fpt);
      fprintf(stderr, "%s", buffer2);
      l = strlen(buffer2) - 1;
      strcpy(buffer2+l, PADD);
      l += strlen(PADD);
    } else l = 0;
     
//    printf("%04d: ", l);

    for(i=0; buffer[i]; i++)
    {
      fprintf(fpo, "%c", 
         (j>= y && i >= x && i < x + l && testchr(buffer2[i-x])) ?
         buffer2[i-x] : buffer[i]
      );
    }
//    printf("\n");
  }

  fclose(fpi); fclose(fpt); if(fpo!=stdout) fclose(fpo);

  return 0;

}

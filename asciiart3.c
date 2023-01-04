/*
  Implementa��o do algoritmo de Floyd Steinberg para dithering
  Utiliza paleta e gamma para converter um bitmap em ascii art
*/  
  
#include <stdio.h>
#include <stdlib.h>
  
#define DITHER 1 /* Enable dithering? */
#define SHARPEN 0 /* Enable sharpening? */
  
#define KS 15 /* kernel = KS * I - BLUR */
#include <math.h>
  
#define GAMMA 0.454545
#define gamma(x) (int) ( powf( (float) (x) / 256, GAMMA) * 256 )
  
#define MIN(a, b) (a > b ? b : a)
#define MAX(a, b) (a > b ? a : b)
#define DIST(x, y) ((x-y)*(x-y)) /* utiliza NORMA L2 */
//#define DIST(x, y) ((x-y)>0?x-y:y-x) /* utiliza NORMA L1 */
#define PIX(x, y) image[(x) + (y) * h1.W]
#define ADJ(x, y, c) PIX(x, y) = MIN( MAX( PIX(x, y) + c * err, 1), 255)
  
//const char acolors[] = " .-:li3OdW";
const char acolors[] = "#HI+;:-. ";

//const char acolors[] = "WI. ";              
  
#define ncolors (sizeof(acolors) - 1)
  
{
  
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

index (int c, int *pal) 
{
  
  
    
    {
      
	
	{
	  
	  
	
    
  



main (int argc, char **argv) 
{
  
  
  
  
  
  
  
// unsigned int pal[ncolors] = { 255, 183, 177, 156, 142, 149, 124, 88, 80 }; /* terminal screen */
    
    /*
       Como foram calculados os valores da paleta:
       
       paleta = #HI+;:-.
       
       { 20,16,11,10,7,6,4,3,0 } = Cheios
       
       12 x 6 = tamanho dos caracteres.
       
       69,2% aspect ratio.
       
       117 block size
       
       { 23.9 17.1 13.7 11.1 8.5 6.8 5.1 3.4 0 } = % preenchimento
       
       { 52.2 44.8 40.5 36.8 32.7 29.5 25.9 21.6 0 } = preenchimento aparente (gamma = 1/2.2)
       
       { 255 219 198 180 160 144 127 105 0 } = range 0-255.
     */ 
    
    
    {
      
      
    
  
  
  
    
    {
      
      
    
  
  
  fgetc (fpi);
  
  
  
    
    {
      
      
    
  
  
    
    {
      
      
    
  
    
    {
      
	
	{
	  
	  
	  
	  
//     PIX(x, y) =  gamma((30 * r + 59 * g + 11 * b) / 100, GAMMA);
	    PIX (x, y) =
	    (30 * gamma (r) + 59 * gamma (g) + 11 * gamma (b)) / 100;
	
      
	fgetc (fpi);
    
  
    
    {
      
//  pal[i] = j; ///gamma(j, 0.4545) ;
	fprintf (stderr, "pal[%d] = %d\n", i, pal[i]);
    
  
    /* sharpen filter */ 
    
#if SHARPEN
    
#define PIX2(x,y) (x + 1 < h1.W && y + 1 < h1.H ? MIN( MAX( (KS * PIX(x,y) - PIX(x+1,y) - PIX(x, y+1) - PIX(x+1, y+1) ) / (KS - 3), 1), 255): PIX(x,y) )
    
    
      
  
#endif	/* 
    
    
    {
      
	
	{
	  
	  
	  
#if DITHER
	    np = pal[j];
	  
	  
	    ADJ (x + 1, y, 0.4375);
	  
	    ADJ (x, y + 1, 0.1875);
	  
	    ADJ (x, y + 1, 0.3125);
	  
	    ADJ (x + 1, y + 1, 0.0625);
	  
#endif	/* 
	    fputc (acolors[j], fpo);
	
      
    
  
  fclose (fpo);



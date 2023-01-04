/* jsmin.c
   2011-01-22

Copyright (c) 2002 Douglas Crockford  (www.crockford.com)

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

The Software shall be used for Good, not Evil.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/  
  
#include <stdlib.h>
#include <stdio.h>




/* isAlphanum -- return true if the character is a letter, digit, underscore,
        dollar sign, or non-ASCII character.
*/ 

isAlphanum (int c) 
{
  
	   (c >= 'A' && c <= 'Z') || c == '_' || c == '$' || c == '\\'
	   || 



/* get -- return the next character from stdin. Watch out for lookahead. If
        the character is a control character, translate it to a space or
        linefeed.
*/ 

get () 
{
  
  
  
    {
      
    
  
    {
      
    
  
    {
      
    
  



/* peek -- get the next character without getting it.
*/ 

peek () 
{
  
  



/* next -- get the next character, excluding comments. peek() is used to see
        if a '/' is followed by a '/' or '*'.
*/ 

next () 
{
  
  
    {
      
	{
	
	  
	    {
	      
	      
		{
		  
		
	    
	
	  
	  
	    {
	      
		{
		
		  
		    {
		      
		      
		    
		  
		
		  
		  
		
	    
	
	  
	
    
  



/* action -- do something! What you do is determined by the argument:
        1   Output A. Copy B to A. Get the next B.
        2   Copy B to A. Get the next B. (Delete A).
        3   Get the next B. (Delete B).
   action treats a string as a single character. Wow!
   action recognizes a regular expression if it is preceded by ( or , or =.
*/ 

action (int d) 
{
  
    {
    
      
	putc (' ', stdout);
      else
	putc (theA, stdout);
    
      
      
	{
	  
	    {
	      
	      
	      
		{
		  
		
	      
		{
		  
		  
		
	      
		{
		  
			    "Error: JSMIN unterminated string literal.");
		  
		
	    
	
    
      
      
	   && (theA == '(' || theA == ',' || theA == '=' || 
	       || theA == '[' || theA == '!' || 
	       || theA == '?' || 
	       || 
	{
	  
	    //putc(theA, stdout);
	    putc (theB, stdout);
	  
	    {
	      
	      
		{
		  
		    {
		      
		      
		      
			{
			  
			
		      
			{
			  
			  
			
		      
			{
			  
				    
			  
			
		    
		
	      else if (theA == '/')
		{
		  
		
	      else if (theA == '\\')
		{
		  
		  
		
	      
		{
		  
			    
		  
		
	      
	    
	  
	
    



/* jsmin -- Copy the input to the output, deleting the characters which are
        insignificant to JavaScript. Comments will be removed. Tabs will be
        replaced with spaces. Carriage returns will be replaced with linefeeds.
        Most spaces and linefeeds will be removed.
*/ 

jsmin () 
{
  
  
  
    {
      
	{
	
	  
	    {
	      
	    
	  else
	    {
	      
	    
	  
	
	  
	    {
	    
	    
	    
	    
	    
	      
	      
	    
	      
	      
	    
	      
		{
		  
		
	      else
		{
		  
		
	    
	  
	
	  
	    {
	    
	      
		{
		  
		  
		
	      
	      
	    
	      
		{
		
		
		
		
		
		
		
		  
		  
		
		  
		    {
		      
		    
		  else
		    {
		      
		    
		
	      
	    
	      
	      
	    
	
    



/* main -- Output any command line arguments as comments
        and then minify the input.
*/ 
extern int 
main (int argc, char *argv[]) 
{
  
  
    {
      
    
  
  



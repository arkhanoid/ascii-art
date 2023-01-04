/* JUSTIFY -- Formats ASCII text.
   Copyright 1994, Tom Almy. All rights reserved.
   May be copied for non-commercial user only.

   Version 1.5, Revised 1997, improves end of sentence checking, adds 'e'
   flag for EMAIL (doesn't remformat lines starting with > or leading lines
   starting with header labels), and 'q' flag to add > to output lines (for
   quoting EMAIL).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#define LINEMAX (8192)
#define WORDMAX (256)

#define INDENTED(string) (*string == ' ')
#define BLANK(string) (*string == '\n')
#define ZEROLEN(string) (*string == '\0')
#define TEXT(string) (*string > ' ')	/* A cheat */
#define min(a,b) ((a)<(b) ? (a) : (b))

// extern unsigned _stklen = 12000;

int columns, indent, body;	/* command line arguments */
int dspace, dblank, iblank, hindent; 	/* command line flags */
int indpar, indbod, fullpar, nohyph, rright, mdash, wproc, olp;
int email, quote;

char line[LINEMAX];	/* Line being composed */
char hword[WORDMAX]; /* word in hyphenation */
int firstline;	/* composing first line */

void usage(void)
{
	fprintf(stderr, "\nUSAGE:\njustify columns [bflditohsrweq] [indent] [body] <source >dest\n"
					"  b - input file paragraph is hanging indented\n"
					"  f - input file paragraph is fully indented\n"
					"  l - input file paragraphs are single lines\n"
			        "  d - delete blank line after paragraph read\n"
			        "  i - insert blank line after paragraph read\n"
			        "  t - indent first paragraph line by indent spaces\n"
				    "  o - indent other paragraph lines by body spaces\n"
					"  h - remove hyphens across line boundaries\n"
				    "  s - double space after . ? ! .\" ?\" or !\"\n"
					"  m - process m-dash adjacent to words\n"
					"  w - output for word processors\n"
			        "  r - ragged right margin (otherwise full justification)\n"
					"  e - EMAIL input -- don't format quotes or headers\n"
					"  q - EMAIL output -- add '>' to non-blank lines\n"
		   );
	exit(4);
}

int trim(char *s) {  /* trim spaces from end of line */
	char *end = s + strlen(s) - 2;

	while (s <= end && *end == ' ') end--;

	*++end = '\n';
	*++end = '\0';

	return 1;	/* return success always */
}

			
void fill(char *s, int size) {
	static int fromleft = 0;		/* alternate sides */
	char *cp;
	int blanks = size - strlen(s);

	fromleft++;

	if (strchr(s, ' ') == NULL) return;	/* cant justify single word line */

	if (fromleft & 1) {	/* insert from left */
		while (blanks) {
			cp = s;	/* start at left edge */
			while (blanks) {
				while (*cp != ' ' && !ZEROLEN(cp)) cp++;	/* find a blank */
				if (ZEROLEN(cp)) break;	/* need another pass */
				memmove(cp+1, cp, strlen(cp)+1);	/* insert a space */
				while (*++cp == ' ');	/* skip past spaces */
				blanks--;
			}
		}
	}
	else { /* insert from right */
		while (blanks) {
			cp = s + strlen(s) - 1;	/* start at right edge */
			while (blanks) {
				while (*cp != ' ' && cp >= s) cp--; /* find a blank */
				if (cp < s) break;	/* need another pass */
				memmove(cp+1, cp, strlen(cp)+1);  /* insert a space */
				while (*--cp == ' ');	/* skip past spaces */
				blanks--;
			}
		}
	}
}


int splitable(char *s, int len) {
	/* see if line can be split (because of mdash) within len characters */
	/* If so, return length of first part, else return 0 */
	char *cp = s;
	int pos = 0;

	while ((cp = strstr(cp, "--")) != NULL && cp-s <= len) {
		pos = cp - s;
		cp++;
	}

	return pos;
}

void emitword(char *s) {
	int wlen = strlen(s);	/* length of this word */
	int llen = firstline ? columns - indent : columns - body;	/*length of this line*/
	int clen = strlen(line);	/* length of line so far */
	int dbl = 0;

	if (!ZEROLEN(hword))	{ /* combine hyphenated word */
		if (!mdash || hword[strlen(hword)-2] != '-') /* hyphen is not m-dash */
			hword[strlen(hword)-1] = '\0';	/* remove hyphen */
		strcat(hword, s);				/* concatenate pieces */
		strcpy(s, hword);
		wlen = strlen(s);
		hword[0] = '\0';
	}

	if (clen==0)	{ /* first word in line */
		strcpy(line,s);
		return;
	}
	if (dspace && ( /* End of sentence */
				   (strchr(".?!", line[clen-1]) != NULL &&
					clen > 2 &&
					isalpha(line[clen-2]) &&
					(isalpha(line[clen-3]) || (line[clen-1] != '.')))
				   ||
				   /* End of quote or parenthetic expression */
				   (strchr("\")", line[clen-1]) != NULL &&
					clen > 3 &&
					(strchr(".?!", line[clen-2]) != NULL ||
					 strchr(".?!", line[clen-3]) != NULL )))) {
		/* guess we are at end of sentence */
		dbl = 1;
	}
	if (wproc)	{ /* special case Word Processor output */
		printf("%s%s", line, dbl ? "  " : " ");	/* print previous word */
		strcpy(line, s);
		return;
	}
	if (wlen+clen+dbl+1 > llen) { /* Word won't fit */
		int	i = firstline ? indent : body;	/* line indentation */
		int j;
		if (mdash && (j = splitable(s, llen-clen-dbl-3)) != 0) {
			/* can split word */
			strcat(line, " ");
			if (dbl) strcat(line, " ");	/* space after sentence */
			strncat(line, s, j+2);
			strcpy(s, s+j+2);
		}
		if (quote) {
			/* Insert quoting */
			putchar('>');
		}
		while (i--) putchar(' ');
		if (!rright) fill(line, llen);	/* full justificaton -- fill to llen*/
		puts(line);	/* write the current line */
		firstline = 0;
		strcpy(line, s);
		return;
	}
	if (dbl) strcat(line, " ");	/* space after sentence */
	strcat(line, " ");	/* space after last word */
	strcat(line, s);
}

void morepar(char *s) {
	/* compose an input line for a paragraph */
	int i;
	char word[WORDMAX], dummy[WORDMAX];
	while (sscanf(s, "%s%n", word, &i) >= 1) {
		s += i;
		if ((nohyph && word[strlen(word)-1] == '-' &&
			strlen(word) > 1 &&
			word[strlen(word)-2] != '-' &&
			sscanf(s, "%s", dummy) <= 0)
			|| (mdash && strlen(word) > 2 &&
				word[strlen(word)-1] == '-' &&
				word[strlen(word)-2] == '-' )) {
			/* line ends with hyphenated word */
			strcpy(hword, word);
		}
		else
			emitword(word);
	}
}

void endpar(void) {
	/* write out remainder of paragraph */
	if (!ZEROLEN(hword)) {	/* have dangling hyphenated word */
		nohyph = 0;
		morepar(hword);
		nohyph = 1;
		hword[0] = '\0';
	}
	if (wproc) { /* special case for Word Processing output */
		/* quote adding won't work in this case */
		puts(line);
		return;
	}
	if (!ZEROLEN(line)) {
		int i = firstline ? indent : body;
		if (quote) putchar('>');
		while (i--) putchar(' ');
		puts(line);
	}
}

void startpar(char *s) {
	/* compose first line of paragraph */
	firstline = 1;
	line[0] = 0;
	morepar(s);
}

void main(int argc, char **argv) {
	static char lbuf[LINEMAX];			/* current line */
	static char nbuf[LINEMAX];			/* next line */
	char *eofchk;
	char *p1, *p2;

	fprintf(stderr, "JUSTIFY V1.5 -- formats text. Copyright 1994 by Tom Almy.\n"
			        "May be freely used and distributed as long as no charge\n"
			        "is made beyond distribution costs and program is not modified.\n");

	if (argc < 2) usage();
	else {
		columns = abs(atoi(argv[1]));
		if (columns==0) usage();
		if (argc > 2) {
			char *cp = argv[2];
			while (*cp) switch (*cp++) {
			case 'b': hindent = 1; break;
			case 'f': fullpar = 1; hindent = 1; break;
			case 'l': olp = 1; break;
			case 'd': dblank = 1; break;
			case 'i': iblank = 1; break;
			case 't': indpar = 1; break;
			case 'o': indbod = 1; break;
			case 'h': nohyph = 1; break;
			case 'r': rright = 1; break;
			case 'm': mdash = 1; break;
			case 's': dspace = 1; break;
			case 'w': wproc = 1; break;
			case 'e': email = 1; break;
			case 'q': quote = 1; break;
			default: usage();
			}
			if (argc !=  indpar + indbod  + 3) usage();
			if (indpar && (indent = abs(atoi(argv[3]))) == 0)
				usage();
			if (indbod && (body = abs(atoi(argv[indpar? 4 : 3]))) == 0)
				usage();
		}
	}

	if (quote) {
		columns--;
		if (columns < 1) usage();
	}
	if (wproc) quote = 0;
	if (fgets(lbuf, LINEMAX, stdin) == NULL) return;
	trim(lbuf);

	while (fgets(nbuf, LINEMAX, stdin) != NULL) { /* figure next action */
		trim(nbuf);
		if (BLANK(lbuf)) {	/* current is blank */
			strcpy(lbuf, nbuf);	/* copy next into current */
			fputs("\n", stdout);	/* emit a blank line */
			continue;
		}
		/* Test for lines that are not part of paragraphs */
		if ((email && (lbuf[0]=='>' ||
					   ((p1=strchr(lbuf, ' ')) != NULL &&
						(p2=strchr(lbuf, ':')) != NULL &&
						(p1 > p2)))) ||
			/* Email header or quoted lines */
			(fullpar && (!INDENTED(lbuf) || BLANK(nbuf))) ||
			/* Full indented body text, line is not indented or next line blank */
			(!fullpar && hindent && (INDENTED(lbuf) || !INDENTED(nbuf))) ||
			 /* Hanging indent and line is is indented or next line is not
			   indented (or is blank) */
			(!hindent &&
			 (INDENTED(nbuf) || (BLANK(nbuf) && strlen(lbuf)-1 <= columns)))) {
			/* next is blank and current line fits in margins, or next is indented */

			if (quote) {
				int i = min(body,indent);
				putchar('>');
				if (lbuf[0] != '>') 
					while (i--) putchar(' ');
			}
			fputs(lbuf, stdout);	/* print line */
			strcpy(lbuf, nbuf);		/* next becomes current */
			continue;
		}
		if (olp) {
			/* all paragraphs are a single line */
			startpar(lbuf);
			endpar();
		}
		else {
			/* We have a paragraph to format */
			startpar(lbuf);
			eofchk = (char *)1; /* Not NULL */
			if (!email || nbuf[0] != '>') do {
				morepar(nbuf);
			} while ((eofchk = fgets(nbuf,LINEMAX,stdin)) != NULL &&
					 trim(nbuf) &&
					 (!email || nbuf[0] != '>') &&
					 ((!INDENTED(nbuf)) ^ hindent) && !BLANK(nbuf));
			endpar();
			if (eofchk == NULL) return;	/* finished at end of paragraph */
		}
		if (iblank) fputs("\n",stdout);	/* optional insert of blank line */
		strcpy(lbuf, nbuf);	/* next becomes current */
		if (dblank && BLANK(lbuf)) { /* optional delete of blank line */
			if (fgets(lbuf, LINEMAX, stdin) == NULL) return; /* End of file */
			trim(lbuf);
		}
	}
	/* file ends with blank line or single line */
	if (olp) {
		startpar(lbuf);
		endpar();
	}
	else  {
		if (quote && (strlen(lbuf) > 0))
			putchar('>');
		fputs(lbuf, stdout);
	}
}

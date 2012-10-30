#include	<stdio.h>
#include	<stdlib.h>

/*
 * untable -- simple version to convert html table data to tsv
 *
 *   input: any simple html
 *  output: one row per table row
 *    note: does not handle tags with attributes right now
 *
 *    idea: read char by char looking for tags.
 *	        when a tag is gotten, then figure out what to do with
 *		    it.
 *    form: a finite state machine with  a few states,
 *		    IN_TAG, COPY, SKIP
 */

#define	IN_TAG	0
#define	COPY	1
#define	SKIP	2
#define	MAXTAG	100

void bail(char *);
int  process_tag( char * );

int main()
{
	int	state = SKIP; 
	int	c;
	char	tag[MAXTAG];
	int	tag_pos;

	while( ( c = getchar() ) != EOF )
	{
		if ( c == '<' ){
			if ( state == SKIP || state == COPY ){
				tag_pos = 0;
				state = IN_TAG;
			}
		}
		else if ( c == '>' ){
			if ( state != IN_TAG ){
				bail("Syntax error.  Unexpected >");
			}
			tag[tag_pos] = '\0';
			state = process_tag( tag );
		}
		else {
			if ( state == IN_TAG ){
				if ( tag_pos < MAXTAG ){
					tag[tag_pos++] = c;
				}
				else 
					bail("Tag is too long");
			}
			if ( state == COPY ){
				putchar(c);
			}
		}
	}
	if ( state != SKIP ){
		bail("end of file unexpected");
	}
	return 0;
}

/*
 * process_tag -- handle the tag
 *   if a TR, then set mode to want_TD which we have not defined yet
 *   if a TD, then change mode to copy, but precede it with a tab
 *   if a </TR> then output a \n
 *   if a </td> then mode goes to skip 
 */
int process_tag(char *tag)
{
	int		new_state = SKIP;
	static int 	td_num = 0;

	if ( strncasecmp(tag, "table",5 ) == 0 ){
		printf("===== new table ====\n");
	}
	else if ( strncasecmp(tag, "/table",6 ) == 0 ){
		printf("===== end of table ====\n");
	}
	else if ( strncasecmp(tag, "tr",2 ) == 0 ){
		td_num = 0;
	}
	else if  ( strncasecmp(tag, "/tr", 3) == 0 ){
		putchar('\n');
	}
	else if  ( strncasecmp(tag, "td", 2) == 0 ){
		new_state = COPY;
		if ( td_num > 0 )
		    putchar('\t');
		td_num++;
	}
	else if  ( strncasecmp(tag, "/td", 3) == 0 ){
		new_state = SKIP;
		putchar('\t');
	}
	return new_state;
}

void bail(char *str)
{
	fprintf(stderr, "error: %s\n", str);
	exit(1);
}

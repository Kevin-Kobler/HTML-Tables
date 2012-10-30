#include	<stdio.h>
#include	<stdlib.h>
#include	<string.h>

/*
 * untable -- simple version to convert html table data to tsv
 *
 *   input: any simple html
 *  output: one row per table row
 *    note: does not handle tags with attributes right now
 *
 *    form: a finite state machine with  a few states,
 *		    IN_TAG, COPY, SKIP
 */

#define	COPY	1
#define	SKIP	2
#define TAG     3
#define END	4
#define CONT	5
#define	MAXTAG	100
#define BUFF_SIZE 20
#define NUM_TAGS 6
#define MAX_CONTENT 100

struct text_block {
		int state;	/* tag, or textblob */
		char *content;
};

int get_content(struct text_block *);
char *get_block();
char *get_tag();
char *read_until(char);
void bail(char *);
int  process_tag( char * );
int adv_to_next_tag();

int main()
{
	int	c;
	struct text_block input;

	input.state = SKIP;

	while( get_content( &input ) != END )
	{
		if ( input.state == TAG ){
			input.state = process_tag(input.content);
		} else if ( input.state == COPY ){
			printf("%s", input.content);
			input.state = SKIP;
		}
		free(input.content);
	}
	return 0;
}

int get_content(struct text_block *input)
/* Function gets a dynamically allocated block of text and
 * uses the find_state function to get assign that block a type
 * 
 * Param: a pointer to the struct text block
 * Return: END if EOF is reached, otherwise CONT
 */
{
	char *s;

	if (input->state == SKIP) {
		if (adv_to_next_tag() == END) {
			return END;
		} else {
			input->state = TAG;
		}
	}
	if (input->state == COPY) {
		s = get_block();
		input->content = (char *) malloc(strlen(s) + 1);
		strcpy(input->content, s);
		return CONT;
	} else if (input->state == TAG) {
		s = get_tag();
		input->content = (char *) malloc(strlen(s) + 1);
		strcpy(input->content, s);
		return CONT;
	}
	return END;
}

char *get_block() 
/* this function gets a block of text from stdin
 * it picks up text until a '<': the signal for a new tag
 *
 * Returns: a pointer to a dynamically allocated block of text
 */
{
	char *s;

	s = read_until('<');
	return s;
}

char *get_tag() 
/* function gets input until '>': end of html tag
 *
 * Returns: block of text
 */
{
	char *s;
	
	s = read_until('>');
	return s;
}


char *read_until(char stop)
/* reads until the stop character is reached
 * returns a pointer to an allocated string
 */
{
	int c;
	char buff[BUFF_SIZE];
	char *s;
	int i = 0;

	s = (char *) malloc (1);
	strcpy(s, "");
	
	while ((c = getchar()) != stop) {
		if (c == EOF) {
			bail("Unexpected End of file");
		}
		buff[i] = c;
		i++;
		if (i == BUFF_SIZE - 1) {
			buff[i] = '\0';
			s = (char *) realloc (s, strlen(s) + BUFF_SIZE);
			strcat(s, buff);
			i = 0;
		}
	}
	buff[i] = '\0';
	s = (char *) realloc (s, strlen(s) + strlen(buff) + 1);
	strcat(s, buff);
	i = strlen(s);
	s[i] = '\0';
	return s;
}

int adv_to_next_tag()
/* this function simply andvances input to the next HTML tag or EOF */
{
	int c;
	while ((c = getchar()) != '<') {
		if (c == EOF) {
			return END;
		}
	}
	return CONT;
}
		
struct tag_handler {
/*
 * lookup table that matches tags with function
 */
			char *tag;
			int (*func)(int*);
	};


int new_table()
{
	printf("===== NEW TABLE ====\n");
	return SKIP;
}
int table_end()
{
	printf("===== END OF TABLE ====\n");
	return SKIP;
}

int new_row(int *td_num)
{
	*td_num = 0;
	return SKIP;
}

int end_row()
{
	putchar('\n');
	return SKIP;
}

int new_data_element(int *td_num)
{
	if ( *td_num > 0 )
		putchar('\t');
	(*td_num)++;
	return COPY;
}

int end_data_element()
{
	putchar('\t');
	return SKIP;
}

struct tag_handler handlers[NUM_TAGS] = {
					{ "table",   new_table },
	  				{ "/table",  table_end },
	  				{ "td",      new_data_element },
					{ "/td",     end_data_element },
					{ "tr",	     new_row },
					{ "/tr",     end_row },
	};
/*
 * process_tag -- handle the tag
 *   if a TR, then set mode to want_TD which we have not defined yet
 *   if a TD, then change mode to copy, but precede it with a tab
 *   if a </TR> then output a \n
 *   if a </td> then mode goes to skip 
 */

int process_tag(char *tag)
{
	static int td_num;
	int i;

	for (i = 0; i < NUM_TAGS; i++)
		if(strncasecmp(tag, handlers[i].tag,
				       	strlen(handlers[i].tag)) == 0) {
			return handlers[i].func(&td_num); /*returns new state*/
		}
	return SKIP;
}

void bail(char *str)
{
	fprintf(stderr, "error: %s\n", str);
	exit(1);
}

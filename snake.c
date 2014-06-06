#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:	the pointer to the member.
 * @type:	the type of the container struct this is embedded in.
 * @member:	the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({			\
	const typeof( ((type *)0)->member ) *__mptr = (ptr);	\
	(type *)( (char *)__mptr - offsetof(type,member) );})


#define LEN 20
#define WIDE 40
#define EDGE '8'
#define SNAKE_COL 31
#define EDGE_COL 34
#define SNAKE_HEAD '0'
#define SNAKE_BODY 'o'

struct node {
	struct node *next;
};

struct snake {
	struct node head;
	int x, y;
};

struct snake *shead = NULL;
int runing = 0;

int init_snake(struct snake **s)
{
	*s = (struct snake *)malloc(sizeof(struct snake));
	if(*s == NULL) {
		return -1;
	}
	
	(*s)->head.next = NULL;
	(*s)->x = 10;
	(*s)->y = 20;
	
	runing = 1;
	
	return 0;
}

int add_snake(struct snake *shead, int x, int y) 
{
	struct snake *s;
	
	s = (struct snake *)malloc(sizeof(struct snake));
	if (s == NULL) {
		return -1;
	}
	
	s->head.next = shead->head.next;
	shead->head.next = &(s->head);
	
	s->x = shead->x;
	s->y = shead->y;
	shead->x = x;
	shead->y = y;
	
	return 0;
}

void del_tail(struct snake *s)
{
	if (s->head.next == NULL) {
		free(s);
		s = NULL;
	} else {
		del_tail(container_of(s->head.next, struct snake, head));
	}
}

void del_snake(struct snake *shead)
{
	if (shead->head.next != NULL) {
		del_snake(container_of(shead->head.next, struct snake, head));
	}
	
	shead->head.next = NULL;
	free(shead);
	shead = NULL;
}

void show_snake(struct snake *s)
{
	if (s == NULL) {
		return;
	}
	
	printf("\33[%d;%dH\33[%dm%c\33[0m", s->x+1, s->y+1, SNAKE_COL, SNAKE_HEAD);
	while (s->head.next != NULL) {
		s = container_of(s->head.next, struct snake, head);
		printf("\33[%d;%dH\33[%dm%c\33[0m", s->x+1, s->y+1, SNAKE_COL, SNAKE_BODY);
	}
	
	printf("\33[23;1H");
	fflush(stdout);
}

enum action {
	up = 0,
	down,
	right,
	left
};

enum action last_ac = 0;

void exchange(int *a, int *b)
{
	int temp;
	temp = *a;
	*a = *b;
	*b = temp;
}

void move_snake(struct snake *s, enum action ac)
{
	if (s == NULL) {
		return;
	}
	
	int x, y;
	
	x = s->x;
	y = s->y;
	struct snake *stmp = s;
	
	switch (ac) {
		case up:
			s->x--;
			break;
		case down:
			s->x++;
			break;
		case right:
			s->y++;
			break;
		case left:
			s->y--;
			break;
		default:
			;
	}
	
	while (s->head.next != NULL) {
		s = container_of(s->head.next, struct snake, head);
		exchange(&(s->x), &x);
		exchange(&(s->y), &y);
	}
	printf("\33[%d;%dH\33[%dm%c\33[0m", x+1, y+1, SNAKE_COL, ' ');
	show_snake(stmp);
}

int look_ahead(void)
{
	int ahead_x = shead->x, ahead_y = shead->y;
	switch (last_ac)
	{
		case up:
			ahead_x--;
			break;
		case down:
			ahead_x++;
			break;
		case right:
			ahead_y++;
			break;
		case left:
			ahead_y--;
			break;
		default:
			;
	}
	
	if (ahead_x == 0 || ahead_x == 21 || 
		ahead_y == 0 || ahead_y == 41)
	{
		return -1;
	}
	
	return 0;
}

void move(int signo)
{
	if (look_ahead() < 0)
	{
		runing = 0;
		return;
	}
	
	move_snake(shead, last_ac);
}

void init_edge(void)
{
	int i;
	
	printf("\33[?25l\33[2J");
	
	for(i=1; i<=LEN+2; i++) {
		printf("\33[%d;1H\33[%dm%c\33[0m", i, EDGE_COL, EDGE);
	}
	for(i=1; i<=LEN+2; i++) {
		printf("\33[%d;%dH\33[%dm%c\33[0m", i, WIDE+2, EDGE_COL, EDGE);
	}
	for(i=1; i<=WIDE; i++) {
		printf("\33[1;%dH\33[%dm%c\33[0m", i+1, EDGE_COL, EDGE);
	}
	for(i=1; i<=WIDE; i++) {
		printf("\33[%d;%dH\33[%dm%c\33[0m", LEN+2, i+1, EDGE_COL, EDGE);
	}
	
	printf("\33[23;1H");
	fflush(stdout);
}

int main()
{
	init_edge();
	
	struct snake *s = NULL;
	init_snake(&s);
	
	shead = s;
	
	show_snake(s);
	sleep(1);
	add_snake(s, 9, 20);
	add_snake(s, 8, 20);
	show_snake(s);
	sleep(1);
	
	last_ac = right;
	
	signal(SIGALRM, move);
	struct itimerval itime = {{1, 0}, {1, 0}};
	setitimer(ITIMER_REAL, &itime, NULL);
	
	while (runing) {
		;
	}
	
	del_snake(s);
	
	return 0;
}

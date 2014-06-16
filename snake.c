#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>
#include <string.h>

#include "list.h"
#include "snake.h"
<<<<<<< HEAD
#include "a_star.h"
#include "find_food.h"

#include <mcheck.h>
=======

#include <mcheck.h>

#define BOOL int
#define TRUE  1
#define FALSE 0
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

static struct point_set *snake = NULL;
static int runing = 0;
static struct coordinate next_point = {0, 0}, food = {0, 0};
<<<<<<< HEAD
=======

#define INIT_POSITION_X 10
#define INIT_POSITION_Y 20

#define LEN 20
#define WIDE 40
#define EDGE '#'
#define SNAKE_COL 31
#define EDGE_COL 33
#define SNAKE_HEAD '0'
#define SNAKE_BODY 'o'
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b

void init_edge(void)
{
    int i;
    
    printf("\33[?25l\33[2J");
    
    for(i=1; i<=LEN+2; i++) {
        printf("\33[%d;1H\33[%dm%c\33[0m", i, EDGE_COL, '0' + ((i-1)%10));
    }
    for(i=1; i<=LEN+2; i++) {
        printf("\33[%d;%dH\33[%dm%c\33[0m", i, WIDE+2, EDGE_COL, '0' + ((i-1)%10));
    }
    for(i=1; i<=WIDE; i++) {
        printf("\33[1;%dH\33[%dm%c\33[0m", i+1, EDGE_COL, '0' + (i%10));
    }
    for(i=1; i<=WIDE; i++) {
        printf("\33[%d;%dH\33[%dm%c\33[0m", LEN+2, i+1, EDGE_COL, '0' + (i%10));
    }
    
    printf("\33[%d;1H\33[33mpress \'q\' or \'Q\' to quit\33[7m\33[0m", LEN + 3);
    fflush(stdout);
}

void exit_edge(void)
{
    printf("\33[1;1H\33[?25h\33[2JGood Bye!\n");
}

int init_snake(struct point_set **s)
{
    *s = (struct point_set *)malloc(sizeof(struct point_set));
    if(*s == NULL) {
        return -1;
    }
    
    INIT_LIST_HEAD(&((*s)->list));
    (*s)->point.x = INIT_POSITION_X;
    (*s)->point.y = INIT_POSITION_Y;
    
    printf("\33[%d;%dH\33[%dm%c\33[0m", (*s)->point.x+1, (*s)->point.y+1, SNAKE_COL, SNAKE_HEAD);
    
    return 0;
}

int snake_add_head(struct point_set *shead, struct coordinate head) 
{
    struct point_set *s;
    
    s = (struct point_set *)malloc(sizeof(struct point_set));
    if (s == NULL) {
        return -1;
    }
    
    list_add(&(s->list), &(shead->list));
<<<<<<< HEAD
    
    memcpy((void *)&(s->point), (void *)&(shead->point), sizeof(struct coordinate));
    memcpy((void *)&(shead->point), (void *)&(head), sizeof(struct coordinate));
    
=======
    
    s->point.x = shead->point.x;
    s->point.y = shead->point.y;
    shead->point.x = head.x;
    shead->point.y = head.y;
    
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
    printf("\33[%d;%dH\33[%dm%c\33[0m", s->point.x+1, s->point.y+1, SNAKE_COL, SNAKE_BODY);
    printf("\33[%d;%dH\33[%dm%c\33[0m", head.x+1, head.y+1, SNAKE_COL, SNAKE_HEAD);
    
    printf("\33[%d;1H", LEN+3);
    fflush(stdout);
    
    return 0;
}

void snake_del_tail(struct point_set *shead)
{
    if (shead->list.next != &(shead->list)) {
		struct point_set *s = container_of(shead->list.prev, struct point_set, list);
		list_del_init(&(s->list));
        free(s);
        s = NULL;
    }
}

void snake_del(struct point_set **shead)
{
    while ((*shead)->list.next != &((*shead)->list)) {
		snake_del_tail(*shead);
	}
	free(*shead);
	*shead = NULL;
}

void snake_move(struct point_set *shead, struct coordinate next)
{
    if (shead == NULL) {
        return;
    }
    
    printf("\33[%d;%dH\33[%dm%c\33[0m", shead->point.x+1, shead->point.y+1, SNAKE_COL, SNAKE_BODY);
    
    struct point_set *s = container_of(shead->list.prev, struct point_set, list);
    int x = s->point.x, y = s->point.y;
    
    snake_add_head(shead, next);
    snake_del_tail(shead);
    
    printf("\33[%d;%dH\33[%dm%c\33[0m", next.x+1, next.y+1, SNAKE_COL, SNAKE_HEAD);
    printf("\33[%d;%dH%c\33[0m", x+1, y+1, ' ');
    
    printf("\33[%d;1H", LEN+3);
    fflush(stdout);
}

void set_time_blank(void)
{
    struct itimerval itime = {{0, 0}, {0, 50000}};
    setitimer(ITIMER_REAL, &itime, NULL);
}

void exchange(int *a, int *b)
{
    int temp;
    temp = *a;
    *a = *b;
    *b = temp;
}

BOOL is_body(struct coordinate point)
<<<<<<< HEAD
{
	if (snake == NULL)
		return FALSE;
	
    struct point_set *s = snake;
    struct list_head *slist = NULL;
    //s = container_of(snake->list.next, struct point_set, list);
    
	list_for_each(slist, &(snake->list)) {
		s = container_of(slist, struct point_set, list);
		if (point.x == s->point.x && point.y == s->point.y) {
			return TRUE;
		}
	}
    
    return FALSE;
}

BOOL is_edge(struct coordinate point)
{
=======
{
	int i = 0;
	
	if (snake == NULL)
		return FALSE;
	
    struct point_set *s = snake;
    struct list_head *slist = NULL;
    //s = container_of(snake->list.next, struct point_set, list);
    
	list_for_each(slist, &(snake->list)) {
		printf("\33[%d;%dH\33[%dm%d\33[0m", 10, WIDE+3, SNAKE_COL, i++);
		s = container_of(slist, struct point_set, list);
		if (point.x == s->point.x && point.y == s->point.y) {
			return TRUE;
		}
	}
    
    return FALSE;
}

BOOL is_edge(struct coordinate point)
{
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
    if (point.x == 0 || point.x == LEN+1 || 
        point.y == 0 || point.y == WIDE+1)
    {
        return TRUE;
    }
    return FALSE;
}

BOOL is_food(struct coordinate point)
{
    if (point.x == food.x && point.y == food.y) {
        return TRUE;
    }
    return FALSE;
}

int look_ahead(struct coordinate point)
{
<<<<<<< HEAD
    if (is_edge(point) == TRUE) {
		return -2;
	} else if (is_body(point) == TRUE) {
        return -1;
    } else if (point.x == snake->point.x && point.y == snake->point.y) {
		return 2;
    } else if (is_food(point) == TRUE) {
=======
    if (is_edge(next_point) == TRUE) {
		return -2;
	} else if (is_body(next_point) == TRUE) {
        return -1;
    } else if (is_food(next_point) == TRUE) {
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
        return 1;
    } else {
        return 0;
    }
    
    return 0;
}

void move(void)
{
<<<<<<< HEAD
	int ahead = look_ahead(next_point);
=======
	int ahead = look_ahead();
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
	
    switch (ahead)
    {
        case 0:
            snake_move(snake, next_point);
            break;
        case 1:
            pthread_mutex_lock(&mutex);
            
            if(snake_add_head(snake, food) < 0)
				runing = 0;
            
            pthread_mutex_unlock(&mutex);
            pthread_cond_signal(&cond);
            break;
        case 2:
			break;
        default:
            runing = 0;
    }
    
    //reclocking
    set_time_blank();
}

<<<<<<< HEAD
=======
void find_food(void)
{
    if (food.x < snake->point.x) {
        next_point.x--;
    } else if (food.x > snake->point.x) {
        next_point.x++;
    } else {
        if (food.y < snake->point.y) {
            next_point.y--;
        } else if (food.y > snake->point.y) {
            next_point.y++;
        } else {
            next_point.x--;
        }
    }
}

>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
void timer_thread(int signo)
{
	struct coordinate point;
	
	point = find_food(snake, food, &runing);
	memcpy((void *)&(next_point), (void *)&point, sizeof(struct coordinate));
	
    move();
}

void init_food(void)
{
    //srand(time(NULL));
}

void *food_thread(void *arg)
{
    struct coordinate f;
<<<<<<< HEAD
    int i = 0;
    
    srandom(time(NULL));
    
    while (runing) {printf("\33[%d;%dH\33[%dmfood:%d\33[0m", 1, WIDE+3, SNAKE_COL, i++);
=======
    int i = 0, j = 0;
    
    srandom(time(NULL));
    
    while (runing) {
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
        f.x = random()%LEN;
        f.y = random()%WIDE;
        
        while(is_body(f) == TRUE || is_edge(f) == TRUE || is_food(f) == TRUE) {
            f.x = random()%LEN;
            f.y = random()%WIDE;
        }
        food.x = f.x;
        food.y = f.y;
        
        printf("\33[%d;%dH\33[%dm%c\33[0m", food.x+1, food.y+1, SNAKE_COL, 'm');
        fflush(stdout);
        
        pthread_cond_wait(&cond, &mutex);
    }
    
    pthread_exit(0);
}

struct termios save, raw;

void init_terminal(void)
{
    tcgetattr(0, &save);
    cfmakeraw(&raw);
    tcsetattr(0, 0, &raw);
}

void exit_terminal(void)
{
    tcsetattr(0, 0, &save);
}

void *key_thread(void *arg)
{
	//int *runing = (int *)arg;
	
    char ch;
    
    while (runing) {
        ch = getchar();
        
key_thread_check_out:
        if (ch == 'q' || ch == 'Q') {
            goto key_thread_out;
        }
        else if (ch == 27) {
            ch = getchar();
            if (ch == '[') {
                ch = getchar();
                switch (ch) {
                    case 'A':
                        //printf("up\n");
                        next_point.x = next_point.x - 1;
                        break;
                    case 'B':
                        //printf("down\n");
                        next_point.x++;
                        break;
                    case 'C':
                        //printf("right\n");
                        next_point.y++;
                        break;
                    case 'D':
                        //printf("left\n");
                        next_point.y--;
                        break;
                    default:
                        goto key_thread_check_out;
                }
                move();
            } else {
                goto key_thread_check_out;
            }
        }
    }

key_thread_out:
    runing = 0;
    pthread_exit(0);
}

int main()
{
	mtrace();
	
    int ret = 0;
    
    init_edge();
    init_terminal();
    
    if (init_snake(&snake) < 0) {
		return -1;
	}
	
    next_point.x = INIT_POSITION_X-1;
    next_point.y = INIT_POSITION_Y;
<<<<<<< HEAD
    
    snake_add_head(snake, next_point);
    next_point.x--;
/*    snake_add_head(snake, next_point);
    next_point.x--;
    snake_add_head(snake, next_point);
    next_point.x--;
    snake_add_head(snake, next_point);
    
    struct point_set *s = snake;
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 10, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    s = container_of(s->list.next, struct point_set, list);
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 11, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    s = container_of(s->list.next, struct point_set, list);
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 12, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    s = container_of(s->list.next, struct point_set, list);
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 13, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    s = container_of(s->list.next, struct point_set, list);
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 14, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    s = container_of(s->list.next, struct point_set, list);
    printf("\33[%d;%dH\33[%dm%p, %p, %p\33[0m", 15, WIDE+3, SNAKE_COL, &(s->list), s->list.prev, s->list.next);
    */
    
    
    runing = 1;
    
=======
    
    runing = 1;
    
>>>>>>> 1b08611d6f408c66a55d133392ddc0e27aff267b
    sleep(1);
    
    pthread_t thd_food, thd_key;
    
    ret = pthread_create(&thd_food, NULL, food_thread, &runing);
    if (ret != 0) {
        goto done;
    }
    
    ret = pthread_create(&thd_key, NULL, key_thread, &runing);
    if (ret != 0) {
        goto done;
    }
    
    signal(SIGALRM, timer_thread);
    set_time_blank();
    
    while (runing) {
        ;
    }
    
done:
    snake_del(&snake);
    exit_terminal();
    exit_edge();
    
    return 0;
}

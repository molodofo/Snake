#include <stdio.h>
#include <unistd.h>
#include <sys/time.h>
#include <signal.h>
#include <stdlib.h>
#include <pthread.h>
#include <termios.h>

#undef offsetof
#ifdef __compiler_offsetof
#define offsetof(TYPE,MEMBER) __compiler_offsetof(TYPE,MEMBER)
#else
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#endif

/**
 * container_of - cast a member of a structure out to the containing structure
 * @ptr:    the pointer to the member.
 * @type:   the type of the container struct this is embedded in.
 * @member: the name of the member within the struct.
 *
 */
#define container_of(ptr, type, member) ({          \
    const typeof( ((type *)0)->member ) *__mptr = (ptr);    \
    (type *)( (char *)__mptr - offsetof(type,member) );})


#define LEN 20
#define WIDE 40
#define EDGE '8'
#define SNAKE_COL 31
#define EDGE_COL 34
#define SNAKE_HEAD '0'
#define SNAKE_BODY 'o'

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

struct node {
    struct node *next;
};

struct snake {
    struct node head;
    int x, y;
};

struct _food {
    int x, y;
}food;

struct snake *shead = NULL;
int runing = 0;

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

void exit_edge(void)
{
    printf("\33[1;1H\33[?25h\33[2JGood Bye!\n");
}

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
}

int is_body(int x, int y)
{
    struct snake *s;
    if (shead->head.next != NULL) {
        s = shead;
        do
        {
            s = container_of(s->head.next, struct snake, head);
            if (x == s->x && y == s->y) {
                return 0;
            }
        } while (s->head.next != NULL);
    }
    
    return -1;
}

int is_edge(int x, int y)
{
    if (x == 0 || x == 21 || 
        y == 0 || y == 41)
    {
        return 0;
    } 
    return -1;
}

int is_food(int x, int y)
{
    if (x == food.x && y == food.y) {
        return 0;
    }
    return -1;
}

void ahead_is(int *ahead_x, int *ahead_y)
{
    (*ahead_x) = shead->x;
    (*ahead_y) = shead->y;
    
    switch (last_ac)
    {
        case up:
            (*ahead_x)--;
            break;
        case down:
            (*ahead_x)++;
            break;
        case right:
            (*ahead_y)++;
            break;
        case left:
            (*ahead_y)--;
            break;
        default:
            ;
    }
}

int look_ahead(void)
{
    int ahead_x, ahead_y;
    
    ahead_is(&ahead_x, &ahead_y);
    
    if (is_edge(ahead_x, ahead_y) == 0 ||
        is_body(ahead_x, ahead_y) == 0) {
        return -1;
    } else if (is_food(ahead_x, ahead_y) == 0) {
        return 1;
    } else {
        return 0;
    }
    
    return 0;
}

void move(void)
{
    switch (look_ahead())
    {
        case 0:
            move_snake(shead, last_ac);
            break;
        case 1:
            pthread_mutex_lock(&mutex);
            
            add_snake(shead, food.x, food.y);
            
            pthread_cond_signal(&cond);
            pthread_mutex_unlock(&mutex);
            break;
        default:
            runing = 0;
    }
    
    show_snake(shead);
    
    //reclocking
    struct itimerval itime = {{0, 0}, {1, 0}};
    setitimer(ITIMER_REAL, &itime, NULL);
}

void move_thread(int signo)
{
    move();
}

void init_food(void)
{
    srandom(getpid());
}

void *food_thread(void *arg)
{
    int x, y;
    
    while (runing) {
        x = random()%LEN;
        y = random()%WIDE;
        
        while(is_body(x, y) == 0 || is_edge(x, y) == 0) {
            x = random()%LEN;
            y = random()%WIDE;
        }
        
        food.x = x;
        food.y = y;
        
        printf("\33[%d;%dH\33[%dm%c\33[0m", food.x+1, food.y+1, SNAKE_COL, 'm');
        
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
    char ch;
    enum action ac;
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
                        ac = up;
                        break;
                    case 'B':
                        //printf("down\n");
                        ac = down;
                        break;
                    case 'C':
                        //printf("right\n");
                        ac = right;
                        break;
                    case 'D':
                        //printf("left\n");
                        ac = left;
                        break;
                    default:
                        goto key_thread_check_out;
                }
                last_ac = ac;
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
    int ret = 0;
    
    init_edge();
    init_terminal();
    
    struct snake *s = NULL;
    init_snake(&s);
    shead = s;
    
    add_snake(s, 9, 20);
    
    sleep(1);
    
    last_ac = right;
    
    pthread_t thd_food, thd_key;
    
    ret = pthread_create(&thd_food, NULL, food_thread, NULL);
    if (ret != 0) {
        goto done;
    }
    
    ret = pthread_create(&thd_key, NULL, key_thread, NULL);
    if (ret != 0) {
        goto done;
    }
    
    signal(SIGALRM, move_thread);
    struct itimerval itime = {{0, 0}, {1, 0}};
    setitimer(ITIMER_REAL, &itime, NULL);
    
    while (runing) {
        ;
    }

done:
    del_snake(s);
    exit_terminal();
    exit_edge();
    
    return 0;
}

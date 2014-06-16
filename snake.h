#ifndef __SNAKE_SNAKE_H__
#define __SNAKE_SNAKE_H__

#include "list.h"

#define BOOL int
#define TRUE  1
#define FALSE 0

#define INIT_POSITION_X 10
#define INIT_POSITION_Y 20

#define LEN 20
#define WIDE 40
#define EDGE '#'
#define SNAKE_COL 31
#define EDGE_COL 33
#define SNAKE_HEAD '0'
#define SNAKE_BODY 'o'

struct coordinate {
	int x, y;
};

struct point_set {
	struct list_head list;
	struct coordinate point;
};

BOOL is_body(struct coordinate point);
BOOL is_edge(struct coordinate point);
BOOL is_food(struct coordinate point);

#endif

#ifndef __INCLUDE_A_STAR_H__
#define __INCLUDE_A_STAR_H__

#include "snake.h"
#include "list.h"

//#define DEBUG

#define ABS(a) ((a)>0?(a):-(a))

typedef int (*__is_barrier)(struct coordinate point);

struct score {
	int f, g, h;
};

struct star_node {
	//struct coordinate parent;
	struct star_node *parent;
	struct coordinate pos;
	struct score s;
	struct list_head list;
};

struct pathway {
	struct coordinate pos;
	struct pathway *next;
};

struct pathway *a_star(struct coordinate start, struct coordinate goal, __is_barrier is_barrier);
void pathway_del(struct pathway *path);

#endif

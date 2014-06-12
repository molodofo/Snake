#ifndef __SNAKE_SNAKE_H__
#define __SNAKE_SNAKE_H__

#include "list.h"

struct coordinate {
	int x, y;
};

struct point_set {
	struct list_head list;
	struct coordinate point;
};

#endif

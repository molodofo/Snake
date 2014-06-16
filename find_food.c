#include <stdio.h>
#include <string.h>

#include "list.h"
#include "snake.h"
#include "find_food.h"
#include "a_star.h"

#define A_STAR
#ifdef A_STAR
int is_barrier(struct coordinate point)
{
	if (is_body(point) == TRUE 
		|| is_edge(point) == TRUE)
		return TRUE;
	
	return FALSE;
}
#endif

struct coordinate find_food(struct point_set *snake, struct coordinate food, int *runing)
{
	struct coordinate point;
	#ifndef A_STAR
	memcpy((void *)&point, (void *)&(snake->point), sizeof(struct coordinate));
    if (food.x < point.x) {
        point.x--;
    } else if (food.x > point.x) {
        point.x++;
    } else {
        if (food.y < point.y) {
            point.y--;
        } else if (food.y > point.y) {
            point.y++;
        } else {
            point.x--;
        }
    }
    
    return point;
    #else
    
    struct pathway *path = NULL;
    struct pathway *head_to_tail = NULL, *head_to_food = NULL, *food_to_tail = NULL;
    struct point_set *tail = NULL;
    
    // Tactics 1: if the snake can find the way to its tail, it will not die
    // look around tail of snake
    tail = container_of(snake->list.prev, struct point_set, list);
    memcpy((void *)&point, (void *)&(tail->point), sizeof(struct coordinate));
    // up
    point.x = point.x - 1;
    if (is_barrier(point) == 0) {
		head_to_tail = a_star(snake->point, point, is_barrier);
		if (head_to_tail != NULL) goto find_food_tactics_2;
    }
    // down
    point.x = point.x + 2;
    if (is_barrier(point) >= 0) {
		head_to_tail = a_star(snake->point, point, is_barrier);
		if (head_to_tail != NULL) goto find_food_tactics_2;
    }
    // right
    point.x = point.x - 1;
    point.y = point.y - 1;
    if (is_barrier(point) >= 0) {
		head_to_tail = a_star(snake->point, point, is_barrier);
		if (head_to_tail != NULL) goto find_food_tactics_2;
    }
    // left
    point.y = point.y + 2;
    if (is_barrier(point) >= 0) {
		head_to_tail = a_star(snake->point, point, is_barrier);
		if (head_to_tail != NULL) goto find_food_tactics_2;
    }
    
    if (head_to_tail == NULL) { // have not pathway, whatever 
        path = NULL;printf("\33[%d;%dH\33[%dm%s\33[0m", 2, WIDE+3, SNAKE_COL, "shit");
        runing = 0;
        return point;
    }
    
find_food_tactics_2:
    // Tactics 2: try to find the food
    head_to_food = a_star(snake->point, food, is_barrier);
    if (head_to_food == NULL) { // if can't, go to find the tail
        path = head_to_tail;
    } else {
        // if go to find the food, can snake go back to its tail
        memcpy((void *)&point, (void *)&(tail->point), sizeof(struct coordinate));
		// up
		point.x = point.x - 1;
		if (is_barrier(point) >= 0) {
			food_to_tail = a_star(head_to_food->pos, point, is_barrier);
			if (food_to_tail != NULL) goto find_food_food_to_tail;
		}
		// down
		point.x = point.x + 2;
		if (is_barrier(point) >= 0) {
			food_to_tail = a_star(head_to_food->pos, point, is_barrier);
			if (food_to_tail != NULL) goto find_food_food_to_tail;
		}
		// right
		point.x = point.x - 1;
		point.y = point.y - 1;
		if (is_barrier(point) >= 0) {
			food_to_tail = a_star(head_to_food->pos, point, is_barrier);
			if (food_to_tail != NULL) goto find_food_food_to_tail;
		}
		// left
		point.y = point.y + 2;
		if (is_barrier(point) >= 0) {
			food_to_tail = a_star(head_to_food->pos, point, is_barrier);
			if (food_to_tail != NULL) goto find_food_food_to_tail;
		}
find_food_food_to_tail:
        if (food_to_tail == NULL) {
            path = head_to_tail;
            pathway_del(head_to_food);
        } else {
            path = head_to_food;
            pathway_del(head_to_tail);
            pathway_del(food_to_tail);
        }
    }
	
	memcpy((void *)&(point), (void *)&(path->pos), sizeof(struct coordinate));
	pathway_del(path);
	return point;
    #endif
}


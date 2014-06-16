#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mcheck.h>
#include <math.h>

#include "a_star.h"

/*
function A*(start,goal)
    closedset := the empty set    // The set of nodes already evaluated.
    openset := {start}    // The set of tentative nodes to be evaluated, initially containing the start node
    came_from := the empty map    // The map of navigated nodes.
 
    g_score[start] := 0    // Cost from start along best known path.
    // Estimated total cost from start to goal through y.
    f_score[start] := g_score[start] + heuristic_cost_estimate(start, goal)
 
    while openset is not empty
        current := the node in openset having the lowest f_score[] value
        if current = goal
            return reconstruct_path(came_from, goal)
 
        remove current from openset
        add current to closedset
        for each neighbor in neighbor_nodes(current)
            if neighbor in closedset
                continue
            tentative_g_score := g_score[current] + dist_between(current,neighbor)
 
            if neighbor not in openset or tentative_g_score < g_score[neighbor] 
                came_from[neighbor] := current
                g_score[neighbor] := tentative_g_score
                f_score[neighbor] := g_score[neighbor] + heuristic_cost_estimate(neighbor, goal)
                if neighbor not in openset
                    add neighbor to openset
 
    return failure
*/

void star_insert_sort(struct star_node *star, 
			struct list_head **sort_list)
{
	if (*sort_list == &(star->list))
		*sort_list = (*sort_list)->next;
	list_del(&(star->list));
	
	struct list_head *list = *sort_list;
	struct star_node *node = container_of(list, struct star_node, list);
	int biggest = 1;
	
	if (star->s.f < node->s.f) { //it's smallest, change list_head
		__list_add(&(star->list), list->prev, list);
		*sort_list = &(star->list);
	} else {
		list_for_each(list, *sort_list) {
			node = container_of(list, struct star_node, list);
			
			if (star->s.f < node->s.f) {
				__list_add(&(star->list), list->prev, list);
				biggest = 0;
				break;
			}
		}
		if (biggest) {
			__list_add(&(star->list), list, list->next);
		}
	}
}

struct star_node *star_add(struct star_node *parent, 
			struct list_head **sort_list, 
			struct star_node *star_list, 
			struct coordinate pos, 
			struct score s)
{
	struct star_node *new;
	
	new = malloc(sizeof(struct star_node));
	if (new == NULL) {
		return NULL;
	}
	
	memcpy((void *)&(new->pos), (void *)&pos, sizeof(struct coordinate));
	memcpy((void *)&(new->s), (void *)&s, sizeof(struct score));
    //memcpy((void *)&(new->parent), (void *)&parent, sizeof(struct coordinate));
    new->parent = parent;
    INIT_LIST_HEAD(&(new->list));
    
	// if you don't neet to sort
	if (sort_list == NULL) {
		if (star_list != NULL) {
			list_add(&(new->list), &(star_list->list));
		}
	} else {
		// sort_list first init
		if (*sort_list == NULL) {
			INIT_LIST_HEAD(&(new->list));
			*sort_list = &(new->list);
			return new;
		}
		//insert sort
		star_insert_sort(new, sort_list);
	}
	return new;
}

void star_del_one(struct star_node *star)
{
    list_del(&(star->list));
    free(star);
}

void star_del(struct star_node *star)
{
	if (star != NULL) {
		while (star->list.next != &(star->list)) {
			struct star_node *s = container_of(star->list.next, struct star_node, list);
			star_del_one(s);
		}
		list_del(&(star->list));
		free(star);
	}
}

struct star_node *star_del_sortlist_head(struct list_head **sort_list)
{
	if ((*sort_list)->next == (*sort_list)) {
		list_del(*sort_list);
		INIT_LIST_HEAD(*sort_list);
		*sort_list = NULL;
		return NULL;
	} else {
		struct list_head *list = *sort_list;
		*sort_list = (*sort_list)->next;
		list_del(list);
		INIT_LIST_HEAD(list);
		return container_of(*sort_list, struct star_node, list);
	}
}

struct star_node *point_in_set(struct coordinate point, 
			struct star_node *star_list)
{
	if (star_list != NULL) {
		struct star_node *star = NULL;
		
		star = star_list;
		do
		{
			if (point.x == star->pos.x && point.y == star->pos.y) {
				return star;
			}
			star = container_of(star->list.next, struct star_node, list);
		} while (star != star_list);
	}
	
	return NULL;
}

struct star_node *point_in_list(struct coordinate point, 
			struct list_head *hlist)
{
	if (hlist != NULL) {
		struct star_node *star = NULL;
		struct list_head *list = hlist;
		
		star = container_of(list, struct star_node, list);
			if (point.x == star->pos.x && point.y == star->pos.y) 
				return star;
		
		list_for_each(list, hlist) {
			star = container_of(list, struct star_node, list);
			if (point.x == star->pos.x && point.y == star->pos.y)
				return star;
		}
	}
	
	return NULL;
}

//#define Euclidean_Distance
int h_score(struct coordinate start, struct coordinate goal)
{
	#ifndef Euclidean_Distance
	// Mamhattan distance
	return ABS(start.x - goal.x) + ABS(start.y - goal.y);
	#else
	// Euclidean distance
	return (int)sqrt(ABS(start.x - goal.x) * ABS(start.x - goal.x) + 
				ABS(start.y - goal.y) * ABS(start.y - goal.y));
	#endif
}

struct pathway *star_construct_path(struct star_node *star)
{
	struct pathway *path = NULL, *prev = NULL;
	struct star_node *parent = NULL;
	
	if (star != NULL) {
		path = malloc(sizeof(struct pathway));
		memcpy((void *)&(path->pos), (void *)&(star->pos), sizeof(struct coordinate));
		path->next = NULL;
		parent = star->parent;
		
		while(parent != NULL) {
			prev = malloc(sizeof(struct pathway));
			memcpy((void *)&(prev->pos), (void *)&(parent->pos), sizeof(struct coordinate));
			prev->next = path;
			parent = parent->parent;
			path = prev;
		}
		
		// delect start point
		prev = path;
		path = path->next;
        if (prev != NULL)
            free(prev);
	}
	
	return path;
}

void pathway_del(struct pathway *path)
{
	struct pathway *next;
	while (path != NULL) {
		next = path->next;
		free(path);
		path = next;
	}
}

void star_check_neighbor(struct coordinate point, 
			struct coordinate start, 
			struct coordinate goal, 
			struct star_node **current, 
			struct star_node **openset, 
			struct star_node **closeset, 
			struct list_head **sort_list,
			__is_barrier is_barrier)
{
	struct score s;
	struct star_node *neighbor = NULL;
	
	if (is_barrier(point) == 0 
		&& point_in_set(point, *closeset) == NULL
		) 
	{
		// count score of this point
		s.g = (*current)->s.g + 1;
		s.h = h_score(point, goal);
		s.f = s.g + s.h;

		neighbor = point_in_list(point, *sort_list);
		if (neighbor != NULL) { // point in openset
			if (s.g < neighbor->s.g) {
				neighbor->parent = *current;
				memcpy((void *)&(neighbor->s), (void *)&s, sizeof(struct score));
				star_insert_sort(neighbor, sort_list);
			}
		} else {
			star_add(*current, sort_list, NULL, point, s);
		}
		#ifdef DEBUG
		if (neighbor != NULL) printf("neighbor: xy(%d, %d), fgh(%d, %d, %d)\n", point.x, point.y, neighbor->s.f, neighbor->s.g, neighbor->s.h);
		else printf("neighbor: xy(%d, %d), fgh(%d, %d, %d)\n", point.x, point.y, s.f, s.g, s.h);
		#endif
	}
}

struct pathway *a_star(struct coordinate start, 
			struct coordinate goal, 
			__is_barrier is_barrier)
{
	struct star_node *openset = NULL, *closeset = NULL, *current = NULL;
	struct list_head *sort_list = NULL;
	struct coordinate point;
	struct pathway *path = NULL;
	struct score s;
	//int i = 0;
	// add start_point in openset
	s.g = 0;
	s.h = h_score(start, goal);
	s.f = s.g + s.h;
	openset = star_add(NULL, &sort_list, NULL, start, s);
	
	// prevent pointer exception
	closeset = openset;
	
	while(sort_list != NULL) {
		// get the lowest f_score in openset
		current = container_of(sort_list, struct star_node, list);
		
		#ifdef DEBUG
		printf("xy(%d, %d), fgh(%d, %d, %d)\n", current->pos.x, current->pos.y, current->s.f, current->s.g, current->s.h);
		#endif
		
		// stop if find the goal
		if(current->pos.x == goal.x && current->pos.y == goal.y)
			goto a_star_finded_pathway;
		
		// push current_point to closeset
		star_del_sortlist_head(&sort_list);
		list_add(&(current->list), &(closeset->list));
		closeset = current;
		
		// check each neighbor
		memcpy((void *)&point, (void *)&(current->pos), sizeof(struct coordinate));
		// up
		point.x = point.x - 1;
		star_check_neighbor(point, start, goal, &current, &openset, &closeset, &sort_list, is_barrier);
		
		// down
		point.x = point.x + 2;
		star_check_neighbor(point, start, goal, &current, &openset, &closeset, &sort_list, is_barrier);
		
		// right
		point.x = point.x - 1;
		point.y = point.y + 1;
		star_check_neighbor(point, start, goal, &current, &openset, &closeset, &sort_list, is_barrier);
		
		// left
		point.y = point.y - 2;
		star_check_neighbor(point, start, goal, &current, &openset, &closeset, &sort_list, is_barrier);
	}
	
	// can not find the way out
	if (sort_list != NULL) {
		openset = container_of(sort_list, struct star_node, list);
		star_del(openset);
	}
	star_del(closeset);
	return NULL;
	
a_star_finded_pathway:
	path = star_construct_path(current);
	if (sort_list != NULL) {
		openset = container_of(sort_list, struct star_node, list);
		star_del(openset);
	}
	
	return path;
}

struct coordinate *a_star_pathfinding(struct pathway **path)
{
	return NULL;
}

#ifdef DEBUG
int is_barrier(struct coordinate point)
{
	if (point.x == 0      
		|| point.x == LEN+1  
		|| point.y == 0      
		|| point.y == WIDE+1 
		|| (point.x == 1 && point.y == 5)
		|| (point.x == 2 && point.y == 5)
		|| (point.x == 3 && point.y == 5)
		|| (point.x == 4 && point.y == 5)
//		|| is_body(point) 
		)
	{
		return 1;
	}
	return 0;
}

int main()
{
    mtrace();
	struct star_node *star = NULL;
	struct list_head *sort = NULL;
	struct coordinate pos = {1, 3};
	struct score s = {5, 2, 3};
	
	star = star_add(star, &sort, NULL, pos, s);
	printf("star = %p, f = %d\n", &(star->list), star->s.f);
	printf("sort = %p\n\n", sort);
	
	s.f = 6;
	pos.x = 2;
	star = star_add(star, &sort, NULL, pos, s);
	printf("star = %p, f = %d\n", &(star->list), star->s.f);
	printf("sort = %p\n\n", sort);
	
	s.f = 3;
	pos.x = 3;
	star = star_add(star, &sort, NULL, pos, s);
	printf("star = %p, f = %d\n", &(star->list), star->s.f);
	printf("sort = %p\n\n", sort);
	
	s.f = 9;
	pos.x = 4;
	star = star_add(star, &sort, NULL, pos, s);
	printf("star = %p, f = %d\n", &(star->list), star->s.f);
	printf("sort = %p\n\n", sort);
	
	star->s.f = 10;
	star_insert_sort(star, &sort);
	printf("sort = %p\n\n", sort);
	
	s.f = 1;
	pos.x = 5;
	star = star_add(star, &sort, NULL, pos, s);
	printf("star = %p, f = %d\n", &(star->list), star->s.f);
	printf("sort = %p\n\n", sort);
	
	star->s.f = 10;
	star_insert_sort(star, &sort);
	printf("sort = %p\n\n", sort);
	
	pos.x = 5;
	printf("point_in_set = %p\n", point_in_set(pos, star));
	pos.x = 6;
	printf("point_in_set = %p\n", point_in_set(pos, star));
	pos.x = 2;
	printf("point_in_set = %p\n", point_in_set(pos, star));
	pos.x = 3;
	printf("point_in_set = %p\n", point_in_set(pos, star));
	
	pos.x = 5;
	printf("point_in_list = %p\n", point_in_list(pos, sort));
	pos.x = 6;
	printf("point_in_list = %p\n", point_in_list(pos, sort));
	pos.x = 2;
	printf("point_in_list = %p\n", point_in_list(pos, sort));
	pos.x = 3;
	printf("point_in_list = %p\n", point_in_list(pos, sort));
	
	struct star_node *close = NULL;
	close = star_add(close, NULL, close, pos, s);
	close = star_add(close, NULL, close, pos, s);
	
	struct star_node *no = star;
	star_insert_sort(no, &sort);
	
	star_del(close);
    star_del(star);
    
    struct pathway *path = NULL;
    struct coordinate start = {3, 2}, goal = {2, 7};
    path = a_star(start, goal, is_barrier);
	printf("pathway:\n");
	while (path != NULL) {
		printf("%d, %d\n", path->pos.x, path->pos.y);
		path = path->next;
	}
	printf("abs(1-23) = %d, abd(23-3) = %d, %d\n", ABS(1-23), ABS(23-3), (1-12)>0?(1-12):-(1-12));
    printf("h_score: %d\n", h_score(start, goal));
	return 0;
}
#endif

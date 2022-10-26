#ifndef __SDB_H__
#define __SDB_H__

#include <common.h>

typedef struct watchpoint{
	int NO;
	struct watchpoint *next;

	char content[32];
} WP;

word_t expr(char *e, bool *success);

void watchpoint_display();

void free_wp(int no);
WP* new_wp(char* content, bool *success);
bool check_all_watchpoints();

#endif

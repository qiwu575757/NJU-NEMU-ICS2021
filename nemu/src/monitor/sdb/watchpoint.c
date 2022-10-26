#include "sdb.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;
static int number = 1;
static uint32_t wp_values[NR_WP];

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = (i == NR_WP - 1 ? NULL : &wp_pool[i + 1]);
  }

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char* content, bool *success) {
	if (free_ == NULL) {
		panic("wps use up.");
	}

	WP *wp = free_->next;
	wp->NO = number ++;
	free_->next = free_->next->next;// 从free中删去 wp 
	wp->next = NULL; // next 初始化
	wp_values[wp->NO] = expr(content, success); // 验证wp的可行性
	strcpy(wp->content, content);

	if (head == NULL) {
		head = wp;
	} else {
		wp->next = head->next;
		head->next = wp;	
	}

	return wp;
}

void free_wp(int no) {
	bool find = false;
	if (head->NO == no) {
		WP *tmp = head->next; // 保存该节点的 next
		head->next = free_->next;
		free_->next = head;
		head = tmp->next;
		find = true;
	} else {
		WP *prev = head;
		while (prev->next != NULL) {
			if (prev->next->NO == no) {
				WP *tmp = prev->next->next;
				prev->next = free_->next;
				free_->next = prev;
				prev->next = tmp;
				find = true;
				break;
			}
			prev = prev->next;
		}
	}
	
	if (!find) {
		printf("Cannot find the \e[1;36mWP(NO.%d)\e[0m\n", no);
	}
	return ;
}

void watchpoint_display() {
	printf("All\t WatchPoints.\n");
	WP *cur = head;
	while (cur != NULL) {
		printf("\e[1;36m%d\e[0m\t\e[0;32m%s\e[0m\n", cur->NO, cur->content);
		cur = cur->next;
	}	
}

bool check_all_watchpoints() {
	WP *cur = head;
	bool is_stop = false;
	while (cur != NULL) {
		bool success = true;
		uint32_t val = expr(cur->content, &success);
		if (wp_values[cur->NO] != val && success) {
			printf("Stoped at \e[1;36mWatchPoint(NO.%d)\e[0m: %s \n", cur->NO, cur->content);
			printf("%s = \e[1;36m%u\e[0m\n", cur->content, val);
			wp_values[cur->NO] = val;
			is_stop = true;	
		}
		cur = cur->next;
	}

	return is_stop;
}

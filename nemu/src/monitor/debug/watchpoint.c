#include "monitor/watchpoint.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

static WP *new_wp() {
  if (free_ == NULL) {
    panic("watchpoint pool is full");
  }
  WP *wp = free_;
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

static void free_wp(WP *wp) {
  WP *p;
  if (head == wp) {
    head = head->next;
  } else {
    for (p = head; p != NULL && p->next != wp; p = p->next)
      ;
    if (p == NULL) {
      panic("watchpoint not in list");
    }
    p->next = wp->next;
  }
  wp->expr[0] = '\0';
  wp->next = free_;
  free_ = wp;
}

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

bool add_wp(char *args) {
  WP *wp = new_wp();
  bool ok = false;
  memcpy(wp->expr, args, strlen(args) + 1);
  assert(wp->expr[strlen(args)] == '\0');
  wp->value = expr(args, &ok);
  if (!ok) {
    free_wp(wp);
    return false;
  }
  return true;
}

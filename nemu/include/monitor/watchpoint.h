#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "monitor/expr.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expr[TOKEN_NUM];
  uint32_t value;

} WP;


// 从空闲链表中取出一个监视点
static WP* new_wp();

// 将监视点 wp 放回空闲链表
static void free_wp(WP *wp);

// 添加监视点
bool add_wp(char *args);

#endif

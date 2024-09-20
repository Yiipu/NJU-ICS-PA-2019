#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */


} WP;


// 从空闲链表中取出一个监视点
WP* new_wp();

// 将监视点 wp 放回空闲链表
void free_wp(WP *wp);

#endif

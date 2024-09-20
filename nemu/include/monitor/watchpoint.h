#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
#include "monitor/expr.h"
#include "monitor/monitor.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  char expr[TOKEN_NUM];
  uint32_t value;

} WP;

// 添加监视点
int add_wp(char *args);

// 检查监视点是否变化，
// 如果变化则打印监视点信息并暂停程序
void check_wp();

// 打印所有监视点信息
void wps_display();

// 移除监视点
int remove_wp(int NO);

#endif

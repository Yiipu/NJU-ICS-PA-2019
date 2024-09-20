#include "debug.h"
#include "monitor/expr.h"
#include "monitor/monitor.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <readline/history.h>
#include <readline/readline.h>
#include <stdlib.h>

void cpu_exec(uint64_t);

void isa_reg_display();

/* We use the `readline' library to provide more flexibility to read from stdin.
 */
static char *rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) { return -1; }

static int cmd_help(char *args);

// si [N] 执行 N 步后暂停，默认为 1
static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
  } else {
    int N = atoi(args);
    cpu_exec(N);
  }
  return 0;
}

// info { r | w } 打印寄存器状态
static int cmd_info(char *args) {
  if (args == NULL) {
    printf("info r: print register status\n");
    printf("info w: print watchpoint status\n");
    return 0;
  }
  switch (args[0]) {
  case 'r': {
    isa_reg_display();
    break;
  }
  case 'w': {
    wps_display();
    break;
  }
  default: {
    printf("Unknown command '%s'\n", args);
    break;
  }
  }
  return 0;
}

static int cmd_p(char *args) {
  bool ok = false;
  int eval = expr(args, &ok);
  if (ok) {
    printf("%d\n", eval);
    return 0;
  } else {
    return -1;
  }
}

static int cmd_x(char *args) { panic("TODO: implement `x`"); }

static int cmd_w(char *args) { return add_wp(args); }

static int cmd_d(char *args) {
  int N = atoi(args);
  return remove_wp(N);
}

// CLI 的命令表，在这里添加命令和命令处理函数
static struct {
  char *name;
  char *description;
  int (*handler)(char *);
} cmd_table[] = {
    {"help", "Display informations about all supported commands", cmd_help},
    {"c", "Continue the execution of the program", cmd_c},
    {"q", "Exit NEMU", cmd_q},
    {"si", "Step through N instructions, default is 1 if N is not given",
     cmd_si},
    {"info", "Print program status", cmd_info},
    {"p", "Evaluate the expression EXPR", cmd_p},
    {"x", "Scan memory starting from the address evaluated from EXPR", cmd_x},
    {"w", "Set a watchpoint for expression EXPR", cmd_w},
    {"d", "Delete the watchpoint with sequence number N", cmd_d},
    /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  } else {
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL;) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) {
      continue;
    }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) {
          return;
        }
        break;
      }
    }

    if (i == NR_CMD) {
      printf("Unknown command '%s'\n", cmd);
    }
  }
}

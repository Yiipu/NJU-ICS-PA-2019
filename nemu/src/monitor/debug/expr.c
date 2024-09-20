#include "monitor/expr.h"
#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include <sys/types.h>

int isa_reg_str2val(const char *, bool *);

#define CANBE_MONOOP(i)                                                        \
  (i == p ||                                                                   \
   (tokens[i - 1].type != TK_DECIMAL && tokens[i - 1].type != TK_RPAREN))

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DECIMAL,  // 十进制整数
  TK_HEX,      // 十六进制整数
  TK_REG,      // 寄存器
  TK_PLUS,     // +
  TK_MINUS,    // -
  TK_MULTIPLY, // *
  TK_DIVIDE,   // /
  TK_LPAREN,   // (
  TK_RPAREN,   // )

  TK_NEG,  // 负号
  TK_DEREF // 取地址
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

    {" +", TK_NOTYPE},             // 空格串
    {"\\+", TK_PLUS},              // +
    {"-", TK_MINUS},               // -
    {"\\*", TK_MULTIPLY},          // *
    {"/", TK_DIVIDE},              // /
    {"\\(", TK_LPAREN},            // (
    {"\\)", TK_RPAREN},            // )
    {"0x[0-9a-fA-F]+", TK_HEX},    // 十六进制整数
    {"0|[1-9][0-9]*", TK_DECIMAL}, // 十进制整数
#if __ISA__ == riscv32
    {"\\$\\$0|\\$[a-z0-9]+", TK_REG}, // 寄存器
#endif
    {"==", TK_EQ} // equal
};
#define NR_REGEX (sizeof(rules) / sizeof(rules[0]))

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

typedef struct stack {
  int top;
  int data[TOKEN_NUM];
} Stack;

static Token tokens[TOKEN_NUM] __attribute__((used)) = {};
static int nr_token __attribute__((used)) = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 &&
          pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i,
            rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        switch (rules[i].token_type) {
        case TK_NOTYPE:
          break;
        case TK_REG:
          substr_start++;
          substr_len--;
        default:
          if (substr_len >= sizeof(tokens[nr_token].str)) {
            fprintf(stderr, "Token too long at position %d\n", position);
            return false;
          }
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start, substr_len);
          tokens[nr_token].str[substr_len] = '\0';
          nr_token++;
#ifdef DEBUG
          Log("got token at %d: type = %d, str = %s\n", nr_token - 1,
              tokens[nr_token - 1].type, tokens[nr_token - 1].str);
#endif
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

// 返回 p 到 q 之间的主运算符的下标
// 主运算符：最后被计算的运算符
static int dominant_operator(int p, int q) {
  int op = -1;
  int op_priority = 0;
  int op_in_parentheses = 0;
  int i;
  for (i = p; i <= q; i++) {
    switch (tokens[i].type) {
    case TK_LPAREN:
      op_in_parentheses++;
      break;
    case TK_RPAREN:
      op_in_parentheses--;
      break;
    case TK_MINUS:
      if (CANBE_MONOOP(i) && op_priority < 1) {
        // 负号
        tokens[i].type = TK_NEG;
        op = i;
        op_priority = 1;
        break;
      }
    case TK_PLUS:
      if (op_in_parentheses == 0 && op_priority < 3) {
        // 加减
        op = i;
        op_priority = 3;
      }
      break;
    case TK_EQ:
      if (op_in_parentheses == 0 && op_priority < 4) {
        // 等于
        op = i;
        op_priority = 4;
      }
      break;
    case TK_MULTIPLY:
      if (CANBE_MONOOP(i) && op_in_parentheses == 0 && op_priority < 1) {
        // 取地址
        tokens[i].type = TK_DEREF;
        op = i;
        op_priority = 1;
        break;
      }
    case TK_DIVIDE:
      if (op_in_parentheses == 0 && op_priority < 2) {
        // 乘除
        op = i;
        op_priority = 2;
      }
      break;
    default:
      break;
    }
  }
  return op;
}

// 检查括号是否可去除
// 要求调用者保证 p < q
// 返回 true：最左和最右是括号且匹配
// 返回 false：最左和最右不都是括号 或 不匹配
static bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN) {
    return false;
  }

  Stack s; // 向上增上的满栈
  s.top = -1;

  // 去掉最外层的括号，用栈检查是否匹配
  p++;
  while (p < q) {
    switch (tokens[p].type) {
    case TK_LPAREN:
      s.data[++s.top] = TK_LPAREN;
      break;
    case TK_RPAREN:
      if (s.top == -1) {
        return false;
      }
      s.top--;
      break;
    default:
      break;
    }
    p++;
  }
  return s.top == -1;
}

static uint32_t eval(int p, int q, bool *ok) {
  if (p > q) {
    /* Bad expression */
    *ok = false;
    printf("Bad expression\n");
    return 0;
  } else if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    switch (tokens[p].type) {
    case TK_DECIMAL:
      return atoi(tokens[p].str);
    case TK_HEX:
      return strtol(tokens[p].str, NULL, 16);
    case TK_REG: {
      if (strcmp(tokens[p].str, "pc") == 0) {
        return cpu.pc;
      }
      int val = isa_reg_str2val(tokens[p].str, ok);
      if (!*ok) {
        printf("Invalid register\n");
      }
      return val;
    }
    default:
      *ok = false;
      printf("Invalid token\n");
      return 0;
    }
  } else if (check_parentheses(p, q) == true) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, ok);
  } else {
    int op = dominant_operator(p, q);
    if (op == -1) {
      *ok = false;
      printf("No dominant operator\n");
      return 0;
    }

    switch (tokens[op].type) {
    case TK_NEG:
      return -eval(op + 1, q, ok);
    case TK_DEREF: {
      int addr = eval(op + 1, q, ok);
      if (!*ok) {
        printf("Invalid address\n");
        return 0;
      }
      return vaddr_read(addr, 4);
    }
    default:
      break;
    }

    int val1 = eval(p, op - 1, ok);
    int val2 = eval(op + 1, q, ok);
#ifdef DEBUG
    Log("read: %u %d %u\n", val1, tokens[op].type, val2);
#endif
    switch (tokens[op].type) {
    case TK_EQ:
      return val1 == val2;
    case TK_PLUS:
      return val1 + val2;
    case TK_MINUS:
      return val1 - val2;
    case TK_MULTIPLY:
      return val1 * val2;
    case TK_DIVIDE:
      if (val2 == 0) {
        *ok = false;
        printf("Divide by zero\n");
        return 0;
      }
      return val1 / val2;
    default:
      panic("Not a valid operator");
    }
  }
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  *success = true;
  return eval(0, nr_token - 1, success);
}

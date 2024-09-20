#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>
#include <sys/types.h>

enum {
  TK_NOTYPE = 256,
  TK_EQ,
  TK_DECIMAL,  // 十进制整数
  TK_PLUS,     // +
  TK_MINUS,    // -
  TK_MULTIPLY, // *
  TK_DIVIDE,   // /
  TK_LPAREN,   // (
  TK_RPAREN    // )
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

    {" +", TK_NOTYPE},           // 空格串
    {"\\+", TK_PLUS},            // +
    {"-", TK_MINUS},             // -
    {"\\*", TK_MULTIPLY},        // *
    {"/", TK_DIVIDE},            // /
    {"\\(", TK_LPAREN},          // (
    {"\\)", TK_RPAREN},          // )
    {"0|[1-9][0-9]*", TK_DECIMAL}, // 十进制整数
    {"==", TK_EQ}                // equal
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
  int data[32]; // 32 是 tokens[] 的长度
} Stack;

static Token tokens[32] __attribute__((used)) = {};
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

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
        case TK_NOTYPE:
          break;
        case TK_DECIMAL:
        case TK_PLUS:
        case TK_MINUS:
        case TK_MULTIPLY:
        case TK_DIVIDE:
        case TK_LPAREN:
        case TK_RPAREN:
        case TK_EQ:
          tokens[nr_token].type = rules[i].token_type;
          strncpy(tokens[nr_token].str, substr_start,
                  substr_len); // 暂时先不管溢出
          tokens[nr_token].str[substr_len] = '\0';
          nr_token++;
#ifdef DEBUG
          printf("tokens[%d].type = %d, tokens[%d].str = %s\n", nr_token - 1,
                 tokens[nr_token - 1].type, nr_token - 1,
                 tokens[nr_token - 1].str);
#endif
          break;
        default:
          panic("Not a valid token type");
          break;
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
int dominant_operator(int p, int q) {
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
    case TK_PLUS:
    case TK_MINUS:
      if (op_in_parentheses == 0 && op_priority < 2) {
        op = i;
        op_priority = 1;
      }
      break;
    case TK_MULTIPLY:
    case TK_DIVIDE:
      if (op_in_parentheses == 0 && op_priority < 1) {
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
bool check_parentheses(int p, int q) {
  if (tokens[p].type != TK_LPAREN || tokens[q].type != TK_RPAREN) {
    return false;
  }

  Stack s; // 向上增上的满栈
  s.top = -1;

  // 去掉最外层的括号，用栈检查是否匹配
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

uint32_t eval(int p, int q, bool *ok) {
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
    return atoi(tokens[p].str);
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
    int val1 = eval(p, op - 1, ok);
    int val2 = eval(op + 1, q, ok);
#ifdef DEBUG
    printf("eval: VAL1:%d OP:%d VAL2%d\n", val1, tokens[op].type, val2);
#endif
    switch (tokens[op].type) {
    case TK_PLUS:
      return val1 + val2;
    case TK_MINUS:
      return val1 - val2;
    case TK_MULTIPLY:
      return val1 * val2;
    case TK_DIVIDE:
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

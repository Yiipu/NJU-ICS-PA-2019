#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// this should be enough
#define BUFFER_SIZE 128
static char buf[BUFFER_SIZE];
static int pos = 0;
void gen_rand_expr();

void gen_num() {
  int num = rand() % 100; // 0~99
#ifdef ALLOW_NEG
  if (rand() % 2 == 0) {
    buf[pos++] = '-';
  }
#endif
  pos += sprintf(buf + pos, "%d", num);
}

void gen_rand_op() {
  switch (rand() % 4) {
  case 0:
    buf[pos++] = '+';
    break;
  case 1:
    buf[pos++] = '-';
    break;
  case 2:
    buf[pos++] = '*';
    break;
  case 3:
    buf[pos++] = '/';
    break;
  }
}

void gen_rand_expr() {
  if (pos >= BUFFER_SIZE - 5) {
    gen_num();
    return;
  }
  int choice = rand() % 3;
  switch (choice) {
  case 0:
    gen_num();
    break;
  case 1:
    buf[pos++] = '(';
    gen_rand_expr();
    buf[pos++] = ')';
    break;
  case 2:
    gen_rand_expr();
    gen_rand_op();
    gen_rand_expr();
    break;
  }
  buf[pos] = '\0';
}
static char code_buf[65536];
static char *code_format = "#include <stdio.h>\n"
                           "int main() { "
#ifdef ALLOW_NEG
                           "  int result = %s; "
                           "  printf(\"%%d\", result); "
#else
                           "  unsigned result = %s; "
                           "  printf(\"%%u\", result); "
#endif
                           "  return 0; "
                           "}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i++) {
    pos = 0;
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0)
      continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    if (fscanf(fp, "%d", &result)) {
#ifdef ALLOW_NEG
      printf("%d %s\n", result, buf);
#else
      printf("%u %s\n", result, buf);
#endif
    }
    pclose(fp);
  }
  return 0;
}

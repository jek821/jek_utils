#include <stdio.h>
int some_func(int *x) {
  *x += 10;
  return 0;
}

int main() {
  int x = 10;

  some_func(&x);

  printf("%d", x);
}
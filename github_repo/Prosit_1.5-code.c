#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int p = malloc(sizeof(int) 10);

  if (p == NULL) {
    printf("Échec de l'allocation\n");
    return EXIT_FAILURE;
  }

  for (unsigned i = 0; i < 10; ++i) {
    p[i] = i * 10;
    printf("p[%u] = %d\n", i, p[i]);
  }

  return 0;
}
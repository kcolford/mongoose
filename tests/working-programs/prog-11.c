#ifdef GCC
#define ptr_t int *
#endif

int main ()
{
  ptr_t a_ptr = malloc (8);
  *a_ptr = 45;
  printf ("%d\n", *a_ptr);
  free (a_ptr);
  return 0;
}

/* this is a
   block comment */

int test1 () {
  int a = 2;
 label:
  // this is a line comment
  a = a + 1;
  if (a != 20)
    goto label;
  int b = 3;
  b = b + 35 * 5 / 7;
  return a + b;
}

int main ()
{
  printf ("%d\n", test1 ());
  return 0;
}

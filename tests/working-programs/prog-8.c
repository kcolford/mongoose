
int foo (int a, int b, int c) {
    return 1 + a + a * (1 + b + b * (1 + c + c * (1 + a + a * (1 + b + b * (1 + c + c * (1 + a + a * (1 + b + b * 2)))))));
}

int main () {
    printf ("%d\n", foo (3, 4, 5));
    return 0;
}



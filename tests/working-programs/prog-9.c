int foo (int a) {
    return a;
}

int bar (int a) {
    return 1 + 1 + foo (a);
}

int main () {
    printf ("%d\n", bar (0));
    return 0;
}

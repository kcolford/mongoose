int foo (int a, int b) {
    if (a == b)
	puts ("a and b are equal");
    else
	puts ("a and b are not equal");
    return 0;
}

int main () {
    foo (2, 2);
    foo (2, 3);
    return 0;
}

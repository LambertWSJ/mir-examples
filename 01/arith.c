typedef struct item {
    int a;
    int b;
    int c;
} item_t;


int add(int a, int b) {
    int c = a + b;
    printf("%d + %d = %d\n", a, b, c);
    return c;
}

int sub(int a, int b) {
    int c = a - b;
    printf("%d - %d = %d\n", a, b, c);
    return c;
}

int multi(int a, int b) {
    int c = a * b;
    printf("%d * %d = %d\n", a, b, c);
    return c;
}

int divide(int a, int b) {
    int c = a / b;
    printf("%d / %d = %d\n", a, b, c);
    return c;
}


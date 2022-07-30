struct io {
    void (*show)(int a, int b, int c, char operator);
};

typedef struct item {
    int a;
    int b;
    int c;
    struct io io;
} item_t;


int add(item_t *item) {
    item->c = item->a + item->b;
    item->io.show(item->a, item->b, item->c, '+');
    return item->c;
}

int sub(item_t *item) {
    item->c = item->a - item->b;
    item->io.show(item->a, item->b, item->c, '-');
    return item->c;
}

int multi(item_t *item) {
    item->c = item->a * item->b;
    item->io.show(item->a, item->b, item->c, '*');
    return item->c;
}

int divide(item_t *item) {
    item->c = item->a / item->b;
    item->io.show(item->a, item->b, item->c, '/');
    return item->c;
}


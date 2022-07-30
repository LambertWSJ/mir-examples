#include <stdio.h>
#include <string.h>
#include "mir.h"
#include "c2mir/c2mir.h"
#include "mir-gen.h"


#define DLIST_ITEM_FOREACH(modules, node)                     \
    for (node = DLIST_HEAD(MIR_item_t, modules->items); node; \
         node = DLIST_NEXT(MIR_item_t, node))

#define DLIST_MODULE_FOREACH(ctx, module)                                      \
    for (module = DLIST_HEAD(MIR_module_t, *MIR_get_module_list(ctx)); module; \
         module = DLIST_NEXT(MIR_module_t, module))

typedef int (*arith_func)(int, int);

typedef struct {
    char *src;
    size_t cur, size, capacity;
} buff_t;

int get_c(void *data)
{
    buff_t *item = data;
    return item->cur >= item->size ? EOF : item->src[item->cur++];
}

char str_add[] = {
    "int add(int a,int b) {"
    "return a + b;"
    "}\n"};

char str_sub[] = {
    "int sub(int a,int b) {"
    "return a - b;"
    "}\n"};

const char *cache = "cache.mirb";

void generate_cache()
{
    char *funcs[] = {
        str_add,
        str_sub,
    };
    size_t length = sizeof(funcs) / sizeof(char *);

    MIR_context_t ctx = MIR_init();
    struct c2mir_options options = {
        .message_file = stderr,
    };

    for (int i = 0; i < length; i++) {
        c2mir_init(ctx);
        MIR_gen_init(ctx, 0);
        MIR_gen_set_optimize_level(ctx, 0, 1);

        buff_t *buff = (buff_t *) malloc(sizeof(buff_t));
        memset(buff, 0, sizeof(buff_t));

        char *str_func = funcs[i];
        size_t len = strlen(funcs[i]);
        buff->src = malloc(len);
        strncpy(buff->src, str_func, len);
        buff->src = str_func;
        buff->size = len;

        if (!c2mir_compile(ctx, &options, get_c, buff, "module", NULL)) {
            assert(!"c2mir_compile");
        }

        MIR_module_t module =
            DLIST_TAIL(MIR_module_t, *MIR_get_module_list(ctx));
        MIR_item_t func_item;
        DLIST_ITEM_FOREACH(module, func_item)
        {
            if (func_item->item_type == MIR_func_item)
                break;
        }

        MIR_load_module(ctx, module);
        MIR_link(ctx, MIR_set_gen_interface, NULL);
        arith_func arith = MIR_gen(ctx, 0, func_item);
        const char *label = func_item->u.func->name;
        printf("%s(60, 10) = %d\n", label, arith(60, 10));

        c2mir_finish(ctx);
        MIR_gen_finish(ctx);
    }

    // save all modules
    printf("save %ld modules\n",
           DLIST_LENGTH(MIR_module_t, *MIR_get_module_list(ctx)));
    MIR_module_t module;
    FILE *mirb = fopen(cache, "wb");
    assert(mirb != NULL);
    MIR_write(ctx, mirb);
    fclose(mirb);
    MIR_finish(ctx);
}

int main(int argc, char **argv)
{
    generate_cache();
    MIR_context_t ctx = MIR_init();
    FILE *mirb = fopen(cache, "rb");
    assert(mirb != NULL);
    MIR_read(ctx, mirb);
    MIR_output(ctx, stderr);
    fclose(mirb);
    printf("\nloaded module length = %ld\n",
           DLIST_LENGTH(MIR_module_t, *MIR_get_module_list(ctx)));

    MIR_module_t module;
    MIR_item_t func_item;
    MIR_gen_init(ctx, 0);
    MIR_gen_set_optimize_level(ctx, 0, 1);

    DLIST_MODULE_FOREACH(ctx, module)
    {
        DLIST_ITEM_FOREACH(module, func_item)
        {
            if (func_item->item_type == MIR_func_item) {
                MIR_load_module(ctx, module);
                MIR_link(ctx, MIR_set_gen_interface, NULL);

                const char *label = func_item->u.func->name;
                arith_func arith = func_item->addr;
                assert(arith);
                printf("%s(60, 10) = %d\n", label, arith(60, 10));
            }
        }
    }

    MIR_gen_finish(ctx);
    MIR_finish(ctx);
    return 0;
}
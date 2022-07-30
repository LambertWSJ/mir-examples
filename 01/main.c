#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "c2mir/c2mir.h"
#include "mir-gen.h"
#include "mir.h"

typedef struct
{
    char *name;
    void (*func)(void);
} func_obj_t;

typedef struct jit_item
{
    char *code;
    size_t code_len;
    size_t curr;
} jit_item_t;

func_obj_t func_list[] = {
    {"printf", printf},
    {"puts", puts},
    {NULL, NULL},
};

void *import_resolver(const char *name)
{
    size_t len = sizeof(func_list) / sizeof(func_obj_t);
    for (int i = 0; func_list->name[i]; i++)
    {
        if (!strcmp(func_list[i].name, name))
            return func_list[i].func;
    }
    return NULL;
}

int get_func(void *data)
{
    jit_item_t *item = data;
    return item->curr >= item->code_len ? EOF : item->code[item->curr++];
}

int main(int argc, char **argv)
{
    MIR_context_t ctx = MIR_init();
    c2mir_init(ctx);
    MIR_gen_init(ctx, 0);
    MIR_gen_set_optimize_level(ctx, 0, 1);
    struct c2mir_options *options =
        (struct c2mir_options *)malloc(sizeof(struct c2mir_options));
    memset(options, 0, sizeof(struct c2mir_options));
    options->message_file = stderr;
    options->verbose_p = 0;
    options->module_num = 0;
    FILE *f = fopen("arith.c", "r");
    if (!f)
    {
        perror("not found file\n");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    size_t f_len = ftell(f);
    rewind(f);
    char *c_func = (char *)malloc(sizeof(char) * f_len);
    if (1 != fread(c_func, f_len, 1, f))
    {
        perror("read failure\n");
        exit(EXIT_FAILURE);
    }
    fclose(f);

    const char *name = "arithmetic";

    jit_item_t jit_item = {.code = c_func, .code_len = f_len, .curr = 0};
    jit_item_t *jit_ptr = &jit_item;

    if (!c2mir_compile(ctx, options, get_func, jit_ptr, name, NULL))
    {
        assert(!"c2mir_compile");
        exit(EXIT_FAILURE);
    }

    MIR_module_t module = DLIST_TAIL(MIR_module_t, *MIR_get_module_list(ctx));
    size_t func_len = DLIST_LENGTH(MIR_item_t, module->items);
    printf("item len = %ld\n", func_len);
    int a = 50, b = 10;

    MIR_load_module(ctx, module);
    MIR_link(ctx, MIR_set_gen_interface, import_resolver);
    puts("--------------- execute ---------------");
    MIR_item_t func = DLIST_HEAD(MIR_item_t, module->items);
    for (int i = 0; i < func_len; i++, func = DLIST_NEXT(MIR_item_t, func))
    {
        if (func->item_type == MIR_func_item)
        {
            printf("find function: ");
            int (*arich)(int, int) = MIR_gen(ctx, 0, func);
            int c = arich(a, b);
            printf("result = %d\n", c);
        }
    }
    puts("--------------------------------------");

    MIR_gen_finish(ctx);
    MIR_finish(ctx);
    return 0;
}
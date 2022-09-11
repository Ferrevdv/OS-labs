#include "kernel/types.h"
#include "user/user.h"

struct memlayout
{
    void *text;
    int *data;
    int *stack;
    int *heap;
};
int CONST = 100;

void func();
void print_struct(struct memlayout *layout);

int main(int argc, char *argv[])
{

    struct memlayout layout;

    int stack_var = 10;
    layout.stack = &stack_var;
    layout.heap = sbrk(sizeof(int));
    layout.text = (void *)func;
    layout.data = &CONST;

    print_struct(&layout);

    exit(0);
}

void func() {}

void print_struct(struct memlayout *layout)
{
    printf("%d\n", layout->text);
    printf("%d\n", layout->data);
    printf("%d\n", layout->stack);
    printf("%d\n", layout->heap);
}
#include "kernel/types.h"
#include "user/user.h"

struct memlayout
{
    void *text;
    int *data;
    int *stack;
    int *heap;
};

struct memvalues
{
    int data;
    int stack;
    int heap;
};

int CONST = 100;

int func() { return 2; }

void print_mem(const char *who, struct memlayout *layout, struct memvalues *values)
{
    printf("%s:stack:%p:%d\n", who, layout->stack, values->stack);
    printf("%s:heap:%p:%d\n", who, layout->heap, values->heap);
    printf("%s:data:%p:%d\n", who, layout->data, values->data);
    printf("%s:text:%p\n", who, layout->text);
}

int main(int argc, char *argv[])
{

    struct memlayout layout;

    int stack_var = 10;
    layout.stack = &stack_var;
    layout.heap = sbrk(sizeof(int));
    *(layout.heap) = 20;
    layout.text = (void *)func;
    layout.data = &CONST;

    int p[2]; // p[0] to read & p[1] to write
    pipe(p);
    if (fork() == 0)
    {
        close(p[0]);

        struct memlayout child_layout;
        CONST = 101;
        child_layout.data = &CONST;
        stack_var = 11;
        child_layout.stack = &stack_var;
        child_layout.heap = sbrk(sizeof(int));
        *(child_layout.heap) = 30;
        child_layout.text = (void *)func;

        struct memvalues child_values;
        child_values.data = *(child_layout.data);
        child_values.stack = stack_var;
        child_values.heap = *(child_layout.heap);

        write(p[1], &child_layout, sizeof(struct memlayout));
        write(p[1], &child_values, sizeof(struct memvalues));
        close(p[1]);
    }
    else
    {
        close(p[1]);
        struct memlayout child_layout;
        struct memvalues child_values;
        read(p[0], &child_layout, sizeof(struct memlayout));
        read(p[0], &child_values, sizeof(struct memvalues));
        close(p[0]);

        print_mem("child", &child_layout, &child_values);

        struct memvalues parent_values;
        parent_values.data = *(layout.data);
        parent_values.stack = *(layout.stack);
        parent_values.heap = *(layout.heap);
        print_mem("parent", &layout, &parent_values);
    }

    exit(0);
}

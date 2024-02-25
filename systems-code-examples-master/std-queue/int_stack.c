#include <stdio.h>
#include <stdlib.h>
#include <sys/queue.h>

typedef struct int_entry {
    int value;
    SLIST_ENTRY(int_entry) entries;
} int_entry_t;

typedef struct int_stack {
    SLIST_HEAD(stackhead, int_entry) head;
    int size;
    int capacity;
} int_stack_t;

void int_stack_init(int_stack_t *stk, int capacity) {
    SLIST_INIT(&stk->head);
    stk->size = 0;
    stk->capacity = capacity;
    printf("Initialized stack with capacity %d\n", capacity);
}

int int_stack_push(int_stack_t *stk, int value) {
    if (stk->size >= stk->capacity) {
        printf("Stack is at full capacity. Cannot push %d.\n", value);
        return 0; // fail
    }

    int_entry_t *newEntry = malloc(sizeof(int_entry_t));
    if (newEntry) {
        newEntry->value = value;
        SLIST_INSERT_HEAD(&stk->head, newEntry, entries);
        stk->size++;
        printf("Pushed %d onto the stack.\n", value);
        return 1; //success
    }
    printf("Failed to push %d onto the stack.\n", value);
    return 0; // fail
}

int int_stack_pop(int_stack_t *stk, int *top_value) {
    int_entry_t *entry = SLIST_FIRST(&stk->head);
    if (entry) {
        int value = entry->value;
        SLIST_REMOVE_HEAD(&stk->head, entries);
        free(entry);
        stk->size--;
        *top_value = value;
        printf("Popped %d from the stack.\n", value);
        return 1; // success
    }
    printf("Failed to pop from an empty stack.\n");
    return 0; // fail
}

int int_stack_top(int_stack_t *stk, int *top_value) {
    int_entry_t *entry = SLIST_FIRST(&stk->head);
    if (entry) {
        *top_value = entry->value;
        printf("The top value of the stack is %d.\n", *top_value);
        return 1; // success
    }
    printf("Failed to get the top value from an empty stack.\n");
    return 0; // fail
}

/* Functions for FORTH language stack operators */

int int_stack_dup(int_stack_t *stk) {
    if (stk->size < 1)
        return 0;
    int top_value;
    int_stack_top(stk, &top_value);
    int_stack_push(stk, top_value);
    printf("Duplicated the top value %d on the stack.\n", top_value);
    return 1; // success only if last operation succeeds
}

int int_stack_swap(int_stack_t *stk) {
    if (stk->size < 2)
        return 0;
    int top_value, next_to_top_value;
    int_stack_pop(stk, &top_value);
    int_stack_pop(stk, &next_to_top_value);
    int_stack_push(stk, top_value);
    int_stack_push(stk, next_to_top_value);
    printf("Swapped the top two values on the stack.\n");
    return 1; // success only if last operation succeeds
}

/* New stack operations */
int int_stack_over(int_stack_t *stk) {
    if (stk->size < 2)
        return 0;
    
    int_entry_t *entry = SLIST_FIRST(&stk->head);
    if (!entry || !(entry->entries.sle_next))
        return 0; // Fail if there's no second item.
    
    int second_value = entry->entries.sle_next->value;
    int_stack_push(stk, second_value);
    printf("Duplicated the second item %d on the stack.\n", second_value);
    return 1; // Success
}

int int_stack_rot(int_stack_t *stk) {
    int value1, value2, value3;

    // Check if the stack has at least three elements
    if (stk->size < 3) {
        return 0; // Fail, not enough elements to perform rot
    }

    // Pop the top three elements
    if (!int_stack_pop(stk, &value1) || !int_stack_pop(stk, &value2) || !int_stack_pop(stk, &value3)) {
        return 0; // Fail, error during pop
    }

    // Push them back in the rotated order
    int_stack_push(stk, value2); // Now the second element becomes the top
    int_stack_push(stk, value1); // Then, what was the top element
    int_stack_push(stk, value3); // Finally, the third element becomes the bottom of these three

    printf("Rotated the top three values on the stack.\n");
    return 1; // Success
}

int int_stack_drop(int_stack_t *stk) {
    if (stk->size < 1)
        return 0;
    int top_value;
    int_stack_pop(stk, &top_value);
    printf("Dropped the top value %d from the stack.\n", top_value);
    return 1;
}

int int_stack_2swap(int_stack_t *stk) {
    if (stk->size < 4)
        return 0;
    int d1, d2, d3, d4;
    int_stack_pop(stk, &d1);
    int_stack_pop(stk, &d2);
    int_stack_pop(stk, &d3);
    int_stack_pop(stk, &d4);
    int_stack_push(stk, d2);
    int_stack_push(stk, d1);
    int_stack_push(stk, d4);
    int_stack_push(stk, d3);
    printf("Swapped the top two pairs of values on the stack.\n");
    return 1;
}

int int_stack_2dup(int_stack_t *stk) {
    if (stk->size < 2)
        return 0;
    
    int top_value, second_value;
    int_stack_pop(stk, &top_value);
    int_stack_pop(stk, &second_value);
    
    // Push them back in reverse order to maintain the original stack order
    int_stack_push(stk, second_value);
    int_stack_push(stk, top_value);
    int_stack_push(stk, second_value);
    int_stack_push(stk, top_value);
    
    printf("Duplicated the top pair of values on the stack.\n");
    return 1;
}

int int_stack_2over(int_stack_t *stk) {
    // Check if the stack has at least two elements
    if (stk->size < 2) {
        return 0; // Not enough elements to perform 2OVER
    }

    int top, second;
    // Pop the top two elements
    if (!int_stack_pop(stk, &top) || !int_stack_pop(stk, &second)) {
        // Handle error if pop fails, though it should not given the size check
        return 0; // Failure
    }

    // Push the second item (the one we want to duplicate) back onto the stack
    int_stack_push(stk, second);
    // Push the top item back onto the stack to restore the original order
    int_stack_push(stk, top);
    // Push the second item again onto the top to duplicate it per 2OVER operation
    int_stack_push(stk, second);

    printf("Duplicated the second pair of values on the stack.\n");
    return 1; // Success
}

int int_stack_2drop(int_stack_t *stk) {
    if (stk->size < 2)
        return 0;
    int d1, d2;
    int_stack_pop(stk, &d1);
    int_stack_pop(stk, &d2);
    printf("Dropped the top pair of values %d and %d from the stack.\n", d1, d2);
    return 1;
}

/* Example of how to create a binary operator that works o top two elements (if present) */

int int_stack_add(int_stack_t *stk) {
    if (stk->size < 2)
        return 0;
    int top_value, next_to_top_value;
    int_stack_pop(stk, &top_value);
    int_stack_pop(stk, &next_to_top_value);
    return int_stack_push(stk, top_value + next_to_top_value);
}

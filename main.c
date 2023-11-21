#include <stdio.h>
#include <stdbool.h>
#include <assert.h>

#define HEAP_CAP 64000
#define HEAP_ALLOCED_CAP 1024

typedef struct {
    void *start;
    size_t size;
} Heap_Chunk;



char heap[HEAP_CAP] = {0};
size_t heap_size = 0;

Heap_Chunk heap_alloced[HEAP_ALLOCED_CAP] = {0};
size_t heap_alloced_size = 0;


/// @brief allocate {size} of bytes in the heap
/// @param size 
/// @return return ptr to allocated memory 
void *heap_alloc(size_t size)
{
    assert(heap_size + size <= HEAP_CAP);
    void *result = heap + heap_size;
    heap_size += size;

    const Heap_Chunk chunk = {
        .start = result,
        .size = size
    };
    assert(heap_alloced_size < HEAP_ALLOCED_CAP);
    heap_alloced[heap_alloced_size++] = chunk;

    return result;
}


void heap_dump_alloced_chunks(void)
{
    printf("Allocated Chunks: (%zu)\n", heap_alloced_size);
    for (size_t i = 0; i < heap_alloced_size; i++) {
        printf("  start: %p, size: %zu\n",
            heap_alloced[i].start,
            heap_alloced[i].size);
    }
}

/// @brief frees memory pointed to by ptr
/// @param ptr 
void heap_free(void *ptr)
{
    (void)ptr;
    assert(false && "TODO: heap_free is not implemented");
}


/// @brief frees memory in heap where nothing is refering to it
void heap_collect()
{
    assert(false && "TODO: heap_collect is not implemented");
}


int main(void)
{
    char *root = heap_alloc(26);
    for (int i = 0; i < 26; i++) {
        root[i] = 'a' + i;
    }

    heap_dump_alloced_chunks();
    return 0;
}
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <assert.h>

#define HEAP_CAP 64000
#define CHUNK_LIST_CAP 1024
#define HEAP_FREED_CAP 1024


#define UNIMPLEMENTED \
    do { \
        fprintf(stderr, "%s:%d: TODO: %s is not implemented\n", __FILE__, __LINE__, __func__); \
        abort(); \
    } while (0) \

#define SWAP(T, a, b) do { T tmp = a; a = b; b = tmp; } while (0)

typedef struct {
    void *start;
    size_t size;
} Chunk;

typedef struct {
    size_t count;
    Chunk chunks[CHUNK_LIST_CAP];
} Chunk_List;



/// @brief print Allocated chunks and their length
/// @param  
void chunk_list_dump(const Chunk_List *list)
{
    printf("Allocated Chunks: (%zu)\n", list->count);
    for (size_t i = 0; i < list->count; i++) {
        printf("  start: %p, size: %zu\n",
            list->chunks[i].start,
            list->chunks[i].size);
    }
}

int chunk_list_find(const Chunk_List *list, void *ptr)
{
    assert(list->count > 0);
    int l = 0, r = list->count - 1;
    int found = -1;
    while (l <= r) {
        int m = (l + r) / 2;
        if (list->chunks[m].start == ptr) {
            found = m;
            break;
        }
        else if (list->chunks[m].start > ptr) {
            r = m - 1;
        }
        else {
            l = m + 1;
        }
    }
    return found;
}

void chunk_list_remove(Chunk_List *list, size_t index)
{
    assert(index < list->count);
    for (size_t i = index; i < list->count - 1; i++) 
    {
        list->chunks[i] = list->chunks[i + 1];
    }
    list->count--;
}

void chunk_list_insert(Chunk_List *list, void *start, size_t size)
{
    assert(list->count < CHUNK_LIST_CAP);
    list->chunks[list->count].start = start;
    list->chunks[list->count].size  = size;
    for (size_t i = list->count;
        i > 0 && list->chunks[i].start < list->chunks[i - 1].start;
        i--)
        {
        SWAP(Chunk, list->chunks[i], list->chunks[i - 1]);
    }

    list->count++;
}

char heap[HEAP_CAP] = {0};
size_t heap_size = 0;

Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {0};


/// @brief allocate {size} of bytes in the heap
/// @param size 
/// @return return ptr to allocated memory 
void *heap_alloc(size_t size)
{
    if (size > 0) {
        assert(heap_size + size <= HEAP_CAP);
        void *ptr = heap + heap_size;
        heap_size += size;
        chunk_list_insert(&alloced_chunks, ptr, size);
        return ptr;
    }
    else {
        return NULL;
    }
}



/// @brief frees memory pointed to by ptr
/// @param ptr 
void heap_free(void *ptr)
{
    if (ptr == NULL) return;
    const int index = chunk_list_find(&alloced_chunks, ptr);   
    assert(index >= 0);
    chunk_list_insert(&freed_chunks,
        alloced_chunks.chunks[index].start, alloced_chunks.chunks[index].size);
    chunk_list_remove(&alloced_chunks, (size_t) index);
}


/// @brief frees memory in heap where nothing is refering to it
void heap_collect()
{
    UNIMPLEMENTED;
}


int main(void)
{
    for (int i = 0; i < 10; i++) {
        void *p = heap_alloc(i);
        if (i % 2 == 0) {
            heap_free(p);
        }
    }

    chunk_list_dump(&alloced_chunks);
    chunk_list_dump(&freed_chunks);

    return 0;
}
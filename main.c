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



void chunk_list_compress(Chunk_List *list)
{
    size_t i = 0;
    for (size_t j = 0; j < list->count; j++) {
        if (list->chunks[j].start == NULL) continue;
        while (i < j && list->chunks[i].start != NULL) i++;
        SWAP(Chunk, list->chunks[i], list->chunks[j]);
    }
    list->count = i + 1;
}

void chunk_list_merge(Chunk_List *list)
{
    if (list->count == 0) return;
    bool need_to_compress = false;
    for (int i = list->count - 2; i >= 0; i--) {
        Chunk chunk = list->chunks[i], next_chunk = list->chunks[i + 1];

        if (chunk.start + chunk.size == next_chunk.start) {
            list->chunks[i].size += list->chunks[i + 1].size;

            list->chunks[i + 1].start = NULL;
            list->chunks[i + 1].size = 0;

            need_to_compress = true;
        }
    }
    if (need_to_compress) {
        chunk_list_compress(list);
    }
}



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

Chunk_List alloced_chunks = {0};
Chunk_List freed_chunks = {
    .count = 1,
    .chunks = {
        [0] = {.start = heap, .size = sizeof(heap)}
    }
};


/// @brief allocate {size} of bytes in the heap
/// @param size 
/// @return return ptr to allocated memory 
void *heap_alloc(size_t size)
{
    if (size <= 0) return NULL;

    int pos = -1;
    size_t best_fit = HEAP_CAP;
    for (size_t i = 0; i < freed_chunks.count && best_fit > 0; i++) {
        if (freed_chunks.chunks[i].size - size < best_fit) {
            pos = i;
            best_fit = freed_chunks.chunks[i].size - size;
        }
    }

    if (pos == -1) {
        return NULL;
    } 

    Chunk chunk = freed_chunks.chunks[pos];

    chunk_list_insert(&alloced_chunks, chunk.start, size);
    chunk_list_remove(&freed_chunks, pos);
    if (best_fit != 0) {
        chunk_list_insert(&freed_chunks, chunk.start + size, best_fit);
    }
    
    chunk_list_merge(&freed_chunks);
    return chunk.start;
}



/// @brief frees memory pointed to by ptr
/// @param ptr 
void heap_free(void *ptr)
{
    if (ptr == NULL) return;
    const int index = chunk_list_find(&alloced_chunks, ptr);   
    assert(index >= 0);
    assert(ptr == alloced_chunks.chunks[index].start);
    chunk_list_insert(&freed_chunks,
        alloced_chunks.chunks[index].start, alloced_chunks.chunks[index].size);
    chunk_list_remove(&alloced_chunks, (size_t) index);
}


/// @brief frees memory in heap where nothing is refering to it
void heap_collect()
{
    UNIMPLEMENTED;
}

#define N 10

void *ptrs[N] = {0};

int main(void)
{
    srand(70);

    for (int i = 0; i < N; i++) {
        ptrs[i] = heap_alloc(i);
    }

    for (int i = 0; i < N; i++) {
        if (i % 2 == 0) {
            heap_free(ptrs[i]);
        }
    }
    
    chunk_list_dump(&alloced_chunks);
    chunk_list_dump(&freed_chunks);
    
    return 0;
}
#ifndef BEVY_DOG_H
#define BEVY_DOG_H

#ifdef BEVY_DOG_USE_BEVY_LOG
    #include "bevy_log.h"
#endif

#define BEVY_DOG_VERSION "1.0"

/*
    #define BEVY_DOG_USE_BEVY_LOG - Use the Bevy Logging system
    #define BEVY_DOG_QUIET - Only log any unallocations at end of program
*/

#define bevy_malloc(size)        _bevy_malloc(size, __FILE__, __LINE__)
#define bevy_calloc(num, size)   _bevy_calloc(num, size, __FILE__, __LINE__)
#define bevy_realloc(size)       _bevy_realloc(size)
#define bevy_free(size)          _bevy_free(size)

typedef struct bevy_dog_node
{
    struct bevy_dog_node *prev, 
                         *next;

    const char *file;
    size_t line;
    size_t allocation_size;
}
bevy_dog_node;

static struct bevy_dog
{
    bevy_dog_node *head;
    size_t total_allocation_size;
} 
bevy_dog;

void *_bevy_malloc(size_t size, const char *file, size_t line)
{
    bevy_dog_node *node = malloc(sizeof(*node) + size);
    if(node != NULL)
    {
        memset(node, 0, sizeof(*node));
    }
    else
    {
        #ifdef BEVY_DOG_USE_BEVY_LOG
            bevy_log_fatal("Could not malloc memory.");
        #else
            // printf
        #endif
        return NULL;
    }

    node->allocation_size = size;
    node->line = line;
    node->file = file;
    bevy_dog.total_allocation_size += size;

    if(bevy_dog.head)
    {
        bevy_dog.head->prev = node;
        node->next = bevy_dog.head;
    }
    bevy_dog.head = node;

    #ifndef BEVY_DOG_QUIET
        #ifdef BEVY_DOG_USE_BEVY_LOG
            bevy_log_trace("Allocated: %d bytes of memory", size);
        #else
            // printf
        #endif
    #endif
    return (char *)node + sizeof(*node);
}

void *_bevy_calloc(size_t num, size_t size, const char *file, size_t line)
{
    bevy_dog_node *node = calloc(num, sizeof(*node) + size);
    
    if(node == NULL)
    {
        bevy_log_fatal("Could not calloc memory.");
        return NULL;
    }

    node->allocation_size = size;
    node->line = line;
    node->file = file;

    bevy_dog.total_allocation_size += size;

    if(bevy_dog.head)
    {
        bevy_dog.head->prev = node;
        node->next = bevy_dog.head;
    }
    bevy_dog.head = node;

    #ifndef BEVY_DOG_QUIET
        #ifdef BEVY_DOG_USE_BEVY_LOG
            bevy_log_trace("Allocated: %d bytes of memory", size);
        #else
            // printf
        #endif
    #endif

    return (char *)node + sizeof(*node);
}

void *_bevy_realloc(void *ptr, size_t new_size)
{
    return realloc(ptr, new_size);
}

void _bevy_free(void *ptr)
{
    bevy_dog_node *node = (bevy_dog_node *)((char *)ptr - sizeof(*node));
    if(ptr == NULL)
    {
        return;
    }
    if(node == bevy_dog.head)
    {
        bevy_dog.head = node->next;
    }
    if(node->prev)
    {
        node->prev->next = node->next;
    }
    if(node->next)
    {
        node->next->prev = node->prev;
    }

    bevy_dog.total_allocation_size -= node->allocation_size;
    #ifndef BEVY_DOG_QUIET
        #ifdef BEVY_DOG_USE_BEVY_LOG
            bevy_log_trace("%d bytes of memory deallocated", 
                            node->allocation_size);
        #else
            // printf
        #endif
    #endif
    
    free(node);
}


void bevy_analyze()
{
    bevy_dog_node *node = bevy_dog.head;

    while(node != NULL)
    {
        bevy_log_error("Unfreed Block: %p allocated at %s:%d", 
                      (char*)node + sizeof(*node), node->file, node->line);

        node = node->next;
    }

    if(bevy_dog.total_allocation_size > 0)
    {
        bevy_log_error("Program ended with %lu byte(s) of unfreed memory.",
                      bevy_dog.total_allocation_size);
    }
}

#endif
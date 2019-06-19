#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtable.h"
#include "cache.h"

/**
 * Allocate a cache entry
 */
struct cache_entry *alloc_entry(char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    struct cache_entry *ce = malloc(sizeof *ce);

    ce->content = malloc(content_length);
    memcpy(ce->content, content, content_length);

    ce->path = path;
    ce->content_type = content_type;
    ce->content_length = content_length;

    ce->prev = NULL;
    ce->next = NULL;

    return ce;
}

/**
 * Deallocate a cache entry
 */
void free_entry(struct cache_entry *entry)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    free(entry->content);
    // free(entry->path);
    // free(entry->content_type);
    free(entry);
}

/**
 * Insert a cache entry at the head of the linked list
 */
void dllist_insert_head(struct cache *cache, struct cache_entry *ce)
{
    // Insert at the head of the list
    if (cache->head == NULL)
    {
        cache->head = cache->tail = ce;
        ce->prev = ce->next = NULL;
    }
    else
    {
        cache->head->prev = ce;
        ce->next = cache->head;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Move a cache entry to the head of the list
 */
void dllist_move_to_head(struct cache *cache, struct cache_entry *ce)
{
    if (ce != cache->head)
    {
        if (ce == cache->tail)
        {
            // We're the tail
            cache->tail = ce->prev;
            cache->tail->next = NULL;
        }
        else
        {
            // We're neither the head nor the tail
            ce->prev->next = ce->next;
            ce->next->prev = ce->prev;
        }

        ce->next = cache->head;
        cache->head->prev = ce;
        ce->prev = NULL;
        cache->head = ce;
    }
}

/**
 * Removes the tail from the list and returns it
 * 
 * NOTE: does not deallocate the tail
 */
struct cache_entry *dllist_remove_tail(struct cache *cache)
{
    struct cache_entry *oldtail = cache->tail;

    cache->tail = oldtail->prev;
    cache->tail->next = NULL;

    cache->cur_size--;

    return oldtail;
}

/**
 * Create a new cache
 * 
 * max_size: maximum number of entries in the cache
 * hashsize: hashtable size (0 for default)
 */
struct cache *cache_create(int max_size, int hashsize)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    struct cache *newCache = malloc(sizeof(struct cache));
    newCache->index = hashtable_create(max_size, NULL);
    newCache->head = NULL;
    newCache->tail = NULL;
    newCache->max_size = max_size;
    newCache->cur_size = hashsize;

    return newCache;
}

void cache_free(struct cache *cache)
{
    struct cache_entry *cur_entry = cache->head;

    hashtable_destroy(cache->index);

    while (cur_entry != NULL)
    {
        struct cache_entry *next_entry = cur_entry->next;

        free_entry(cur_entry);

        cur_entry = next_entry;
    }

    free(cache);
}

/**
 * Store an entry in the cache
 *
 * This will also remove the least-recently-used items as necessary.
 * 
 * NOTE: doesn't check for duplicate cache entries
 */
void cache_put(struct cache *cache, char *path, char *content_type, void *content, int content_length)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    // Allocate a new cache entry with the passed parameters.
    struct cache_entry *ce = alloc_entry(path, content_type, content, content_length);

    // Insert the entry at the head of the doubly-linked list
    dllist_insert_head(cache, ce);
    // Store the entry in the hashtable as well, indexed by the entry's path.
    hashtable_put(cache->index, path, ce);

    // Increment the current size of the cache.
    cache->cur_size++;

    // If the cache size is greater than the max size:
    // Remove the cache entry at the tail of the linked list.
    if (cache->cur_size > cache->max_size)
    {
        struct cache_entry *ce = dllist_remove_tail(cache);

        // Remove that same entry from the hashtable, using the entry's path and the hashtable_delete function.
        hashtable_delete(cache->index, ce->path);
        // Free the cache entry.
        free_entry(ce);
        // Ensure the size counter for the number of entries in the cache is correct.
    }
}

/**
 * Retrieve an entry from the cache
 */
struct cache_entry *cache_get(struct cache *cache, char *path)
{
    ///////////////////
    // IMPLEMENT ME! //
    ///////////////////

    struct cache_entry *ce = hashtable_get(cache->index, path);

    if (ce == NULL)
    {
        return NULL;
    }

    dllist_move_to_head(cache, ce);

    return ce;
}

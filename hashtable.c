#include <stdio.h>
#include <stdlib.h>
#include "linkedlist.h"
#include "hashtable.h"

struct hashtable {
    // TODO: define hashtable struct to use linkedlist as buckets
    linkedlist_t **buckets;
    int num_buckets;
    int num_mappings;

};

/**
 * Hash function to hash a key into the range [0, max_range)
 */
static int hash(int key, int max_range) {
    // TODO: feel free to write your own hash function (NOT REQUIRED)
    key = (key > 0) ? key : -key;
    return key % max_range;
}

hashtable_t *ht_init(int num_buckets) {
    // TODO: create a new hashtable
    hashtable_t *table = malloc(sizeof(hashtable_t));
    table->num_buckets = num_buckets;
    table->num_mappings = 0;
    table->buckets = malloc(num_buckets * sizeof(linkedlist_t *));
    for (int i = 0; i < num_buckets; i++) {
        table->buckets[i] = ll_init();
    }
    return table;
}

void ht_add(hashtable_t *table, int key, int value) {
    // TODO: create a new mapping from key -> value.
    // If the key already exists, replace the value.
    int bucket = hash(key, table->num_buckets);
    if(ll_get(table->buckets[bucket], key) != 0){
        ll_add(table->buckets[bucket], key, value);
        return;
    }
    ll_add(table->buckets[bucket], key, value);
    table->num_mappings++;

}

int ht_get(hashtable_t *table, int key) {
    // TODO: retrieve the value mapped to the given key.
    // If it does not exist, return 0
    int bucket = hash(key, table->num_buckets);
    return ll_get(table->buckets[bucket], key);
}

int ht_size(hashtable_t *table) {
    // TODO: return the number of mappings in this hashtable
    return table->num_mappings;
}

/* Implements the abstract hash table. 
 * Author: Kenny Corman 
 * This hashtable is implemented with seperate chaining
 */
#include <assert.h>
#include <stdlib.h>

#include "hash.h"

//Initial size
#define INITIAL_CAPACITY 7
//Growth Factor
#define RESIZE_FACTOR 2
//Maximum free bucket to element ratio before resize
#define MAX_FILLED_RATIO 0.5

/* Using an entry of this type is just a suggestion. You're
 * free to change this. */
typedef struct _hash_entry {
  void* key;
  void* value;
} hash_entry;

/* A simple linked list node used for a seperate chaining hashtable */
typedef struct _link_node_ {
  hash_entry entry;
  struct _link_node_ *next;
} link_node;

/* A Bucket is just a pointer to a link node. */
typedef struct _bucket_ {
  link_node *head;  
} bucket;

/* The hashtable type contains a pointer to the supplied hasher function, 
 * a pointer to the supplied comparison function,
 * and a list of buckets that makes up the actual data structure
 * It also contains the number of entries in the table
 */
struct _hash_table {
  hash_hasher hasher_func;
  hash_compare compare_func;
  bucket *bucket_list;
  size_t num_buckets;
  size_t size;
};

static link_node *push_node(bucket *bucket, hash_entry entry);
static link_node *find_node(hash_table *ht, bucket *bucket, const void *key);
static bucket *get_bucket(hash_table *ht, const void *key);
static bool resize_table(hash_table *ht);

/* See hash.h for documentation */
hash_table* hash_create(hash_hasher hash_hasher, hash_compare hash_compare){
  hash_table *table = (hash_table*) malloc(sizeof(hash_table));
  if(table == NULL) return NULL;
  table->hasher_func = hash_hasher;
  table->compare_func = hash_compare;
  table->bucket_list = (bucket*) malloc(INITIAL_CAPACITY * sizeof(bucket));
  table->num_buckets = INITIAL_CAPACITY;
  table->size = 0;
  if(table-> bucket_list == NULL){
    free(table);
    return NULL;
  }
  //initialize the bucket array to NULL so we know if they are valid or not later
  for(int i = 0;i<table->num_buckets;i++){
    table->bucket_list[i].head = NULL;
  }
  return table;
}

/* see hash.h for documentation */
void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr){
  hash_entry entry;
  entry.key = key;
  entry.value = value;
  bucket *buck = get_bucket(ht, key);
  link_node *target = find_node(ht, buck, key);
  if(target == NULL){
    //node is not in the bucket, so create it and append it to the bucket
    target = push_node(buck, entry);
    //Increase size ct
    ht->size++;
    //check if we need to resize table and do so if needed
    if(((double)ht->size)/((double)ht->num_buckets) > MAX_FILLED_RATIO){
      resize_table(ht); 
    }
    *removed_value_ptr = NULL;
    *removed_key_ptr = NULL;
  }else{
    //node IS in the bucket; return original values and reassign
    *removed_value_ptr = target->entry.value;
    *removed_key_ptr = target->entry.key;
    target->entry = entry; 
  }
  assert(target != NULL);
}

/* see hash.h for documentation */
bool hash_lookup(hash_table* ht, const void* key, void** value_ptr){ 
  assert(ht != NULL);
  bucket *bucket = get_bucket(ht, key);
  link_node *target = find_node(ht, bucket, key);
  if(target == NULL){
    //key is not in the table 
    *value_ptr = NULL;
    return false;
  }else{
    *value_ptr = target->entry.value;
    return true;
  }
}


/* see hash.h for documentation */
bool hash_is_present(hash_table* ht, const void* key){
  void *unused_val_ptr;
  return hash_lookup(ht, key, &unused_val_ptr);
}


/* see hash.h for documentation */
bool hash_remove(hash_table* ht, const void* key,
                 void** removed_key_ptr, void** removed_value_ptr){
  
  bucket *bucket = get_bucket(ht, key);
  link_node *target = find_node(ht, bucket, key);
  if(target == NULL) return false;
  //remove target and return its key/value
  *removed_key_ptr = target->entry.key;
  *removed_value_ptr = target->entry.value;
  //remove target from the bucket
  if(bucket->head == target){
    link_node *tgt_next = target->next;
    free(target);
    bucket->head = tgt_next;
    return true;
  }
  //target bucket is NOT the head
  for(link_node *current = bucket->head; current->next != target; current = current->next){
    assert(current != NULL);
    current->next = target->next;
    free(target);
  }
  return true; 
}

/* see hash.h for documentation */
void hash_destroy(hash_table* ht, bool free_keys, bool free_values){
  for(int i = 0;i<ht->num_buckets;i++){
    if(ht->bucket_list[i].head == NULL) continue;
    link_node *current = ht->bucket_list[i].head;
    while(current != NULL){
      //free up buckets
      link_node *next = current->next;
      if(free_keys) free(current->entry.key);
      if(free_values) free(current->entry.value);
      free(current);
      current = next;
    }
  }  
  //finally free ht itself
  free(ht);
}



static link_node *push_node(bucket *bucket, hash_entry entry){
  //Push a new node onto our linked list
  link_node *current = (link_node *) malloc(sizeof(link_node));
  current->next = bucket->head;
  current->entry = entry;
  bucket->head = current;
  return current;
}

//Returns a pointer to the node with the given key if it exists in the bucket
//Otherwise returns NULL
static link_node *find_node(hash_table *ht, bucket *bucket, const void *key){
  assert(bucket != NULL);
  link_node *current = bucket->head;  
  while(current != NULL){
    if(ht->compare_func(key, current->entry.key) == 0){
      return current;
    }
    current = current->next;
  }
  return NULL;
}

/* Returns the bucket that the given key maps to */
static bucket *get_bucket(hash_table *ht, const void *key){
  uint64_t hash_val = ht->hasher_func(key);
  hash_val = hash_val%(ht->num_buckets);
  return ht->bucket_list+hash_val;
}

/* Private function used for growing the table size.
 * returns true upon success, false upon failure (due to malloc failure)
 * num_buckets will increase, as will the actual size of the buckets array
 * The buckets themselves will be filled again via rehashing the old table
 * The old table's bucket list is freed, so it should not be referenced anymore
 */
static bool resize_table(hash_table *ht){
  size_t original_size = ht->size;  //this should not change
  bucket *old_buckets = ht->bucket_list;
  size_t num_old_buckets = ht->num_buckets;
  ht->num_buckets *= RESIZE_FACTOR;
  ht->size = 0;
  bucket *new_buckets = (bucket *) malloc(sizeof(bucket) * ht->num_buckets);
  if(new_buckets == NULL) return false;
  ht->bucket_list = new_buckets;
  //initialize all buckets to null
  for(int i = 0;i<ht->num_buckets;i++){
    ht->bucket_list[i].head = NULL;
  }
  //now ht is essentially an empty hashtable, but since we have a reference to the old
  //list of buckets we can reinsert everything in those old buckets
  void *unused_key_ptr;
  void *unused_value_ptr;
  for(size_t i = 0;i<num_old_buckets;i++){
    if(old_buckets[i].head == NULL) continue;
    link_node *current = old_buckets[i].head;
    while(current != NULL){
      //insert this entry into our ht
      hash_insert(ht, current->entry.key, current->entry.value, 
          &unused_key_ptr, &unused_value_ptr);
      //free up buckets
      link_node *next = current->next; 
      free(current);
      current = next;
    }
  }  
  free(old_buckets);
  assert(ht->size == original_size);
  return true;
}

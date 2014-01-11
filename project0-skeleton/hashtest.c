#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "hash.h"

static const size_t kBufferLength = 32;
static const uint32_t kMaxInsertions = 100000;
static const char kNotFoundKey[] = "not-found key";

/* Matches the hash_hasher definition in hash.h: */
static uint64_t hash_fn(const void* k) {
  uint64_t hash_val = 0;
  uint64_t coefficient = 1;

  for (const char* p = (const char*) k; *p != '\0'; p++) {
    hash_val += coefficient * (*p);
    coefficient *= 37;
  }

  return hash_val;
}

static void additional_tests();

/* Matches the hash_compare definition in hash.h. This function compares
 * two keys that are strings. */
static int hash_strcmp(const void* k1, const void* k2) {
  return strcmp((const char*) k1, (const char*) k2);
}

int main(int argc, char* argv[]) {
  /* Check for correct invocation: */
  if (argc != 2) {
    printf("Usage: %s <N>\n"
        "Run test inserting a total of N items\n", argv[0]);
    return 1;
  }
  int N = atoi(argv[1]);
  if (N <= 0 || N > kMaxInsertions) {
    N = kMaxInsertions;
  }
  /* Create the hash table. */
  hash_table* ht = hash_create(hash_fn, hash_strcmp);

  /* First phase: insert some data. */
  printf("\nInsert phase:\n");
  char* k;
  int64_t* v;
  char* removed_key = NULL;
  int64_t* removed_value = NULL;
  for (int i = 0; i < N * 2; i++) {
    k = (char*) malloc(kBufferLength);
    snprintf(k, kBufferLength, "String %d", i % N);
    v = (int64_t*) malloc(sizeof(int64_t));
    *v = i;
    // The hash map takes ownership of the key and value:
    hash_insert(ht, k, v, (void**) &removed_key, (void**) &removed_value);
    if (removed_value != NULL) {
      printf("Replaced (%s, %" PRIi64 ") while inserting (%s, %" PRIi64 ")\n",
             removed_key, *removed_value, k, *v);
      free(removed_key);
      free(removed_value);
    } else {
      printf("Inserted (%s, %" PRIi64 ")\n", k, *v);
    }
  }

  /* Second phase: look up some data. */
  printf("\nLookup phase:\n");
  char strbuf[kBufferLength];
  for (int i = N - 1; i >= 0; i--) {
    snprintf(strbuf, kBufferLength, "String %d", i);
    if (!hash_lookup(ht, strbuf, (void**) &v)) {
      printf("Entry for %s not found\n", strbuf);
    } else {
      printf("%s -> %" PRIi64 "\n", strbuf, *v);
    }
  }

  /* Look up a key that hasn't been inserted: */
  if (!hash_lookup(ht, kNotFoundKey, (void**) &v)) {
    printf("Lookup of \"%s\" failed (as expected)\n", kNotFoundKey);
  } else {
    printf("%s -> %" PRIi64 " (unexpected!)\n", kNotFoundKey, *v);
  }

  /* Destroy the hash table and free things that we've allocated. Because
   * we allocated both the keys and the values, we instruct the hash map
   * to free both.
   */
  hash_destroy(ht, true, true);
  additional_tests();
  return 0;
}

//Assumes the key is a pointer to an integer
static uint64_t int_hash_func(const void *key){
  int prime = 31;
  return (uint64_t) (*(int *)key)*prime;
}

//assumes the key is a poitner to an integer
static int int_compare_func(const void *key1, const void *key2){
  int res = *(int *)key1 - *(int *)key2;
  if(res < 0) return -1;
  if(res == 0) return 0;
  return 1;
}

//returns a new hash table that uses integer keys and integer values
static hash_table *get_int_ht(){
  hash_table *ht = hash_create(&int_hash_func, &int_compare_func);
  return ht;
}

//Tries inserting a few values and ensuring that they are in the table
static void insert_test(){
  hash_table *ht = get_int_ht();
  int *old_key_ptr = NULL;
  int *old_val_ptr = NULL;
  int key1 = 7;
  int val1 = 93;
  hash_insert(ht, (void *) &key1, (void *)&val1, (void **)&old_key_ptr, (void **)&old_val_ptr);
  assert(old_key_ptr == NULL);
  assert(hash_is_present(ht, (void *)&key1));
  int key2 = -54;
  int val2 = 902943;
  hash_insert(ht, (void *)&key2, (void *)&val2, (void **)&old_key_ptr, (void **)&old_val_ptr);
  assert(old_key_ptr == NULL);
  assert(hash_is_present(ht, (void *)&key2)); 
  assert(hash_is_present(ht, (void *)&key1));
  //replace key1
  hash_insert(ht, (void *)&key1, (void *)&val2, (void **)&old_key_ptr, (void **)&old_val_ptr);
  assert(*old_key_ptr == key1);
  assert(*old_val_ptr == val1);
  assert(hash_is_present(ht, (void *)&key1));
  //lookup key 1
  hash_lookup(ht, (void *)&key1, (void **)&old_val_ptr);
  assert(*old_val_ptr == val2);
  hash_destroy(ht,false,false);
  printf("insert test successful.\n");
}

//inserts some values and them removes them
static void remove_test(){
  hash_table *ht = get_int_ht();
  int *old_key_ptr = NULL;
  int *old_val_ptr = NULL;
  int key1 = 7;
  int val1 = 93;
  hash_insert(ht, (void *) &key1, (void *)&val1, (void **)&old_key_ptr, (void **)&old_val_ptr);
  assert(hash_is_present(ht, (void *)&key1));
  int key2 = -54;
  int val2 = 902943;
  hash_insert(ht, (void *)&key2, (void *)&val2, (void **)&old_key_ptr, (void **)&old_val_ptr);
  assert(hash_is_present(ht, (void *)&key2)); 
  assert(hash_is_present(ht, (void *)&key1));
  assert(hash_remove(ht, (void *) &key1, (void **)&old_key_ptr, (void **)&old_val_ptr));
  assert(*old_key_ptr == key1);
  assert(*old_val_ptr == val1);
  assert(hash_is_present(ht, (void *)&key2)); 
  assert(!hash_is_present(ht, (void *)&key1)); 
  hash_destroy(ht, false, false);
  printf("remove test successful.\n");
}

static void additional_tests(){
  insert_test();
  remove_test();
}




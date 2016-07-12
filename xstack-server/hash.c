#include <stdlib.h>
#include <stdio.h>
#include "hash.h"


kv_pair *hash[HASH_SIZE];

int get_block(int key)
{
    return key % HASH_SIZE;
}


int get_value(int key)
{
    int block;

    block = get_block(key);

    while(hash[block])
    {
        if (hash[block]->key == key)
            return hash[block]->value;

        block++;
        block %= HASH_SIZE;
    }

    perror("key does not exist in dictionary!");
    exit(1);
}


void insert(int key, int value)
{
    int block;
    kv_pair *item;

    item = malloc(sizeof(kv_pair));
    item->key = key;
    item->value = value;

    block = get_block(key);

    while(hash[block])
    {
        if(hash[block]->key == key)
        {
            free(hash[block]);
            break;
        }

        block++;
        block %= HASH_SIZE;
    }

    hash[block] = item;
}

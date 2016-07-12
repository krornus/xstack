#define HASH_SIZE 250

int get_block(int key);
int get_value(int key);
void insert(int key, int value);

typedef struct kv_pair_s {
    int key;
    int value;
} kv_pair;


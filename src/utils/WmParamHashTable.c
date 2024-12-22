#include <stdlib.h>
#include <stdio.h>
#include "WmParamHashTable.h"

int getHash(int key){
    return (key % MAX_WM_PARAM_AMOUNT);
}

HashTable_t * WmParamHashTable_Init(){
    HashTable_t * hastable = (HashTable_t*) calloc(1, sizeof(HashTable_t));

    if(!hastable){
        return NULL;
    }

    return hastable;
}

int WmParamHashTable_Insert(HashTable_t * hashtable, int key, MessageHandler_t handler){
    if(!hashtable){
        return -1;
    }

    if(hashtable->size >= 128){
        printf("WmParamHashTable Overflow");
        return -1;
    }

    int idx = getHash(key);

    while(hashtable->content[idx].key != 0){
        if(hashtable->content[idx].key == key){
            hashtable->content[idx].handler = handler;
            return 0;
        }
    
        idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;

    } 

    hashtable->content[idx].key = key;
    hashtable->content[idx].handler = handler;

    hashtable->size++;

    return 0;
}

MessageHandler_t WmParamHashTable_Get(HashTable_t * hashtable, int key){

    if(!hashtable){
        return NULL;
    }

    int idx = getHash(key);
    int counter = 0;

    while(hashtable->content[idx].key != key){
        idx = (idx + 1) % MAX_WM_PARAM_AMOUNT;
        counter++;

        if (counter > (MAX_WM_PARAM_AMOUNT - 1)){
            return NULL;
        }
    }

    return hashtable->content[idx].handler;

}

void WmParamHashTable_Destroy(HashTable_t * hashtable){
    free(hashtable);
}

/**
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 * 
 * Rules:
 * -don't change speller.c
 * -altering dictionary.h/.c is okay
 * -don't change declarations for load, check, size, or unload
 * -altering makefile is okay
 * -you can add functions to dict.h, as long as make compiles it still
 * -check should be CASE INSENSITIVE
 * -check should only return true for words in the dict
 * -only possessives allowed are those in the dict
 * -dict structure is assumed to be same: lexographic, end in \n
 * -no word will be longer than LENGTH
 * -you can research hash functions, as long as you cite the origin
 */

#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "dictionary.h"

unsigned int num_words = 0;

typedef struct node
{
    bool is_end;
    struct node* children[27];
}node;

// generate the root of the TRIE
node* root[26];

/**
 * Returns the bucket identity, so to speak, of the current letter
 */
int hash(char first){
    
    int hash_id;
    
    if(first == 39)
        hash_id = 26;
    else if(first > 96)
        hash_id = first % 97;
    else
        hash_id = first % 65;
    
    return hash_id;
}

// Check if the end of the word matches
bool check_end(const char* word, node* curr_pt){
    
    if(curr_pt -> is_end == true)
        return true;
    else
        return false;
}

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    node* curr_pt;
    int curr_bucket = hash(word[0]);
    
    curr_pt = root[curr_bucket];
    
    if(curr_pt == NULL)
        return false;
    
    // is first letter even in the TRIE's root
    
    int i = 1;
    
    // check all letters after second letter
    while(word[i]!='\0'){
        
        // find hash bucket of current letter
        curr_bucket = hash(word[i]);
        
        // is the pointer to that bucket NULL? If yes, we don't have a matching word, if no, keep checking
        if(curr_pt -> children[curr_bucket] == NULL)
            return false;
        else
            curr_pt = curr_pt -> children[curr_bucket];
            
        i++;
        
    }
    
    // check if exact word is a match
    if(curr_pt -> is_end == true)
        return true;
    else
        return false;
}


/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
 
 /**
  * Hash tables are an array in which each array element contains a linked
  * list. A hash function returns the array index that an item belongs to.
  * Implement a TRIE datastructure to organize the loaded dictionary words.
  */
bool load(const char* dictionary)
{
    // open dictionary file for reading
    FILE* fp = fopen(dictionary,"r");
    node* curr_pt;
    
    if(fp == NULL){
        printf("Couldn't open the dictionary\n");
        return false;
    }
    // dictionary is structured s.t. each line has one word ending with \n
    // words are composed of alphabetic characters and apostrophes
    
    char word[46];
    int curr_bucket;
    
    // while there are still words left to be read, read a word
    while(fscanf(fp,"%s",word)!=EOF){
        
        // print the word you just stored
        //printf("DEBUG: %s was read\n",word);
        
        // initialize pointer to root bucket
        curr_bucket = hash(word[0]);
        
        if(root[curr_bucket] == NULL)
            root[curr_bucket] = malloc(sizeof(node));
            
        curr_pt = root[curr_bucket];
        
        
        // while not at the end of the word
        for(int i = 1; word[i] != '\0'; i++){
            
            // check which bucket this new letter belongs to
            curr_bucket = hash(word[i]);
            
            // does this bucket have any memory? If not, give it some. If it does, point to this new bucket.
            if(curr_pt->children[curr_bucket] == NULL)
                curr_pt->children[curr_bucket] = malloc(sizeof(node));
            
            curr_pt = curr_pt->children[curr_bucket];
            
        
        }
        
        num_words++;
        // you're at the end of the word. last pointer should be at the last bucket. Set the "end" bool to TRUE
        curr_pt -> is_end = true;
        
    }
    
    fclose(fp);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return num_words;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 * 
 * While children of current node are not all null, point to first non-null node
 * When you've finally found a node whose children are all null, go up a level, delete it, and start over
 * 
 */

// Checks if children of current pointee are all null
bool null_children(node* curr_pt){
    
    for(int i = 0; i < 27; i++){
        if(curr_pt->children[i] == NULL)
            ;
        else
            return false;
    }
    
    return true;
}

unsigned int first_nonnull_child(node* curr_pt){
    
    for(int i = 0; i < 27; i++){
        if(curr_pt->children[i] != NULL)
            return i;
    }
    
    return 50;
    
}

unsigned int first_nonnull_bucket(void){
    
    for(int i = 0; i < 27; i++){
        if(root[i] != NULL)
            return i;
    }
    
    return 50;
    
}


bool unload(void)
{
    node* curr_pt;
    node* last_pt;
    unsigned int curr_bucket;
    bool root_flag;
    
    // for each branch of root...
    while(first_nonnull_bucket() != 50){
    
        // find the first non-null hash bucket and point to it
        curr_bucket = first_nonnull_bucket();
        curr_pt = root[curr_bucket];
        last_pt = root[curr_bucket];
        
        root_flag = true;
        
        /**if(null_children(curr_pt)){
            free(root[curr_bucket]);
            root[curr_bucket] = NULL;
            goto start;
        }
        */
        
        // finds low
        while(!null_children(curr_pt) && root_flag == true){
            
            root_flag = false;
            
            // find first non-null child
            curr_bucket = first_nonnull_child(curr_pt);
            
            // point to that non-null child
            last_pt = curr_pt;
            curr_pt = curr_pt -> children[curr_bucket];
            
            // memory is freed, set
            
        }
        
        // current pointee's children are all null
        free(curr_pt);
        last_pt->children[curr_bucket] = NULL;
        
        if(root_flag == true){
            root[curr_bucket] = NULL;
        }
        
        
    }
    
    return true;
}

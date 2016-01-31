/* anagram.c This version reads the two dictionary files
 * into arrays and used binary search to look for matches. */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>


#define MAXWORDLEN 30
/* File to use for missing letters. */
#define DICTIONARY "words_by_length.txt"
/* File to use for anagrams. */
#define ANAGRAMS "anag_sorted.txt"

struct anag {
    char *word;
    char *sort;
};

struct missing {
    char *word;
    int length;
};

int bSearch(struct anag **arr, char *key, int len);
int checkword(char *word);
int findAnag(struct anag **arr, char *word, int len);
int findMiss(struct missing **arr, char *word, int len);
int ismatch(char *pat, char *str);
int lenbSearch(struct missing **arr, int key, int len);
int readAnag(struct anag **arr);
int readMissing(struct missing **arr);
void sortletters(char *word);
int test(struct anag **arr, int len);

/* Read in words, search for matches. */
int main(void)
{
    char word[MAXWORDLEN + 1], format[10];
    int i, found, lenAnag, lenMiss;
 
    /* Pointers to store start of struct arrays. */
    struct anag *anagArr = NULL;
    struct missing *missArr = NULL;

    /* Make a format string based on MAXWORDLEN. */
    sprintf(format, "%%%ds", MAXWORDLEN);
    printf("*** Anagram Solver ***\n\n");
    printf("To find a word with missing letters instead,\n");
    printf("use a . to stand for the unknown letter(s).\n");

    /* Read in the words. */
    lenAnag = readAnag(&anagArr);
    lenMiss = readMissing(&missArr);

    while (1) {
        /* Get user's word. */
        printf("\nEnter your word (or use 'q' to quit): ");
        scanf(format, word);

        if (!(strcmp("q", word)))
            break;

        /* Reset counter. */ 
        found = 0;
        
        /* Change the word to lower case & check for invalid characters. 
         * Return value of checkword() tells us what to do next. */
        switch (checkword(word)) {
            /* -1 means we do nothing. */
            case -1:
                printf("Invalid word.\n");
                found = -1;
                break;
            /* Zero means we look for an anagram. */
            case 0:
                found = findAnag(&anagArr, word, lenAnag);
                break;
            /* One means look for word with missing letters. */
            case 1:
                found = findMiss(&missArr, word, lenMiss);
                break;
            case 2:
                found = test(&anagArr, lenAnag);
                break;
            case 3:
                break;
            default:
                /* Shouldn't ever get here. */
                break;
        }
        if (!found)
            printf("No matches found.\n"); 
    }
    
    /* Free struct arrays. */
    for (i = 0; i < lenAnag; i++){
        free(anagArr[i].word);
        free(anagArr[i].sort);
    }
    free(anagArr);

    for (i = 0; i < lenMiss; i++)
        free(missArr[i].word);
    free(missArr);

    return 0;
}

/* Read in the anagrams dictionary. Returns number of entries. */
int readAnag(struct anag **arr)
{
    int i, length = 0;
    size_t m, size = 1;
    char *buf = NULL;

    FILE *f = fopen(ANAGRAMS, "r");
    assert(f != NULL);

    /* Prepare the struct array for getline. */
    *arr = malloc(sizeof(struct anag));
    assert(*arr != NULL);
    
    while ((i = getline(&buf, &m, f)) != -1) {
        /* Remove newline. */
        buf[--i] = '\0';

        /* Make i equal to the length of the word.*/
        i /= 2;

        /* Resize if necessary. */
        if (size == length) {
            size *= 2;
            *arr = realloc(*arr, size * sizeof(struct anag));
            assert(*arr != NULL);
        }

        /* Make space for strings. */
        (*arr)[length].word = malloc(i + 1);
        assert((*arr)[length].word != NULL);
        (*arr)[length].sort = malloc(i + 1);
        assert((*arr)[length].sort != NULL);

        /* Copy strings. */
        strcpy((*arr)[length].word, &buf[i]);
        strncpy((*arr)[length].sort, buf, (size_t)i);

        /* Must add null terminator to this string. */ 
        (*arr)[length].sort[i] = '\0';
        
        /* Increment length of array. */
        length++;
    }
    
    fclose(f);
    free(buf);
    return length;
}

/* Read in dictionary sorted by length. */
int readMissing(struct missing **arr)
{
    int i, length = 0;
    size_t m, size = 1;
    char *buf = NULL;

    FILE *f = fopen(DICTIONARY, "r");
    assert(f != NULL);

    /* Prepare the struct array for getline. */
    *arr = malloc(sizeof(struct missing));
    assert(*arr != NULL);

    while ((i = getline(&buf, &m, f)) != -1) {
        /* Remove newline. */
        buf[--i] = '\0';

        /* Resize if necessary. */
        if (size == length) {
            size *= 2;
            *arr = realloc(*arr, size * sizeof(struct missing));
            assert(*arr != NULL);
        }

        /* Make space for string. */
        (*arr)[length].word = malloc(i + 1);
        assert((*arr)[length].word != NULL);

        /* Copy string.*/
        strcpy((*arr)[length].word, buf);
        (*arr)[length].length = i;

        /* Increment length of array. */
        length++;
    }
    
    fclose(f);
    free(buf);
    return length;
}

/* Use insertion sort to sort letters in a word. */
void sortletters(char *word)
{
    int t, i, j;

    /* Loop will stop when word[i] is '\0'. */
    for (i = 1; word[i]; i++){
        for (j = i, t = word[j]; j && word[j - 1] > t; j--)
            word[j] = word[j-1];
        word[j] = t;
    }
}


/* Check the pattern against the string. */
int ismatch(char *pat, char *str)
{
    while(*pat){
        if (*pat != *str && *pat != '.')
            return 0;
        *pat++;
        *str++;
    }
    return 1;
}

/* Change the string to lower case. Return 0 if only letters found.
 * Return 1 if '.' found. */
int checkword(char *w)
{
    int dot = 0;
    char c;
    if (!(strcmp("t", w)))
        return 2;

    while (*w){
        c = tolower(*w);
        if (c == '.')
            dot = 1;
        else if (c < 'a' || c > 'z')
            return -1;
        *w = c;
        w++;
    }
    return dot;
}

/* Use binary search to get first match (if any), then print them. */ 
int findAnag(struct anag **arr, char *word, int len)
{
    int index, found = 0;
    char *sorted = malloc(strlen(word) + 1);
    assert(sorted != NULL);
    strcpy(sorted, word);
    sortletters(sorted);

    /* Get index of first occurence. */
    index = bSearch(arr, sorted, len);

    if (index != -1){
        while (!(strcmp((*arr)[index].sort, sorted))){
            if (strcmp((*arr)[index].word, word)){
                if (!found){
                    found = 1;
                    printf("%s: ", word);
                }
                printf("%s ", (*arr)[index].word);
            }
            index++;
        }
    }
        free(sorted);
        return found;
    }

/* Use binary search to find the first word of the same length
 * as the user's word. Then look for and print matches. */
int findMiss(struct missing **arr, char *word, int len)
{
    int found = 0;
    int keyLen = strlen(word);
    int index = lenbSearch(arr, keyLen, len);
    if (index != -1){
        while ((*arr)[index].length == keyLen) {
            if (ismatch(word, (*arr)[index].word)){
                if(!found){
                    found = 1;
                    printf("%s: ", word);
                }
                printf("%s ",(*arr)[index].word);  
            }
            index++;
        }
    }
    return found;
}

/* Find first record in the array that matches the key. */
int bSearch(struct anag **arr, char *key, int len)
{
    int mid, lo, hi;
    for (lo = 0, hi = len - 1; lo < hi; ){
        /* Set mid using right shift. */
        mid = (lo + hi) >> 1;
        if (strcmp((*arr)[mid].sort, key) >= 0)
            hi = mid;
        else if (strcmp((*arr)[mid].sort, key) < 0)
            lo = mid + 1;
    }
    return (strcmp((*arr)[hi].sort, key)) ? -1 : hi;
}

/* Find first record in the array that has length "key". */ 
int lenbSearch(struct missing **arr, int key, int len)
{
    int mid, lo, hi;
    for(lo = 0, hi = len -1; lo < hi;) {
        mid = (lo + hi) >> 1;
        if ((*arr)[mid].length >= key)
            hi = mid;
        else if ((*arr)[mid].length < key)
            lo = mid + 1;
    }
    return ((*arr)[hi].length != key) ? -1 : hi;
}

/* Test function that prints all words that are anagrams. */
int test(struct anag **arr, int len)
{
    int i, j, n;
    
    for(i = 0; i < len;) {
        j = i + 1;
        n = 0;
        while(j < len && !(strcmp((*arr)[i].sort, (*arr)[j].sort))){
            if (!n){
                n = 1;
                printf("\n%s: ", (*arr)[i].word);
            }
            printf("%s ", (*arr)[j++].word);
        }
        i = j;
    }
    return 1;
}

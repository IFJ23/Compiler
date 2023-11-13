#ifndef IFJ_STRING_H
#define IFJ_STRING_H

#define INIT_ALLOC_SIZE 10

#include <stdbool.h>

typedef struct {
    char *value; // retezec
    int length; // delka retezce
    int lengthAllocated; // alokovana delka
} string;

int stringInit(string *);
int stringAddChar(string *, char);
int stringAddChars(string *str, char* c);
void stringUpdateLastChar(string *, char);
void stringDeleteLastChar(string *);
int stringAddFirstChar(string *, char);
int stringClear(string *);
int stringLenght(string *);
bool stringCompare(string *a, string *b);

void stringDispose(string *str);
//Pomocne funkce pro generovani unikatnich navesti
char stringGetCharOnIndex(string *, int);

void stringUpdateCharOnIndex(string *str, char c, int index);

void stringToLowercase(string *);

/* ----------------------------------------POMOCNE FUNKCE PRO SCANNER-------------------------------------------------*/
int stringIsKeyWord(string *);
int stringIsResKeyWord(string *);
char stringGetLastChar(string *);

/* ----------------------------------------FUNKCE PRO PRACI SE ZNAKY--------------------------------------------------*/
void charUndo(char);
int charIsSpace(char);
int charIsTab(char);
int charIsDigit(char);
int charIsLetter(char);
int charToDec(char);
char decToChar(int);

#endif //IFJ_STRING_H
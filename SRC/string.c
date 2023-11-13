#include "string.h"
#include "error_code.h"
#include <stdlib.h>
#include <string.h>

/*
 * Inicializace stringu
 */
int stringInit(string *str) {
    // alokovani pameti
    if ( (str->value = (char*) malloc(INIT_ALLOC_SIZE * sizeof(char))) == NULL )
        return ERROR_CODE_INTERNAL;
    str->length = 0;
    str->value[0] = '\0';
    str->lengthAllocated = INIT_ALLOC_SIZE;
    // pokud vse probehlo v poradku
    return ERROR_CODE_OK;
}

int stringLenght(string *str){
    /*
    char *retezec = str->value;
    unsigned int i = 0;
    while(retezec[i] != '\0')
        i++;
    return i;
    */
    return strlen(str->value);
}

bool stringCompare(string *str1, string *str2){
    if ( strcmp(str1->value, str2->value) == 0 )
        return true;
    else
        return false;
    /*
    char *a = str1->value; char *b = str2->value;
    int i = 0;
    while(i < stringLenght(str1))
        if(a[i] != b[i])
            return false;
        else
            i++;
    return true;
    */
}

/*
 * Vlozeni znaku na konec stringu
 */
int stringAddChar(string *str, char c) {
    // pokud neni misto pro dalsi znak
    if (str->length+1 >= str->lengthAllocated) {
        // realokace pameti: navyseni o INIT_ALLOC_SIZE
        if ((str->value = (char *) realloc(str->value, str->lengthAllocated + INIT_ALLOC_SIZE * sizeof(char))) == NULL)
            return ERROR_CODE_INTERNAL;
        str->lengthAllocated = str->lengthAllocated + INIT_ALLOC_SIZE;
    }
    // pridani znaku na konec retezce
    str->value[str->length] = c;
    str->value[str->length+1] = '\0';
    str->length++;
    // pokud vse probehlo v poradku
    return ERROR_CODE_OK;
}

/*
 * Vlozeni retezce na konec stringu
 */
int stringAddChars(string *str, char* c) {
    int result_code;
    for (unsigned int i = 0; i < strlen(c); i++) {
        result_code = stringAddChar(str, c[i]);
    }
    return result_code;
}

/*
 * Zmena hodnoty posledniho znaku stringu
 */
void stringUpdateLastChar(string *str, char c) {
    if (str->length > 0) {
        str->value[str->length-1] = c;
    }
}

void stringUpdateCharOnIndex(string *str, char c, int index) {
    if ((str->length > 0) && (str->length >= index)) {
        str->value[index - 1] = c;
    }
}

/*
 * Odstraneni posledniho znaku stringu
 */
void stringDeleteLastChar(string *str) {
    str->value[str->length-1] = '\0';
    str->length -= 1;
}

/*
 * Vlozeni znaku na zacatek stringu
 */

int stringAddFirstChar(string * str, char c) {
    // pokud neni misto pro dalsi znak
    if (str->length+1 >= str->lengthAllocated) {
        // realokace pameti: navyseni o INIT_ALLOC_SIZE
        if ((str->value = (char *) realloc(str->value, str->lengthAllocated + INIT_ALLOC_SIZE * sizeof(char))) == NULL)
            return ERROR_CODE_INTERNAL;
        str->lengthAllocated = str->lengthAllocated + INIT_ALLOC_SIZE;
    }
    // posunuti retezce o jeden znak: vytvoreni mista pro zapsani prvniho znaku
    for (int i=str->length; i>=0; i--) {
        str->value[i+1] = str->value[i];
    }
    // pridani znaku na zacatek retezce
    str->value[0] = c;
    str->length++;
    // pokud vse probehlo v poradku
    return ERROR_CODE_OK;
}

/*
 * Vyprazdneni stringu
 */
int stringClear(string *str) {
    // smaze znaky v retezci
    for (int i = 0; i < str->lengthAllocated; i++) {
        str->value[i] = '\0';
    }
    str->length = 0;
    // pokud vse probehlo v poradku
    return ERROR_CODE_OK;
}

/*
 * Vraceni posledniho znaku stringu
 */
char stringGetLastChar(string *str) {
    if (str->length > 0)
        return str->value[str->length-1];
    else
        return -1;
}

char stringGetCharOnIndex(string *str, int index) {
    if ((str->length > 0) && (str->length >= index))
        return str->value[index - 1];
    else
        return -1;
}


/*
 * Zjisteni, zda string je klicove slovo
 */
int stringIsKeyWord(string *str) {
    char *keywords[] = {
            "as\0", /*"asc\0" ,*/ "declare\0", "dim\0", "do\0", "double\0", "else\0", "end\0",/* "chr\0",*/
            "function\0", "if\0", "input\0", "integer\0", /*"length\0",*/ "loop\0", "print\0", "return\0",
            "scope\0", "string\0",/* "substr\0",*/ "then\0" , "while\0"
    };
    unsigned int keywordsLength = sizeof(keywords) / sizeof(keywords[0]); // pocet prvku v poli keywords
    // porovnani tokenu s klicovymi slovy
    for (unsigned int i=0; i<keywordsLength; i++) {
        if ( strcmp(keywords[i], str->value) == 0 )
            return ERROR_CODE_TRUE;
    }
    return ERROR_CODE_FALSE;
}

/*
 * Zjisteni, zda string je rezervovane klicove slovo
 */
int stringIsResKeyWord(string *str) {
    char *resKeywords[] = {
            "and\0", "boolean\0", "continue\0", "elseif\0", "exit\0", "false\0", "for\0", "next\0",
            "not\0", "or\0", "shared\0", "static\0", "true\0"
    };
    unsigned int resKeywordsLength = sizeof(resKeywords) / sizeof(resKeywords[0]); // pocet prvku v poli keywords
    // porovnani tokenu s klicovymi slovy
    for (unsigned int i=0; i<resKeywordsLength; i++) {
        if ( strcmp(resKeywords[i], str->value) == 0 )
            return ERROR_CODE_TRUE;
    }
    return ERROR_CODE_FALSE;
}

/*
 * Prevod stringu na mala pismena
 */
void stringToLowercase(string *str) {
    // prevod na lowercase
    for (int i=0; i<str->length; i++) {
        if ( (str->value[i] >= 65) && (str->value[i] <= 90) ) {
            str->value[i] = str->value[i] + 32;
        }
    }
}

/*
 * Uvolneni stringu
 */
void stringDispose(string *str) {
    free(str->value);
    str->value = NULL;
    str->length = str->lengthAllocated = 0;
}

/* ----------------------------------------FUNKCE PRO PRACI SE ZNAKY--------------------------------------------------*/

/*
 * Vrati znak do stdin
 */
void charUndo(char c) {
    ungetc(c, stdin);
}

/*
 * Zjisteni, jestli znak je mezera
 */
int charIsSpace(char c) {
    if (c == ' ')
        return ERROR_CODE_TRUE;
    else
        return ERROR_CODE_FALSE;
}

/*
 * Zjisteni, jestli znak je tabulator
 */
int charIsTab(char c) {
    if ( c == '\t' )
        return ERROR_CODE_TRUE;
    else
        return ERROR_CODE_FALSE;
}

/*
 * Zjisteni, jestli znak je cislice
 */
int charIsDigit(char c) {
    if (c >= '0' && c <= '9')
        return ERROR_CODE_TRUE;
    else
        return ERROR_CODE_FALSE;
}

/*
 * Zjisteni, jestli znak je pismeno
 */
int charIsLetter(char c) {
    if ( (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') )
        return ERROR_CODE_TRUE;
    else
        return ERROR_CODE_FALSE;
}

/*
 * Vraci ascii hodnotu znaku
 */
int charToDec(char c) {
    return c-48;
}

/*
 * Vraci znak s ascii hodnotou 'c'
 */
char decToChar(int c) {
    return c+48;
}


// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Vsevolod Pokhvalenko (xpokhv00)

#ifndef _STACK_H_
#define _STACK_H_

#include <stdio.h>
#include <stdbool.h>

#define MAX_STACK 20

extern int STACK_SIZE;

/** Globální proměnná - indikuje, zda operace volala chybu. */
extern bool error_flag;
/** Globální proměnná - indikuje, zda byla operace řešena. */
extern bool solved;

/** Celkový počet možných chyb. */
#define MAX_SERR    3
/** Chyba při Stack_Init. */
#define SERR_INIT   1
/** Chyba při Stack_Push. */
#define SERR_PUSH   2
/** Chyba při Stack_Top. */
#define SERR_TOP    3

/** ADT zásobník implementovaný v dynamickém poli. */
typedef struct {
    /** Pole pro uložení hodnot. */
    char *array;
    /** Index prvku na vrcholu zásobníku. */
    int topIndex;
} Stack;

void Stack_Error( int );

void Stack_Init( Stack * );

bool Stack_IsEmpty( const Stack * );

bool Stack_IsFull( const Stack * );

void Stack_Top( const Stack *, char * );

void Stack_Pop( Stack * );

void Stack_Push( Stack *, char );

void Stack_Dispose( Stack * );

#endif
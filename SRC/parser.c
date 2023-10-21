#include "parser.h"

Scanner scanner;

int main(){

    //run parsing
    statement_list();
}

void statement_list(){
    Token token;
    int result = get_token(&scanner, &token);
    assert(result == EXIT_SUCCESS);
    fprintf(stderr,"Token type is: %i\n",token.type);
    fprintf(stderr,"Token line is: %i\n",token.line);
}

void statement(){

}
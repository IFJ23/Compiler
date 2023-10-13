// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Ivan Onufriienko (xonufr00)

#include "scanner.h"



int get_token(Scanner *scanner, Token *token){
    while(true){
        int c = fgetc(scanner->file);
        if (c == EOF){
        token->type = TYPE_EOF;
        return EXIT_SUCCESS;
        }
        if(isspace(c)){
            continue;
        }
        if(c == '\n'){
            scanner->line++;
            continue;
        }
        

    }
}
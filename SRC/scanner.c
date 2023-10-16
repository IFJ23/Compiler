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
        
        switch(c){
            case '+':
                token->type = TYPE_PLUS;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case '-':
                int c2 = fgetc(scanner->file);
                if(c2 == '>'){
                    token->type = TYPE_RETURN_ARROW;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_MINUS;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }             
            case '*':
                token->type = TYPE_MUL;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case '/':
            int c2 = fgetc(scanner->file);
                if(c2 == '/'){
                    while(c2 != '\n'){
                        c2 = fgetc(scanner->file);
                        if (c2 == EOF) {
                            token->type = TYPE_EOF;
                            token->line = scanner->line;
                            return EXIT_SUCCESS;
                        }
                    }
                    break;
                }
                else if(c2 == '*'){
                    while(true){
                        c2 = fgetc(scanner->file);
                        if(c2 == EOF){
                            exit(LEXICAL_ERROR);
                        }
                        if(c2 == '\n'){
                            scanner->line++;
                        }
                        if(c2 == '*'){
                            c2 = fgetc(scanner->file);
                            if(c2 == EOF){
                                exit(LEXICAL_ERROR);
                            }
                            if(c2 == '/'){
                                break;
                            }
                            else{
                                if(c2 == '\n') {
                                scanner->line += 1;
                                }
                            }
                        }
                    }
                }
                else{
                ungetc(c2, scanner->file);
                token->type = TYPE_DIV;
                token->line = scanner->line;
                return EXIT_SUCCESS;
                }
            case '<':
                c2 = fgetc(scanner->file);
                if(c2 == '='){
                    token->type = TYPE_LESS_EQUAL;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_LESS;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            case '>':
                c2 = fgetc(scanner->file);
                if(c2 == '='){
                    token->type = TYPE_MORE_EQUAL;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_MORE;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            case '=':
                c2 = fgetc(scanner->file);
                if(c2 == '='){
                    token->type = TYPE_EQUAL;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_ASSIGN;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            case '!':
                c2 = fgetc(scanner->file);
                if(c2 == '='){
                    token->type = TYPE_NOT_EQUAL;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_FACTORIAL;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            case '(':
                token->type = TYPE_LEFT_BRACKET;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case ')':
                token->type = TYPE_RIGHT_BRACKET;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case ',':
                token->type = TYPE_COMMA;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case ':':
                token->type = TYPE_COLON;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case '{':
                token->type = TYPE_LEFT_CURLY_BRACKET;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case '}':
                token->type = TYPE_RIGHT_CURLY_BRACKET;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            case '?':
                c2 = fgetc(scanner->file);
                if(c2 == '?'){
                    token->type = TYPE_NIL_COALESCING_OPERATOR;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
                else{
                    ungetc(c2, scanner->file);
                    token->type = TYPE_NIL_ASSIGNER;
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            case '"':
                int c2 = fgetc(scanner->file);
                if(c2 == '"'){
                    int c3 = fgetc(scanner->file);
                    if(c3 == '"'){
                        token->type = TYPE_MULTILINE_STRING;
                        token->line = scanner->line;
                        //todo
                    }
                    else{
                        ungetc(c3, scanner->file);                       
                        token->type = TYPE_STRING;
                        token->value.string = "";
                        token->line = scanner->line;
                        return EXIT_SUCCESS;
                    }
                }
                int counter = 0;
                int size = 20;
                char *string = malloc(sizeof(char) * size);
                if(string == NULL){
                    exit(INTERNAL_ERROR);
                }
                while(c2 != '"'){
                    if(c2 == EOF){
                        exit(LEXICAL_ERROR);
                    }
                    if(counter == size){
                        size *= 2;
                        string = realloc(string, sizeof(char) * size);
                        if(string == NULL){
                            exit(INTERNAL_ERROR);
                        }
                    }
                    string[counter] = c2;
                    counter++;
                    c2 = fgetc(scanner->file);
                }
                string[counter] = '\0';
                token->type = TYPE_STRING;
                token->value.string = string;
                token->line = scanner->line;
                return EXIT_SUCCESS;
                
        }
        if(isdigit(c)){
            int counter = 0;
            int size = 20;
            char *number = malloc(sizeof(char) * size);
            if(number == NULL){
                exit(INTERNAL_ERROR);
            }
            // TODO
        }
        if((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_'){
            int counter = 0;
            int size = 20;
            char *id = malloc(sizeof(char) * size);
            if(id == NULL){
                exit(INTERNAL_ERROR);
            }
            while((isalnum(c) || c == '_') && c != EOF){
                if(counter == size){
                    size *= 2;
                    id = realloc(id, sizeof(char) * size);
                    if(id == NULL){
                        exit(INTERNAL_ERROR);
                    }
                }
                id[counter] = c;
                counter++;
                c = fgetc(scanner->file);
            }
            ungetc(c, scanner->file);
            id[counter] = '\0';
            if(keyword_from_token(token, id) == EXIT_SUCCESS){
                token->line = scanner->line;
                free(id);
                return EXIT_SUCCESS;
            }
            token->type = TYPE_ID;
            token->value.id = id;
            token->line = scanner->line;
            return EXIT_SUCCESS;
        }

    }
}

int keyword_from_token(Token *token, char *c){
    if(strcmp(c, "double") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_DOUBLE;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "else") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_ELSE;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "func") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_FUNC;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "if") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_IF;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "int") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_INT;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "let") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_LET;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "nil") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_NIL;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "return") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_RETURN;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "string") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_STRING;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "var") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_VAR;
        return EXIT_SUCCESS;
    }
    if(strcmp(c, "while") == 0){
        token->type = TYPE_KW;
        token->value.kw = KW_WHILE;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}
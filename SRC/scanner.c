// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
/**
 * @file scanner.c
 * @brief Compiler to IFJ23 language
 * @author Ivan Onufriienko (xonufr00)
 */
#include "scanner.h"

int get_token(Scanner *scanner, Token *token)
{
    while (true)
    {
        int c = fgetc(scanner->file);

        if (c == EOF)               // End of file
        {
            token->type = TYPE_EOF;
            return EXIT_SUCCESS;
        }

        if (isspace(c))             // Whitespaces
        {
            if (c == '\n')
            {
                token->type = TYPE_EOL;
                token->line = scanner->line;
                scanner->line++;
                return EXIT_SUCCESS;
            }
            else
            {
                continue;
            }
        }

        if (isdigit(c))             // Numbers
        {
            int counter, exponent, dot, sign;
            counter = exponent = dot = sign = 0;
            int size = 20;
            char *number = malloc(sizeof(char) * size);

            if (number == NULL)
            {
                exit(INTERNAL_ERROR);
            }

            while (isdigit(c) || c == '.' || c == 'e' || c == 'E' || c == '+' || c == '-')  // Float or double
            {
                if (counter == size)
                {
                    size *= 2;
                    number = realloc(number, sizeof(char) * size);

                    if (number == NULL)
                    {
                        exit(INTERNAL_ERROR);
                    }
                }

                number[counter] = c;
                counter++;
                c = fgetc(scanner->file);

                if ((number[counter - 1] >= '0' && number[counter - 1] <= '9') && (c == '+' || c == '-'))
                {
                    break;
                }

                if (!isdigit(c) && number[counter - 1] == '.')
                {
                    token->type = TYPE_ERROR;
                    token->line = scanner->line;
                    free(number);
                    printError(scanner->line, "Lexical error: Missing number after dot");
                    return LEXICAL_ERROR;
                }
            }

            ungetc(c, scanner->file);
            number[counter] = '\0';

            if (!isdigit(number[counter - 1]))
            {
                token->type = TYPE_ERROR;
                token->line = scanner->line;
                free(number);
                printError(scanner->line, "Lexical error: Missing number after sign or exponent");
                return LEXICAL_ERROR;
            }

            counter = 0;
            while (number[counter] != '\0') // Check if the number is valid
            {

                if (number[counter] == '.')
                {

                    if (exponent > 0)
                    {
                        token->type = TYPE_ERROR;
                        token->line = scanner->line;
                        free(number);
                        printError(scanner->line, "Lexical error: Dot cannot be after exponent");
                        return LEXICAL_ERROR;
                    }

                    dot++;
                }

                if (strchr(number, 'e') != NULL || strchr(number, 'E') != NULL)
                {
                    exponent++;
                }

                if (strchr(number, '+') != NULL || strchr(number, '-') != NULL)
                {
                    sign++;
                }
                counter++;
            }

            if (dot == 0 && exponent == 0 && sign == 0)
            {
                token->type = TYPE_INT;
                token->value.integer = strtoll(number, NULL, 10);
                token->line = scanner->line;
                free(number);
                return EXIT_SUCCESS;
            }
            else if ((sign == 0 && exponent == 0 && dot == 1) || (sign == 0 && exponent == 1 && dot == 0) ||
                     (sign == 0 && exponent == 1 && dot == 1) || (sign == 1 && exponent == 1 && dot == 0) || (sign == 1 && exponent == 1 && dot == 1))
            {
                token->type = TYPE_DOUBLE;
                token->value.decimal = strtod(number, NULL);
                token->line = scanner->line;
                free(number);
                return EXIT_SUCCESS;
            }
            else
            {
                token->type = TYPE_ERROR;
                token->line = scanner->line;
                free(number);
                printError(scanner->line, "Lexical error: Invalid number");
                return LEXICAL_ERROR;
            }
        }
        if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_')   // Keywords and identifiers
        {
            int counter = 0;
            int size = 20;
            char *id = malloc(sizeof(char) * size);

            if (id == NULL)
            {
                exit(INTERNAL_ERROR);
            }

            while ((isalnum(c) || c == '_' || c == '?') && c != EOF)    // Main cycle
            {

                if (counter == size)
                {
                    size *= 2;
                    id = realloc(id, sizeof(char) * size);

                    if (id == NULL)
                    {
                        exit(INTERNAL_ERROR);
                    }
                }

                id[counter] = c;
                counter++;

                if (c == '?')
                {
                    c = fgetc(scanner->file);
                    break;
                }

                c = fgetc(scanner->file);
            }

            ungetc(c, scanner->file);
            id[counter] = '\0';

            if (keyword_from_token(token, id) == EXIT_SUCCESS)  // Check for possible keywords
            {
                token->line = scanner->line;
                free(id);
                return EXIT_SUCCESS;
            }

            if (id[counter - 1] == '?')
            {
                token->type = TYPE_ERROR;
                token->line = scanner->line;
                free(id);
                printError(scanner->line, "Lexical error: This is not a keyword that is nullable");
                return LEXICAL_ERROR;
            }

            // Check the context to distinguish between function and variable identifiers
            bool isFunction = false; 

            // Check if the next character is '(' to indicate a function identifier
            int nextChar = getc(scanner->file);
            if (nextChar == '(')
            {
                isFunction = true; 
            }
            ungetc(nextChar, scanner->file); 

            if (isFunction)
            {
                token->type = TYPE_IDENTIFIER_FUNC;
            }
            else
            {
                token->type = TYPE_IDENTIFIER_VAR;
            }
            token->value.id = id;
            token->line = scanner->line;
            return EXIT_SUCCESS;
        }

        int c2;
        switch (c)
        {

        case '+':
            token->type = TYPE_PLUS;
            token->line = scanner->line;
            return EXIT_SUCCESS;

        case '-':
            c2 = fgetc(scanner->file);

            if (c2 == '>')
            {
                token->type = TYPE_RETURN_ARROW;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
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
            c2 = fgetc(scanner->file);

            if (c2 == '/')  // single line comment
            {

                while (c2 != '\n')
                {
                    c2 = fgetc(scanner->file);

                    if (c2 == EOF)
                    {
                        token->type = TYPE_EOF;
                        token->line = scanner->line;
                        return EXIT_SUCCESS;
                    }
                }

                scanner->line++;

                break;
            }

            else if (c2 == '*') // multiline comment
            {

                int nested = 0; // nested comments

                while (true)
                {
                    c2 = fgetc(scanner->file);

                    if (c2 == EOF)
                    {
                        printError(scanner->line, "Lexical error: Missing closing comment");
                        exit(LEXICAL_ERROR);
                    }

                    else if (c2 == '\n')
                    {
                        scanner->line++;
                    }
                    else if (c2 == '/')
                    {
                        c2 = fgetc(scanner->file);

                        if (c2 == EOF)
                        {
                            printError(scanner->line, "Lexical error: Missing closing comment");
                            exit(LEXICAL_ERROR);
                        }

                        else if (c2 == '*')
                        {
                            int c3 = fgetc(scanner->file);
                            if (c3 == '/')
                            {
                                if (nested == 0)
                                {
                                    break;
                                }
                                else
                                {
                                    nested--;
                                }
                            }
                            else
                            {
                                nested++;
                                ungetc(c3, scanner->file);
                            }
                        }

                        else
                        {
                            ungetc(c2, scanner->file);
                        }
                    }
                    else if (c2 == '*')
                    {
                        c2 = fgetc(scanner->file);

                        if (c2 == EOF)
                        {
                            printError(scanner->line, "Lexical error: Missing closing comment");
                            exit(LEXICAL_ERROR);
                        }

                        else if (c2 == '/')
                        {
                            if (nested == 0)
                            {
                                break;
                            }
                            else
                            {
                                nested--;
                            }
                        }
                        else
                        {
                            if (c2 == '\n')
                            {
                                scanner->line += 1;
                            }
                        }
                    }
                }
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_DIV;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            break;

        case '<':
            c2 = fgetc(scanner->file);

            if (c2 == '=')
            {
                token->type = TYPE_LESS_EQUAL;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_LESS;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }

        case '>':
            c2 = fgetc(scanner->file);

            if (c2 == '=')
            {
                token->type = TYPE_MORE_EQUAL;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_MORE;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }

        case '=':
            c2 = fgetc(scanner->file);

            if (c2 == '=')
            {
                token->type = TYPE_EQUAL;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_ASSIGN;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }

        case '!':
            c2 = fgetc(scanner->file);

            if (c2 == '=')
            {
                token->type = TYPE_NOT_EQUAL;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_EXCLAMATION_MARK;
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

            if (c2 == '?')
            {
                token->type = TYPE_NIL_COALESCING_OPERATOR;
                token->line = scanner->line;
                return EXIT_SUCCESS;
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_ERROR;
                token->line = scanner->line;
                printError(scanner->line, "Lexical error: There is no such operator as '?'");
                return LEXICAL_ERROR;
            }

        case '"':
            c2 = fgetc(scanner->file);

            if (c2 == '"')
            {
                int c3 = fgetc(scanner->file);

                if (c3 == '"')
                {
                    token->type = TYPE_MULTILINE_STRING;
                    token->line = scanner->line;
                    
                }
                else
                {
                    ungetc(c3, scanner->file);
                    token->type = TYPE_STRING;
                    token->value.string = "";
                    token->line = scanner->line;
                    return EXIT_SUCCESS;
                }
            }
            else
            {
                ungetc(c2, scanner->file);
                token->type = TYPE_STRING;
                token->line = scanner->line;
            }

            int counter = 0;
            int size = 20;
            char *string = malloc(sizeof(char) * size);

            if (string == NULL)
            {
                exit(INTERNAL_ERROR);
            }

            while (true)
            {
                c2 = fgetc(scanner->file);

                if (counter == size)
                {
                    size *= 2;
                    string = realloc(string, sizeof(char) * size);

                    if (string == NULL)
                    {
                        exit(INTERNAL_ERROR);
                    }
                }

                string[counter] = c2;
                counter++;

                if (c2 < 32 || c2 > 255)    // Check for invalid characters
                {
                    if (token->type == TYPE_MULTILINE_STRING)
                    {
                        continue;
                    }
                    else
                    {
                        token->type = TYPE_ERROR;
                        token->line = scanner->line;
                        free(string);
                        printError(scanner->line, "Lexical error: Invalid character in string");
                        return LEXICAL_ERROR;
                    }
                }
                else if (c2 == '\\')    // Check for escape sequences
                {
                    c2 = fgetc(scanner->file);

                    if (c2 == 'n')
                    {
                        string[counter - 1] = '\n';
                    }
                    else if (c2 == 'r')
                    {
                        string[counter - 1] = '\r';
                    }
                    else if (c2 == 't')
                    {
                        string[counter - 1] = '\t';
                    }
                    else if (c2 == '"')
                    {
                        string[counter - 1] = '"';
                    }
                    else if (c2 == '\\')
                    {
                        string[counter - 1] = '\\';
                    }
                    else if (c2 == 'u')     // Unicode escape sequence
                    {
                        int c3 = fgetc(scanner->file);

                        if (c3 == '{')
                        {
                            int c4 = fgetc(scanner->file);

                            if (isalnum(c4))
                            {
                                int c5 = fgetc(scanner->file);

                                if (isalnum(c5))
                                {
                                    int c6 = fgetc(scanner->file);

                                    if (c6 == '}')
                                    {
                                        char hex[] = {c4, c5};
                                        int number = strtol(hex, NULL, 16);
                                        string[counter - 1] = number;
                                    }
                                    else
                                    {
                                        token->type = TYPE_ERROR;
                                        token->line = scanner->line;
                                        free(string);
                                        printError(scanner->line, "Lexical error: Invalid unicode escape sequence");
                                        return LEXICAL_ERROR;
                                    }
                                }
                                else
                                {
                                    token->type = TYPE_ERROR;
                                    token->line = scanner->line;
                                    free(string);
                                    printError(scanner->line, "Lexical error: Invalid unicode escape sequence");
                                    return LEXICAL_ERROR;
                                }
                            }
                            else
                            {
                                token->type = TYPE_ERROR;
                                token->line = scanner->line;
                                free(string);
                                printError(scanner->line, "Lexical error: Invalid unicode escape sequence");
                                return LEXICAL_ERROR;
                            }
                        }
                        else
                        {
                            token->type = TYPE_ERROR;
                            token->line = scanner->line;
                            free(string);
                            printError(scanner->line, "Lexical error: Invalid unicode escape sequence");
                            return LEXICAL_ERROR;
                        }
                    }
                    else
                    {
                        token->type = TYPE_ERROR;
                        token->line = scanner->line;
                        free(string);
                        printError(scanner->line, "Lexical error: Invalid unicode escape sequence");
                        return LEXICAL_ERROR;
                    }
                }
                else if (c2 == '"')    // Check for closing quote
                {

                    if (token->type == TYPE_MULTILINE_STRING)
                    {
                        int c3 = fgetc(scanner->file);

                        if (c3 == '"')
                        {
                            int c4 = fgetc(scanner->file);

                            if (c4 == '"')
                            {
                                token->line = scanner->line;
                                free(string);
                                return EXIT_SUCCESS;
                            }
                            else
                            {
                                ungetc(c4, scanner->file);
                                token->type = TYPE_ERROR;
                                token->line = scanner->line;
                                free(string);
                                printError(scanner->line, "Lexical error: Invalid multiline string");
                                return LEXICAL_ERROR;
                            }
                        }
                        else
                        {
                            ungetc(c3, scanner->file);
                            token->type = TYPE_ERROR;
                            token->line = scanner->line;
                            free(string);
                            printError(scanner->line, "Lexical error: Invalid multiline string");
                            return LEXICAL_ERROR;
                        }
                    }

                    string[counter - 1] = '\0';
                    token->value.string = string;
                    return EXIT_SUCCESS;
                }
                else if (c2 == EOF)
                {
                    token->type = TYPE_ERROR;
                    token->line = scanner->line;
                    free(string);
                    printError(scanner->line, "Lexical error: Missing closing quote");
                    return LEXICAL_ERROR;
                }
            }
        default:
            token->type = TYPE_ERROR;
            token->line = scanner->line;
            printError(scanner->line, "Lexical error: Invalid character");
            return LEXICAL_ERROR;
        }
    }
}

int keyword_from_token(Token *token, char *c)
{

    if (strcmp(c, "Double") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_DOUBLE;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "else") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_ELSE;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "func") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_FUNC;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "if") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_IF;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "Int") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_INT;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "let") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_LET;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "nil") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_NIL;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "return") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_RETURN;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "String") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_STRING;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "var") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_VAR;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "while") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_WHILE;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "Int?") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_UNDEFINED_INT;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "Double?") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_UNDEFINED_DOUBLE;
        return EXIT_SUCCESS;
    }

    if (strcmp(c, "String?") == 0)
    {
        token->type = TYPE_KW;
        token->value.kw = KW_UNDEFINED_STRING;
        return EXIT_SUCCESS;
    }

    return EXIT_FAILURE;
}

int peek_token(Scanner *scanner, Token *token)
{
    long int file_pos = ftell(scanner->file);   // Store current file position

    int result = get_token(scanner, token);     // Get the next token

    fseek(scanner->file, file_pos, SEEK_SET);   // Restore file position
    return result;                              // Return the result of get_token
}
// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Oleksii Shelest (xshele02)

#include "stdio.h"
#include "generator.h"

extern Parser parser;

void generatorHead()
{
    // Initialize the IFJcode23 header for the code generation
    printf(".IFJcode23\n");

    // Declare and initialize global variable for the function name
    printf("DEFVAR GF@$$funcname\n");
    printf("MOVE GF@$$funcname string@main\n");

    // Declare and initialize global variable for the main function result
    printf("DEFVAR GF@$$mainresult\n");
    printf("MOVE GF@$$mainresult nil@nil\n");

    // Declare and initialize global variable for operator usage
    printf("DEFVAR GF@$$operator\n");
    printf("MOVE GF@$$operator nil@nil\n");

    // Declare and initialize global variable for jump equality (je) instruction
    printf("DEFVAR GF@$$je\n");

    // Declare and initialize global variables for parameter types
    printf("DEFVAR GF@$$param$$type$$1\n");
    printf("MOVE GF@$$param$$type$$1 nil@nil\n");
    printf("DEFVAR GF@$$param$$type$$2\n");
    printf("MOVE GF@$$param$$type$$2 nil@nil\n");
    printf("DEFVAR GF@$$param$$type$$opt\n");
    printf("MOVE GF@$$param$$type$$opt nil@nil\n");

    // Create and push a new frame for the main function
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generatorStackPush(Token t)
{
    switch (t.type)
    {
        case TYPE_STRING:
        {
            printf("PUSHS string@");

            int i = 0;

            while (t.value.string[i] != '\0')
            {
                char currentChar = t.value.string[i];

                if (currentChar < 33 || currentChar == 35 || currentChar == 92 || currentChar > 126)
                {
                    putchar('\\');

                    if (currentChar < 0)
                        currentChar += 256;

                    printf("%03d", currentChar);
                }
                else
                {
                    putchar(currentChar);
                }

                i++;
            }

            putchar('\n');
            break;
        }

        case TYPE_INT:
            printf("PUSHS int@%d\n", t.value.integer);
            break;

        case TYPE_DOUBLE:
            printf("PUSHS float@%a\n", t.value.decimal);
            break;

        case TYPE_KW:
            printf("PUSHS nil@nil\n");
            break;

        case TYPE_EXCLAMATION_MARK:
            break;

        case TYPE_PLUS:
            generatorPLUS();
            break;

        case TYPE_MINUS:
            generatorMINUS();
            printf("SUBS\n");
            break;

        case TYPE_MUL:
            generatorMULTIPLY();
            printf("MULS\n");
            break;

        case TYPE_DIV:
            generatorDIVISION();
            printf("DIVS\n");
            break;

        case TYPE_EQUAL:
            generatorEQUALS();
            printf("EQS\n");
            generatorConvert_TRUE_FALSE();
            break;

        case TYPE_NOT_EQUAL:
            generatorEQUALS();
            printf("EQS\n");
            printf("NOTS\n");
            generatorConvert_TRUE_FALSE();
            break;

        case TYPE_MORE:
            generatorMORE_LESS();
            printf("POPFRAME\n");
            printf("GTS\n");
            generatorConvert_TRUE_FALSE();
            break;
        case TYPE_MORE_EQUAL:
            generatorMORE_LESS();
            printf("GTS\n");
            printf("POPS GF@$$mainresult\n");
            printf("PUSHS GF@$$mainresult\n");
            printf("PUSHS LF@tmp2$$value\n");
            printf("PUSHS LF@tmp1$$value\n");
            printf("EQS\n");
            printf("POPS GF@$$mainresult\n");
            printf("PUSHS GF@$$mainresult\n");
            printf("POPFRAME\n");
            printf("ORS\n");
            generatorConvert_TRUE_FALSE();
            break;

        case TYPE_LESS:
            generatorMORE_LESS();
            printf("POPFRAME\n");
            printf("LTS\n");
            generatorConvert_TRUE_FALSE();
            break;
        case TYPE_LESS_EQUAL:
            generatorMORE_LESS();
            printf("LTS\n");
            printf("POPS GF@$$mainresult\n");
            printf("PUSHS GF@$$mainresult\n");
            printf("PUSHS LF@tmp2$$value\n");
            printf("PUSHS LF@tmp1$$value\n");
            printf("EQS\n");
            printf("POPS GF@$$mainresult\n");
            printf("PUSHS GF@$$mainresult\n");
            printf("POPFRAME\n");
            printf("ORS\n");
            generatorConvert_TRUE_FALSE();
            break;

        case TYPE_NIL_COALESCING_OPERATOR:
            generatorCOALESCING();
            break;

        case TYPE_IDENTIFIER_VAR:
            printf("POPFRAME\n");
            printf("PUSHS LF@%s\n", t.value.string);
            printf("PUSHFRAME\n");

        default:
            break;
    }
}

void generatorConvert_TRUE_FALSE()
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Initialize a new frame for boolean conversion
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare a local variable to store the boolean value
    printf("DEFVAR LF@$$bool$$value\n");

    // Pop the top of the stack and store it in the boolean variable
    printf("POPS LF@$$bool$$value\n");

    // Jump to true label if the boolean value is true
    printf("JUMPIFEQ $$bool%d$$true LF@$$bool$$value bool@true\n", i);

    // Set the boolean variable to 0 if the condition is not met
    printf("MOVE LF@$$bool$$value int@0\n");

    // Jump to the exit label
    printf("JUMP $$bool%d$$exit\n", i);

    // True label: set the boolean variable to 1
    printf("LABEL $$bool%d$$true\n", i);
    printf("MOVE LF@$$bool$$value int@1\n");

    // Exit label: end of boolean conversion
    printf("LABEL $$bool%d$$exit\n", i);

    // Push the boolean value back onto the stack
    printf("PUSHS LF@$$bool$$value\n");

    // Pop the boolean frame
    printf("POPFRAME\n");

    // Increment the static variable for label uniqueness
    i++;
}

void generatorExpressionBegin()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void generatorExpressionEnd()
{
    printf("POPS GF@$$mainresult\n");
    printf("POPFRAME\n");
}

void generatorIfElse1(int num)
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Label to check the truth value
    printf("LABEL $$truth$$check$$if%d\n", i);

    // Create a new frame for type checking
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare a variable to store the type of the result
    printf("DEFVAR LF@$$iftype%d\n", i);

    // Get the type of the result and store it in the variable
    printf("TYPE LF@$$iftype%d GF@$$mainresult\n", i);

    // Declare a boolean variable for intermediate checks
    printf("DEFVAR LF@$$helpbool$$if%d\n", i);

    // Check if the type is integer
    printf("JUMPIFEQ $$iftype$$int%d LF@$$iftype%d string@int\n", i, i);

    // Check if the type is float
    printf("JUMPIFEQ $$iftype$$float%d LF@$$iftype%d string@float\n", i, i);

    // Check if the type is nil
    printf("JUMPIFEQ $$iftype$$nil%d LF@$$iftype%d string@nil\n", i, i);

    // Check if the type is string
    printf("JUMPIFEQ $$iftype$$string%d LF@$$iftype%d string@string\n", i, i);

    // If none of the expected types, exit with an error
    printf("EXIT int@7\n");

    // Label for integer type
    printf("LABEL $$iftype$$int%d\n", i);

    // Check if the integer value is equal to 0 and negate the result
    printf("EQ LF@$$helpbool$$if%d GF@$$mainresult int@0\n", i);
    printf("NOT LF@$$helpbool$$if%d LF@$$helpbool$$if%d\n", i, i);
    printf("MOVE GF@$$mainresult LF@$$helpbool$$if%d\n", i);

    // Jump to the exit label
    printf("JUMP $$out$$if%d\n", i);

    // Label for float type
    printf("LABEL $$iftype$$float%d\n", i);

    // Check if the float value is equal to 0 and negate the result
    printf("EQ LF@$$helpbool$$if%d GF@$$mainresult float@0x0.0p+0\n", i);
    printf("NOT LF@$$helpbool$$if%d LF@$$helpbool$$if%d\n", i, i);
    printf("MOVE GF@$$mainresult LF@$$helpbool$$if%d\n", i);

    // Jump to the exit label
    printf("JUMP $$out$$if%d\n", i);

    // Label for nil type
    printf("LABEL $$iftype$$nil%d\n", i);

    // Set the result to false for nil type
    printf("MOVE GF@$$mainresult bool@false\n");

    // Jump to the exit label
    printf("JUMP $$out$$if%d\n", i);

    // Label for string type
    printf("LABEL $$iftype$$string%d\n", i);

    // Check if the string is empty or "0" and set the result accordingly
    printf("JUMPIFEQ $$iftype$$string$$false%d GF@$$mainresult string@0\n", i);
    printf("JUMPIFEQ $$iftype$$string$$false%d GF@$$mainresult string@\n", i);
    printf("MOVE GF@$$mainresult bool@true\n");
    printf("JUMP $$out$$if%d\n", i);

    // Label for false case for string type
    printf("LABEL $$iftype$$string$$false%d\n", i);

    // Set the result to false for string type
    printf("MOVE GF@$$mainresult bool@false\n");

    // Jump to the exit label
    printf("JUMP $$out$$if%d\n", i);

    // Exit label
    printf("LABEL $$out$$if%d\n", i);

    // Pop the frame used for type checking
    printf("POPFRAME\n");

    // Jump to the else label if the result is false
    printf("JUMPIFEQ if$$%d$$else GF@$$mainresult bool@false\n", num);

    // Increment the static variable for label uniqueness
    i++;
}

void generatorIfElse2(int num)
{
    printf("JUMP if$$%d$$else$$2\n", num);
    printf("LABEL if$$%d$$else\n", num);
}

void generatorIfElse3(int num)
{
    printf("LABEL if$$%d$$else$$2\n", num);
    printf("JUMP if$$%d$$else$$end\n", num);
    printf("LABEL if$$%d$$else$$end\n", num);
}

void generatorWhile1(int num)
{
    printf("LABEL while$$%d$$start\n", num);
}

void generatorWhile2(int num)
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Label to check the truth value for the while loop
    printf("LABEL $$truth$$check$$while%d\n", i);

    // Create a new frame for type checking
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare a variable to store the type of the result
    printf("DEFVAR LF@$$whiletype%d\n", i);

    // Get the type of the result and store it in the variable
    printf("TYPE LF@$$whiletype%d GF@$$mainresult\n", i);

    // Declare a boolean variable for intermediate checks
    printf("DEFVAR LF@$$helpbool$$while%d\n", i);

    // Check if the type is integer
    printf("JUMPIFEQ $$whiletype$$int%d LF@$$whiletype%d string@int\n", i, i);

    // Check if the type is float
    printf("JUMPIFEQ $$whiletype$$float%d LF@$$whiletype%d string@float\n", i, i);

    // Check if the type is nil
    printf("JUMPIFEQ $$whiletype$$nil%d LF@$$whiletype%d string@nil\n", i, i);

    // Check if the type is string
    printf("JUMPIFEQ $$whiletype$$string%d LF@$$whiletype%d string@string\n", i, i);

    // If none of the expected types, exit with an error
    printf("EXIT int@7\n");

    // Label for integer type
    printf("LABEL $$whiletype$$int%d\n", i);

    // Check if the integer value is equal to 0 and negate the result
    printf("EQ LF@$$helpbool$$while%d GF@$$mainresult int@0\n", i);
    printf("NOT LF@$$helpbool$$while%d LF@$$helpbool$$while%d\n", i, i);
    printf("MOVE GF@$$mainresult LF@$$helpbool$$while%d\n", i);

    // Jump to the exit label
    printf("JUMP $$out$$while%d\n", i);

    // Label for float type
    printf("LABEL $$whiletype$$float%d\n", i);

    // Check if the float value is equal to 0 and negate the result
    printf("EQ LF@$$helpbool$$while%d GF@$$mainresult float@0x0.0p+0\n", i);
    printf("NOT LF@$$helpbool$$while%d LF@$$helpbool$$while%d\n", i, i);
    printf("MOVE GF@$$mainresult LF@$$helpbool$$while%d\n", i);

    // Jump to the exit label
    printf("JUMP $$out$$while%d\n", i);

    // Label for nil type
    printf("LABEL $$whiletype$$nil%d\n", i);

    // Set the result to false for nil type
    printf("MOVE GF@$$mainresult bool@false\n");

    // Jump to the exit label
    printf("JUMP $$out$$while%d\n", i);

    // Label for string type
    printf("LABEL $$whiletype$$string%d\n", i);

    // Check if the string is empty or "0" and set the result accordingly
    printf("JUMPIFEQ $$whiletype$$string$$false%d GF@$$mainresult string@0\n", i);
    printf("JUMPIFEQ $$whiletype$$string$$false%d GF@$$mainresult string@\n", i);
    printf("MOVE GF@$$mainresult bool@true\n");
    printf("JUMP $$out$$while%d\n", i);

    // Label for false case for string type
    printf("LABEL $$whiletype$$string$$false%d\n", i);

    // Set the result to false for string type
    printf("MOVE GF@$$mainresult bool@false\n");

    // Jump to the exit label
    printf("JUMP $$out$$while%d\n", i);

    // Exit label
    printf("LABEL $$out$$while%d\n", i);

    // Pop the frame used for type checking
    printf("POPFRAME\n");

    // Jump to the end label if the result is false
    printf("JUMPIFEQ while$$%d$$end GF@$$mainresult bool@false\n", num);

    // Increment the static variable for label uniqueness
    i++;
}

void generatorWhile3(int num)
{
    printf("JUMP while$$%d$$start\n", num);
    printf("LABEL while$$%d$$end\n", num);
}

void generatorFRAME()
{
    // Initialize a new frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare variables to store temporary values and their types
    printf("DEFVAR LF@tmp1$$value\n");
    printf("DEFVAR LF@tmp1$$value$$type\n");

    // Pop the top of the stack and store its value and type in variables
    printf("POPS LF@tmp1$$value\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");

    // Declare variables for another set of temporary values and their types
    printf("DEFVAR LF@tmp2$$value\n");
    printf("DEFVAR LF@tmp2$$value$$type\n");

    // Pop the top of the stack and store its value and type in variables
    printf("POPS LF@tmp2$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
}

void generatorPLUS()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$ADDS$$check%d\n", i);
    printf("JUMPIFEQ $$ADDS$$check%d$$1 LF@tmp1$$value$$type string@string\n", i);
    printf("JUMP $$ADDS$$checkk%d\n", i);
    printf("LABEL $$ADDS$$check%d$$1\n", i);
    printf("JUMPIFEQ $$ADDS$$check%d$$2 LF@tmp2$$value$$type string@string\n", i);
    printf("DPRINT int@7\n");
    printf("LABEL $$ADDS$$check%d$$2\n", i);
    printf("CONCAT LF@tmp1$$value LF@tmp2$$value LF@tmp1$$value\n");
    printf("JUMP $$operator$$ADDS$$exit%d\n", i);
    printf("LABEL $$ADDS$$checkk%d\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$1 LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$1 LF@tmp1$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$1 LF@tmp1$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$checkk%d$$1\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$2 LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$2 LF@tmp2$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$2 LF@tmp2$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$checkk%d$$2\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$ADDS$$checkk%d$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$help2tozero LF@tmp2$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$control%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);

    printf("LABEL $$ADDS$$checkk%d$$3\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$tmp1$$valuetofloat LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$ADDS$$checkk%d$$help2tofloat LF@tmp2$$value$$type string@int\n", i);
    printf("JUMP $$control%d\n", i);

    printf("LABEL $$ADDS$$checkk%d$$tmp1$$valuetofloat\n", i);
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$control%d\n", i);

    printf("LABEL $$ADDS$$checkk%d$$help2tofloat\n", i);
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$control%d\n", i);

    printf("LABEL $$ADDS$$checkk%d$$tmp1$$valuetozero\n", i);
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$ADDS$$checkk%d$$2\n", i);

    printf("LABEL $$ADDS$$checkk%d$$help2tozero\n", i);
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$ADDS$$checkk%d$$2\n", i);

    printf("LABEL $$control%d\n", i);
    printf("ADD LF@tmp1$$value LF@tmp1$$value LF@tmp2$$value\n");
    printf("JUMP $$operator$$ADDS$$exit%d\n", i);

    printf("LABEL $$operator$$ADDS$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}

void generatorMINUS()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$MINUS$$check%d\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$1 LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$1 LF@tmp1$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$1 LF@tmp1$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$MINUS$$check%d$$1\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$2 LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$2 LF@tmp2$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$2 LF@tmp2$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$MINUS$$check%d$$2\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$MINUS$$check%d$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$help2tozero LF@tmp2$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$operator$$MINUS$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);

    printf("LABEL $$MINUS$$check%d$$3\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$tmp1$$valuetofloat LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MINUS$$check%d$$help2tofloat LF@tmp2$$value$$type string@int\n", i);
    printf("JUMP $$operator$$MINUS$$exit%d\n", i);

    printf("LABEL $$MINUS$$check%d$$tmp1$$valuetofloat\n", i);
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$operator$$MINUS$$exit%d\n", i);

    printf("LABEL $$MINUS$$check%d$$help2tofloat\n", i);
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$operator$$MINUS$$exit%d\n", i);

    printf("LABEL $$MINUS$$check%d$$tmp1$$valuetozero\n", i);
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$MINUS$$check%d$$2\n", i);

    printf("LABEL $$MINUS$$check%d$$help2tozero\n", i);
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$MINUS$$check%d$$2\n", i);

    printf("LABEL $$operator$$MINUS$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}

void generatorMULTIPLY()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$MULTIPLY$$check%d\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$1 LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$1 LF@tmp1$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$1 LF@tmp1$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$MULTIPLY$$check%d$$1\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$2 LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$2 LF@tmp2$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$2 LF@tmp2$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$MULTIPLY$$check%d$$2\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$help2tozero LF@tmp2$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$operator$$MULTIPLY$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);

    printf("LABEL $$MULTIPLY$$check%d$$3\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$tmp1$$valuetofloat LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MULTIPLY$$check%d$$help2tofloat LF@tmp2$$value$$type string@int\n", i);
    printf("JUMP $$operator$$MULTIPLY$$exit%d\n", i);

    printf("LABEL $$MULTIPLY$$check%d$$tmp1$$valuetofloat\n", i);
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$operator$$MULTIPLY$$exit%d\n", i);

    printf("LABEL $$MULTIPLY$$check%d$$help2tofloat\n", i);
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$operator$$MULTIPLY$$exit%d\n", i);

    printf("LABEL $$MULTIPLY$$check%d$$tmp1$$valuetozero\n", i);
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$MULTIPLY$$check%d$$2\n", i);

    printf("LABEL $$MULTIPLY$$check%d$$help2tozero\n", i);
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$MULTIPLY$$check%d$$2\n", i);

    printf("LABEL $$operator$$MULTIPLY$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}

void generatorDIVISION()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$DIVISION$$check%d\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$1 LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$1 LF@tmp1$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$1 LF@tmp1$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$DIVISION$$check%d$$1\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$2 LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$2 LF@tmp2$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$2 LF@tmp2$$value$$type string@nil\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$DIVISION$$check%d$$2\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$DIVISION$$check%d$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$help2tozero LF@tmp2$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$DIVISION$$check%d$$next LF@tmp1$$value$$type string@float\n", i);
    printf("JUMP $$DIVISION$$check%d$$tmp1$$valuetofloat\n", i);

    printf("LABEL $$DIVISION$$check%d$$next\n", i);
    printf("JUMPIFEQ $$operator$$DIVISION$$exit%d LF@tmp2$$value$$type string@float\n", i);
    printf("JUMP $$DIVISION$$check%d$$help2tofloat\n", i);

    printf("LABEL $$DIVISION$$check%d$$tmp1$$valuetofloat\n", i);
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$DIVISION$$check%d$$2\n", i);

    printf("LABEL $$DIVISION$$check%d$$help2tofloat\n", i);
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$DIVISION$$check%d$$2\n", i);

    printf("LABEL $$DIVISION$$check%d$$tmp1$$valuetozero\n", i);
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$DIVISION$$check%d$$2\n", i);

    printf("LABEL $$DIVISION$$check%d$$help2tozero\n", i);
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$DIVISION$$check%d$$2\n", i);

    printf("LABEL $$operator$$DIVISION$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}
void generatorEQUALS()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$EQUALS$$check%d\n", i);
    printf("JUMPIFEQ $$operator$$EQUALS$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$operator$$EQUALS$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}

void generatorMORE_LESS()
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$MORE$$LESS$$check%d\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$nil$$tmp1$$value LF@tmp1$$value$$type nil@nil\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$nil$$tmp2$$value LF@tmp2$$value$$type nil@nil\n", i);
    printf("JUMP $$MORE$$LESS$$check%d$$2\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$nil$$tmp1$$value\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$tmp1$$valuetozero$$int LF@tmp2$$value$$type string@nil\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$tmp1$$valuetozero$$int LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$tmp1$$valuetozero$$float LF@tmp2$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$tmp1$$valuetoempty LF@tmp2$$value$$type string@string\n", i);
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$tmp1$$valuetozero$$int\n", i);
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$tmp1$$valuetozero$$float\n", i);
    printf("MOVE LF@tmp1$$value float@0x0.0p+0\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$tmp1$$valuetoempty\n", i);
    printf("MOVE LF@tmp1$$value string@\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$nil$$tmp2$$value\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$help2tozero$$int LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$help2tozero$$float LF@tmp1$$value$$type string@float\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$help2toempty LF@tmp1$$value$$type string@string\n", i);
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$help2tozero$$int\n", i);
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);
    printf("LABEL $$MORE$$LESS$$check%d$$help2tozero$$float\n", i);
    printf("MOVE LF@tmp2$$value float@0x0.0p+0\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$help2toempty\n", i);
    printf("MOVE LF@tmp2$$value string@\n");
    printf("JUMP $$MORE$$LESS$$check%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$2\n", i);
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$str LF@tmp1$$value$$type string@string\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$str LF@tmp2$$value$$type string@string\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$tmp1$$valueint LF@tmp1$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$MORE$$LESS$$check%d$$help2int LF@tmp2$$value$$type string@int\n", i);
    printf("JUMPIFEQ $$operator$$MORE$$LESS$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);
    printf("JUMP $$operator$$MORE$$LESS$$exit%d\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$str\n", i);
    printf("JUMPIFEQ $$operator$$MORE$$LESS$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);
    printf("EXIT int@7\n");

    printf("LABEL $$MORE$$LESS$$check%d$$tmp1$$valueint\n", i);
    printf("JUMPIFEQ $$operator$$MORE$$LESS$$exit%d LF@tmp1$$value$$type LF@tmp2$$value$$type\n", i);
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$MORE$$LESS$$check%d$$2\n", i);

    printf("LABEL $$MORE$$LESS$$check%d$$help2int\n", i);
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$MORE$$LESS$$check%d$$2\n", i);

    printf("LABEL $$operator$$MORE$$LESS$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    i++;
}

void generatorCOALESCING() // function for ??
{
    static int i = 0;
    generatorFRAME();
    printf("LABEL $$COALESCING$$check%d$$\n", i);
    printf("JUMPIFEQ $$COALESCING$$check%d$$2 LF@tmp2$$value nil@nil\n", i);
    printf("MOVE LF@tmp1$$value LF@tmp2$$value\n");
    printf("JUMP $$operator$$COALESCING$$exit%d\n", i);
    printf("LABEL $$COALESCING$$check%d$$2\n", i);

    printf("LABEL $$operator$$COALESCING$$exit%d\n", i);
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    i++;
}

void generatorWrite(int numofparams)
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    static int i = 0;
    for (int j = numofparams; j > 0; j--)
    {
        printf("DEFVAR LF@$$write%d\n", j);
        printf("POPS LF@$$write%d\n", j);
        printf("JUMPIFEQ $$WRITE$$nil%d LF@$$write%d nil@nil\n", i, j);
        printf("JUMP $$WRITE$$exit%d\n", i);
        printf("LABEL $$WRITE$$nil%d\n", i);
        printf("MOVE LF@$$write%d string@\\010\n", j);
        printf("LABEL $$WRITE$$exit%d\n", i);
        i++;
    }
    for (int j = 1; j <= numofparams; j++)
    {
        printf("WRITE LF@$$write%d\n", j);
    }
    printf("POPFRAME\n");
}

void generatorLength()
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Initialize a new frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare variables to store the string, its type, and the length result
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@length$$retval\n");

    // Pop the top of the stack and store it as the string
    printf("POPS LF@string\n");

    // Get the type of the string and store it in the 'type' variable
    printf("TYPE LF@type LF@string\n");

    // Check if the variable is of type string, exit with an error if not
    printf("JUMPIFEQ func$$length$$%d$$isstring LF@type string@string\n", i);
    printf("EXIT int@4\n");

    // Label for when the variable is of type string
    printf("LABEL func$$length$$%d$$isstring\n", i);

    // Get the length of the string and store it in the 'length$$retval' variable
    printf("STRLEN LF@length$$retval LF@string\n");

    // Pop the current frame
    printf("POPFRAME\n");

    // Move the result to the global variable $$mainresult
    printf("MOVE GF@$$mainresult TF@length$$retval\n");

    // Increment the static variable for label uniqueness
    i++;
}

void generatorOrd()
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Initialize a new frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare variables to store the string, its type, and the ASCII result
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@ASCII$$retval\n");

    // Pop the top of the stack and store it as the string
    printf("POPS LF@string\n");

    // Get the type of the string and store it in the 'type' variable
    printf("TYPE LF@type LF@string\n");

    // Check if the variable is of type string, exit with an error if not
    printf("JUMPIFEQ func$$ord$$%d$$isstring LF@type string@string\n", i);
    printf("EXIT int@4\n");

    // Label for when the variable is of type string
    printf("LABEL func$$ord$$%d$$isstring\n", i);

    // Get the length of the string and store it in the 'ASCII$$retval' variable
    printf("STRLEN LF@ASCII$$retval LF@string\n");

    // Check if the string is empty, jump to the exit label if true
    printf("JUMPIFEQ func$$ord$$%d$$empty LF@ASCII$$retval int@0\n", i);

    // Convert the first character of the string to ASCII and store it in 'ASCII$$retval'
    printf("STRI2INT LF@ASCII$$retval LF@string int@0\n");

    // Jump to the exit label
    printf("JUMP func$$ord$$%d$$exit\n", i);

    // Label for when the string is empty
    printf("LABEL func$$ord$$%d$$empty\n", i);

    // Set the result to 0 for an empty string
    printf("MOVE LF@ASCII$$retval int@0\n");

    // Label for the exit
    printf("LABEL func$$ord$$%d$$exit\n", i);

    // Pop the current frame
    printf("POPFRAME\n");

    // Move the result to the global variable $$mainresult
    printf("MOVE GF@$$mainresult TF@ASCII$$retval\n");

    // Increment the static variable for label uniqueness
    i++;
}

void generatorChr()
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Initialize a new frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare variables to store the number, its type, and the character result
    printf("DEFVAR LF@number\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@char$$retval\n");

    // Pop the top of the stack and store it as the number
    printf("POPS LF@number\n");

    // Get the type of the number and store it in the 'type' variable
    printf("TYPE LF@type LF@number\n");

    // Check if the variable is of type int, exit with an error if not
    printf("JUMPIFEQ func$$chr$$%d$$isint LF@type string@int\n", i);
    printf("EXIT int@4\n");

    // Label for when the variable is of type int
    printf("LABEL func$$chr$$%d$$isint\n", i);

    // Convert the number to a character and store it in 'char$$retval'
    printf("INT2CHAR LF@char$$retval LF@number\n");

    // Pop the current frame
    printf("POPFRAME\n");

    // Move the result to the global variable $$mainresult
    printf("MOVE GF@$$mainresult TF@char$$retval\n");

    // Increment the static variable for label uniqueness
    i++;
}

void generatorReadString()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readString$$retval\n");
    printf("READ LF@readString$$retval string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readString$$retval\n");
}

void generatorReadInt()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readInt$$retval\n");
    printf("READ LF@readInt$$retval int\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readInt$$retval\n");
}

void generatorReadDouble()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readDouble$$retval\n");
    printf("READ LF@readDouble$$retval float\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readDouble$$retval\n");
}

void generatorInt2Double()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$Int2Double$$%d$$retval\n", i);
    printf("DEFVAR LF@$$Int2Double$$%d$$val\n", i);
    printf("DEFVAR LF@$$Int2Double$$%d$$type\n", i);
    printf("POPS LF@$$Int2Double$$%d$$val\n", i);
    printf("TYPE LF@$$Int2Double$$%d$$type LF@$$Int2Double$$%d$$val\n", i, i);
    printf("JUMPIFEQ $$func$$Int2Double$$%d$$int LF@$$Int2Double$$%d$$type string@int\n", i, i);
    printf("EXIT int@4\n");
    printf("LABEL $$func$$Int2Double$$%d$$int\n", i);
    printf("INT2FLOAT LF@$$Int2Double$$%d$$retval LF@$$Int2Double$$%d$$val\n", i, i);
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@$$Int2Double$$%d$$retval\n", i);
    i++;
}

void generatorDouble2Int()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$Double2Int$$%d$$retval\n", i);
    printf("DEFVAR LF@$$Double2Int$$%d$$val\n", i);
    printf("DEFVAR LF@$$Double2Int$$%d$$type\n", i);
    printf("POPS LF@$$Double2Int$$%d$$val\n", i);
    printf("TYPE LF@$$Double2Int$$%d$$type LF@$$Double2Int$$%d$$val\n", i, i);
    printf("JUMPIFEQ $$func$$Double2Int$$%d$$float LF@$$Double2Int$$%d$$type string@float\n", i, i);
    printf("EXIT int@4\n");
    printf("LABEL $$func$$Double2Int$$%d$$float\n", i);
    printf("FLOAT2INT LF@$$Double2Int$$%d$$retval LF@$$Double2Int$$%d$$val\n", i, i);
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@$$Double2Int$$%d$$retval\n", i);
    i++;
}

void generatorSubstring()
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Initialize a new frame
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Declare variables for substring operation
    printf("DEFVAR LF@$$substring$$%d$$retval\n", i);
    printf("DEFVAR LF@$$substring$$%d$$curr$$chr\n", i);
    printf("DEFVAR LF@$$i\n");
    printf("DEFVAR LF@$$j\n");
    printf("DEFVAR LF@$$string\n");
    printf("DEFVAR LF@$$pres\n");
    printf("DEFVAR LF@$$length\n");

    // Initialize the result to nil@nil
    printf("MOVE LF@$$substring$$%d$$retval nil@nil\n", i);

    // Pop values from the stack for substring operation
    printf("POPS LF@$$j\n");
    printf("POPS LF@$$i\n");
    printf("POPS LF@$$string\n");

    // Check if $$j is of type int
    printf("TYPE LF@$$pres LF@$$j\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@int\n", i);

    // Check if $$i is of type int
    printf("TYPE LF@$$pres LF@$$i\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@int\n", i);

    // Check if $$string is of type string
    printf("TYPE LF@$$pres LF@$$string\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@string\n", i);

    // Jump to label when types are correct
    printf("JUMP $$correct$$type%d\n", i);

    // Label for incorrect types
    printf("LABEL $$incorrect$$type%d\n", i);
    printf("EXIT int@4\n");

    // Label for correct types
    printf("LABEL $$correct$$type%d\n", i);

    // Check conditions for an invalid substring
    printf("GT LF@$$pres LF@$$i LF@$$j\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("LT LF@$$pres LF@$$i int@0\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("LT LF@$$pres LF@$$j int@0\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$string\n");
    printf("EQ LF@$$pres LF@$$pres LF@$$i\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$string\n");
    printf("GT LF@$$pres LF@$$i LF@$$pres\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$string\n");
    printf("GT LF@$$pres LF@$$j LF@$$pres\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);

    // Calculate the length of the substring
    printf("MOVE LF@$$length LF@$$j\n");
    printf("SUB LF@$$length LF@$$length LF@$$i\n");

    // Initialize the result to an empty string
    printf("MOVE LF@$$substring$$%d$$retval string@\n", i);

    // Jump to the return label if the length is 0
    printf("JUMPIFEQ $$return$$0$$%d LF@$$length int@0\n", i);

    // Label for the loop
    printf("LABEL $$for%d\n", i);

    // Get the character at the current position and concatenate it to the result
    printf("GETCHAR LF@$$substring$$%d$$curr$$chr LF@$$string LF@$$i\n", i);
    printf("CONCAT LF@$$substring$$%d$$retval LF@$$substring$$%d$$retval LF@$$substring$$%d$$curr$$chr\n", i, i, i);

    // Update the length and position
    printf("SUB LF@$$length LF@$$length int@1\n");
    printf("ADD LF@$$i LF@$$i int@1\n");

    // Jump back to the loop if the length is not zero
    printf("JUMPIFNEQ $$for%d LF@$$length int@0\n", i);

    // Label for the return
    printf("LABEL $$return$$0$$%d\n", i);

    // Pop the current frame
    printf("POPFRAME\n");

    // Move the result to the global variable $$mainresult
    printf("MOVE GF@$$mainresult TF@$$substring$$%d$$retval\n", i);

    // Increment the static variable for label uniqueness
    i++;
}

void generatorFuncCall(char *funcname, int paramCount, ListNode *returnType)
{
    static int retnum = 0;
    if (strcmp(funcname, "write") == 0)
    {
        generatorWrite(paramCount);
    }
    else if (strcmp(funcname, "length") == 0)
    {
        generatorLength();
    }
    else if (strcmp(funcname, "ord") == 0)
    {
        generatorOrd();
    }
    else if (strcmp(funcname, "chr") == 0)
    {
        generatorChr();
    }
    else if (strcmp(funcname, "readString") == 0)
    {
        generatorReadString();
    }
    else if (strcmp(funcname, "readInt") == 0)
    {
        generatorReadInt();
    }
    else if (strcmp(funcname, "readDouble") == 0)
    {
        generatorReadDouble();
    }
    else if (strcmp(funcname, "Int2Double") == 0)
    {
        generatorInt2Double();
    }
    else if (strcmp(funcname, "Double2Int") == 0)
    {
        generatorDouble2Int();
    }
    else if (strcmp(funcname, "substring") == 0)
    {
        generatorSubstring();
    }
    else
    {
        printf("CALL $$func$$%s\n", funcname);
        if (returnType->type != KW_VOID)
        {
            // Not implemented
        }
    }
    retnum++;
}

void generatorDefineVariable(Token t)
{
    printf("DEFVAR LF@%s\n", t.value.string);
}

void generatorAssignVariable(Token t)
{
    printf("MOVE LF@%s GF@$$mainresult\n", t.value.string);
}

void generatorCheckDefined(Token t)
{
    // Static variable to ensure uniqueness in label names
    static int i = 0;

    // Pop the current frame
    printf("POPFRAME\n");

    // Check the type of the variable and store it in the global variable $$je
    printf("TYPE GF@$$je LF@%s\n", t.value.string);

    // Push a new frame
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");

    // Jump to label if the variable exists (its type is not string@)
    printf("JUMPIFNEQ $$existuje%i GF@$$je string@\n", i);

    // Exit with an error code if the variable does not exist
    printf("EXIT int@5\n");

    // Label for when the variable exists
    printf("LABEL $$existuje%i\n", i);

    // Pop the current frame
    printf("POPFRAME\n");

    // Increment the static variable for label uniqueness
    i++;
}

void generatorReturn(char *funcname, bool expr)
{
    // Unused parameter to avoid a compiler warning
    (void)expr;

    // Check if the function is not "main"
    if (strcmp(funcname, "main") != 0)
    {
        // Move the result to the function's return value
        printf("MOVE LF@%s$$retval GF@$$mainresult\n", funcname);

        // Pop the current frame
        printf("POPFRAME\n");

        // Return from the function
        printf("RETURN\n");
    }
    else
    {
        // Exit with a success code if the function is "main"
        printf("EXIT int@0\n");
    }
}
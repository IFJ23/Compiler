// Compiler to IFJ23 language
// Faculty of Information Technology Brno University of Technology
// Authors:
// Oleksii Shelest (xshele02)

#include "stdio.h"
#include "generator.h"

extern Parser parser;

void genPrintHead()
{
    printf(".IFJcode23\n");
    printf("DEFVAR GF@$$funcname\n");
    printf("MOVE GF@$$funcname string@main\n");
    printf("DEFVAR GF@$$mainresult\n");
    printf("MOVE GF@$$mainresult nil@nil\n");
    printf("DEFVAR GF@$$operator\n");
    printf("MOVE GF@$$operator nil@nil\n");
    printf("DEFVAR GF@$$je\n");
    printf("DEFVAR GF@$$param$$type$$1\n");
    printf("MOVE GF@$$param$$type$$1 nil@nil\n");
    printf("DEFVAR GF@$$param$$type$$2\n");
    printf("MOVE GF@$$param$$type$$2 nil@nil\n");
    printf("DEFVAR GF@$$param$$type$$opt\n");
    printf("MOVE GF@$$param$$type$$opt nil@nil\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genConvertBool()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$bool$$value\n");
    printf("POPS LF@$$bool$$value\n");
    printf("JUMPIFEQ $$bool%d$$true LF@$$bool$$value bool@true\n", i);
    printf("MOVE LF@$$bool$$value int@0\n");
    printf("JUMP $$bool%d$$out\n", i);
    printf("LABEL $$bool%d$$true\n", i);
    printf("MOVE LF@$$bool$$value int@1\n");
    printf("LABEL $$bool%d$$out\n", i);
    printf("PUSHS LF@$$bool$$value\n");
    printf("POPFRAME\n");
    i++;
}

void genStackPush(Token t)
{
    switch (t.type)
    {
    case TYPE_STRING:
    {
        printf("PUSHS string@");

        int i = 0;
        char currentChar;

        while ((currentChar = t.value.string[i]) != '\0')
        {
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
        free(t.value.string);
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
        // Now doesnt implemented yet
        break;

    case TYPE_PLUS:
        printf("MOVE GF@$$operator string@ADDS\n");
        printf("CALL $$math$$check\n");
        break;

    case TYPE_MINUS:
        printf("MOVE GF@$$operator string@SUBS\n");
        printf("CALL $$math$$check\n");
        printf("SUBS\n");
        break;

    case TYPE_MUL:
        printf("MOVE GF@$$operator string@MULS\n");
        printf("CALL $$math$$check\n");
        printf("MULS\n");
        break;

    case TYPE_DIV:
        printf("MOVE GF@$$operator string@DIVS\n");
        printf("CALL $$math$$check\n");
        printf("DIVS\n");
        break;

    case TYPE_MORE_EQUAL:
        printf("MOVE GF@$$operator string@GTE\n");
        printf("CALL $$math$$check\n");
        printf("GTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@GTE$$help\n");
        printf("POPS LF@GTE$$help\n");
        printf("EQS\n");
        printf("PUSHS LF@GTE$$help\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_LESS_EQUAL:
        printf("MOVE GF@$$operator string@LTE\n");
        printf("CALL $$math$$check\n");
        printf("LTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@LTE$$help\n");
        printf("POPS LF@LTE$$help\n");
        printf("EQS\n");
        printf("PUSHS LF@LTE$$help\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_MORE:
        printf("MOVE GF@$$operator string@GTS\n");
        printf("CALL $$math$$check\n");
        printf("GTS\n");
        genConvertBool();
        break;

    case TYPE_LESS:
        printf("MOVE GF@$$operator string@LTS\n");
        printf("CALL $$math$$check\n");
        printf("LTS\n");
        genConvertBool();
        break;

    case TYPE_EQUAL:
        printf("MOVE GF@$$operator string@EQS\n");
        printf("CALL $$math$$check\n");
        printf("EQS\n");
        genConvertBool();
        break;

    case TYPE_NOT_EQUAL:
        printf("MOVE GF@$$operator string@EQS\n");
        printf("CALL $$math$$check\n");
        printf("EQS\n");
        printf("NOTS\n");
        genConvertBool();
        break;

    case TYPE_NIL_COALESCING_OPERATOR:
        printf("MOVE GF@$$operator string@COALESCING\n");
        printf("CALL $$math$$check\n");
        printf("ADDS\n");
        break;

    case TYPE_IDENTIFIER_VAR:
        printf("POPFRAME\n");
        printf("PUSHS LF@%s\n", t.value.string);
        printf("PUSHFRAME\n");

    default:
        break;
    }
}

void genExpressionBegin()
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
}

void genExpressionEnd()
{
    printf("POPS GF@$$mainresult\n");
    printf("POPFRAME\n");
}

void genCheckTruth()
{
    printf("JUMP $$skip$$checking\n");
    printf("LABEL $$truth$$check\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$iftype\n");
    printf("TYPE LF@$$iftype GF@$$mainresult\n");
    printf("DEFVAR LF@$$helpbool\n");
    printf("JUMPIFEQ $$iftype$$int LF@$$iftype string@int\n");
    printf("JUMPIFEQ $$iftype$$float LF@$$iftype string@float\n");
    printf("JUMPIFEQ $$iftype$$nil LF@$$iftype string@nil\n");
    printf("JUMPIFEQ $$iftype$$string LF@$$iftype string@string\n");
    printf("EXIT int@7\n");
    printf("LABEL $$iftype$$int\n");
    printf("EQ LF@$$helpbool GF@$$mainresult int@0\n");
    printf("NOT LF@$$helpbool LF@$$helpbool\n");
    printf("MOVE GF@$$mainresult LF@$$helpbool\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$float\n");
    printf("EQ LF@$$helpbool GF@$$mainresult float@0x0.0p+0\n");
    printf("NOT LF@$$helpbool LF@$$helpbool\n");
    printf("MOVE GF@$$mainresult LF@$$helpbool\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$nil\n");
    printf("MOVE GF@$$mainresult bool@false\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$string\n");
    printf("JUMPIFEQ $$iftype$$string$$false GF@$$mainresult string@0\n");
    printf("JUMPIFEQ $$iftype$$string$$false GF@$$mainresult string@\n");
    printf("MOVE GF@$$mainresult bool@true\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$string$$false\n");
    printf("MOVE GF@$$mainresult bool@false\n");
    printf("JUMP $$out\n");
    printf("LABEL $$out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$skip$$checking\n");
}

void genIfElse1(int num)
{
    printf("CALL $$truth$$check\n");
    printf("JUMPIFEQ if$$%d$$else GF@$$mainresult bool@false\n", num);
}

void genIfElse2(int num)
{
    printf("JUMP if$$%d$$else$$2\n", num);
    printf("LABEL if$$%d$$else\n", num);
}

void genIfElse3(int num)
{
    printf("LABEL if$$%d$$else$$2\n", num);
    printf("JUMP if$$%d$$else$$end\n", num);
    printf("LABEL if$$%d$$else$$end\n", num);
}

void genWhileLoop1(int num)
{
    printf("LABEL while$$%d$$start\n", num);
}

void genWhileLoop2(int num)
{
    printf("CALL $$truth$$check\n");
    printf("JUMPIFEQ while$$%d$$end GF@$$mainresult bool@false\n", num);
}

void genWhileLoop3(int num)
{
    printf("JUMP while$$%d$$start\n", num);
    printf("LABEL while$$%d$$end\n", num);
}

void genMathInstCheck()
{
    printf("JUMP $$skipcheck\n");
    printf("LABEL $$math$$check\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@tmp1$$value\n");
    printf("DEFVAR LF@tmp1$$value$$type\n");
    printf("POPS LF@tmp1$$value\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("DEFVAR LF@tmp2$$value\n");
    printf("DEFVAR LF@tmp2$$value$$type\n");
    printf("POPS LF@tmp2$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$ADDS$$check GF@$$operator string@ADDS\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check GF@$$operator string@SUBS\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check GF@$$operator string@MULS\n");
    printf("JUMPIFEQ $$DIVS$$check GF@$$operator string@DIVS\n");
    printf("JUMPIFEQ $$EQS$$check GF@$$operator string@EQS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$operator string@GTS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$operator string@LTS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$operator string@GTE\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$operator string@LTE\n");
    printf("JUMPIFEQ $$COALESCING$$check$$ GF@$$operator string@COALESCING\n");
    printf("JUMP $$math$$check$$exit\n");
    // Checking for all +, -, *, /
    printf("LABEL $$ADDS$$check\n");
    printf("JUMPIFEQ $$ADDS$$check$$1 LF@tmp1$$value$$type string@string\n");
    printf("JUMP $$ADDS$$checkk\n");
    printf("LABEL $$ADDS$$check$$1\n");
    printf("JUMPIFEQ $$ADDS$$check$$2 LF@tmp2$$value$$type string@string\n");
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$check$$2\n");
    printf("CONCAT LF@tmp1$$value LF@tmp2$$value LF@tmp1$$value\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$ADDS$$checkk\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$1 LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$1 LF@tmp1$$value$$type string@float\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$1 LF@tmp1$$value$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$checkk$$1\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$2 LF@tmp2$$value$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$2 LF@tmp2$$value$$type string@float\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$2 LF@tmp2$$value$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$checkk$$2\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$help2tozero LF@tmp2$$value$$type string@nil\n");
    printf("JUMPIFEQ $$control LF@tmp1$$value$$type LF@tmp2$$value$$type\n");

    printf("LABEL $$ADDS$$checkk$$3\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$tmp1$$valuetofloat LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$checkk$$help2tofloat LF@tmp2$$value$$type string@int\n");
    printf("JUMP $$control\n");

    printf("LABEL $$ADDS$$checkk$$tmp1$$valuetofloat\n");
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$control\n");

    printf("LABEL $$ADDS$$checkk$$help2tofloat\n");
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$control\n");

    printf("LABEL $$ADDS$$checkk$$tmp1$$valuetozero\n");
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$ADDS$$checkk$$2\n");

    printf("LABEL $$ADDS$$checkk$$help2tozero\n");
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$ADDS$$checkk$$2\n");

    printf("LABEL $$control\n");
    printf("ADD LF@tmp1$$value LF@tmp1$$value LF@tmp2$$value\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$SUBS$$MULS$$check\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$1 LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$1 LF@tmp1$$value$$type string@float\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$1 LF@tmp1$$value$$type string@nil\n");
    printf("EXIT int@7\n");

    printf("LABEL $$SUBS$$MULS$$check$$1\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$2 LF@tmp2$$value$$type string@int\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$2 LF@tmp2$$value$$type string@float\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$2 LF@tmp2$$value$$type string@nil\n");
    printf("EXIT int@7\n");

    printf("LABEL $$SUBS$$MULS$$check$$2\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$help2tozero LF@tmp2$$value$$type string@nil\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp1$$value$$type LF@tmp2$$value$$type\n");

    printf("LABEL $$SUBS$$MULS$$check$$3\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$tmp1$$valuetofloat LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$SUBS$$MULS$$check$$help2tofloat LF@tmp2$$value$$type string@int\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$SUBS$$MULS$$check$$tmp1$$valuetofloat\n");
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$SUBS$$MULS$$check$$help2tofloat\n");
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$SUBS$$MULS$$check$$tmp1$$valuetozero\n");
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$SUBS$$MULS$$check$$2\n");

    printf("LABEL $$SUBS$$MULS$$check$$help2tozero\n");
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$SUBS$$MULS$$check$$2\n");

    printf("LABEL $$DIVS$$check\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmp1$$value$$type string@float\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmp1$$value$$type string@nil\n");
    printf("EXIT int@7\n");

    printf("LABEL $$DIVS$$check$$1\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$value$$type string@int\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$value$$type string@float\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$value$$type string@nil\n");
    printf("EXIT int@7\n");

    printf("LABEL $$DIVS$$check$$2\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$DIVS$$check$$tmp1$$valuetozero LF@tmp1$$value$$type string@nil\n");
    printf("JUMPIFEQ $$DIVS$$check$$help2tozero LF@tmp2$$value$$type string@nil\n");
    printf("JUMPIFEQ $$DIVS$$check$$next LF@tmp1$$value$$type string@float\n");
    printf("JUMP $$DIVS$$check$$tmp1$$valuetofloat\n");

    printf("LABEL $$DIVS$$check$$next\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp2$$value$$type string@float\n");
    printf("JUMP $$DIVS$$check$$help2tofloat\n");

    printf("LABEL $$DIVS$$check$$tmp1$$valuetofloat\n");
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$DIVS$$check$$2\n");

    printf("LABEL $$DIVS$$check$$help2tofloat\n");
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$DIVS$$check$$2\n");

    printf("LABEL $$DIVS$$check$$tmp1$$valuetozero\n");
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$DIVS$$check$$2\n");

    printf("LABEL $$DIVS$$check$$help2tozero\n");
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$DIVS$$check$$2\n");
    // Checking for ==
    printf("LABEL $$EQS$$check\n");
    printf("JUMPIFEQ $$EQS$$check$$1 LF@tmp1$$value$$type LF@tmp2$$value$$type\n");
    printf("EXIT int@7\n");

    printf("LABEL $$EQS$$check$$1\n");
    printf("JUMP $$math$$check$$exit\n");
    // Checking for ??
    printf("LABEL $$COALESCING$$check$$\n");
    printf("JUMPIFNEQ $$COALESCING$$check$$1 LF@tmp1$$value int@0\n");
    printf("JUMPIFEQ $$COALESCING$$check$$2 LF@tmp1$$value int@0\n");

    printf("LABEL $$COALESCING$$check$$1\n");
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$COALESCING$$check$$2\n");
    printf("MOVE LF@tmp1$value int@0\n");
    printf("JUMP $$math$$check$$exit\n");
    // Checking for >, <, >=, <=
    printf("LABEL $$LTS$$GTS$$check$$1\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$nil$$tmp1$$value LF@tmp1$$value$$type nil@nil\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$nil$$help2 LF@tmp2$$value$$type nil@nil\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");

    printf("LABEL $$LTS$$GTS$$check$$nil$$tmp1$$value\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp1$$valuetozero$$int LF@tmp2$$value$$type string@nil\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp1$$valuetozero$$int LF@tmp2$$value$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp1$$valuetozero$$float LF@tmp2$$value$$type string@float\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp1$$valuetoempty LF@tmp2$$value$$type string@string\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$tmp1$$valuetozero$$int\n");
    printf("MOVE LF@tmp1$$value int@0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$tmp1$$valuetozero$$float\n");
    printf("MOVE LF@tmp1$$value float@0x0.0p+0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$tmp1$$valuetoempty\n");
    printf("MOVE LF@tmp1$$value string@\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$nil$$help2\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$help2tozero$$int LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$help2tozero$$float LF@tmp1$$value$$type string@float\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$help2toempty LF@tmp1$$value$$type string@string\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$help2tozero$$int\n");
    printf("MOVE LF@tmp2$$value int@0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$help2tozero$$float\n");
    printf("MOVE LF@tmp2$$value float@0x0.0p+0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$help2toempty\n");
    printf("MOVE LF@tmp2$$value string@\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");

    printf("LABEL $$LTS$$GTS$$check$$2\n");
    printf("TYPE LF@tmp1$$value$$type LF@tmp1$$value\n");
    printf("TYPE LF@tmp2$$value$$type LF@tmp2$$value\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$str LF@tmp1$$value$$type string@string\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$str LF@tmp2$$value$$type string@string\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp1$$valueint LF@tmp1$$value$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$help2int LF@tmp2$$value$$type string@int\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp1$$value$$type LF@tmp2$$value$$type\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$LTS$$GTS$$check$$str\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp1$$value$$type LF@tmp2$$value$$type\n");
    printf("EXIT int@7\n");

    printf("LABEL $$LTS$$GTS$$check$$tmp1$$valueint\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp1$$value$$type LF@tmp2$$value$$type\n");
    printf("INT2FLOAT LF@tmp1$$value LF@tmp1$$value\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");

    printf("LABEL $$LTS$$GTS$$check$$help2int\n");
    printf("INT2FLOAT LF@tmp2$$value LF@tmp2$$value\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");
    // End of checking
    printf("JUMPIFEQ $$OR$$EQUAL$$VARIANT GF@$$operator string@LTE\n");
    printf("JUMPIFEQ $$OR$$EQUAL$$VARIANT GF@$$operator string@GTE\n");
    printf("JUMP $$math$$check$$exit\n");

    printf("LABEL $$OR$$EQUAL$$VARIANT\n");
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("JUMP $$math$$check$$exit\n");
    // EXIT
    printf("LABEL $$math$$check$$exit\n");
    printf("PUSHS LF@tmp2$$value\n");
    printf("PUSHS LF@tmp1$$value\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    // Checking types for our variables
    printf("LABEL $$param$$type$$func\n");
    printf("JUMPIFEQ $$match$$nil$$incl GF@$$param$$type$$opt int@1\n");
    printf("JUMPIFEQ $$match$$exact GF@$$param$$type$$opt int@0\n");

    printf("LABEL $$match$$nil$$incl\n");
    printf("JUMPIFEQ $$match$$passed GF@$$param$$type$$1 GF@$$param$$type$$2\n");
    printf("JUMPIFEQ $$match$$passed GF@$$param$$type$$1 string@nil\n");
    printf("EXIT int@4\n");

    printf("LABEL $$match$$exact\n");
    printf("JUMPIFEQ $$match$$passed GF@$$param$$type$$1 GF@$$param$$type$$2\n");
    printf("EXIT int@4\n");

    printf("LABEL $$match$$passed\n");
    printf("RETURN\n");
    printf("LABEL $$skipcheck\n");
}

void genWrite(int numofparams)
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    for (int i = numofparams; i > 0; i--)
    {
        printf("DEFVAR LF@$$write%d\n", i);
        printf("POPS LF@$$write%d\n", i);
    }
    for (int i = 1; i <= numofparams; i++)
    {
        printf("WRITE LF@$$write%d\n", i);
    }
    printf("POPFRAME\n");
}

void genLength() // length
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@length$$retval\n");
    printf("POPS LF@string\n");
    printf("TYPE LF@type LF@string\n");
    printf("JUMPIFEQ func$$length$$%d$$isstring LF@type string@string\n", i);
    printf("EXIT int@4\n");
    printf("LABEL func$$length$$%d$$isstring\n", i);
    printf("STRLEN LF@length$$retval LF@string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@length$$retval\n");
    i++;
}

void genOrd()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@string\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@length$$retval\n");
    printf("POPS LF@string\n");
    printf("TYPE LF@type LF@str\n");
    printf("JUMPIFEQ func$$ord$$%d$$isstring LF@type string@string\n", i);
    printf("EXIT int@4\n");
    printf("LABEL func$$ord$$%d$$isstring\n", i);
    printf("STRLEN LF@length$$retval LF@string\n");
    printf("JUMPIFEQ func$$ord$$%d$$empty LF@length$$retval int@0\n", i);
    printf("STRI2INT LF@length$$retval LF@string int@0\n");
    printf("LABEL func$$ord$$%d$$empty\n", i);
    printf("MOVE LF@length$$retval int@0\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@length$$retval\n");
    i++;
}

void genChr()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@number\n");
    printf("DEFVAR LF@type\n");
    printf("DEFVAR LF@string$$retval\n");
    printf("POPS LF@number\n");
    printf("TYPE LF@type LF@number\n");
    printf("JUMPIFEQ func$$chr$$%d$$isint LF@type string@int\n", i);
    printf("EXIT int@4\n");
    printf("LABEL func$$chr$$%d$$isint\n", i);
    printf("INT2CHAR LF@string$$retval LF@number\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@string$$retval\n");
    i++;
}

void genReadString() // readString
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readString$$retval\n");
    printf("READ LF@readString$$retval string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readString$$retval\n");
}

void genReadInt() // readInt
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readInt$$retval\n");
    printf("READ LF@readInt$$retval int\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readInt$$retval\n");
}

void genReadDouble() // readDouble
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readDouble$$retval\n");
    printf("READ LF@readDouble$$retval float\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@readDouble$$retval\n");
}

void genInt2Double() // Int2Double
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

void genDouble2Int() // Double2Int
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

void genSubstring()
{
    static int i = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$substring$$%d$$retval\n", i);
    printf("DEFVAR LF@$$substring$$%d$$curr$$chr\n", i);
    printf("DEFVAR LF@$$i\n");
    printf("DEFVAR LF@$$j\n");
    printf("DEFVAR LF@$$str\n");
    printf("DEFVAR LF@$$pres\n");
    printf("DEFVAR LF@$$len\n");
    printf("MOVE LF@$$substring$$%d$$retval nil@nil\n", i);
    printf("POPS LF@$$j\n");
    printf("POPS LF@$$i\n");
    printf("POPS LF@$$str\n");
    printf("TYPE LF@$$pres LF@$$j\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@int\n", i);
    printf("TYPE LF@$$pres LF@$$i\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@int\n", i);
    printf("TYPE LF@$$pres LF@$$str\n");
    printf("JUMPIFNEQ $$incorrect$$type%d LF@$$pres string@string\n", i);
    printf("JUMP $$correct$$type%d\n", i);

    printf("LABEL $$incorrect$$type%d\n", i);
    printf("EXIT int@4\n");
    printf("LABEL $$correct$$type%d\n", i);
    printf("GT LF@$$pres LF@$$i LF@$$j\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("LT LF@$$pres LF@$$i int@0\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("LT LF@$$pres LF@$$j int@0\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$str\n");
    printf("EQ LF@$$pres LF@$$pres LF@$$i\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$str\n");
    printf("GT LF@$$pres LF@$$i LF@$$pres\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);
    printf("STRLEN LF@$$pres LF@$$str\n");
    printf("GT LF@$$pres LF@$$j LF@$$pres\n");
    printf("JUMPIFEQ $$return$$0$$%d LF@$$pres bool@true\n", i);

    printf("MOVE LF@$$len LF@$$j\n");
    printf("SUB LF@$$len LF@$$len LF@$$i\n");
    printf("MOVE LF@$$substring$$%d$$retval string@\n", i);
    printf("JUMPIFEQ $$return$$0$$%d LF@$$len int@0\n", i);
    printf("LABEL $$for%d\n", i);
    printf("GETCHAR LF@$$substring$$%d$$curr$$chr LF@$$str LF@$$i\n", i);
    printf("CONCAT LF@$$substring$$%d$$retval LF@$$substring$$%d$$retval LF@$$substring$$%d$$curr$$chr\n", i, i, i);
    printf("SUB LF@$$len LF@$$len int@1\n");
    printf("ADD LF@$$i LF@$$i int@1\n");
    printf("JUMPIFNEQ $$for%d LF@$$len int@0\n", i);
    printf("LABEL $$return$$0$$%d\n", i);
    printf("POPFRAME\n");
    printf("MOVE GF@$$mainresult TF@$$substring$$%d$$retval\n", i);
    i++;
}

void genFuncDef1(char *funcname, int parCount, LinkedList ll)
{

    printf("JUMP $$jump$$over$$%s\n", funcname);
    printf("LABEL $$func$$%s\n", funcname);
    printf("CREATEFRAME\n");
    printf("DEFVAR TF@%s$$retval\n", funcname);
    ListNode *tmp = ll.head;
    for (size_t i = parCount; i > 0; i--)
    {
        if (tmp->name == NULL)
        {
            printf("DEFVAR TF@nil\n");
            printf("POPS TF@nil\n");
            tmp = tmp->next;
        }
        else
        {
            printf("DEFVAR TF@%s\n", tmp->name);
            printf("POPS TF@%s\n", tmp->name);
            tmp = tmp->next;
        }
    }
    tmp = ll.head;
    for (size_t i = parCount; i > 0; i--)
    {
        char *matchingType = "";
        if (tmp->type == 1)
        {
            matchingType = "float";
        }
        else if (tmp->type == 4)
        {
            matchingType = "int";
        }
        else if (tmp->type == 7)
        {
            matchingType = "string";
        }

        if (tmp->name == NULL)
        {
            printf("DEFVAR TF@nil\n");
            printf("MOVE GF@$$param$$type$$2 string@%s\n", matchingType);
            printf("MOVE GF@$$param$$type$$opt int@%d\n", tmp->opt);
            printf("CALL $$param$$type$$func\n");
            tmp = tmp->next;
        }
        else
        {
            printf("TYPE GF@$$param$$type$$1 TF@%s\n", tmp->name);
            printf("MOVE GF@$$param$$type$$2 string@%s\n", matchingType);
            printf("MOVE GF@$$param$$type$$opt int@%d\n", tmp->opt);
            printf("CALL $$param$$type$$func\n");
            tmp = tmp->next;
        }
    }
    printf("PUSHFRAME\n");
}

void genFuncDef2(char *funcname)
{
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$jump$$over$$%s\n", funcname);
}

void genFuncCall(char *funcname, int paramCount, ListNode *returnType)
{
    static int retnum = 0;
    if (strcmp(funcname, "write") == 0)
    {
        genWrite(paramCount);
    }
    else if (strcmp(funcname, "strlen") == 0)
    {
        genLength();
    }
    else if (strcmp(funcname, "ord") == 0)
    {
        genOrd();
    }
    else if (strcmp(funcname, "chr") == 0)
    {
        genChr();
    }
    else if (strcmp(funcname, "readString") == 0)
    {
        genReadString();
    }
    else if (strcmp(funcname, "readInt") == 0)
    {
        genReadInt();
    }
    else if (strcmp(funcname, "readDouble") == 0)
    {
        genReadDouble();
    }
    else if (strcmp(funcname, "Int2Double") == 0)
    {
        genInt2Double();
    }
    else if (strcmp(funcname, "Double2Int") == 0)
    {
        genDouble2Int();
    }
    else if (strcmp(funcname, "substring") == 0)
    {
        genSubstring();
    }
    else
    {
        printf("CALL $$func$$%s\n", funcname);
        if (returnType->type != KW_VOID)
        {
            printf("TYPE GF@$$param$$type$$1 TF@%s$$retval\n", funcname);
            printf("JUMPIFEQ $$nil$$type%d GF@$$param$$type$$1 string@nil\n", retnum);
            printf("JUMPIFNEQ $$ret$$type%d GF@$$param$$type$$1 string@\n", retnum);
            printf("LABEL $$nil$$type%d\n", retnum);
            if (returnType->opt == true)
            {
                printf("JUMPIFEQ $$ret$$type%d GF@$$param$$type$$1 string@nil\n", retnum);
            }
            printf("EXIT int@4\n");
            printf("LABEL $$ret$$type%d\n", retnum);
            printf("MOVE GF@$$mainresult TF@%s$$retval\n", funcname);
        }
    }
    retnum++;
}

void genDefineVariable(Token t)
{
    printf("DEFVAR LF@%s\n", t.value.string);
}

void genAssignVariable(Token t)
{
    printf("MOVE LF@%s GF@$$mainresult\n", t.value.string);
}

void genCheckDefined(Token t)
{
    static int n = 0;
    printf("POPFRAME\n");
    printf("TYPE GF@$$je LF@%s\n", t.value.string);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("JUMPIFNEQ $$existuje%i GF@$$je string@\n", n);
    printf("EXIT int@5\n");
    printf("LABEL $$existuje%i\n", n);
    printf("POPFRAME\n");
    n++;
}

void genReturn(char *funcname, bool expr)
{
    (void)expr;
    if (strcmp(funcname, "main") != 0)
    {
        printf("MOVE LF@%s$$retval GF@$$mainresult\n", funcname);
        printf("POPFRAME\n");
        printf("RETURN\n");
    }
    else
    {
        printf("EXIT int@0\n");
    }
}

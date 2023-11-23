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
    printf("DEFVAR GF@$$exprresult\n");
    printf("MOVE GF@$$exprresult nil@nil\n");
    printf("DEFVAR GF@$$curr$$inst\n");
    printf("MOVE GF@$$curr$$inst nil@nil\n");
    printf("DEFVAR GF@$$exists\n");
    printf("DEFVAR GF@$$corrtype$$1\n");
    printf("MOVE GF@$$corrtype$$1 nil@nil\n");
    printf("DEFVAR GF@$$corrtype$$2\n");
    printf("MOVE GF@$$corrtype$$2 nil@nil\n");
    printf("DEFVAR GF@$$corrtype$$opt\n");
    printf("MOVE GF@$$corrtype$$opt nil@nil\n");
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

        int i = 0, c = 0;

        while ((c = t.value.string[i]) != '\0')
        {
            if (c < 33 || c == 35 || c == 92 || c > 126)
            {
                putchar('\\');
                if (c < 0)
                    c += 256;
                printf("%03d", c);
            }
            else
            {
                putchar(c);
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

    case TYPE_PLUS:
        printf("MOVE GF@$$curr$$inst string@ADDS\n");
        printf("CALL $$math$$check\n");
        printf("ADDS\n");
        break;

    case TYPE_MINUS:
        printf("MOVE GF@$$curr$$inst string@SUBS\n");
        printf("CALL $$math$$check\n");
        printf("SUBS\n");
        break;

    case TYPE_MUL:
        printf("MOVE GF@$$curr$$inst string@MULS\n");
        printf("CALL $$math$$check\n");
        printf("MULS\n");
        break;

    case TYPE_DIV:
        printf("MOVE GF@$$curr$$inst string@DIVS\n");
        printf("CALL $$math$$check\n");
        printf("DIVS\n");
        break;

    case TYPE_MORE_EQUAL:
        printf("MOVE GF@$$curr$$inst string@GTE\n");
        printf("CALL $$math$$check\n");
        printf("GTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@GTE$$tmp\n");
        printf("POPS LF@GTE$$tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@GTE$$tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_LESS_EQUAL:
        printf("MOVE GF@$$curr$$inst string@LTE\n");
        printf("CALL $$math$$check\n");
        printf("LTS\n");
        printf("CREATEFRAME\n");
        printf("PUSHFRAME\n");
        printf("DEFVAR LF@LTE$$tmp\n");
        printf("POPS LF@LTE$$tmp\n");
        printf("EQS\n");
        printf("PUSHS LF@LTE$$tmp\n");
        printf("ORS\n");
        printf("POPFRAME\n");
        genConvertBool();
        break;

    case TYPE_MORE:
        printf("MOVE GF@$$curr$$inst string@GTS\n");
        printf("CALL $$math$$check\n");
        printf("GTS\n");
        genConvertBool();
        break;

    case TYPE_LESS:
        printf("MOVE GF@$$curr$$inst string@LTS\n");
        printf("CALL $$math$$check\n");
        printf("LTS\n");
        genConvertBool();
        break;

    case TYPE_EQUAL:
        printf("MOVE GF@$$curr$$inst string@EQS\n");
        printf("CALL $$math$$check\n");
        printf("EQS\n");
        genConvertBool();
        break;

    case TYPE_NOT_EQUAL:
        printf("MOVE GF@$$curr$$inst string@EQS\n");
        printf("CALL $$math$$check\n");
        printf("EQS\n");
        printf("NOTS\n");
        genConvertBool();
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
    printf("POPS GF@$$exprresult\n");
    printf("POPFRAME\n");
}

void genCheckTruth()
{
    printf("JUMP $$initialskip\n");
    printf("LABEL $$truthcheck\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$iftype\n");
    printf("TYPE LF@$$iftype GF@$$exprresult\n");
    printf("DEFVAR LF@$$tmpbool\n");
    printf("JUMPIFEQ $$iftype$$int LF@$$iftype string@int\n");
    printf("JUMPIFEQ $$iftype$$float LF@$$iftype string@float\n");
    printf("JUMPIFEQ $$iftype$$nil LF@$$iftype string@nil\n");
    printf("JUMPIFEQ $$iftype$$string LF@$$iftype string@string\n");
    printf("EXIT int@7\n");
    printf("LABEL $$iftype$$int\n");
    printf("EQ LF@$$tmpbool GF@$$exprresult int@0\n");
    printf("NOT LF@$$tmpbool LF@$$tmpbool\n");
    printf("MOVE GF@$$exprresult LF@$$tmpbool\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$float\n");
    printf("EQ LF@$$tmpbool GF@$$exprresult float@0x0.0p+0\n");
    printf("NOT LF@$$tmpbool LF@$$tmpbool\n");
    printf("MOVE GF@$$exprresult LF@$$tmpbool\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$nil\n");
    printf("MOVE GF@$$exprresult bool@false\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$string\n");
    printf("JUMPIFEQ $$iftype$$string$$false GF@$$exprresult string@0\n");
    printf("JUMPIFEQ $$iftype$$string$$false GF@$$exprresult string@\n");
    printf("MOVE GF@$$exprresult bool@true\n");
    printf("JUMP $$out\n");
    printf("LABEL $$iftype$$string$$false\n");
    printf("MOVE GF@$$exprresult bool@false\n");
    printf("JUMP $$out\n");
    printf("LABEL $$out\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    printf("LABEL $$initialskip\n");
}

void genIfElse1(int num)
{
    printf("CALL $$truthcheck\n");
    printf("JUMPIFEQ if$$%d$$else GF@$$exprresult bool@false\n", num);
}

void genIfElse2(int num)
{
    printf("JUMP if$$%d$$ifdefs\n", num);
    printf("LABEL if$$%d$$else\n", num);
}

void genIfElse3(int num)
{
    printf("LABEL if$$%d$$ifdefs\n", num);
    printf("JUMP if$$%d$$end\n", num);
    printf("LABEL if$$%d$$end\n", num);
}

void genWhileLoop1(int num)
{
    printf("LABEL while$$%d$$start\n", num);
}

void genWhileLoop2(int num)
{
    printf("CALL $$truthcheck\n");
    printf("JUMPIFEQ while$$%d$$end GF@$$exprresult bool@false\n", num);
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
    printf("DEFVAR LF@tmbool$$value\n");
    printf("DEFVAR LF@tmbool$$value$$type\n");
    printf("POPS LF@tmbool$$value\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("DEFVAR LF@tmp2\n");
    printf("DEFVAR LF@tmp2$$type\n");
    printf("POPS LF@tmp2\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check GF@$$curr$$inst string@ADDS\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check GF@$$curr$$inst string@SUBS\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check GF@$$curr$$inst string@MULS\n");
    printf("JUMPIFEQ $$DIVS$$check GF@$$curr$$inst string@DIVS\n");
    printf("JUMPIFEQ $$CONCAT$$check GF@$$curr$$inst string@CONCAT\n");
    printf("JUMPIFEQ $$EQS$$check GF@$$curr$$inst string@EQS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$curr$$inst string@GTS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$curr$$inst string@LTS\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$curr$$inst string@GTE\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$1 GF@$$curr$$inst string@LTE\n");
    printf("JUMP $$math$$check$$exit\n");
    // addition, subtraction, multiplication type check
    printf("LABEL $$ADDS$$SUBS$$MULS$$check\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$1 LF@tmbool$$value$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$1 LF@tmbool$$value$$type string@float\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$1 LF@tmbool$$value$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$1\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$2 LF@tmp2$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$2 LF@tmp2$$type string@float\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$2 LF@tmp2$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$2\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$tmbool$$valuetozero LF@tmbool$$value$$type string@nil\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$tmp2tozero LF@tmp2$$type string@nil\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmbool$$value$$type LF@tmp2$$type\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$3\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$tmbool$$valuetofloat LF@tmbool$$value$$type string@int\n");
    printf("JUMPIFEQ $$ADDS$$SUBS$$MULS$$check$$tmp2tofloat LF@tmp2$$type string@int\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$tmbool$$valuetofloat\n");
    printf("INT2FLOAT LF@tmbool$$value LF@tmbool$$value\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$tmbool$$valuetozero\n");
    printf("MOVE LF@tmbool$$value int@0\n");
    printf("JUMP $$ADDS$$SUBS$$MULS$$check$$2\n");
    printf("LABEL $$ADDS$$SUBS$$MULS$$check$$tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP $$ADDS$$SUBS$$MULS$$check$$2\n");
    // division type check
    printf("LABEL $$DIVS$$check\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmbool$$value$$type string@int\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmbool$$value$$type string@float\n");
    printf("JUMPIFEQ $$DIVS$$check$$1 LF@tmbool$$value$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$DIVS$$check$$1\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$type string@int\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$type string@float\n");
    printf("JUMPIFEQ $$DIVS$$check$$2 LF@tmp2$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$DIVS$$check$$2\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$DIVS$$check$$tmbool$$valuetozero LF@tmbool$$value$$type string@nil\n");
    printf("JUMPIFEQ $$DIVS$$check$$tmp2tozero LF@tmp2$$type string@nil\n");
    printf("JUMPIFEQ $$DIVS$$check$$next LF@tmbool$$value$$type string@float\n");
    printf("JUMP $$DIVS$$check$$tmbool$$valuetofloat\n");
    printf("LABEL $$DIVS$$check$$next\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmp2$$type string@float\n");
    printf("JUMP $$DIVS$$check$$tmp2tofloat\n");
    printf("LABEL $$DIVS$$check$$tmbool$$valuetofloat\n");
    printf("INT2FLOAT LF@tmbool$$value LF@tmbool$$value\n");
    printf("JUMP $$DIVS$$check$$2\n");
    printf("LABEL $$DIVS$$check$$tmp2tofloat\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP $$DIVS$$check$$2\n");
    printf("LABEL $$DIVS$$check$$tmbool$$valuetozero\n");
    printf("MOVE LF@tmbool$$value int@0\n");
    printf("JUMP $$DIVS$$check$$2\n");
    printf("LABEL $$DIVS$$check$$tmp2tozero\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP $$DIVS$$check$$2\n");
    // equals type check
    printf("LABEL $$EQS$$check\n");
    printf("JUMPIFEQ $$EQS$$check$$1 LF@tmbool$$value$$type LF@tmp2$$type\n");
    printf("MOVE LF@tmbool$$value int@0\n");
    printf("MOVE LF@tmp2 int@1\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$EQS$$check$$1\n");
    printf("JUMP $$math$$check$$exit\n");
    // lesser than & greater than type check
    printf("LABEL $$LTS$$GTS$$check$$1\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$nil$$tmbool$$value LF@tmbool$$value$$type nil@nil\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$nil$$tmp2 LF@tmp2$$type nil@nil\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");
    printf("LABEL $$LTS$$GTS$$check$$nil$$tmbool$$value\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmbool$$valuetozero$$int LF@tmp2$$type string@nil\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmbool$$valuetozero$$int LF@tmp2$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmbool$$valuetozero$$float LF@tmp2$$type string@float\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmbool$$valuetoempty LF@tmp2$$type string@string\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmbool$$valuetozero$$int\n");
    printf("MOVE LF@tmbool$$value int@0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmbool$$valuetozero$$float\n");
    printf("MOVE LF@tmbool$$value float@0x0.0p+0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmbool$$valuetoempty\n");
    printf("MOVE LF@tmbool$$value string@\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$nil$$tmp2\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp2tozero$$int LF@tmbool$$value$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp2tozero$$float LF@tmbool$$value$$type string@float\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp2toempty LF@tmbool$$value$$type string@string\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmp2tozero$$int\n");
    printf("MOVE LF@tmp2 int@0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmp2tozero$$float\n");
    printf("MOVE LF@tmp2 float@0x0.0p+0\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP $$LTS$$GTS$$check$$1\n");
    printf("LABEL $$LTS$$GTS$$check$$2\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$str LF@tmbool$$value$$type string@string\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$str LF@tmp2$$type string@string\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmbool$$valueint LF@tmbool$$value$$type string@int\n");
    printf("JUMPIFEQ $$LTS$$GTS$$check$$tmp2int LF@tmp2$$type string@int\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmbool$$value$$type LF@tmp2$$type\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$LTS$$GTS$$check$$str\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmbool$$value$$type LF@tmp2$$type\n");
    printf("EXIT int@7\n");
    printf("LABEL $$LTS$$GTS$$check$$tmbool$$valueint\n");
    printf("JUMPIFEQ $$math$$check$$exit LF@tmbool$$value$$type LF@tmp2$$type\n");
    printf("INT2FLOAT LF@tmbool$$value LF@tmbool$$value\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");
    printf("LABEL $$LTS$$GTS$$check$$tmp2int\n");
    printf("INT2FLOAT LF@tmp2 LF@tmp2\n");
    printf("JUMP $$LTS$$GTS$$check$$2\n");
    // concatenation type check
    printf("LABEL $$CONCAT$$check\n");
    printf("JUMPIFEQ $$CONCAT$$check$$1 LF@tmbool$$value$$type string@string\n");
    printf("JUMPIFEQ $$CONCAT$$check$$1 LF@tmbool$$value$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$CONCAT$$check$$1\n");
    printf("JUMPIFEQ $$CONCAT$$check$$2 LF@tmp2$$type string@string\n");
    printf("JUMPIFEQ $$CONCAT$$check$$2 LF@tmp2$$type string@nil\n");
    printf("EXIT int@7\n");
    printf("LABEL $$CONCAT$$check$$2\n");
    printf("TYPE LF@tmbool$$value$$type LF@tmbool$$value\n");
    printf("TYPE LF@tmp2$$type LF@tmp2\n");
    printf("JUMPIFEQ $$CONCAT$$check$$tmbool$$valuetoempty LF@tmbool$$value$$type string@nil\n");
    printf("JUMPIFEQ $$CONCAT$$check$$tmp2toempty LF@tmp2$$type string@nil\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$CONCAT$$check$$tmbool$$valuetoempty\n");
    printf("MOVE LF@tmbool$$value string@\n");
    printf("JUMP $$CONCAT$$check$$2\n");
    printf("LABEL $$CONCAT$$check$$tmp2toempty\n");
    printf("MOVE LF@tmp2 string@\n");
    printf("JUMP $$CONCAT$$check$$2\n");
    // end check for LTE & GTE operations
    printf("JUMPIFEQ $$OR$$EQUAL$$VARIANT GF@$$curr$$inst string@LTE\n");
    printf("JUMPIFEQ $$OR$$EQUAL$$VARIANT GF@$$curr$$inst string@GTE\n");
    printf("JUMP $$math$$check$$exit\n");
    printf("LABEL $$OR$$EQUAL$$VARIANT\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmbool$$value\n");
    printf("JUMP $$math$$check$$exit\n");
    // exit for all
    printf("LABEL $$math$$check$$exit\n");
    printf("PUSHS LF@tmp2\n");
    printf("PUSHS LF@tmbool$$value\n");
    printf("POPFRAME\n");
    printf("RETURN\n");
    // type check for function parameters
    printf("LABEL $$corrtype$$func\n");
    printf("JUMPIFEQ $$match$$nil$$incl GF@$$corrtype$$opt int@1\n");
    printf("JUMPIFEQ $$match$$exact GF@$$corrtype$$opt int@0\n");
    printf("LABEL $$match$$nil$$incl\n");
    printf("JUMPIFEQ $$match$$passed GF@$$corrtype$$1 GF@$$corrtype$$2\n");
    printf("JUMPIFEQ $$match$$passed GF@$$corrtype$$1 string@nil\n");
    printf("EXIT int@4\n");
    printf("LABEL $$match$$exact\n");
    printf("JUMPIFEQ $$match$$passed GF@$$corrtype$$1 GF@$$corrtype$$2\n");
    printf("EXIT int@4\n");
    printf("LABEL $$match$$passed\n");
    printf("RETURN\n");
    printf("LABEL $$skipcheck\n");
}

void generateWrite(int numofparams)
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

void genStrLen() // length
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
    printf("MOVE GF@$$exprresult TF@length$$retval\n");
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
    printf("JUMPIFEQ func$$ord$$%d$$prazdny LF@length$$retval int@0\n", i);
    printf("STRI2INT LF@length$$retval LF@string int@0\n");
    printf("LABEL func$$ord$$%d$$prazdny\n", i);
    printf("MOVE LF@length$$retval int@0\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@length$$retval\n");
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
    printf("MOVE GF@$$exprresult TF@string$$retval\n");
    i++;
}

void genReads() // readString
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readString$$retval\n");
    printf("READ LF@readString$$retval string\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@readString$$retval\n");
}

void genReadi() // readInt
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readInt$$retval\n");
    printf("READ LF@readInt$$retval int\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@readInt$$retval\n");
}

void genReadf() // readDouble
{
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@readDouble$$retval\n");
    printf("READ LF@readDouble$$retval float\n");
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@readDouble$$retval\n");
}

void genFloatval() // Int2Double
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$Int2Double$$%d$$retval\n", n);
    printf("DEFVAR LF@$$Int2Double$$%d$$val\n", n);
    printf("DEFVAR LF@$$Int2Double$$%d$$type\n", n);
    printf("POPS LF@$$Int2Double$$%d$$val\n", n);
    printf("TYPE LF@$$Int2Double$$%d$$type LF@$$Int2Double$$%d$$val\n", n, n);
    printf("JUMPIFEQ $$func$$Int2Double$$%d$$int LF@$$Int2Double$$%d$$type string@int\n", n, n);
    printf("EXIT int@4\n");
    printf("LABEL $$func$$Int2Double$$%d$$int\n", n);
    printf("INT2FLOAT LF@$$Int2Double$$%d$$retval LF@$$Int2Double$$%d$$val\n", n, n);
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@$$Int2Double$$%d$$retval\n", n);
    n++;
}

void genIntval() // Double2Int
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$Double2Int$$%d$$retval\n", n);
    printf("DEFVAR LF@$$Double2Int$$%d$$val\n", n);
    printf("DEFVAR LF@$$Double2Int$$%d$$type\n", n);
    printf("POPS LF@$$Double2Int$$%d$$val\n", n);
    printf("TYPE LF@$$Double2Int$$%d$$type LF@$$Double2Int$$%d$$val\n", n, n);
    printf("JUMPIFEQ $$func$$Double2Int$$%d$$float LF@$$Double2Int$$%d$$type string@float\n", n, n);
    printf("EXIT int@4\n");
    printf("LABEL $$func$$Double2Int$$%d$$float\n", n);
    printf("FLOAT2INT LF@$$Double2Int$$%d$$retval LF@$$Double2Int$$%d$$val\n", n, n);
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@$$Double2Int$$%d$$retval\n", n);
    n++;
}

void genSubstring()
{
    static int n = 0;
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("DEFVAR LF@$$substring$$%d$$retval\n", n);
    printf("DEFVAR LF@$$substring$$%d$$currchr\n", n);
    printf("DEFVAR LF@$$i\n");
    printf("DEFVAR LF@$$j\n");
    printf("DEFVAR LF@$$str\n");
    printf("DEFVAR LF@$$throwaway\n");
    printf("DEFVAR LF@$$len\n");
    printf("MOVE LF@$$substring$$%d$$retval nil@nil\n", n);
    printf("POPS LF@$$j\n");
    printf("POPS LF@$$i\n");
    printf("POPS LF@$$str\n");
    printf("TYPE LF@$$throwaway LF@$$j\n");
    printf("JUMPIFNEQ $$badtype%d LF@$$throwaway string@int\n", n);
    printf("TYPE LF@$$throwaway LF@$$i\n");
    printf("JUMPIFNEQ $$badtype%d LF@$$throwaway string@int\n", n);
    printf("TYPE LF@$$throwaway LF@$$str\n");
    printf("JUMPIFNEQ $$badtype%d LF@$$throwaway string@string\n", n);
    printf("JUMP $$correcttype%d\n", n);
    printf("LABEL $$badtype%d\n", n);
    printf("EXIT int@4\n");
    printf("LABEL $$correcttype%d\n", n);
    printf("GT LF@$$throwaway LF@$$i LF@$$j\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);
    printf("LT LF@$$throwaway LF@$$i int@0\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);
    printf("LT LF@$$throwaway LF@$$j int@0\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);
    printf("STRLEN LF@$$throwaway LF@$$str\n");
    printf("EQ LF@$$throwaway LF@$$throwaway LF@$$i\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);
    printf("STRLEN LF@$$throwaway LF@$$str\n");
    printf("GT LF@$$throwaway LF@$$i LF@$$throwaway\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);
    printf("STRLEN LF@$$throwaway LF@$$str\n");
    printf("GT LF@$$throwaway LF@$$j LF@$$throwaway\n");
    printf("JUMPIFEQ $$returnnull%d LF@$$throwaway bool@true\n", n);

    printf("MOVE LF@$$len LF@$$j\n");
    printf("SUB LF@$$len LF@$$len LF@$$i\n");
    printf("MOVE LF@$$substring$$%d$$retval string@\n", n);
    printf("JUMPIFEQ $$returnnull%d LF@$$len int@0\n", n);
    printf("LABEL $$for%d\n", n);
    printf("GETCHAR LF@$$substring$$%d$$currchr LF@$$str LF@$$i\n", n);
    printf("CONCAT LF@$$substring$$%d$$retval LF@$$substring$$%d$$retval LF@$$substring$$%d$$currchr\n", n, n, n);
    printf("SUB LF@$$len LF@$$len int@1\n");
    printf("ADD LF@$$i LF@$$i int@1\n");
    printf("JUMPIFNEQ $$for%d LF@$$len int@0\n", n);
    printf("LABEL $$returnnull%d\n", n);
    printf("POPFRAME\n");
    printf("MOVE GF@$$exprresult TF@$$substring$$%d$$retval\n", n);
    n++;
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
            printf("MOVE GF@$$corrtype$$2 string@%s\n", matchingType);
            printf("MOVE GF@$$corrtype$$opt int@%d\n", tmp->opt);
            printf("CALL $$corrtype$$func\n");
            tmp = tmp->next;
        }
        else
        {
            printf("TYPE GF@$$corrtype$$1 TF@%s\n", tmp->name);
            printf("MOVE GF@$$corrtype$$2 string@%s\n", matchingType);
            printf("MOVE GF@$$corrtype$$opt int@%d\n", tmp->opt);
            printf("CALL $$corrtype$$func\n");
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
        generateWrite(paramCount);
    }
    else if (strcmp(funcname, "strlen") == 0)
    {
        genStrLen();
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
        genReads();
    }
    else if (strcmp(funcname, "readInt") == 0)
    {
        genReadi();
    }
    else if (strcmp(funcname, "readDouble") == 0)
    {
        genReadf();
    }
    else if (strcmp(funcname, "Int2Double") == 0)
    {
        genFloatval();
    }
    else if (strcmp(funcname, "Double2Int") == 0)
    {
        genIntval();
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
            printf("TYPE GF@$$corrtype$$1 TF@%s$$retval\n", funcname);
            printf("JUMPIFEQ $$hasnil%d GF@$$corrtype$$1 string@nil\n", retnum);
            printf("JUMPIFNEQ $$hasretvalue%d GF@$$corrtype$$1 string@\n", retnum);
            printf("LABEL $$hasnil%d\n", retnum);
            if (returnType->opt == true)
            {
                printf("JUMPIFEQ $$hasretvalue%d GF@$$corrtype$$1 string@nil\n", retnum);
            }
            printf("EXIT int@4\n");
            printf("LABEL $$hasretvalue%d\n", retnum);
            printf("MOVE GF@$$exprresult TF@%s$$retval\n", funcname);
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
    printf("MOVE LF@%s GF@$$exprresult\n", t.value.string);
}

void genCheckDefined(Token t)
{
    static int n = 0;
    printf("POPFRAME\n");
    printf("TYPE GF@$$exists LF@%s\n", t.value.string);
    printf("PUSHFRAME\n");
    printf("CREATEFRAME\n");
    printf("PUSHFRAME\n");
    printf("JUMPIFNEQ $$doesexist%i GF@$$exists string@\n", n);
    printf("EXIT int@5\n");
    printf("LABEL $$doesexist%i\n", n);
    printf("POPFRAME\n");
    n++;
}

void genReturn(char *funcname, bool expr)
{
    (void)expr;
    if (strcmp(funcname, "main") != 0)
    {
        printf("MOVE LF@%s$$retval GF@$$exprresult\n", funcname);
        printf("POPFRAME\n");
        printf("RETURN\n");
    }
    else
    {
        printf("EXIT int@0\n");
    }
}

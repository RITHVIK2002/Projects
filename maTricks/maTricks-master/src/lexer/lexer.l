/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*   Lexical Specification to generate the scanner  */
/****************************************************/
/*                  Contributions
 -----------------------------------------------------
*   P Srivardhan -- CS19BTECH11052 --  30 Lines 
*   K Rithvik    -- CS19BTECH11038 --  30 Lines     
*   A Mahidhar   -- CS19BTECH11046 --  30 Lines
*   R Prashanth  -- CS19BTECH11042 --  30 Lines
*   V Nikhilish  -- ES19BTECH11030 --  30 Lines
*   M Shathand   -- CS19BTECH11005 --  Tester
*   Eshwar       -- CS19BTECH11040 --  Tester
*/
/*******************************************************    
Last Modified: 20/09/2021
------------ ChangeLog -------*-------
1 Changed some lexical tokens and performes some
  semantic checks at lexical level for structs
------------ Known Bugs----------------

/******************************************************/

%{

    #include <stdio.h>
    #include <string.h>

    #include "parser.tab.h" //To be genereated by parser generator
    #include "colors.h"
    #include "semantic.h"
    
%}


%{
    /**Saving the current line for reporting errors*/
    char linebuf[500];
    int yycolumn = 1;
    
    /**saving the location of the current token to report errors*/
    #define YY_USER_ACTION                                  \
        yylloc.first_line = yylloc.last_line = yylineno;    \
        yylloc.first_column = yycolumn;                     \
        yylloc.last_column = yycolumn+yyleng-1;             \
        yycolumn += yyleng;    

    extern int yydebug_lex;
    //Custom return function to print and return token
    #define RETURN(token)   \
            if(yydebug_lex) printf(MAG"%s\t"CYN#token"\n" RESET,yytext);   \
            return token

    //Helper Function Declarations
    #define copy_line()                                 \
        strncpy(linebuf, yytext+1, sizeof(linebuf));    \
        yyless(1);                                      \
        yycolumn = 1;                                   \

    
%}



%option noyywrap yylineno

%x INITIAL_CONDITION ML_COMMENT


/********************************** Definitions ********************************/

DIGIT [0-9]
ALPHA [A-Za-z]
ALPHANUM [A-Za-z0-9]
SPACE [ \t]
DELIMITERS [;,()\[\]{}]
ARTHEMTIC_OPERATORS [+-]


/**********************************  Rules *************************************/

%%


    /*---------------------------------------
    |      Intialisation code and rules      |
    ----------------------------------------*/

    static int did_init = 0;

    if ( ! did_init ){
        BEGIN(INITIAL_CONDITION);
        did_init = 1;
    }


<INITIAL_CONDITION>{
    \n*     {/*Ignore intial blank space*/}
    .*      {strncpy(linebuf, yytext+1, sizeof(linebuf));  yyless(0); yycolumn=1; BEGIN(INITIAL);}
}



    /*-------------------------------------
    |             Data types               |
    --------------------------------------*/


"mInt"           RETURN(MINT);
"mFloat"        RETURN(MFLOAT);
"mString"       RETURN(MSTRING);
"mStruct"       RETURN(MSTRUCT);
"scalar"        RETURN(SCALAR);




    /*--------------------------------------
    |           CONTROL FLOW               |
    --------------------------------------*/


"if"            RETURN(IF);
"else"          RETURN(ELSE);
"for"           RETURN(FOR);
"break"         RETURN(BREAK);
"pass"          RETURN(PASS);

   
   
    /*--------------------------------------
    |         Functional Keywords          |
    --------------------------------------*/


"Def"           RETURN(DEF);
"return"        RETURN(RETURN);


    /*--------------------------------------
    |            Other Keywords            |
    --------------------------------------*/


"ONES"          RETURN(T_ONES);
"ZEROS"         RETURN(T_ZEROS);
"ROW"           RETURN(T_ROW);
"COLOUM"        RETURN(T_COLOUM);
"IDENTITY"      RETURN(T_IDENTITY);
"lambda"        RETURN(T_LAMBDA);


    /*--------------------------------------
    |               Comments               |
    --------------------------------------*/



"//".*          { /* Single line comment - Just Ignore */ }
"/*"            {BEGIN(ML_COMMENT);}

<ML_COMMENT>{

    [^*\n]*         {/*Just Ignore*/}
    
    "*"+[^*/\n]*    {/*Just Ignore*/}

    \n.*            {copy_line();}

    "*"+"/"         BEGIN(INITIAL);

    <<EOF>>         {
                        extern int WARNING_CNT;
                        WARNING_CNT++;
                        printf(YEL"\nWarning"RESET" --Unterminated comment till EOF\n");
                        yyterminate();
                    }
}



    /*--------------------------------------
    |             Opeartors                |
    --------------------------------------*/


"="             RETURN(OP_ASSIGN);
"++"            RETURN(OP_INCRE);
"--"            RETURN(OP_DECRE);
"**"            RETURN(OP_ELE_MUL);
"*"             RETURN(OP_MAT_MUL);
"$"             RETURN(OP_ACCUMULATOR);
"/"             RETURN(OP_ELE_DIV);
"#"             RETURN(OP_INVERSE);
"."             RETURN(OP_DOT_MUL);
"'"             RETURN(OP_TRANSPOSE);
"=="            RETURN(OP_EQ);
">="            RETURN(OP_GRT_EQ);
"<="            RETURN(OP_LESS_EQ);
">"             RETURN(OP_GRT);
"<"             RETURN(OP_LESS);
"!="            RETURN(OP_NOT_EQ);
"|"             RETURN(OP_MAT_OR);
"||"            RETURN(OP_ELE_OR);
"&"             RETURN(OP_MAT_AND);
"&&"            RETURN(OP_ELE_AND);
"!"             RETURN(OP_ELE_NOT);
"~"             RETURN(OP_MAT_XOR);
"~~"            RETURN(OP_ELE_XOR);
"=>"            RETURN(OP_ASSIGN_FUNC);
"+="            RETURN(OP_ASSIGN_ADD);
"-="            RETURN(OP_ASSIGN_SUB);
"*="            RETURN(OP_ASSIGN_MULT);
"**="            RETURN(OP_ASSIGN_ELM);
"/="            RETURN(OP_ASSIGN_DIV);
{ARTHEMTIC_OPERATORS}     RETURN(yytext[0]);



    /*-------------------------------------
    |            Indentifier              |
    --------------------------------------*/

{ALPHA}{ALPHANUM}*  {    
                        char* buff = (char*)malloc(sizeof(char)*50);
                        strcpy(buff,yytext);

                        yylval.identifier.name = strcat(buff,"\0");
                        yylval.identifier.locinfo = yylloc;

                        Entry* entry = sym_tab_lookup(buff,ENCLOSING_SCOPES);
                        //Check if it is declaration of a struct
                        if(entry && entry->entry_type == TYPE_STRUCT_DECL){
                            RETURN(STRUCT_DTYPE);
                        }
                        else //It is a identifier
                        {
                            RETURN(IDENTIFIER);
                        }
                    }   




    /*-------------------------------------
    |         Constants/Literals          |
    --------------------------------------*/


{DIGIT}+                {yylval.i = atoi(yytext);RETURN(INT_CONST);} 
{DIGIT}+"."{DIGIT}*     {yylval.d = atoi(yytext);RETURN(FLOAT_CONST); }

    /**String literals**/
\"[^\"\n]*\"            RETURN(STRING);
\"[^\"\n]*$             RETURN(STRING);  




   /*-------------------------------------
    |               Others               |
    --------------------------------------*/

    
\n.*                copy_line();     /*Used to save line to report error messages*/  

{DELIMITERS}        RETURN(yytext[0]);    
{SPACE}*            /*just ignore*/

.                   RETURN(TOKEN_INVALID); yyterminate();


%%


/*****************************************************/
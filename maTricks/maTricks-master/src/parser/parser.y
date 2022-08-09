/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*   Lexical Specification to generate the scanner  */
/****************************************************/
/*                  Contributions
 -----------------------------------------------------
*   P Srivardhan -- CS19BTECH11052 --  75 Lines 
*   K Rithvik    -- CS19BTECH11038 --  75 Lines     
*   A Mahidhar   -- CS19BTECH11046 --  50 Lines
*   R Prashanth  -- CS19BTECH11042 --  50 Lines
*   V Nikhilish  -- ES19BTECH11030 --  50 Lines
*   M Shathand   -- CS19BTECH11005 --  Tester
*   Eshwar       -- CS19BTECH11040 --  Tester
*/
/*******************************************************    
Last Modified: 06/11/2021
------------ ChangeLog -------*-------
1. Augmuented the grammar with semantic action
------------ Known Bugs----------------
1. Initalisation of matrix is not accuracte
2. Functions arguments with size of matrix as variable
   not supported
/******************************************************/
%{
    #include <stdio.h>
    #include <stdlib.h>
    
    #include "semantic.h"
    #include "colors.h"

    int yylex();
    void yyerror(const char *s);
    extern char* yytext;

    
    #define err_msg(msg) fprintf(stderr,"\t>> "msg"\n\n")


    /*To debug yacc parsing change to 1*/
    extern int yydebug;

    /*Global varibles to build symbol table*/
    int curr_id_type;
    char* struct_type_name;
    //typedef struct expression Expression;
    
    
    
%}

%code requires{
    typedef struct YYLTYPE
    {
        int first_line;
        int first_column;
        int last_line;
        int last_column;
    }YYLTYPE;
    #define YYLTYPE_IS_DECLARED 1
    typedef struct expression
    {
        void* info;
        int type;
        int value;
        char* ir_name;//For IR
    }Expression;

}

/*****Attributes for each token/symbol****/

%union {
    char * string;
    int i;
    float f;
    double  d;
    void* info_ptr;
    struct identifier
    {
        char* name;
        YYLTYPE locinfo;
    }identifier;
    Expression* expression;
    
}


/*------------------------------------------------------
| Need to clean all these tokens (with types)   |
--------------------------------------------------------*/
%token MINT MFLOAT MSTRING 
%token<string> STRUCT_DTYPE FUNCTION_CALL_NAME 
%token<info_ptr> MSTRUCT
%token IF ELSE FOR BREAK PASS
%token DEF RETURN
%token<info_ptr> T_ONES T_ZEROS  T_IDENTITY T_LAMBDA
%token<i> T_ROW T_COLOUM
%token  OP_ASSIGN_FUNC

%token<f> SCALAR 
%token<string> STRING
%token<i> INT_CONST 
%token<f> FLOAT_CONST
%token TOKEN_INVALID
%token<identifier> IDENTIFIER 
%token OP_INCRE OP_DECRE

/***Precedence and associativity****/

%left OP_ASSIGN
%left OP_ASSIGN_ADD OP_ASSIGN_SUB
%left OP_ASSIGN_MULT OP_ASSIGN_ELM OP_ASSIGN_DIV
%left OP_MAT_OR OP_ELE_OR OP_MAT_AND OP_ELE_AND OP_ELE_NOT OP_MAT_XOR OP_ELE_XOR
%left OP_EQ OP_GRT_EQ OP_LESS_EQ OP_GRT OP_LESS OP_NOT_EQ


%right <i> '+' '-'
%right <i> OP_MAT_MUL OP_ELE_MUL OP_ELE_DIV OP_DOT_MUL
%left OP_ACCUMULATOR   OP_INVERSE  OP_TRANSPOSE 
%left '('

%locations


%type<i> type_specifier;

%type<expression>  expression cast_expression primary_expr infix_expression postfix_expression assignment_expression function_call 

/************************* Start of Grammer ************************/

%%

statement_list
    : statement_list statement 
    |       
    ;

statement
    : declaration_statement 
    | expression_statement  ';'    
    | compound_statement
    | return_statement     ';'
    | selection_statement    
    | iteration_statement    
    | function_definition
    | struct_definition
    | ';'                    /*Empty statement*/
    | error ';'             /*Prevent cascade of errors*/
    | TOKEN_INVALID  error ';'
    ;



return_statement
    : RETURN 
    | RETURN expression_statement


compound_statement
    : '{' {enter_new_scope();} statement_list {exit_curr_scope();} '}'




    /*-------------------------------------
    |             Declarations            |    ------   Change lexer to return  SCALAR instead of INT_CONST in size specification
    --------------------------------------*/

declaration_list
    : declaration_list declaration_statement
    | declaration_statement
    ;

declaration_statement
    : type_specifier identifier_list ';'         
    | type_specifier error ';'                   {err_msg("Expected a delimineter after a declaration");}

type_specifier
    :MINT                                        {curr_id_type = TYPE_MINT;}
    |MFLOAT                                      {curr_id_type = TYPE_MFLOAT;}
    |STRUCT_DTYPE                                {curr_id_type = TYPE_MSTRUCT; struct_type_name=$1;}
    |MSTRING                                     {curr_id_type = TYPE_MSTRING;}
    |SCALAR                                      {curr_id_type = TYPE_MINT;}
    ;


identifier_list
    : identifier_list ',' identifier                
    | identifier                                 
    ;


identifier
    : IDENTIFIER '(' INT_CONST ',' INT_CONST ')' identifier_initialisation   {check_and_insert_identifier($1.name,curr_id_type,$3,$5,$1.locinfo);}  
    | IDENTIFIER  error identifier_initialisation                            {err_msg("Expected size of matrix while declaring");}
    ;

identifier_initialisation
    :    /*declaration without intialisation*/ 
    | OP_ASSIGN '(' ')' compound_statement
    | OP_ASSIGN T_ONES                                  {ONES_ZEROS_initilisation();}
    | OP_ASSIGN T_ZEROS                                 {ONES_ZEROS_initilisation();}
    | OP_ASSIGN assignment_expression                   {/*FIXME:Check the type of matrix to assignment expression. Avaliable in global varaiable curr_id_type */}
    /*****To handle errors in size declaration*****/
    | INT_CONST                                         {if(!YYRECOVERING()){yyerror("");err_msg("Expected size of matrix while declaring");}}
    | ')'                                               {if(!YYRECOVERING()){yyerror("");err_msg("Expected size of matrix while declaring");}}




struct_definition      
    : MSTRUCT IDENTIFIER {$<info_ptr>$=struct_def_start($2.name,yylloc);} '{' declaration_list '}'   ';'   {struct_def_end($<info_ptr>3);}
    ;


    /*-------------------------------------
    |             Expressions             |
    --------------------------------------*/


primary_expr
    : IDENTIFIER                                         {$$=convert_to_expression($1.name);}                                                                                                                              
    | IDENTIFIER '['INT_CONST']''['INT_CONST']'          {$$=expression_with_type2(find_type(convert_to_expression($1.name)->info),$1.name,$3,$6);}   //ele type
    | INT_CONST                                          {$$=expression_with_type(EXP_INT,$1);}
    | FLOAT_CONST                                        {$$=expression_with_type(EXP_FLOAT,$1);}
    | STRING                                             {$$=expression_with_type(EXP_STRING,0);}
    | SCALAR                                             {$$=expression_with_type(EXP_FLOAT,$1);}
    | T_IDENTITY                                         {$$=expression_with_type(EXP_INT,0);}
    | T_COLOUM                                           {$$=expression_with_type(EXP_INT,$1);}
    | T_ROW                                              {$$=expression_with_type(EXP_INT,$1);}
    | '(' expression ')'                                 {$$=$2;}       
    | function_call                                      {$$=$1;}

    /*Error Handling*/
    ;

infix_expression
    :primary_expr                                         {$$=$1;}
    |OP_INCRE primary_expr                                {$$=$2;}                       
    |OP_DECRE primary_expr                                {$$=$2;} 
    ;


postfix_expression
    :infix_expression                                     {$$=$1;}
    |infix_expression OP_INCRE                            {$$=$1;} 
    |infix_expression OP_DECRE                            {$$=$1;} 
    ;


cast_expression
    :postfix_expression                                   {$$=$1;} 
    |'['type_specifier']' cast_expression                 {$$=$4;}  //FIXME: type checking
    ;


expression
    :cast_expression                                    {$$=$1;} 
    |expression OP_MAT_MUL expression                   {$$ = check_operators_mat($3,OP_MAT_MUL,$1);  }
    |expression OP_DOT_MUL expression                   {$$ = check_operators_mat($3,OP_DOT_MUL,$1);  }
    |expression OP_ELE_MUL expression                   {$$ = check_operators_mat($3,OP_ELE_MUL,$1);  }
    |expression OP_ELE_DIV expression                   {$$ = check_operators_mat($3,OP_ELE_DIV,$1);  } /*tobechanged*/
    |OP_INVERSE expression                              {$$ = check_operators_mat($2,OP_INVERSE,$2);  } /*tobechanged*/
    |expression OP_TRANSPOSE                            {$$ = check_operators_mat($1,OP_TRANSPOSE,$1);}   
    |OP_ACCUMULATOR expression                          {$$ = $2;}     
    |expression '+' expression                          {$$ = check_operators_mat($3,'+',$1);}
    |expression '-' expression                          {$$ = check_operators_mat($3,'-',$1);}
    |expression OP_GRT_EQ expression                    {$$ = check_operators_mat($3,OP_GRT_EQ,$1);}     
    |expression OP_LESS_EQ expression                   {$$ = check_operators_mat($3,OP_LESS_EQ,$1);}   
    |expression OP_GRT expression                       {$$ = check_operators_mat($3,OP_GRT,$1);}
    |expression OP_LESS expression                      {$$ = check_operators_mat($3,OP_LESS,$1);}
    |OP_ELE_NOT expression                              {$$ = $2;}
    |expression OP_EQ expression                        {$$ = check_operators_mat($3,OP_EQ,$1);}  
    |expression OP_NOT_EQ expression                    {$$ = check_operators_mat($3,OP_NOT_EQ,$1);}     
    |expression OP_MAT_OR expression                    {$$ = check_operators_mat($3,OP_MAT_OR,$1);}    
    |expression OP_ELE_OR expression                    {$$ = check_operators_mat($3,OP_ELE_OR,$1);}   /*tobechanged*/ 
    |expression OP_ELE_AND expression                   {$$ = check_operators_mat($3,OP_ELE_AND,$1);}  /*tobechanged*/ 
    |expression OP_MAT_AND expression                   {$$ = check_operators_mat($3,OP_MAT_AND,$1);}  
    |expression OP_MAT_XOR expression                   {$$ = check_operators_mat($3,OP_MAT_XOR,$1);} 
    |expression OP_ELE_XOR expression                   {$$ = check_operators_mat($3,OP_ELE_XOR,$1);}  /*tobechanged*/
    ; 

assignment_expression
    : expression                                         {$$=$1;}
    | primary_expr OP_ASSIGN assignment_expression      {$$=check_operators_mat($3,OP_ASSIGN,$1);}   
    | primary_expr OP_ASSIGN_ADD assignment_expression  {$$=check_operators_mat($3,OP_ASSIGN_ADD,$1);}
    | primary_expr OP_ASSIGN_MULT assignment_expression {$$=check_operators_mat($3,OP_ASSIGN_MULT,$1);}
    | primary_expr OP_ASSIGN_SUB assignment_expression  {$$=check_operators_mat($3,OP_ASSIGN_SUB,$1);}
    | primary_expr OP_ASSIGN_ELM assignment_expression  {$$=check_operators_mat($3,OP_ASSIGN_ELM,$1);}
    | primary_expr OP_ASSIGN_DIV assignment_expression  {$$=check_operators_mat($3,OP_ASSIGN_DIV,$1);}
    ;
expression_statement
    : assignment_expression  
    | expression_statement ',' assignment_expression
    ;


 
    /*---------------------------------------------------
    |        Selction and Iterative statements           |
    ----------------------------------------------------*/

selection_statement
    :IF '(' expression_statement ')'  compound_statement                                          
    |IF '(' expression_statement ')'  compound_statement    ELSE   selection_statement
    |IF '(' expression_statement ')'  compound_statement    ELSE   compound_statement
    ;

iteration_statement
    :FOR '(' expression_statement ';' expression_statement ';' expression_statement ')'  '{' statement_list '}'
    ;

    /*----------------------------------------
    |          Functional statements          |
    -----------------------------------------*/

parameter
    :type_specifier IDENTIFIER '(' INT_CONST ',' INT_CONST ')'             {function_add_arg(curr_id_type,$2.name,$4,$6,$2.locinfo);}

parameter_list
    : parameter                         
    | parameter ',' parameter_list
    ;

function_definition
    : DEF IDENTIFIER {$<info_ptr>$=function_def_start($2.name,yylloc);} '(' parameter_list ')' OP_ASSIGN_FUNC type_specifier '(' INT_CONST ',' INT_CONST ')' '{' statement_list '}' 
            {function_def_end($<info_ptr>3,$8,$10,$12);}
    ;



function_call
    : IDENTIFIER  {function_call($1.name,$1.locinfo);} '(' arg_list ')'      {$$=function_call_end();}                   
    ;
arg_list
    : arg_list ',' arg
    | arg 
    |
    ;

    //Change of grammar is required to support expressions as arguments
arg 
    : IDENTIFIER          {function_check_arg($1.name);}
    ;


%%


/************************** Sub Routines********************************/

void yyerror(const char *s){

    extern char linebuf[500];
    extern int ERROR_CNT;
    
    ERROR_CNT++;


    if(yylloc.first_line)
    fprintf(stderr,
            RED"Error  "RESET"--"CYN"l%d:c%d | l%d:c%d"RESET" ---> Unexpected token '"YEL"%s"RESET"' in line\n\t>>  %s\n\t    ",
            yylloc.first_line, yylloc.first_column,
            yylloc.last_line,  yylloc.last_column,
            yytext,linebuf);


    //Highlight the width
    int i=1;
    while(i<=yylloc.last_column) 
        if(i++>=yylloc.first_column)
            fprintf(stderr,RED"~"RESET);
        else
            fprintf(stderr," ");
    fprintf(stderr,"\n");
}


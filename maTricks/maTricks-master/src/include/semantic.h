/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*                Semantics Header                 */
/****************************************************/
/*                  Contributions
 -----------------------------------------------------
*   P Srivardhan -- CS19BTECH11052
*   K Rithvik    -- CS19BTECH11038
*/
/*******************************************************    
Last Modified: 04/12/2021
------------ ChangeLog -------*-------
1.Intial creation
/******************************************************/

#ifndef SEMANTICS_H
#define SEMANTICS_H

#include "sym_tab.h"
#include "parser.tab.h"


enum EXPRESSION_TYPE
{
    EXP_INT,
    EXP_FLOAT,
    EXP_STRING,
    EXP_MAT,
    EXP_BOOL
};

/*****************************************************************/
FuncInfo* curr_func_info_ptr;


void semantic_err(char* token_name,char* msg,YYLTYPE* locinfo);


void check_and_insert_identifier(char* name,int type,int rows,int cols,YYLTYPE locinfo);
void ONES_ZEROS_initilisation();


StructInfo* struct_def_start(char* name, YYLTYPE locinfo);
void struct_def_end();


FuncInfo* function_def_start(char* name,YYLTYPE locinfo);
void function_add_arg(int type,char* name,int rows,int cols,YYLTYPE locinfo);
void function_def_end(FuncInfo* info,int ret_type,int rows,int cols);
void function_call(char* name,YYLTYPE info);
void function_check_arg(char* name);
Expression* function_call_end();


/**OPERATIONS CHECKING**/
Expression* check_operators_mat(Expression*  Av,int op,Expression*  Bv);
void check_size(MatInfo * A,int op,MatInfo * B);
MatInfo* Mat_Operation(MatInfo*  A,int op,MatInfo*  B);
Expression* convert_to_expression(char *a);
int matrixoperations(int op);
void  check_type(void* Av,int op,void* Bv);
int compatibletype(enum TYPE a,enum TYPE b);
char* sizeprint(MatInfo* A,MatInfo* B);
struct expression* expression_with_type(int d,int v);
Expression*  expression_with_type2(int d,char *v,int a,int b);
int convert_type_reverse(int a);
int convert_type(int a);
int precedence(int a,int b);
char* type_print(int a);
char* print_compatale_error(int a,int b);
int find_type(MatInfo *A);




#endif
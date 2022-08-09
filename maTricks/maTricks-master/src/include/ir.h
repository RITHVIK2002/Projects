/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*                   To IR Header                  */
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
------------ Known Bugs----------------
/******************************************************/

#ifndef IR_H
#define IR_H

#include "sym_tab.h"
#include "parser.tab.h"


#define IR_PROG_FILE "ir_prog_file.ll"
#define IR_FUNC_FILE "ir_func_file.ll"
#define IR_RUN_FILE "run.cpp"


void ir_define(char* name,int type,int rows,int cols);
char* ir_binary_op(char* A,int op, char * B,MatInfo* matInfo);


void ir_func_def_start(char* name);
void ir_add_func_arg();
void ir_func_def_end();

void ir_func_call(char* name,FuncInfo* funcInfo);
void ir_func_call_add_arg(char* name);
void ir_func_call_end();

#endif
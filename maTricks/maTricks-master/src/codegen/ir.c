/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*                      To IR                        */
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ir.h"

#define FPRINTF fprintf(func_def_active?ir_func_file:ir_prog_file

FILE* ir_prog_file;
FILE* ir_func_file;

int temp_counter=0;
int func_def_active=0;

void ir_init()
{
    if(ir_prog_file==NULL)
    {
        ir_prog_file = fopen(IR_PROG_FILE,"w");
        if(ir_prog_file==NULL)
        {
            fprintf(stderr,"Cannot open IR..\n");
            exit(1);
        }
    }
    if(ir_func_file==NULL)
    {
        ir_func_file = fopen(IR_FUNC_FILE,"w");
        if(ir_func_file==NULL)
        {
            fprintf(stderr,"Cannot open IR..\n");
            exit(1);
        }
    }
}




void ir_define(char* name,int type,int rows,int cols)
{
    ir_init();
    
    FPRINTF,
        "define %d %s %d,%d\n",type,name,rows,cols
        );
}

void ir_assign(char* lhs,char* rhs)
{
    ir_init();
    FPRINTF,"assign %s %s\n",lhs,rhs);

}

char* ir_binary_op(char* A,int op, char* B, MatInfo* matInfo)
{
    ir_init();
    if(op==OP_ASSIGN)
    {
        if(matInfo!=NULL)
        {   
            fprintf(ir_prog_file,"assign %s %s\n",B,A);
            return A;
        }
        else
        {
            fprintf(ir_prog_file,"val-assign %s %s\n",B,A);
            return A;  
        }
    }
        
    //Make new temporary var and define it
    char* t_var = (char*)malloc(10*sizeof(char));
    sprintf(t_var,"t%d",temp_counter);
    
    if(matInfo!=NULL)
    {
        ir_define(t_var,1,matInfo->rows,matInfo->cols);
        FPRINTF,"assign %s %s %d %s\n",t_var,A,op,B);
        //Add size info to ir_name
        sprintf(t_var,"t%d %d,%d",temp_counter++,matInfo->rows,matInfo->cols);
        return t_var;
    }
    else{
        ir_define(t_var,-1,-1,-1);
        FPRINTF,"val-assign %s %s %d %s\n",t_var,A,op,B);
        //Add size info to ir_name
        sprintf(t_var,"t%d %d,%d",temp_counter++,-1,-1);
        return t_var;
    }
    //Assign it
   
}


/******************************************************************/

void ir_func_def_start(char* name)
{
    ir_init();
    func_def_active=1;
    fprintf(ir_func_file,"%%func %s\n",name);
}

void ir_add_func_arg()
{
    ir_init();
    fprintf(ir_func_file,"arg-");
}


void ir_func_def_end(){
    ir_init();
    fprintf(ir_func_file,"%%end\n");
    func_def_active=0;
}




void ir_func_call(char* name,FuncInfo* funcInfo){
    ir_init();

    //Define temp var for return type
    char* t_var = (char*)malloc(10*sizeof(char));
    sprintf(t_var,"t%d",temp_counter);
    ir_define(t_var,funcInfo->retType,funcInfo->rows,funcInfo->cols);
    sprintf(t_var,"t%d %d,%d",temp_counter++,funcInfo->rows,funcInfo->cols);
    //Assign it
    FPRINTF,"call-assign %s %s",t_var,name);
    funcInfo->ir_name =  t_var;
}

void ir_func_call_add_arg(char* name){
    ir_init();
    FPRINTF," %s",name);
}

void ir_func_call_end(){
    ir_init();
    FPRINTF,"\n");
}
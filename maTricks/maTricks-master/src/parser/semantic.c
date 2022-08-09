/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*         File containing the semantic logic       */
/****************************************************/
/*                  Contributions
 -----------------------------------------------------
*   P Srivardhan -- CS19BTECH11052  
*   K Rithvik    -- CS19BTECH11038   
*   A Mahidhar   -- CS19BTECH11046  
*/
/*******************************************************    
Last Modified: 04/12/2021
------------ ChangeLog -------*-------
1.Intial creation
------------ Known Bugs----------------
1. Currently arguments to functions should be only vars
   expressions as args are not supported.
   Slight Change of grammar is required to get this but
   semantics is performed assuming expressions as args

/******************************************************/

#include <stdio.h>
#include <stdlib.h>

#include "semantic.h"
#include "colors.h"
#include "ir.h"

extern int temp_counter;
/******** Helper Error Function **********/
void semantic_err(char* token_name,char* msg,YYLTYPE* locinfo)
{
    
    extern char linebuf[500];
    extern int ERROR_CNT;
    extern char* yytext;

    ERROR_CNT++;


    if((*locinfo).first_line)
    fprintf(stderr,
            RED"Error  "RESET"--"CYN"l%d:c%d | l%d:c%d"RESET" ---> %s '"YEL"%s"RESET"'\n\t>>  %s\n\t    ",
            (*locinfo).first_line, (*locinfo).first_column,
            (*locinfo).last_line,  (*locinfo).last_column,
            msg,token_name,linebuf);


    //Highlight the width
    int i=1;
    while(i<=(*locinfo).last_column) 
        if(i++>=(*locinfo).first_column)
            fprintf(stderr,RED"~"RESET);
        else
            fprintf(stderr," ");
    fprintf(stderr,"\n");
}

void check_and_insert_identifier(char* name,int type,int rows,int cols,YYLTYPE locinfo)
{


    //Else insert into normal symbol table
    if(sym_tab_lookup(name,CURRENT_SCOPE))
    {
        semantic_err(name,"Identifier already defined ",&locinfo);
        return;
    }


    /*Creating MatInfo*/
    MatInfo* info= (MatInfo*)malloc(sizeof(MatInfo));
    info->rows=rows;info->cols=cols ;
    info->entry_type=type;
    /*Insertion*/
    sym_tab_insert(name,type,locinfo,info);
    // printf("locinfo %s %d,%d,%d,%d\n",name,locinfo.first_column,locinfo.first_line,locinfo.last_column,locinfo.last_line);


    //For Codegen
    ir_define(name,type,rows,cols);

}


//Handles the initilasation matrices of all ones and zeros of INT and FLOAT types
void ONES_ZEROS_initilisation()
{
    extern int curr_id_type;

    if(curr_id_type!=TYPE_MINT && curr_id_type!=TYPE_MFLOAT) 
        semantic_err("","Uncompatiable element types. Cannot be initialised",&yylloc);
    else
    {
        //FIXME: Add initialisation in code gen phase
    }    
}




//Handling struct declaration
StructInfo* struct_def_start(char* name, YYLTYPE locinfo)
{
    
    //Create new symbol table for struct declaration
    StructInfo* info = (StructInfo*) malloc(sizeof(StructInfo));
    sym_tab_insert(name,TYPE_STRUCT_DECL,locinfo,info);
    info->prev_tab = curTable;
    curTable =  info->sym_tab = new_Table();
    return info;
}

void struct_def_end(StructInfo* info)
{
    curTable = info->prev_tab;
}




/**************Functions********************************/

ArgInfo* curArg;
int skip_func_check,arg_cnt;

FuncInfo* function_def_start(char* name,YYLTYPE locinfo)
{
    FuncInfo* info = (FuncInfo*)malloc(sizeof(FuncInfo));
    //Insert into symtable
    sym_tab_insert(name,TYPE_FUNCTION,locinfo,info);
    //Increment the scope
    enter_new_scope();
    info->sym_tab = curTable;
    //Setting the global current function pointer
    curr_func_info_ptr = info;
    //
    curArg = NULL;
    //CodeGen-IR
    ir_func_def_start(name);
    return info;
}


void function_add_arg(int type,char* name,int rows,int cols,YYLTYPE locinfo)
{
    FuncInfo* info = curr_func_info_ptr;
    //Add arg and increment count
    info->_argCount++;
    //Codegen-IR
    ir_add_func_arg();
    //add the argument to symtab
    check_and_insert_identifier(name,type,rows,cols,locinfo);
    //Add argment info for semantic checking
    ArgInfo* argInfo =  (ArgInfo* ) malloc(sizeof(argInfo));
    argInfo->cols = cols; argInfo->rows = rows; argInfo->entry_type = type;
    
    if(!curArg)
        info->arg_list = argInfo;
    else
        curArg->next = argInfo;
    curArg = argInfo;

}


void function_def_end(FuncInfo* info,int ret_type,int rows,int cols)
{
    //Get Out of current scope
    exit_curr_scope();
    curr_func_info_ptr=NULL;
    //Set return val
    info->retType =ret_type;
    info->rows = rows;
    info->cols = rows;
    //Ir_instruction
    ir_func_def_end();
}



void function_call(char* name,YYLTYPE info)
{
    Entry* e = sym_tab_lookup(name,ENCLOSING_SCOPES);
    if(e==NULL)
    {
        semantic_err(name,"No Such Function named",&info);
        skip_func_check =1;
    }
    FuncInfo* funcInfo = (FuncInfo*)e->info; 
    curArg = funcInfo->arg_list;
    curr_func_info_ptr = funcInfo;
    arg_cnt = funcInfo->_argCount;
    //Ir
    ir_func_call(name,funcInfo);
}


void function_check_arg(char* name)
{
    
    if(skip_func_check) 
        return;
    if(arg_cnt<=0)
    {
        semantic_err("","too many arguments in function call",&yylloc);
        return;
    }
    //FIXME: change this to support expressions as arguments
    Entry* e = sym_tab_lookup(name,ENCLOSING_SCOPES);
    if(!e)
    {
        semantic_err(name,"Identifier not declared earilier",&yylloc);
        arg_cnt--;
        curArg= curArg->next;
        return;
    }
    MatInfo* info = (MatInfo*) e->info;
    //Check the argument with the arg_list stored in the sym_tab
    if(curArg->entry_type != info->entry_type)
    {
        semantic_err("","Argument type mismatch",&yylloc);
    }
    else if(curArg->cols != info->cols || curArg->rows != info->rows)
    {
        semantic_err("","Argument size mismatch",&yylloc);
        fprintf(stderr,"\t>>  Found argument of size (%d,%d) but expected (%d,%d) \n\n",info->rows,info->cols,curArg->rows,curArg->cols);

    }
    arg_cnt--;
    curArg= curArg->next;
    //Ir
    ir_func_call_add_arg(name);

}


Expression* function_call_end()
{
    if(arg_cnt!=0)
    {
        semantic_err("","Too few arguments for the function call",&yylloc);
    }
    //FIXME: Need to make it more efficient

    if(skip_func_check)
    {
        skip_func_check = 0;
        Expression *res = (Expression *)malloc(sizeof(Expression));
        MatInfo *matInfo =(MatInfo*) malloc(sizeof(MatInfo));
        matInfo->entry_type = TYPE_INT;
        res->info = matInfo;
        res->type = EXP_MAT;

        return res;
    }
    else
    {
        //Info for return Type
        FuncInfo* funcInfo = curr_func_info_ptr;
        MatInfo *matInfo =(MatInfo*) malloc(sizeof(MatInfo));
        Expression *res = (Expression *)malloc(sizeof(Expression));
        matInfo->cols = funcInfo->cols; matInfo->rows = funcInfo->rows;
        matInfo->entry_type = funcInfo->retType;
        res->info = matInfo;
        res->type = EXP_MAT;
        res->ir_name = funcInfo->ir_name;
        ir_func_call_end();
        return res;
    }
}

/**************Expressions***************/
Expression* convert_to_expression(char *a)
{
    Entry* e = sym_tab_lookup(a,ENCLOSING_SCOPES);
    if(e)
        {
            Expression * res=(Expression *)malloc(sizeof(Expression));
            res->info=e->info;
            res->type=EXP_MAT;
            //Ir_name append size to it
            char* temp_ir_name = (char*)malloc(10);
            sprintf(temp_ir_name,"%s %d,%d",a,((MatInfo*)e->info)->rows,((MatInfo*)e->info)->cols);
            res->ir_name = temp_ir_name;
            return res;
        }
    else
    {
        semantic_err(a,"Non Declared Variable  ",&yylloc);
        return NULL;
    } 

}
int precedence(int a,int b)
{
     if ( a>=b)
     {
         return a;
     }
     else{
         return b;
     }
}

MatInfo* Mat_Operation(MatInfo*  A,int op,MatInfo*  B)
{
     /*matrix operations to be added DONT FORGET ME**/
    MatInfo* t=(MatInfo*)malloc(sizeof(MatInfo));


    switch(op)
    {
        case OP_MAT_MUL://multplication
            t->rows=A->rows;
            t->cols=B->cols;
            t->entry_type=precedence(A->entry_type,B->entry_type);
            return t;
        case OP_TRANSPOSE: //transpose
            t->rows=A->cols;
            t->cols=A->rows;
            t->entry_type=precedence(A->entry_type,B->entry_type);
            return t;
        case OP_INVERSE: //transpose
            t->rows=A->rows;
            t->cols=A->cols;
            t->entry_type=precedence(A->entry_type,B->entry_type);
            return t;
        case OP_DOT_MUL://dot
            t->rows=A->rows;
            t->cols=1;
            t->entry_type=precedence(A->entry_type,B->entry_type);
            return t;
        case OP_MAT_OR://logical and remaining arthematic operations
        case OP_MAT_AND:
        case OP_MAT_XOR:
        case OP_ASSIGN:
        case OP_ASSIGN_ADD:
        case OP_ASSIGN_MULT:
        case OP_ASSIGN_SUB:
        case 43://add
        case 45://sub
            t->rows=B->rows;
            t->cols=B->cols;
            t->entry_type=precedence(A->entry_type,B->entry_type);
            return t; 

    }
}
char* sizeprint(MatInfo* A,MatInfo* B)
{
    char *s = (char*)malloc (sizeof (char)*200); 
    sprintf(s, "(%d,%d) and (%d,%d)", A->rows,A->cols,B->rows,B->cols);
    //printf("%s",s);
    
    return s;
}
void check_size(MatInfo * A,int op,MatInfo * B)
{

    switch(op)
    {
        case OP_ASSIGN_MULT:
        case OP_MAT_MUL: //OP_MAT_MUL
            if(A->cols!=B->rows)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be multiplied",&yylloc);  
            }
            break;
        case OP_INVERSE://INVERSE
            if(A->rows!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be inverted",&yylloc);
                
            }
            break;
        case OP_DOT_MUL://DOT
            if(A->rows!=B->rows||A->cols!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be dot product",&yylloc);
            }
            break;
        case OP_MAT_OR://LOGICAL
        case OP_MAT_AND:
        case OP_MAT_XOR:
            if(A->rows!=B->rows||A->cols!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be logical operated",&yylloc);
            }
            break;
        case OP_ASSIGN_ADD:
        case '+'://ADD
             if(A->rows!=B->rows||A->cols!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be added",&yylloc);
            }
            break;
        case OP_ASSIGN_SUB:
        case '-'://SUB
            if(A->rows!=B->rows||A->cols!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be substraction",&yylloc);
            }
            break;
        case OP_ASSIGN:
            if(A->rows!=B->rows||A->cols!=B->cols)
            {
                semantic_err(sizeprint(A,B),"Uncompatiable element sizes cannot be assigned",&yylloc);
            }
            break;
    }
}
int compatibletype(enum TYPE a,enum TYPE b)
{
    
    switch(a)
    {
        case TYPE_MINT://MINT
        case TYPE_MFLOAT://MFLOAT
            if(b==TYPE_MFLOAT||b==TYPE_MINT)
            {
                return 1;
            }
            break;

        case TYPE_MSTRING://MSTRING
            if(b==TYPE_MSTRING)
            {
                return 1;
            }
            break;

    }
    return 0;
}
char* type_print(int a)
{
    switch(a)
    {
    case TYPE_MINT: return "MINT" ;
    case TYPE_MFLOAT: return "MFLOAT" ;
    case TYPE_MSTRING: return "MSTRING";
    case TYPE_MSTRUCT: return "MSTRUCT";
    case  TYPE_FUNCTION: return "Function";
    case  TYPE_STRUCT_DECL: return "Struct decl";
    }
    return " ";
}
char* print_compatale_error(int a,int b)
{
    char *ah=(char*)malloc(sizeof(char)*200);
    sprintf(ah,"%s  %s",type_print(a),type_print(b));
    return ah;
}
void check_type(void* Av,int op,void* Bv)
{
    if(matrixoperations(op))
    {
        MatInfo *   A=(MatInfo * ) Av; 
        MatInfo *   B=(MatInfo * ) Bv;
        //printf("%d %d\n",A->entry_type,B->entry_type);
        if(compatibletype(A->entry_type,B->entry_type)==0)
        {
            
             semantic_err(print_compatale_error(A->entry_type,B->entry_type),"Uncompatiable element type for the matrix operation ",&yylloc);
        }
    }

}
int find_type(MatInfo *A)
{
    //printf("%d %d",convert_type_reverse(A->entry_type),EXP_STRING);
    //printf("dfsf");
    //printf("%d",convert_type_reverse(A->entry_type));
    return convert_type_reverse(A->entry_type);
}
int assignoperations(int op)
{
    if(op==OP_ASSIGN||op==OP_ASSIGN_ADD||op==OP_ASSIGN_MULT||op==OP_ASSIGN_SUB)
    {
        return 1;
    }
    return 0;
}
int matrixoperations(int op)
{
    if( assignoperations( op)||op==OP_MAT_MUL||op==OP_DOT_MUL ||op==OP_INVERSE||op==OP_MAT_OR||op==OP_MAT_AND||op==OP_MAT_XOR||op==OP_TRANSPOSE||op== 43 || op==45 ||op==OP_ASSIGN||op==OP_ASSIGN_ADD||op==OP_ASSIGN_SUB||op==OP_ASSIGN_MULT)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int elemoperation(int op)
{
    if(op==OP_ELE_OR||op==OP_ELE_AND||op==OP_ELE_XOR||op==OP_ELE_MUL||op==OP_ELE_DIV||op==OP_ASSIGN_ELM||op==OP_ASSIGN_DIV)
    {
        return 1;
    }
    return 0;
}


int convert_type_reverse(int a)
{
    if(a==TYPE_MINT)
    {
        return EXP_INT;
    }
    else if(a==TYPE_MFLOAT)
    {
        return EXP_FLOAT;
    }
    else if(a==TYPE_MSTRUCT)
    {
        return EXP_MAT;
    }
    
    return EXP_STRING;
}
int convert_type(int a)
{
    if(a==EXP_INT)
    {
        return TYPE_MINT;
    }
    else if(a==EXP_FLOAT)
    {
        return TYPE_MFLOAT;
    }
    return TYPE_MSTRING;
}
Expression*  expression_with_type(int d,int v)
{
    Expression* R=(Expression* )malloc(sizeof(Expression));
    R->type=d;
    R->info=NULL;
    R->value=v;
    char* temp_ir_name = (char*)malloc(10);
    sprintf(temp_ir_name,"%d %d,%d",v,-1,-1);
    R->ir_name = temp_ir_name;
    //printf(" type %d\n" ,R->type);
    return R;
}
Expression*  expression_with_type2(int d,char *v,int a,int b)
{
    Expression* R=(Expression* )malloc(sizeof(Expression));
    R->type=d;
    R->info=NULL;
    R->value=0;
    char* temp_ir_name = (char*)malloc(10);
    sprintf(temp_ir_name,"%s %d,%d",v,a,b);
    R->ir_name = temp_ir_name;
    //printf(" type %d\n" ,R->type);
    return R;
}

Expression* check_operators_mat(Expression*  Av,int op,Expression*  Bv)
{
     
        if(Av==NULL)
        {
            semantic_err("","Element Not found ",&yylloc);
            if(Bv==NULL)
            {
               
                return NULL;
            }
            return Bv;
        }
        else
        {
            if(Bv==NULL)
            {
                semantic_err("","Element Not found ",&yylloc);
                return Av;
            }
        }
         
        if(op>=298 && op<=303)
        {
            
            //return Mat_Operation(A,op,B);
            /***matrix comparition operations to be added  DONT FIXME: FORGET ME**/
           
            Expression* R=(Expression* )malloc(sizeof(Expression));
            R->type=EXP_INT;
            return R;
        }
        
        if(Av->type==EXP_MAT && Bv->type==EXP_MAT)
        {
        
            MatInfo *   B= Bv->info;
            MatInfo *   A= Av->info;
            if(B->entry_type!=TYPE_MSTRUCT && B->entry_type!=TYPE_FUNCTION) 
            {
                
                //MatInfo *   B= Bv->info;
                if(A->entry_type!=TYPE_MSTRUCT) 
                {
                    //MatInfo *   A= Av->info;
                    
                    if(compatibletype(convert_type(Av->type),convert_type(Bv->type))==0)
                    {

                     semantic_err(print_compatale_error(A->entry_type,B->entry_type),"Uncompatiable element type for the matrix operation ",&yylloc);
                    }
                    check_size(A,op,B);
                    Expression* R=(Expression* )malloc(sizeof(Expression));
                    R->type=Av->type;//FIX ME: to be changed based on priority
                    R->info=Mat_Operation(A,op,B);
                    //Codegen
                    R->ir_name = ir_binary_op(Av->ir_name,op,Bv->ir_name,R->info);
                    return R;
    
                }
                else
                {
                   // StructInfo* A=Av->info;
                    semantic_err("","Uncompatiable element struct type for the matrix operation ",&yylloc);
                    return Bv;
    
                }
            }
            else
            {
                semantic_err("","Uncompatiable element struct type for the matrix operation ",&yylloc);
                return Bv;
            }
        }
        else 
        {
           
            if(elemoperation(op))
            {
                if(Av->type==EXP_MAT)
                {                           //***** ELEMENTARY OPERATIONS *******//

                    MatInfo *   A= Av->info;
                    if(A->entry_type!=TYPE_MSTRUCT && A->entry_type!=TYPE_FUNCTION) 
                    {
                        //MatInfo *   A= Av->info;
                        
                        if(compatibletype(A->entry_type,convert_type (Bv->type))==0)
                        {
                           semantic_err(print_compatale_error(A->entry_type,convert_type (Bv->type)),"Uncompatiable element  type for the matrix operation ",&yylloc); 
                        }
                        return Av;
                    }
                    else{
                        
                        semantic_err("","Uncompatiable element struct type for the matrix operation ",&yylloc);
                        return Av;
                    }
                }
                else if(Bv->type==EXP_MAT)
                {
                    MatInfo *   B= Bv->info;  
                    if(B->entry_type!=TYPE_MSTRUCT && B->entry_type!=TYPE_FUNCTION) 
                    {
                       
                        if(compatibletype(convert_type (Av->type),B->entry_type)==0)
                        {
                           semantic_err(print_compatale_error(convert_type (Av->type),B->entry_type),"Uncompatiable element  type for the matrix operation ",&yylloc); 
                        }
                        return Bv;
                    }
                    else{
                        
                        semantic_err("","Uncompatiable element struct type for the matrix operation ",&yylloc);
                        return Bv;
                    }
                }
                else{
                    semantic_err("","Uncompatiable element type elementary operation ",&yylloc);
                    return Bv;
                }
            }
            else if(matrixoperations(op))
            {
               
                 if(Av->type==EXP_MAT)
                {
                   
                semantic_err("","Uncompatiable element  type for the matrix operation ",&yylloc);
                return Av;
                    
                }
                else if(Bv->type==EXP_MAT)
                {
                     
                    semantic_err("","Uncompatiable element  type for the matrix operation ",&yylloc);
                    return Bv;
                    
                }
                else{
                   
                    if(compatibletype(convert_type (Av->type),convert_type (Bv->type))==0)
                    {
                        semantic_err(print_compatale_error(convert_type (Av->type),convert_type (Bv->type)),"Uncompatiable element  type for the matrix operation ",&yylloc); 
                    }
                
                    Expression* R=(Expression* )malloc(sizeof(Expression));
                    R->type=Bv->type;
                    R->info=NULL;
                    R->value=Av->value+Bv->value;
                   // printf("found\n");
                    
                    R->ir_name = ir_binary_op(Av->ir_name,op,Bv->ir_name,R->info);
                    /******* NORMAL OPERATIONS CHECKS***/
                    return R;
                }
            }
            else{
               // printf("found2\n");
                
               if(compatibletype(convert_type (Av->type),convert_type (Bv->type))==0)
              {
                    semantic_err(print_compatale_error(convert_type (Av->type),convert_type (Bv->type)),"Uncompatiable element  type for the matrix operation ",&yylloc); 
              }
               return Av;
            }
        }
         //printf("Found %d  %d\n",op,A->rows);
        
        //printf("Found %d \n",op);
        //printf("%d %d \n",A->rows,A->rows); 
        /*matrix operations to be added DONT FORGET ME**/
   
}





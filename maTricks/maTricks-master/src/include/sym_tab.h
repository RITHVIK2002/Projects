/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*                 Symbol table Header             */
/****************************************************/
/*                  Contributions
 -----------------------------------------------------
*   P Srivardhan -- CS19BTECH11052
*   K Rithvik    -- CS19BTECH11038
*/
/*******************************************************    
Last Modified: 02/11/2021
------------ ChangeLog -------*-------
1.Intial creation
/******************************************************/

/*******************************************************
Chain of Tables can be used to get scopes

TABLE----> LIST of Entry's 
Entry --> Contains all the fields and a pointer to the 
          other info particular to that type of entry
          like... MatInfo, FunctionInfo, StructInfo

*********************************************************/

#ifndef SYM_TAB_H
#define SYM_TAB_H

#include <stdio.h>
#include "parser.tab.h"


//Top level entry types
enum TYPE{
    TYPE_MINT,
    TYPE_MFLOAT,
    TYPE_MSTRING,
    TYPE_MSTRUCT,
    TYPE_FUNCTION,
    TYPE_STRUCT_DECL
};

//Primitive types of entries
enum SUB_TYPE{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_STRING,
    TYPE_STRUCT
};

enum SCOPE{
    CURRENT_SCOPE,
    ENCLOSING_SCOPES
};

//Contains list of entries
typedef struct Table{
    int scope;
    struct Entry* entries[100];   //Hash implementation
    struct Table *prev;
}Table ;

//Contain Description for an entry in table
typedef struct Entry{
    enum TYPE entry_type;
    char* name;
    unsigned int scope;
    unsigned int size;
    unsigned int offset;
    YYLTYPE locInfo;
    void* info;

    //If it is SRTUCT_DTYPE
    char* struct_type_name;

    //Next entry
    struct Entry *next;
}Entry;





/***************************************************/
//Contain description of genralised matrix
typedef struct MatInfo{
    unsigned rows,cols;
    void* data;
    enum TYPE entry_type;
    Entry* init_func;
}MatInfo;

//Function argument 
typedef struct ArgInfo{
    char* name;
    int rows;
    int cols;
    enum TYPE entry_type;
    struct ArgInfo* next;
}ArgInfo;

//Contain Description for Function
typedef struct FuncInfo{
    enum TYPE retType;
    unsigned int rows,cols;
    unsigned int _argCount;
    //Symbol table for its params and body
    Table* sym_tab;
    //pointing to the enclosping scope for enabling functions in functions 
    Table* prev_tab; 

    //Arument list for semantic checking
    ArgInfo* arg_list;

    //IR 
    char* ir_name;
}FuncInfo;



//Contain Description for struct
typedef struct StructInfo{
    //Symbol table for its fields
    Table* sym_tab;
    Table* prev_tab;
}StructInfo;

/*******************************************************/

Table* curTable;

Entry* sym_tab_lookup(char* name, int where);
void sym_tab_insert(char* name,int type,YYLTYPE locinfo, void* info);
void exit_curr_scope();
void enter_new_scope();
void sym_tab_init();
Table* new_Table();

#endif
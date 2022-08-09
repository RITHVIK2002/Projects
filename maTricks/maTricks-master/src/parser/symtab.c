/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*           Implementation of symbol table         */
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
------------ Known Bugs----------------
1. Resources are not freed while deletion of resources
/******************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sym_tab.h"



int curScope = 0;   //Global scope

int get_hash(char* id);
Table* new_Table();
Table* delete_Table();


/*************************************************/

void sym_tab_init(){
    curTable = new_Table();
}



void enter_new_scope(){
    curScope++;
    curTable = new_Table();
}

void exit_curr_scope(){
    curScope--;
    curTable = delete_Table();
}


void sym_tab_insert(char* name,int type,YYLTYPE locinfo, void* info)
{
    if(curTable==NULL)
        sym_tab_init();
    int hash_val = get_hash(name);
    //Make an entry
    Entry* new_entry = (Entry*)malloc(sizeof(Entry));
    new_entry->entry_type = type;
    new_entry->info = info;
    new_entry->locInfo =locinfo;
    new_entry->scope = curScope;
    new_entry->next = NULL;
    new_entry->name = name;

    if(type ==  STRUCT_DTYPE)
    {
        new_entry->struct_type_name = "";
    }

    //Insert into sym_tab
    Entry* temp = curTable->entries[hash_val];

    if(temp!=NULL)
    {
        //Find end of list
        while(temp->next)
            temp = temp->next;
        //Insert at end
        temp->next = new_entry;
    }
    else
    {
        curTable->entries[hash_val] = new_entry;
    }
}

Entry* sym_tab_lookup(char* name, int where)
{
    if(!curTable)
        sym_tab_init();
    
    Table* tempTable = curTable;
    //Look in all the enclosing scopes
    while(tempTable)
    {
        int hash_val = get_hash(name);
        Entry* temp = curTable->entries[hash_val];
        while(temp)
        {
            if(strcmp(temp->name,name)==0)
                return temp;
            else
                temp= temp->next;
        }
        tempTable = tempTable->prev;
        if(where==CURRENT_SCOPE)
            break;
    }
    return NULL;
}



/***************************************************/
int get_hash(char* id)
{
    
    int asciiSum = 0;
    for (int i = 0; id[i]!='\0'; i++) {
        asciiSum = asciiSum + id[i];
    }
    return (asciiSum % 100);
}


Table* new_Table(){
    Table* new_tab = (Table*)malloc(sizeof(Table));
    new_tab->prev = curTable;
    new_tab->scope = curScope;
    for(int i=0;i<100;i++){
        new_tab->entries[i] = NULL;
    }
    return new_tab;
}

Table* delete_Table(){
    Table* temp = curTable->prev;
    //Free all resources of the table including all the entries     --> TODO
    // free(curTable);
    return temp;
}
/*********************************************************************/
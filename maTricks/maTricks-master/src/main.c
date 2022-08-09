/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*              CLI behaviour                       */
/****************************************************/

/*******************************************************    
Last Modified: 04/12/2021
------------ ChangeLog ---------------
Added support for compilation stages
/******************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h> 
#include <ir.h>

#include "colors.h"


/*Variables referenced from parser/lexer*/
extern int yyparse();int yylex();
extern FILE *yyin;
int ERROR_CNT=0, WARNING_CNT=0;
int yydebug=0,yydebug_lex=0;

/*Compiler Flags*/
static int LEX_ONLY;
static int PARSE_ONLY;
static int IR_ONLY;
static int KEEP_IR;


int main(int argc, char *argv[]){
    


    enum options {
        LEXER_VERBOSE = 1000,
        PARSER_VERBOSE,
        SHOW_IR
    };

    /*Get opt setting*/
    int c;
    while (1)
    {
        int option_index = 0;
        static struct option long_options[] =
            {
                {"input_file",required_argument,NULL,'i'},
                /*Debug options*/
                {"lexer_verbose", no_argument, NULL, LEXER_VERBOSE},
                {"parser_verbose", no_argument, NULL, PARSER_VERBOSE},
                {"show_ir",no_argument, NULL,SHOW_IR},
                /*Certain phases*/
                {"till_lex",    no_argument,    NULL,'l'},
                {"till_parsing",  no_argument,    NULL,'p'},
                {"till_ir",  no_argument,    NULL,'r'},
                {NULL, 0, NULL, 0}
            };

        /* Still need to provide an option string for the short options */
        c = getopt_long(argc, argv, "-:i:lp", long_options, &option_index);
        if (c == -1)
            break;

        switch (c)
        {
            case LEXER_VERBOSE:
                yydebug_lex = 1;
                break;
            
            case PARSER_VERBOSE:
                yydebug = 1;
                break;

            case SHOW_IR:
                KEEP_IR=1;
                break;
            
            //Input file
            case 1:         //Non optional arguments are treated as file name
            case 'i':
                if(yyin)
                    {printf(YEL"Input file already specified.. ignoring %s\n"RESET,optarg);break;}
                yyin = fopen(optarg,"r");
                if(!yyin)
                    {fprintf(stderr,RED"'%s' file not found\n"RESET,optarg); return 1;}
                break;


            case 'l':
                LEX_ONLY = 1;
                yydebug_lex = 1;
                break;
            
            case 'p':
                PARSE_ONLY = 1;
                
                break;
            
            case 'r':
                IR_ONLY = 1;
                KEEP_IR =1;
                break;
                
            case '?':
                //"Unknown option 
                break;

            case ':':
                //Missing args
                if(optopt=='i')
                    {fprintf(stderr,"No input file specified");return 1;}
                break;

            default:
                break;
        }
    }



    /*Check if the input file is vaild */
    if(!yyin)
        {fprintf(stderr,"No input file specified\n"); return 1;}

    if(LEX_ONLY)
    {
        while (yylex()>0)
            ;
        return 0;
    }
    
    yyparse();    
    if(ERROR_CNT==0 && WARNING_CNT==0)
        printf("Parsed Succesfully\n");
    else
        printf("\nParsing failed due to "RED"%d errors"RESET" and "YEL"%d warnings\n"RESET,ERROR_CNT,WARNING_CNT);
    

    //IR Generation
    
    extern FILE* ir_prog_file;
    extern FILE* ir_func_file;
    fclose(ir_prog_file);
    fclose(ir_func_file);
    if(PARSE_ONLY ||ERROR_CNT!=0)
    {
        //Stop here if there are errors
        system("rm "IR_PROG_FILE" "IR_FUNC_FILE);
        exit(ERROR_CNT==0);
    }
    //system("cat "IR_PROG_FILE">>"IR_FUNC_FILE" && rm "IR_PROG_FILE" && mv "IR_FUNC_FILE" ir_gen.ll");


    //Translating IR
    if(!IR_ONLY)
        system("./build/codegen/gen.out");

    if(KEEP_IR){
        system("echo \"-----IR-----\" && cat "IR_FUNC_FILE" && cat "IR_PROG_FILE" && echo \"----------\"");
    }else{
        //Remove the traces
        system("rm "IR_PROG_FILE" "IR_FUNC_FILE);
    }

    if(IR_ONLY)
        exit(0);

    //Compile run.cpp with libeigen and run
    system("g++ "IR_RUN_FILE" -o a.out");
    if(!KEEP_IR)
        system("rm "IR_RUN_FILE);

    return 0;
}
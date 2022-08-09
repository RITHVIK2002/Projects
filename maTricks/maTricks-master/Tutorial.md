#           MATRICKS LANGUAGE TUTORIAL
Let us begin with a quick introduction in MATRICKS. Our aim is to show the essential elements of the language in real programs, but without getting bogged down in details, rules, and exceptions.
Lets look at the basic features of the language.
>Primitive types
>Expressions
>Control flow
>Functions
>User defined types

lets look at a small code snippets which do not make much meaning but are illustrative to get along with the language and dissect it and see the possible features.

#### Code 1: Operations between variables
```python=
mInt A(1,1) = 2;
mInt B(1,1) = 4;
mInt C(3,3) = (){return ROW+COLUMN+1;} #function of ROW ,COLUMN WHERE  THEY ARE KEYWORDS for intialization
mInt Sum(1,1) = A+B   ;     #Matrix Addition
mInt Subtract(1,1) = A-B  ; #Matrix Subtraction
mInt product (1,1) = A*B ;  #Matrix Multiplication
scalar a=4;
mInt division(1,1) = A//a  ;
#Matrix division by scalar
```
The above code snippet says different types present the language and the various expressions possible in it.

## Primitive types
Type defines the set of values objects may take.
The types supported by our language are:
matrixInt: denoted by keyword mInt 
matrixFloat: denoted by keyword mFloat
matrixString: denoted by keyword mString
Scalar: denoted by  keyword scalar 
   Remark: scalar represents a floating point number.
Index: denoted by keyword index
#### Code2:
```clike=
mInt persons(1,2)=1,0;//wrong  to be [1,0] 
mInt p1(2,2)=[0,1;1,0];//right
mInt p2(1,1)=[1]; //is same as  only for (1,1)
mInt p3(1,1)=1;
```
The above code illustrates the initialisations which are not valid ones.

## Expressions
" * " Is used for multiplication of 2 matrices.
" + " Is used to add two matrices or two scalars.
" // " Is used go divide a scalar with a matrix
" - " is used to subract two matrices or two scalars
" $ " is used to get the sum of all the elements in the required matrix.
" ’ " is used to evaluate the transpose of the matrix.
“#” is used to evalute the inverse of the matrix.

## operators
1.“==” checks if the two operands are equal and returns true if equal else false.
2.similarly “!=”, "<", ">","<=",">=" return true if the corresponding mathematical statement is true.
3.'|','||','&','&&','!','~',' ~~ ', are the elementwise bitwise operators which do  or,or with scalar,and,and with scalar,not, not with scalar respectively



#### Code 3: Student details
```clike=
mStruct Person
{										
	mString Name;
	mString Age;
	mString Gender;
	mString DOB;
};
Person persons(1,2);
/*  btech column     mtech column  (these are the two columns persons are the elements of rows)*/ 
persons[0][0].Name = “Ramu”;	/*btech person*/ 
persons[0][0].Age = “25”;
persons[0][0].Gender = “Male”;
persons[0][0].DOB  = “13/01/1996” ;

persons[0][1].Name = “Laxman”;	/*mtech person*/
persons[0][1].Age = “27”;
persons[0][1].Gender = “Male”;
persons[0][1].DOB  = “23/09/1994”; 

if(person[0][1].Age=="27"){
    person[0][1].Age="twentyseven";
}
else{
    person[0][1].Age="not twenty seven";
}
 
//yet another way to initialise names
for(index i=0;i<1 ;i++)
{
    if(i==0){
        person[0][i].Name="Ramu"
    }
    else if(i==1){
        person[0][i].Name="Laxman"
    }
}

```
The above code snippet illustrates the userdefined data type and how to initialise it and control flow.
### CONTROL FLOW
control flow (or flow of control) is the order in which individual statements, instructions or function calls of an imperative program are executed or evaluated.
#### Choice
 **IF ELSE STATEMENTS** 
    if, else are the keywords and  { is used for opening the statement and } used for closing the statement
  ``` clike=
 if(index){  
    statements
}else if(index){            
    statements
}else{ 
    statements
}
```
Remark:- Boolean is not a primitive data type in our language. So booleans must be encoded in the form of scalars or indices.(> 0) is evaluated as true.
#### Loops
  for is the keyword and  statements are present in the { statements }
  Most programming languages have constructions for repeating a loop a certain number of times. In most cases counting can go downwards instead of upwards and step sizes other than 1 can be used.
  (intialization;condition;function of the variables)
``` clike=
for(index i=0;i<INDEXtype ;i++)
{
	
}
```

### User Defined Types
User-defined functions are functions that you use to organize your code in the body of a policy.mStruct is matrix struct which means it is matrix of struct element.
--clike
mStruct Dtype
{										
	mInt Aa;
};
Dtype Name(2,1);


#### Code4:
```clike=
/*This example illustrates the concept of typeCasting,
 * Inversion, operator Precedence and associativity*/
 
//Solving for system of linear equations Ax=B
Def SolveEquations(mInt A,mInt B)=> mFloat{
    //Inverse operator left associativity
    //Precedence of Inverse is greater than Multiplication    
    //Here before the Inversion of matrix it should be
    //typecasted to Float as the inversion of matrix can 
    //lead to loss of precesion 
    mFloat X= B*(mFloat)A#;    
    return X;
}

f(A,B)
```
The above code snippet illustrates how a function call is invoked and how it is defined.
### FUNCTIONS
Def is the keyword used for defining any function .arguments are present in the function brackets return type is present in the brackets .Return tyrpe is given at the function

```clike=
Def functionName(Args) =>returntype
{
  statements
}

Def f(mInt x,mInt y)=>mInt
{
     statements
     return expression;
}
//function calling:
f(x,y)
```










 
    
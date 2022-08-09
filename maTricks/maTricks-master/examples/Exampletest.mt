
mFloat saA(2,3),B(2,2),C(2,2),A(2,2);
saA=saA+saA;
B=A*C;
B=B+C;


Def SolveEquations(mFloat A(2,2),mFloat B(2,2))=> mFloat(2,2){
return B*(#A);
}

A = SolveEquations(A,B);
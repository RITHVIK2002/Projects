
mFloat saA(2,3),saA(2,3) ;
mInt saA(4,6);
mString saA(4,6)=ONES;

mStruct Hello{
    mInt avgsdf(5,3); 
    mInt saA(4,5);
    mInt saA(4,6);
};

// Using the above struct to declare a var
Hello I(2,5);       //Here it is a struct decalaration
Hello bye(3,8)=ONES;
Hello hj(2.3,5.2);

mFloat mg(4,3),me(3,4),mo(4,4);
mString f(3,4);
mInt a(1,1);
a=a**f[2][2];
me=mg*bye;

me=mo+me*mg +mo;

//me=hj;
//me =[mString]me;
me =#bye;


Def SolveEquations(mInt A(4,6),mString B(2,8))=> mFloat(4,5){
    mFloat X(1,1)=#B;
    
    return X;
}




//Now no error
mString B(2,8);
mInt A(4,6);
SolveEquations(A,B);

//Too many args
SolveEquations(A,B,B);


//Type mismatch
SolveEquations(B,A);

//Size mismatch
mInt A1(4,2);
SolveEquations(A1,B);


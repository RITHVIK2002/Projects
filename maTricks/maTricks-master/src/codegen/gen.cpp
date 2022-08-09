/****************************************************/
/*                  Team-4  (maTricks)              */
/*--------------------------------------------------*/
/*                    From IR to CPP               */
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
MINT ASSIGNING TO MFLOAT GIVING AN ERROR
DECIMAL FROM LEX IS GIVING ZERO IN GEN
NEGETIVE NUMBERS NOT HANDLED
INVERSE IS ONLY POSSIBLE FOR FLOAT
/******************************************************/

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "ir.h"
#include "parser.tab.h"

using namespace std;

ofstream rout(IR_RUN_FILE);

vector<string> get_strings(string myText)
{
    string word = "";
    vector<string> v;
    for (auto x : myText)
    {
        if (x == ' ')
        {
            v.push_back(word);
            word = "";
        }
        else if (x == ',')
        {
            v.push_back(word);
            word = "";
        }
        else
        {
            word = word + x;
        }
    }
    v.push_back(word);
    return v;
}
double str_to_double(string s)
{
    stringstream geek(s);

    // The object has the value 12345 and stream
    // it to the integer x
    double x = 0;
    geek >> x;
    return x;
}
string op(int a)
{
    switch (a)
    {
    case 284:
        return "++";
    case 285:
        return "--";
    case 286:
        return "=";
    case 287:
        return "+=";
    case 288:
        return "-=";
    case 289:
        return "*=";
    case 290:
        return "**=";
    case 291:
        return "/=";
    case 43:
        return "+";
    case 45:
        return "-";
    case 305:
        return "*";
    case 306:
        return "**";
    case 307:
        return "/";
    case 308:
        return ".";
    case 309:
        return "$";
    case 310:
        return "#";
    case 311:
        return "'";
    }
    return "";
}
void call_libraries()
{
    rout << "#include<bits/stdc++.h>" << endl;
    rout << "using namespace std;" << endl;
    rout << "#include<eigen3/Eigen/Dense>" << endl;
}
void start_main()
{
    rout << "int main(){" << endl;
}
void end_main()
{
    rout << "cout<<\"Executed\"<<endl;";
    rout << "}" << endl;
}
void function_start()
{
}
void function_end()
{
}
int main()
{
    string myText;
    ifstream irfile(IR_PROG_FILE);

    call_libraries();
    start_main();
    while (getline(irfile, myText))
    {
        // Output the text from the file
        vector<string> v = get_strings(myText);
        if (v[0] == "assign")
        {
            if (v.size() > 7)
            {
                if (op(stoi(v[5])) == "#")
                {
                    rout << v[1] << "=" << v[2] << ".inverse();" << endl;
                }
                else if (op(stoi(v[5])) == "'")
                {
                    rout << v[1] << "=" << v[2] << ".transpose();" << endl;
                }
                else if (op(stoi(v[5])) == "$")
                {
                    rout << v[1] << "=" << v[2] << ".sum();" << endl;
                }
                else if (op(stoi(v[5])) == "**")
                {
                    rout << v[1] << "=" << v[2] << " "
                         << "*"
                         << " " << v[6] << ";" << endl;
                }
                else
                {
                    rout << v[1] << "=" << v[2] << " " << op(stoi(v[5])) << " " << v[6] << ";" << endl;
                    ;
                }
            }
            else
            {
                rout << v[1] << "=" << v[4] << ";" << endl;
            }
        }
        else if (v[0] == "define")
        {
            if (str_to_double(v[1]) == -1)
            {
                rout << "double " << v[2] << ";" << endl;
            }
            else if (str_to_double(v[1]) == 0)
            {
                rout << "Eigen::MatrixXi " << v[2] << "(" << v[3] << "," << v[4] << ");" << endl;
            }
            else if (str_to_double(v[1]) == 1)
            {
                rout << "Eigen::MatrixXd " << v[2] << "(" << v[3] << "," << v[4] << ");" << endl;
            }
        }
        else if (v[0] == "val-assign")
        {
            if (v.size() > 7)
            {

                if (str_to_double(v[3]) != -1)
                {

                    if (str_to_double(v[7]) != -1)
                    {
                        rout << v[1] << "=" << v[2] << '(' << v[3] << "," << v[4] << ')' << " " << op(stoi(v[5])) << " " << v[6] << '(' << v[7] << "," << v[8] << ')' << ";" << endl;
                    }
                    else
                    {
                        rout << v[1] << "=" << v[2] << '(' << v[3] << "," << v[4] << ')' << " " << op(stoi(v[5])) << " " << v[6] << ";" << endl;
                    }
                }
                else
                {
                    if (str_to_double(v[7]) != -1)
                    {
                        rout << v[1] << "=" << v[2] << " " << op(stoi(v[5])) << " " << v[6] << '(' << v[7] << "," << v[8] << ')' << ";" << endl;
                    }
                    else
                    {

                        rout << v[1] << "=" << v[2] << " " << op(stoi(v[5])) << " " << v[6] << ";" << endl;
                    }
                }
            }
            else
            {
                rout << v[1] << '(' << v[2] << "," << v[3] << ")=" << v[4] << ";" << endl;
            }
        }
    }
    end_main();
}
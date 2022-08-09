#include<fstream>
#include<iostream>
#include<vector>
using namespace std;


int main()
{
    FILE *file=fopen("source.txt","r");
    string title(100,"\n"),author(100,"\n"),venue(100,"\n"),abstract(100,"\n");
    int year,id;
    vector<int> ids;
    while(!EOF)
    {
        fscanf(file,"#*%s\n",&title[0]);
        fscanf(file,"#@%s",&author[0]);
        fscanf(file,"#t %d",&year[0]);
        fscanf(file,"#c%s",&venue[0]);
        fscanf(file,"#index %d",id);
        fscanf(file,"#%");
        int i;
        while(fscanf(file,"%d ",i))
        {
            ids.push_back(i);
        }
        fscanf(file,"#! %s",&abstract[0]);
        
    }

}
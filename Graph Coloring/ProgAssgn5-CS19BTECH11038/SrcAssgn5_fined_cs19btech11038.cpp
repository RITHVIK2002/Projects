#include<iostream>
#include<semaphore.h>
#include<thread>
#include<fstream>
using namespace std;
int **graph;
int NODE;
class Node {
public:
    int data;
    int inout;
    Node* next;
};

class Partionlist{
public:
    Partionlist() { // constructor
        head = NULL;
    }
    ~Partionlist() {}; // destructor
    void addNode(int val);
    void assigninout();
    void display();
    bool find(int i);
    Node* head;
};

// function to add node to a list
void Partionlist::addNode(int val) {
    Node* newnode = new Node();
    newnode->data = val;
    newnode->inout=-1;
    newnode->next = NULL;
    if (head == NULL) {
        head = newnode;
    }
    else {
        Node* temp = head; // head is not NULL
        if(head->data>=val)
        {
        	newnode->next=head;
        	head=newnode;
        }
        else
       { while (temp->next != NULL&&temp->next->data<val) { 
                    temp = temp->next; // go to end of list
                }
                newnode->next=temp->next;
                temp->next = newnode;} // linking to newnode
    }
}

void Partionlist::assigninout()
{
	 if (head == NULL) {
        cout << "List is empty!" << endl;
    }
    else {
        Node* temp = head;
        while (temp != NULL) {
            //cout << temp->data << " ";
            for(int i=0;i<NODE;i++)
            {
            	if(graph[temp->data][i]&&(!find(i)))
            	{
            		temp->inout=1;
            	}
            }
            if(temp->inout==-1)
            {
            	temp->inout=0;
            }
            temp = temp->next;
        }
        //cout << endl;
    }
}
bool Partionlist::find(int i)
{
	 Node* temp = head;
        while (temp != NULL) {
           // cout << temp->data << " ";
            if(temp->data==i)
            {
            	return true;
            }
            temp = temp->next;
        }
        return false;
        //cout << endl;
}

void Partionlist::display() {
    if (head == NULL) {
        cout << "List is empty!" << endl;
    }
    else {
        Node* temp = head;
        while (temp != NULL) {
            cout << temp->data<<","<<temp->inout << "   ";
            temp = temp->next;
        }
        cout << endl;
    }
}
Partionlist *p;
int npartions;
int *color;
 bool *colorUsed;
 void vertexcolour(int u)
 {
 		int col;
      for(int v = 0; v<NODE; v++) {
         if(graph[u][v]){
            if(color[v] != -1)    //when one color is assigned, make it unavailable
              { colorUsed[color[v]] = true;}
         }
     }

     
     for(col = 0; col<NODE; col++)
      {  if(!colorUsed[col])    //find a color which is not assigned
                { break;}}
         
     color[u] = col;    //assign found color in the list
     int v;    
     for( v = 0; v<NODE; v++) {    //for next iteration make color availability to false
        if(graph[u][v]) {
           if(color[v] != -1)
              {colorUsed[color[v]] = false;}
        }
     } 
 }
 sem_t *mutex;
void graphColoringofpartion(int i) {
	
   if (p[i].head == NULL) {
        cout << "List is empty!" << endl;
    	}
   		 Node* temp = p[i].head;
        while (temp != NULL) {
          
            if(temp->inout==0)									//internal vertice
            {	
            	vertexcolour(temp->data);

            }
            temp = temp->next;
        }
       
          temp = p[i].head;
        while (temp != NULL) {
          
            if(temp->inout==1)
            {
            	for(int i=0;i<NODE;i++)															//externalvertice
            	{																					
            		if(graph[temp->data][i]||i==temp->data)										//lock all external com colouring vertice in increasing order
            		{
            			sem_wait(&mutex[i]);
            		}
            	}
            	
            	vertexcolour(temp->data);														//vertex colouring
            	for(int i=0;i<NODE;i++)
            	{
            		if(graph[temp->data][i]||i==temp->data)										//releasing lock
            		{
            			sem_post(&mutex[i]);
            		}
            	}
            }
            temp = temp->next;
        }
  
}

int main()
{

	 //NODE=500;
	ifstream fin("input_params.txt");
	 fin>>npartions;
	 fin>>NODE;
	 mutex= new sem_t[NODE];
	 for(int i=0;i<NODE;i++)
	{sem_init(&mutex[i],0,1);}
 	color=new int[NODE];
 	colorUsed=new bool[NODE];
	graph = new int *[NODE];
	for(int i = 0; i <NODE; i++)
  		 graph[i] = new int[NODE];

		
 	int o;
    for(int i=0;i<NODE;i++)
    {
      
      fin>>o;
    }
    //cout<<endl;
 	 for(int i=0;i<NODE;i++)
	{
    	 fin>>o;
		for(int j=0;j<NODE;j++)
		{
			fin>>graph[i][j];
		}
	} 
	fin.close();
 	
	auto start = std::chrono::high_resolution_clock::now();
	 

	 thread *t[npartions];
	 p=new Partionlist[npartions];
	for(int i=0;i<NODE;i++)					//partioning
	{
		p[i%npartions].addNode(i);
		//p[i%4].addNode(i+4);
	}
	
	for(int i=0;i<npartions;i++)
	{
		p[i].assigninout();
	}
	
	 color[0] = 0;    //Assign first color for the first node
      //Used to check whether color is used or not

   for(int i = 1; i<NODE; i++)
      color[i] = -1;    //initialize all other vertices are unassigned

   for(int i = 0; i<NODE; i++)
      colorUsed[i] = false;    //initially any colors are not chosen
    
	//cout<<"started"<<npartions<<endl;
   for(int i=0;i<npartions;i++)
   {
   	//cout<<"started"<<endl;
   //	t.push_back(thread(graphColoringofpartion(i)));
   	t[i]=new thread(graphColoringofpartion,i);
   	 
        // cout << endl;
   }
 
   ofstream fout("output_fined_Params.txt");
   ofstream fout2("output_fined_results.txt");
   for(int i=0;i<npartions;i++)
   {
   		t[i]->join();
   }
   auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long microseconds = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
    int ma=-1;
 	 for(int u = 0; u<NODE; u++)
    { 
    	fout <<"V" << u << " - " <<color[u] <<endl;
        ma=max(color[u],ma);
    }
  	fout2<<"Time taken by algorithm in miliseconds:"<<microseconds/1000<<"miliseconds"<<endl;
 	fout2<<"number of coloures used"<<ma+1<<endl;


}
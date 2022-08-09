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
           //cout << temp->data << " ";
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

      for(int v = 0; v<NODE; v++) {
         if(graph[u][v]){
            if(color[v] != -1)    //when one color is assigned, make it unavailable
               colorUsed[color[v]] = true;
         }
     }

     int col;
     for(col = 0; col<NODE; col++)
        if(!colorUsed[col])    //find a color which is not assigned
           break;
         
     color[u] = col;    //assign found color in the list
         
     for(int v = 0; v<NODE; v++) {    //for next iteration make color availability to false
        if(graph[u][v]) {
           if(color[v] != -1)
              colorUsed[color[v]] = false;
        }
     } 
 }
 sem_t mutex;
void graphColoringofpartion(int i) {
	//cout<<"thread"<<i<<"entered"<<endl;
   if (p[i].head == NULL) {
        cout << "List is empty!" << endl;
    	}
   		 Node* temp = p[i].head;
        while (temp != NULL) {                    //internal vertices
           
            if(temp->inout==0)
            {
            	
            	vertexcolour(temp->data);

            }
            temp = temp->next;
        }
       
          temp = p[i].head;
        while (temp != NULL) {                    //external vertices
            if(temp->inout==1)
            {
            	
            	sem_wait(&mutex);
              vertexcolour(temp->data);
            	sem_post(&mutex);
            }
            temp = temp->next;
        }
  
}

int main()
{

	
    ifstream fin("input_params.txt");
   fin>>npartions;
   fin>>NODE;
	sem_init(&mutex,0,1);
 	color=new int[NODE];
 	colorUsed=new bool[NODE];
	graph = new int *[NODE];
	for(int i = 0; i <NODE; i++)
  		 graph[i] = new int[NODE];

		 
    //cout<<" ";
    int o;
    for(int i=0;i<NODE;i++)
    {
      
      fin>>o;
    }
    
 	 for(int i=0;i<NODE;i++)
	 {
      fin>>o;
			for(int j=0;j<NODE;j++)
			{
				fin>>graph[i][j];
			}
			//cout<<endl;
		}
	auto start = std::chrono::high_resolution_clock::now();
	
	 thread *t[npartions];
	 p=new Partionlist[npartions];
	for(int i=0;i<NODE;i++)
	{
		p[i%npartions].addNode(i);
		
	}

	for(int i=0;i<npartions;i++)             //partioning and assigning external and internal
	{
		p[i].assigninout();
	}
	
	 color[0] = 0;    //Assign first color for the first node
      //Used to check whether color is used or not

   for(int i = 1; i<NODE; i++)
      color[i] = -1;    //initialize all other vertices are unassigned

   for(int i = 0; i<NODE; i++)
      colorUsed[i] = false;    //initially any colors are not chosen
    
	
   for(int i=0;i<npartions;i++)
   {
   	
   	t[i]=new thread(graphColoringofpartion,i);
   	 
        
   }
   
   ofstream fout("output_coasre_Params.txt");
   ofstream fout2("output_coasre_results.txt");
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
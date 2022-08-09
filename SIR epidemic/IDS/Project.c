#include <stdio.h> 
#include <stdlib.h> 
#include<time.h>
// A structure to represent an adjacency list node 
struct AdjListNode 
{ 
    struct person *personnode; 
    struct AdjListNode* next; 
}; 
  
// A structure to represent an adjacency list 
struct AdjList 
{ 
    struct AdjListNode *head;  
}; 
struct person
{
    int person_id;
    char status;
    int pred_inf_time;
    int rec_time;
    int rows;						//rows in adjlist or no. of neighbours to find max limit of rows
};
struct personlists
{
  struct person* person;
  struct personlists* next;  
};
struct event
{
    struct person * node;
    int time ;
    char action;
    struct event* next;
};
  
// A structure to represent a graph. A graph 
// is an array of adjacency lists. 
// Size of array will be V (number of vertices  
// in graph) 
struct Graph 
{ 
    int V; 
    struct AdjList* array; 
}; 
struct event* newEvent(struct person * node, int time ,char action) ;
void insertpriorityqueue(struct event** head,struct person * node, int time ,char action) ;
void removepqueue(struct event** head,int id) ;
void process_tras_SIR(struct Graph* graph,struct person* node,int time,struct personlists** suspectableslist,struct personlists** infectableslist,struct personlists** recoverylist,struct event**eventhead,int tmax);
void removeperson(struct personlists** head,struct person*person);
void insertinpersonlist(struct personlists** head,struct person*person);
void process_rec_SIR(struct event**eventhead,struct person * node,int time,struct personlists**suspectableslist,struct personlists**infectableslist,struct personlists**recoverylist);
// A utility function to create a new adjacency list node 
struct AdjListNode* newAdjListNode(struct person* personnode) 
{ 
    
    struct AdjListNode* newNode = (struct AdjListNode*) malloc(sizeof(struct AdjListNode)); 
    newNode->personnode = personnode; 
    newNode->next = NULL; 
    return newNode; 
} 
  
// A utility function that creates a graph of V vertices 
struct Graph* createGraph(int V) 
{ 
    struct Graph* graph =  (struct Graph*) malloc(sizeof(struct Graph)); 
    graph->V = V; 
  
    // Create an array of adjacency lists.  Size of  
    // array will be V 
    graph->array =  
      (struct AdjList*) malloc(V * sizeof(struct AdjList)); 
  
    // Initialize each adjacency list as empty by  
    // making head as NULL 
    int i; 
    for (i = 0; i < V; ++i) 
        graph->array[i].head = NULL; 
  
    return graph; 
} 
  

// Adds an edge to an undirected graph 
void addEdge(struct Graph* graph, struct person* person1, struct person* person2) 
{ 
    // Add an edge from src to dest.  A new node is  
    // added to the adjacency list of src.  The node 
    // is added at the begining 
    struct AdjListNode* newNode = newAdjListNode(person1); 
    newNode->next = graph->array[person2->person_id].head; 
    graph->array[person2->person_id].head = newNode;
    person2->rows++; 
  
    // Since graph is undirected, add an edge from 
    // dest to src also 
    newNode = newAdjListNode(person2); 
    newNode->next = graph->array[person1->person_id].head; 
    graph->array[person1->person_id].head = newNode;
    person1->rows++; 
} 
  
// A utility function to print the adjacency list  
// representation of graph 
void printGraph(struct Graph* graph) 
{ 
    int v; 
    for (v = 0; v < graph->V; ++v) 
    { 
        struct AdjListNode* pCrawl = graph->array[v].head; 
        printf("\n Adjacency list of vertex %d\n head ", v); 
        while (pCrawl) 
        { 
            printf(" %d-> ", pCrawl->personnode->person_id);
             
            pCrawl = pCrawl->next; 
        } 
        printf("\n"); 
    } 
}
// A utility function to create a new personnode
struct person* createperson(int personid,char status, int pred_inf_time,   int rec_time,int rows) 
{
     
  struct person* temp=malloc(sizeof(struct person));
    temp->person_id=personid;
  temp->status=status;
  temp->pred_inf_time=pred_inf_time;
  temp->rec_time=rec_time;
  temp->rows=rows;	
  
  return temp;  
}
  
// Driver program to test above functions 
int main() 
{ 
    // create the graph given in above fugure 
    int V = 10000,R=3000,TMAX=301	; 

    struct Graph* graph = createGraph(V); 
    struct person* temp[V];
    struct personlists* suspectableslist=NULL;
    //creating V number of personnodes with infinity(10000)as prefinftime and infinity(10000) as rectime 
    for(int j=0;j<V;j++)
    {   
        temp[j]=createperson(j,'s',10000,10000,0);
        insertinpersonlist(&suspectableslist,temp[j]);
        
    }
    int r2[R];
    //creating randedges in graphs
    for(int j=0;j<V;j++)
    {   
        
    for(int i=0;i<R;i++)
    {

        int r=rand()%2;
        r2[i]=-1;
        if(r==1)
        {
             r2[i]=rand()%(V-j)+j;
             int prod=0;
            for(int k=0;k<i;k++)//prod is used to find :Is edge already present or not?
            {
                if(r2[k]==r2[i])
                {
                    prod++;
                }
            }            
            if(prod==0&&r2[i]!=j&&temp[j]->rows<(R-1)&&temp[r2[i]]->rows<(R-1)){
            
            addEdge(graph,temp[j],temp[r2[i]]);}

        }

    }
    
    
    }
    printGraph(graph);
    //assigning random personnodes having intial infectacts
    struct event *eventhead=NULL;
    for(int i=0;i<V;i++)
    {

        int ra=rand()%2;
        if(ra==1)
        {
             temp[i]->pred_inf_time=0;
            insertpriorityqueue(&eventhead,temp[i],0,'t');
           


        }
    }
   
   
    
    
    
    
    
    struct personlists* infectableslist=NULL;
    struct personlists* recoverylist=NULL; 

    /*Algorithm given ::
    while Q is not empty do do
Event ← earliest remaining event in Q
if Event.action is transmit then
if Event.node.status is susceptible then
process trans SIR(G, Event.node, τ , γ) [See Below]
else {Event is Recovery}
process rec SIR(Event.node, Event.time) [See Below]
end if
end if
end while*/
    while(eventhead!=NULL&&eventhead->time<TMAX)
    {
                
        struct event* temphead=eventhead;
        if(temphead->action=='t')
        {
            if(temphead->node->status=='s')
            {
                 
                struct event*temphead2=eventhead;
                 
                
                process_tras_SIR(graph,temphead->node,temphead2->time,&suspectableslist,&infectableslist,&recoverylist,&eventhead,TMAX);
                removepqueue(&eventhead,eventhead->node->person_id);
            }
            


        }
        else
           
        { 
            
             struct event* temphead45=eventhead;
             
            process_rec_SIR(&eventhead,temphead45->node,temphead45->time,&suspectableslist,&infectableslist,&recoverylist);
            
        }
        
    }
    struct personlists* templist=suspectableslist;

    while(templist!=NULL)
    {
        printf("suspectableslist\n");
        printf("[id:%d,st:%c]\n",templist->person->person_id,templist->person->status);
        templist=templist->next;
    }
    struct personlists* templist32=infectableslist;
    printf("\n");
    while(templist32!=NULL)
    {
        printf("infectableslist\n");
        printf("[id:%d,inf:%c]",templist32->person->person_id,templist32->person->status);
        templist32=templist32->next;
    }

    struct personlists* templist3=recoverylist;
	printf("\n");
    while(templist3!=NULL)
    {
        printf("recoverylist\n");
        printf("[id:%d,rec:%crectime:%d]",templist3->person->person_id,templist3->person->status,templist3->person->rec_time);
        templist3=templist3->next;
    }
    printf("\n");
    for(int i=0;i<V;i++){
        printf("%d:%c",i,temp[i]->status);
    }


    

    
  
    
    
  
    return 0; 
} 
//function of trasmision of SIR
void process_tras_SIR(struct Graph* graph,struct person* node,int time,struct personlists** suspectableslist,struct personlists** infectableslist,struct personlists** recoverylist,struct event**eventhead,int tmax)
{

int rtime=node->rec_time;
int r=node->status;


(*eventhead)->node->status='i';

node->rec_time=time;
while(rand()%5!=4)// recrate is 0.2
{


    node->rec_time++;
}
node->rec_time++;		// i th trail so i days
removeperson(suspectableslist,node);
node->status= 'i';
insertinpersonlist(infectableslist,node);
if(node->rec_time<tmax){

    insertpriorityqueue(eventhead,node,node->rec_time,'r');
}

int base=0;
  


struct AdjListNode*temp=graph->array[node->person_id].head;//neighbours of nodeid 
while(temp!=NULL)
{
	
    struct person* target=temp->personnode;
    if(temp->personnode->status=='s')
    {

        int inf_time=time;
        while(rand()%2!=1)//transrate is 0.5
        {
            inf_time++;
        }
        inf_time++;
        
        if(inf_time<node->rec_time&&inf_time<target->pred_inf_time&&inf_time<tmax)//inf time < minimum(source.rec time, target.pred inf time, tmax)
        {
            temp->personnode->pred_inf_time=inf_time;
             removepqueue(eventhead,temp->personnode->person_id);//removing old ones if exists and inf_time<target->pred_inf_time
             
            insertpriorityqueue(eventhead,temp->personnode,inf_time,'t');
              

        }
     }
 
      
    temp=temp->next;
      

}
  


  
}
//function of recovery of SIR
void process_rec_SIR(struct event**eventhead,struct person * node,int time,struct personlists**suspectableslist,struct personlists**infectableslist,struct personlists**recoverylist)
{
	(*eventhead)->node->status='r';
    removeperson(infectableslist,node);
    node->status='r';
    
    insertinpersonlist(recoverylist,node);

            
    removepqueue(eventhead,(*eventhead)->node->person_id);
            

}











//removing person for personlist
//linkedlist removing element 
/*

				  ->newnode-
.......-nodeprev-|          |->nodenext............
*/
void removeperson(struct personlists** head,struct person*person) 
{ 
    
    
    struct personlists* temp = (*head);
    struct personlists* tempprev =NULL;
    if((*head)!=NULL){
    if(temp->person==person)
    {
        if((*head)->next!=NULL)
        {
            (*head)=(*head)->next;
        }else
        {
            (*head)=NULL;
        }
    }else{
        while(temp->person!=person&&temp!=NULL)
        {
            tempprev=temp;
            temp=temp->next;
            if(temp==NULL)
            {
                break;
            }
        }
        if(temp!=NULL)
        {
            if(temp->next!=NULL){
            tempprev->next=temp->next;}
            else
            {
              tempprev->next=NULL;  
            }
        }
        }
    free(temp);
    }

} 

//inserting person node as head
void insertinpersonlist(struct personlists** head,struct person*person)
{
   
    
    
    struct personlists* temp =malloc(sizeof(struct personlists)) ;

    temp->person=person;
    if((*head)==NULL)
    {
        temp->next=NULL;
        *head=temp;
    }else 
    {
    temp->next = (*head); 
        (*head) = temp;}
}
//removing queue similar to rremoving elemnt in ll
void removepqueue(struct event** head,int id) 
{ 
    
    struct event* temp = (*head);
    struct event* tempprev =NULL;
    if((*head)!=NULL){
    if(temp->node->person_id==id)
    {
        if((*head)->next!=NULL)
        {
            (*head)=(*head)->next;
        }else
        {
            (*head)=NULL;
        }
    }else{
        while(temp->node->person_id!=id&&temp!=NULL)
        {
            tempprev=temp;
            temp=temp->next;
            if(temp==NULL)
            {
                break;
            }
        }
        if(temp!=NULL)
        {
            if(temp->next!=NULL){
            tempprev->next=temp->next;}
            else
            {
              tempprev->next=NULL;  
            }
        }
        }
    free(temp);
    }
} 

 void insertpriorityqueue(struct event** head,struct person * node, int time ,char action) 
{ 

    struct event* start = (*head); 
 
    // Create new Node 
    struct event* temp = newEvent(node,time,action ); 
  if(((*head)==NULL))
  {
    (*head)=temp;
  }else{
    // Special Case: The head of list has lesser 
    // priority than new node. So insert new 
    // node before head node and change head node. 
    if ((*head)->time >time) { 
  
        // Insert New Node before head 
        temp->next = *head; 
        (*head) = temp; 
    } 
    else { 
  
        // Traverse the list and find a 
        // position to insert new node 
        while (start->next != NULL && 
               start->next->time < time) { 
            start = start->next; 
        } 
  
        // Either at the ends of the list 
        // or at required position 
        temp->next = start->next; 
        start->next = temp; 
    }
    } 
}
// A utility function to create a new Event
struct event* newEvent(struct person * node, int time ,char action) 
{ 
    struct event* temp = (struct event*)malloc(sizeof(struct event)); 
    temp->node = node; 
    temp->time = time;
    temp->action=action; 
    temp->next = NULL; 
  
    return temp; 
} 







#include <iostream>
#include <fstream>
using namespace std;

class AStarNode{
public:
    int ID,numEdgesFromStart,gStar,hStar,fStar;
    AStarNode* next;
    AStarNode* parent;
    
    AStarNode(int n){
        ID = n;
        numEdgesFromStart = 0;
        parent = NULL;
        next = NULL;
        gStar = hStar = fStar = 0;
        
    }
};

class AStarSearch{
public:
    int numNodes,start,HFunction,sumEdges,numEdges,minEdge = 99999,medianEdge;
    int** costMatrix;
    AStarNode* OpenList;
    AStarNode* CloseList;
    int* childAry;
    
    
    AStarSearch(int n){
        numNodes = n;
        sumEdges = numEdges = 0;
        OpenList = new AStarNode(-1);
        CloseList = new AStarNode(-1);
        cout<<"starting node between 1-"<<numNodes<<": ";
        cin>>start;
        while(start<1 || start>numNodes){
            cout<<"starting node between 1-"<<numNodes<<": ";
            cin>>start;
        }
        cout<<"choose HFunction. 1,2, or 3: ";
        cin>>HFunction;
        while(HFunction<1 || HFunction >3){
            cout<<"choose HFunction. 1,2, or 3: ";
            cin>>HFunction;
        }
        
        childAry = new int[numNodes]();
        costMatrix = new int*[numNodes];
        for (int i = 0; i<numNodes; i++) {
            costMatrix[i] = new int[numNodes]();
        }
        for(int i=0; i<numNodes; i++){
            for(int j=0; j<numNodes; j++){
                if(i!=j)
                    costMatrix[i][j] = -1;
            }
        }
    }
    void loadMatrix(ifstream& in, ofstream& out, ofstream& debug){
        int ni, nj, cost;
        out<<"Below is the input graph for this program.\n";
        out<<numNodes<<endl;
        while(in>>ni && in>>nj && in>>cost){
            out<<ni<<"  "<<nj<<"  "<<cost<<endl;
            costMatrix[ni-1][nj-1] = cost;
            costMatrix[nj-1][ni-1] = cost;
            sumEdges += cost;
            numEdges++;
            if(cost<minEdge)
                minEdge = cost;
        }
        medianEdge = median(costMatrix);
    }
    void run(ofstream& out, ofstream& debug){

        AStarNode* sNode = new AStarNode(start);
        openInsert(sNode);
        printOpen(debug);
        
        AStarNode* childNode;
        AStarNode* currentNode;
        
        out<<"\nStarting Node is: "<<start<<endl;
        out<<"HFunction used is: "<<HFunction<<endl;
        
        currentNode = openRemove();
        
        while(checkPath(currentNode) == false || currentNode->ID != start){
            copyChildList(currentNode->ID-1);

            int childIndex = 0;
            while(childIndex<numNodes){
                if(childAry[childIndex] > 0 && !isInCloseList(currentNode,childIndex+1)){
                    childNode = new AStarNode(childIndex+1);
                    childNode->parent = currentNode;
                    childNode->numEdgesFromStart= currentNode->numEdgesFromStart+1;
                    computeGstar(childNode);
                    computeHstar(childNode);
                    computeFstar(childNode);
                    openInsert(childNode);
                }
                else{
                    removeFromClose();
                }
                childIndex++;
            }
            printOpen(debug);
            pushToClose(currentNode);
            printClose(debug);
            currentNode = openRemove();
        }
        tracePath(currentNode, out);
    }
    
    void tracePath(AStarNode* currentNode, ofstream& out){
        AStarNode* cur = currentNode;
        int path[numNodes+1];
        for (int i = 0; i<numNodes+1; i++) {
            path[i]=0;
        }
        for (int i = numNodes+1; i>0; i--) {
            path[i] = cur->ID;
            cur = cur->parent;
        }
        out<<endl;
        out<<"** The search result of the path: ";
        out<<"Starts from node"<< start <<"using A* search (assuming the start node given is "<< start<<")\n\n";
        for (int i = 1; i<=numNodes; i++) {
            out<<path[i]<<" "<<path[i+1]<<" ";
            out<<costMatrix[(path[i])-1][(path[i+1])-1]<<endl;
        }
        out<<"\nThe total cost of the simple-path is "<<currentNode->gStar<<endl;
    }
    
    bool checkPath(AStarNode* node){
        int path[numNodes];
        for (int i = 0; i<numNodes; i++) {
            path[i]=0;
        }
        AStarNode* spot = node;
        while(spot->parent != NULL){
            spot = spot->parent;
            path[spot->ID-1]=1;
        }
        for (int i = 0; i < numNodes; i++) {
            if(path[i] != 1)
                return false;
        }
        return true;
    }
    void printClose(ofstream& out){
        AStarNode *cur = CloseList->next;
        out<<"\n*** CLOSE list ***\n";
        while(cur != NULL  && cur->ID != -1){
            out<<cur->ID<<"  ";
            cur = cur->next;
        }
        out<<endl;
    }
    void pushToClose(AStarNode* node){
        if(CloseList == NULL){
            CloseList = node;
        }else{
            node->next = CloseList;
            CloseList = node;
        }
    }

    void removeFromClose(){
        AStarNode* temp = NULL;
        if(CloseList != NULL){
            temp = CloseList;
            CloseList = CloseList->next;
        }
    }
    bool isInCloseList(AStarNode* current,int childIndex){
        AStarNode* spot = current->parent;
        while(spot != NULL){
            if(childIndex == spot->ID && start != childIndex){
                return true;
            }else
                spot = spot->parent;
        }
        return false;
    }
    void computeHstar(AStarNode* n){
        int nodesRemaining = numNodes - n->numEdgesFromStart;
        switch (HFunction) {
            case 1:
                n->hStar = sumEdges/numEdges* nodesRemaining;
                break;
            case 2:
                n->hStar = minEdge * nodesRemaining;
                break;
            case 3:
                n->hStar = medianEdge * nodesRemaining;
                break;
            default:
                cout<<"Reached Default case in computeHStar\n";
                break;
        }
    }
    void computeGstar(AStarNode* n){
        if(n->parent == NULL){
            n->gStar =  costMatrix[start][n->ID-1];
        }else{
            n->gStar = n->parent->gStar + costMatrix[n->ID-1][n->parent->ID-1];
        }
    }
    void computeFstar(AStarNode* n){
        n->fStar = n->gStar + n->hStar;
    }
    void openInsert(AStarNode* n){
        AStarNode* spot = OpenList;
        if(OpenList->next == NULL){
            OpenList->next = n;
        }
        else{
            while(spot->next != NULL && spot->next->fStar < n->fStar ){
                spot = spot->next;
            }
            AStarNode* temp = spot->next;
            spot->next = n;
            n->next = temp;
        }
    }
    AStarNode* openRemove(){
        if(OpenList->next != NULL){
            AStarNode* front = OpenList->next;
            OpenList->next = front->next;
            return front;
        }
        return NULL;
    }

    void printOpen(ofstream& out){
        AStarNode *cur = OpenList->next;
        out<<"\n*** OPEN list ***\n";
        while(cur != NULL && cur->ID != -1){
            out<<cur->ID<<"  ";
            cur = cur->next;
        }
        out<<endl;  
    }
    void copyChildList(int matrixIndex){
        for (int i=0; i<numNodes; i++) {
            childAry[i] = costMatrix[matrixIndex][i];
        }
    }
    double median(int** costMatrix){
        if(numEdges % 2 != 0)
            return costMatrix[numNodes/2][numNodes/2];
        else
            return (costMatrix[(numNodes-2)/2][numNodes-1] + costMatrix[numNodes/2][0])/2;
    }
};



int main(int argc, const char * argv[]) {
    
    ifstream in;
    ofstream out,debug;
    in.open(argv[1]);
    out.open(argv[2]);
    debug.open(argv[3]);
    
    int numNodes;
    in>>numNodes;
    
    AStarSearch* search = new AStarSearch(numNodes);
    search->loadMatrix(in, out, debug);
    search->run(out, debug);
    
    return 0;
}

#include<vector>
#include <stxxl/vector>
#include <stxxl/stack>
#include <stxxl/sorter>
#include <stxxl/stats>
#include <stxxl/timer>
#include <limits>
#include <iostream>
#include<fstream>
#include<string>
#pragma comment(linker, "/STACK:200000000")
#pragma comment(linker, "/HEAP:200000000")
using namespace std;
/*
Author: Akif Ahmed Khan
BufferedRangeTree for 2 dimensions
Conventions used:
M: Buffer Size
N: Input size
B: Block size = 32KB used 
n=N/B
m=M/B
*/

/*
For testing purpose:
Assumptions:
M=12;
N=18;
B=3;
m=4
n=6

*/


struct my_comparator
{
    bool operator () (const float& a, const float& b) const
    {
        return a < b;
    }
    float min_value() const
    {
        return std::numeric_limits<float>::min();
    }
    float max_value() const
    {
        return std::numeric_limits<float>::max();
    }
};
long long int BufferSize=4000000; // It would be (m*B)=M
typedef stxxl::VECTOR_GENERATOR<float,1,1,786432>::result vector;
struct Data
{
	float Lat, Lon;
    Data()
    { }
	Data(float _Lat, float _Lon)
        : Lat(_Lat), Lon(_Lon)
    { }
};
struct DataComparator
{
    bool operator () (const Data& a, const Data& b) const
    {
        return a.Lat < b.Lat;
    }
    Data min_value() const
    {
		return Data(std::numeric_limits<float>::min(), std::numeric_limits<float>::min());
    }
    Data max_value() const
    {
		return Data(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
    }
};
struct Buffer
	{
		//<DataType,Number oF Blocks per Page, Number of Pages per Cache,Block Size,Cache Replacement Strategy(Optional)>
		typedef stxxl::VECTOR_GENERATOR<Data,1,1,786432>::result vector;
		vector content; // Size will be m blocks with each block of size 'B'
	};

struct Leaf
	{
		typedef stxxl::VECTOR_GENERATOR<Data,1,1,786432>::result vector;
		vector LeafArr;  //Size of this vector will be Value Of B
		int FullFlag; //'1' if the Leaf is Full , '0' otherwise
		long long int Size; //Number of elements in the Leaf
	};
struct Node
	{
	
	std::vector<float> ValArray; // Put 0 in its first position whenever used:: Maximum size goes upto 'm'
	Buffer NodeBuffer;
	Node *NodeChild[4]; //Later, put size of 'm'
	Node *Parent; 
	Leaf *LeafChild[4];
	int ChildFullFlag,NodeBufferFullFlag,Level; 
	int NumberOFChild,EmptyNumberOfChildRequired; // Will hold number of children of a particular Node
	long long int NodeBufferSize; //Holds count of number of elements in the Buffer
	int LeafChildFlag; //'1' if child is the Leaf and '0' for Node child
	int ValArraySize; // Stores the size of valArray
	struct Node *YTreeRoot; //Pointer to Associate Y Tree
	};

/* 
I/Os that do not contribute to Tree Construction/Search,etc. 
 Such I/Os are basically involved in Copying OR reading Input files,accessing User made Local Stack,etc.
 */
long long int NonTreeIOs=0; 
double NonTreeIOTime=0;


class BufferRangeTree
{
	int RootCreateFlag;
	int NumberOfNodes;
	long long int TreeN,TreeM,TreeB,mTree,nTree;
	Node *temp;// 'temp' Node is to be used on different instances just to know which Node we were previously pointing to while requesting
							// for a New Node/Leaf
	Node *a,*b,*c,*NodeForYDimensionSearch; // For temporary unexpected requirement
	Buffer ManageBuffer;
	int LevelUpFlag; //Used in recursive Node addition for initial Level Up on the Tree in SplitRoot Node case
	int *p,FirstRootSplitFlag,LatTreeFlag,LonTreeFlag,splitRootNodeCallFlag; // LatTreeFlag and LonTreeFlag indicates whether the current Tree belongs Latitude or Longitude Data
	long long int ManageBufferSize;
	typedef stxxl::VECTOR_GENERATOR<float,1,1,786432>::result vector;
	typedef stxxl::VECTOR_GENERATOR<Data,1,1,786432>::result vector_Data;
	typedef stxxl::STACK_GENERATOR<Node*,stxxl::external,stxxl::grow_shrink,4,512>::result stack;
	stack recursion_stack;  // create empty stack object
	stack PreorderStack,EmptyBufferStack; // Preorder Traversal for Y Tree construction

	// generate Class stats instance
	stxxl::stats *ClassStats;// = stxxl::stats::get_instance();
	
	public:
	//Root is public so  that it can passed to be attached as Associate Y-Tree
	Node *Root;
		BufferRangeTree(long long int InM,long long int InN,long long int InB)
			{
				
				ClassStats=stxxl::stats::get_instance();
				//start measurement here
				stxxl::stats_data stats_begin(*ClassStats);
				NodeForYDimensionSearch=NULL;
				NumberOfNodes=0;
				TreeN=InN;
				TreeM=InM;
				TreeB=InB;
				mTree=TreeM/TreeB;
				nTree=TreeN/TreeB;
				ManageBufferSize=0;
				RootCreateFlag=0;
				Root=NULL;
				temp=NULL;
				a=NULL;
				b=NULL;
				c=NULL;
				p=NULL;
				splitRootNodeCallFlag=0;
				
				LevelUpFlag=0; // Used initially in SplitRootNode
				FirstRootSplitFlag=0; // Will be used as a flag whenever 2 new Internal Nodes will be required 
		}

		int InsertBlock(Buffer BlockSizedBuffer)
			{
				
				long long int a,b;
				double c,d;
				a=stxxl::stats_data(*ClassStats).get_reads();
				c=stxxl::stats_data(*ClassStats).get_pio_time();
					
				// Push each  of the 'm' blocks into the temporary Buffer Vector "ManageBuffer"
				//Change to the complete Buffer Size
				for(long long int i=0;i<BufferSize;i++)

					{

						
						ManageBuffer.content.push_back(BlockSizedBuffer.content.at(i));
						ManageBufferSize++;
						

				
						//DEBUG
						//std::cout<<endl<<" managebuffersize="<<ManageBufferSize<<" buffersize="<<BufferSize<<endl;
					}
				b=stxxl::stats_data(*ClassStats).get_reads();
				d=stxxl::stats_data(*ClassStats).get_pio_time();
				NonTreeIOs+=(b-a);
				NonTreeIOTime+=(d-c);

				if(ManageBufferSize==BufferSize)
					{
					//Sort the ManageBuffer, anyway next step is to insert it into the Tree
					ManageBuffer=SortBufferItems(ManageBuffer,ManageBufferSize);
					std::cout<<endl<<"Buffer sorted..."<<endl;
					std::cout<<"MANAGE BUFFER FULL!!!";

					//std::cout<<endl<<"ManageBuffer: "<<endl;
					//PrintBuffer(&ManageBuffer,ManageBufferSize);
					
					return 1; //Indicating the Full Buffer
					}
				else
					return 0; // Indicating Buffer is not yet Full
			}


		/* Return value:
			0- Nothing required
			1- new Intenal Node required
			2- new Leaf required
			3- new Leaf Node required

			Operation Flag indicating kind of value came:
			0- Nothing new 
			1- New Internal Node arrived
			2- New Leaf arrived
			3- New Leaf Node arrived

		*/
		// REMINDER: Update LeafChildFlag for each newly arrived Node Leaf/nonLeaf
		//Caution: Don't use 'temp' pointer anywhere else apart from the case in which Root is a Leaf Node
		int Operate(Node *InNode,int OperationFlag)

			{	
				// Nothing new have been send from function main() but control here indicates a Full ManageBuffer ready to get into the Tree
				if(OperationFlag==0)
					{
					// Root possibly can point to 3 kind of data
					/*
					1. NULL - Indicating Empty Tree
					2. Leaf Node-(1 level) which will have Full Leaf Nodes always
						In this case, request 2 new Internal Node(1 for new Root and 1 for holding split with the current Node)
					3. Internal Node
						In this case, call Empty Internal Buffer method which recursively empty other internal buffer(s)
					*/


					//Check for Empty Tree (case 1)
					//Action required: In this case, create a new Leaf Node as a Root Node
					if(Root==NULL)
						{
						//Indicate to create and send a new Leaf Node from function main()
						return 3;
						}
					
					// Check if Root is a Leaf Node (case 2)
					//Action requried: In this case, request 2 new Node(1 for new Internal Node Root and 1 Leaf Node for holding split with the current Node)
					if((*Root).LeafChildFlag==1)
						//LeafChildFlag=1 indicates that current Node have Leaves as its Child
						{
						
						temp=Root; // Hold the calling Node

						//ask for First Internal Node (new Root Node)
						return 1;
						}



					//Check if Root is an Internal Node(case 3)
					//Action required: In this case, call Empty Internal Buffer method which recursively empty other internal buffer(s)
					if((*Root).LeafChild[0]==NULL)
					// LeafChild=NULL indicates that current Node don't have any Leaf as its child,hence not a Leaf Node
					// So, it can be concluded that it is an Internal Node 
						{
							cout<<"In case 3";
							//CHECK FOR CHILDFULLFLAG here and decide whether to call SplitRootNode OR not


							//If Child is Full, we need to increase the Level(Height) of the Tree by 1 which will be done using an internal 
							//recursive function
							if((*Root).ChildFullFlag==1)
								{
								//Indicates requirement of internal Node recursively
								return 4;

								}

							//else, we can call EmptyInternalBuffer
							else
								{

									//Copy from ManageBuffer to Root Buffer
									CopyFromManageBufferToRootBuffer();

									EmptyInternalBuffer(Root);
								}

						}


					}
				//New Internal Node Arrived for recursive calls
				if(OperationFlag==4)
					{
						//Initial Level Up of the Tree by '1'
						if(LevelUpFlag==0)
							{
								/* Made new InNode as a new Root Node
								And old Root Node is now a Child of new InNode(now Root) node
								*/

								
								(*InNode).NodeChild[0]=Root;
								Root=InNode;

								//Set Root as the Parent of the Old Root Node
								(*(*Root).NodeChild[0]).Parent=Root;

								//Up the Level of Root Node
								(*Root).Level=(*(*Root).NodeChild[0]).Level+1;
								//Decrement Empty Child requirement
								(*Root).EmptyNumberOfChildRequired--;
								
								LevelUpFlag=1;
								
								// Push the current Node into the Stack
								recursion_stack.push(Root);

								//need new Internal Node
								return 4;
							}
						// Recursive calls till the Stack do not get Empty to add Empty Nodes in the Tree
						if(recursion_stack.size()>0)
							{
								//Take the Top element of Stack into 'a' and POP it out from the Stack
								a=recursion_stack.top();
								recursion_stack.pop();

								// Add InNode as a new Empty Child of the Top of Stack Node
								(*a).NodeChild[mTree-((*a).EmptyNumberOfChildRequired)]=InNode;
								(*a).EmptyNumberOfChildRequired--;
								(*InNode).Parent=a;
								(*InNode).Level=(*a).Level-1;

								if((*a).EmptyNumberOfChildRequired>0 && (*a).Level>1)
									{
										recursion_stack.push(a);

										
									}

								if((*InNode).EmptyNumberOfChildRequired>0 && (*InNode).Level>1)
									{
										recursion_stack.push(InNode);
									}
								// Reset 'a' so that it can be used in further iterations
								a=NULL;
								
								//Check for current InNode Level status to decide for next request of either Internal Node(=4) or
								// Leaf Node(=5)

								if((*InNode).Level==2)
									return 5; //Need Leaf Node
								else
									return 4;
							}
						// Need nothing anymore
						else
							{
							//Recursive Calls are over, so Reset LevelUpFlag
							LevelUpFlag=0;
							
							//Need to Split Root here

							//ROOT updates
							//Previous Root is at Index 0 in the Node Child list of current Root

							//After split, Root will have 2 Children
							(*Root).NumberOFChild=2;
							
							//Update NodeChild[0] of Root
							//We need not update the Buffer of Old Root because it will always be empty as we didn't call 
							//"CopyFromManageBufferToRootBuffer()"

							(*(*Root).NodeChild[0]).ChildFullFlag=0;
							
							//Pass second half of NodeChild from Current Root's NodeChild[0] to NodeChild[1]
							int tempNumChild=(*(*Root).NodeChild[0]).NumberOFChild;
							Node *tempNodePtr=NULL;
							for(int t=((*(*Root).NodeChild[0]).NumberOFChild/2),u=0;t<tempNumChild;t++,u++)
								{
									tempNodePtr=(*(*Root).NodeChild[1]).NodeChild[u];
									(*(*Root).NodeChild[1]).NodeChild[u]=(*(*Root).NodeChild[0]).NodeChild[t];
									(*(*Root).NodeChild[0]).NodeChild[t]=tempNodePtr;

									//Update NumberOFChild
									(*(*Root).NodeChild[0]).NumberOFChild--;
									(*(*Root).NodeChild[1]).NumberOFChild++;
								}
							//Reset tempNodePtr
							tempNodePtr=NULL;

							//Update ValArray of NodeChild[1]
							int tempValSize=(*(*Root).NodeChild[0]).ValArraySize;
							for (int i = (((*(*Root).NodeChild[0]).ValArraySize+1)/2); i <tempValSize; i++)
								{
									(*(*Root).NodeChild[1]).ValArray.push_back((*(*Root).NodeChild[0]).ValArray.at(i));
									(*(*Root).NodeChild[1]).ValArraySize++;


								}

							//Remove valArray elements of NodeChild[0]
							for (int i = (((*(*Root).NodeChild[0]).ValArraySize+1)/2); i <tempValSize; i++)
								{
									(*(*Root).NodeChild[0]).ValArray.pop_back();
									(*(*Root).NodeChild[0]).ValArraySize--;


								}
	
							//Update ValArray of 'Root'

							(*Root).ValArray.push_back((*(*Root).NodeChild[0]).ValArray.at((*(*Root).NodeChild[0]).ValArraySize-1));
							(*Root).ValArraySize++;

							(*Root).ValArray.push_back((*(*Root).NodeChild[1]).ValArray.at((*(*Root).NodeChild[1]).ValArraySize-1));
							(*Root).ValArraySize++;


							//Since, we are done with the ONE Level Up recursive calls and addition of Empty Nodes

							
							//Copy from ManageBuffer to Root Buffer
							CopyFromManageBufferToRootBuffer();
							
							//We can call now EmptyInteranalBuffer					
							EmptyInternalBuffer(Root);
							}
					}

				//New Leaf Node arrived for recursive Calls
				if(OperationFlag==5)
					{
						b=recursion_stack.top();
						recursion_stack.pop();

						(*b).NodeChild[mTree-((*b).EmptyNumberOfChildRequired)]=InNode;
						(*b).EmptyNumberOfChildRequired--;

						(*InNode).Parent=b;

						if((*b).EmptyNumberOfChildRequired>0)
							{
								recursion_stack.push(b);
								return 5;
							}
						else
							return 4;
					}


				//New Internal Node arrived
				if(OperationFlag==1)
					{
					//check for the Calling Node OR check for the previously pointing Node in the Tree when the Node was required
					//If it is a Root then it means that Buffer of Root Node is Full and their is no space left in its Children Buffer(s)
					// We do need to make the new Node as the Root and existing Root as its Child Node
					//Case 1: part 1: Root is a Leaf Node
					if(temp==Root)
						{
						// Since we need 2 New nodes for this case, we will ask for 1 more Leaf Node and store currently arrived Internal
							// Node in 'a' temporarily
							a=InNode;
							
							//indicates 1 Internal Node as arrived, need 1 more Leaf Node
							FirstRootSplitFlag=1;
							std::cout<<endl<<"Request for 2nd Leaf Node"<<endl;
							temp=NULL;
							//Asking for 2nd Leaf Node for Split of Root Node
							return 3;
						}

					
					


					}


				//Case for Empty Partially Filled Buffers
				if(OperationFlag==6)
					{   Node *y;
						//Push all NodeChild Of Root Node in the Stack for Preorder traversal, Right->Left
					if((*Root).Level>=3)
						{
						for(int i=(*Root).NumberOFChild-1;i>=0;i--)
							{
								EmptyBufferStack.push((*Root).NodeChild[i]);
							}
						}

						while(!EmptyBufferStack.empty())
							{
							y=EmptyBufferStack.top();

							if((*y).NodeBufferSize>0)
								{
									//Make Size of y's NodeBuffer to BufferSize because we need always need a Full Buffer to be passed down the Tree
									long long int YBufferSize=(*y).NodeBufferSize;
									
									//Sort the Buffer
									(*y).NodeBuffer=SortBufferItems((*y).NodeBuffer,(*y).NodeBufferSize);

									Data LastData=(*y).NodeBuffer.content.at(YBufferSize-1);
									if(YBufferSize<BufferSize)
										{
										//Add the Last Data in the Buffer repeatedly till it gets filled
											for(long long int u=YBufferSize;u<BufferSize;u++)
												{
												(*y).NodeBuffer.content.push_back(LastData);
												(*y).NodeBufferSize++;
												}

											if((*y).NodeBufferSize==BufferSize)
												{
												(*y).NodeBufferFullFlag=1;
												}
										}

									EmptyInternalBuffer(y);
								}

							//once visited, remove the Node from the Stack
							EmptyBufferStack.pop();

							//Move its Children into the Stack only if y.Level>=3, because EmptyInternalBuffer have Base case at Level 2
							if((*y).Level>=3)
								{
								for(int i=(*y).NumberOFChild-1;i>=0;i--)
									{
										EmptyBufferStack.push((*y).NodeChild[i]);
									}
								}
							}

					}

				// New Leaf Node arrived
				if(OperationFlag==3)
					{

					
					//CASE 1: Check for Empty Tree
					if(Root==NULL)
						{
						Root=InNode;
						std::cout<<endl<<"Just came Buffer"<<endl;
						//PrintNode(Root);
						
						//TEST=PASS
						/*
						std::cout<<endl<<"Root: "<<Root;
						for(long long int x=0;x<mTree;x++)
									{
										std::cout<<endl<<"LeafChild of Root: "<<x<<": "<<(*Root).LeafChild[x];
									}
						*/
						std::cout<<endl<<"CASE 1 Begin";
						//Root Created
						RootCreateFlag=1;
						(*Root).LeafChildFlag=1; //because it will have leaves as its child(REMINDER done)

						CopyFromManageBufferToRootBuffer();
						
						//TEST Buffer=PASS
						/*
						for(long long int y=0;y<BufferSize;y++)
							{
							std::cout<<endl<<"Root Buffer: "<<(*Root).NodeBuffer.content.at(y);
							}
						*/
						//Since, LeafBuffer is Full, we should distribute its data into the underlying Leaves


						//TEST=PASS
						std::cout<<endl<<"Before Empty Leaf Buffer"<<endl;
						//PrintNode(Root);
						//PrintBuffer(&((*Root).NodeBuffer),(*Root).NodeBufferSize);
						

						EmptyLeafBuffer(Root);
						

						std::cout<<endl<<"CASE 1 End";
						
						
						std::cout<<endl<<"After Empty Leaf Buffer"<<endl;
						//PrintNode(Root);
						std::cout<<endl<<"Root Address: "<<Root<<endl;
						}


					//CASE 2: Inserting two new Nodes
					// FirstRootSplitFlag=1 will imply that we have 2 new Nodes in 'a'(Internal Node) and 'InNode'(Leaf Node)
					if(FirstRootSplitFlag==1)
						{
						//TEST for two new Nodes
							std::cout<<endl<<"Address of new nodes: a="<<a<<endl;
							std::cout<<endl<<"Address of new nodes: InNode="<<InNode<<endl;
							//std::cout<<endl<<"Address of Root node: Root="<<Root<<endl;
							//PrintClassData();


							//std::cout<<endl<<"Root Before Split"<<endl;
							//PrintNode(Root);
							
							// After splitting the current Root Node, Root will be changed to new Internal Node 'a'
							// Update the new Root Node with the return value of SplitRootNode()
							if(splitRootNodeCallFlag==0)
								{
								//std::cout<<endl<<"Root Before Split"<<endl;
								//PrintNode(Root);
								Root=SplitRootNode(a,InNode);

								//Level of Root Node will be "+1" than its Child's Level
								(*Root).Level=((*(*Root).NodeChild[0]).Level)+1;
								//std::cout<<endl<<"Root After Split"<<endl;
								//PrintNode(Root);
								
								//This will be required only in  case we make Level 2 as our Base case in EmptyInternalBuffer
								/*
								//We want to seal these leaves here itself with no further addition of data into it
								(*(*Root).NodeChild[0]).ChildFullFlag=1;
								(*(*Root).NodeChild[1]).ChildFullFlag=1;

								//Delete Unused Leaves of children of Root at NodeChild[0] and NodeChild[1]
								for(long long int s=0;s<2;s++)
									{
									for(long long int g=((1+mTree)/2);g<mTree;g++)
										{
											delete	(*(*Root).NodeChild[s]).LeafChild[g];
										}
									}
								*/
								return 3;
								}

							//Add Empty Nodes to Root
							if((*Root).EmptyNumberOfChildRequired>0)
								{
								//One more new LeafNode arrived in InNode, attach it to Root
								(*Root).NodeChild[mTree-(*Root).EmptyNumberOfChildRequired]=InNode;
								(*Root).EmptyNumberOfChildRequired--;
								return 3;
								}
							
							//std::cout<<endl<<"Root after Split and Empty Nodes added"<<endl;
							
							//PrintNode(Root);

							//std::cout<<endl<<"Children of Root"<<endl;
							//for(long long int y=0;y<(*Root).NumberOFChild;y++)
								//PrintNode((*Root).NodeChild[y]);
							//PrintClassData();

							//Reset all other temporary Node Pointers a,b,c,temp
							a=NULL;
							b=NULL;
							c=NULL;
							temp=NULL;
							CopyFromManageBufferToRootBuffer();
							
							
							// Start emptying Buffer starting from Root 

							EmptyInternalBuffer(Root);
							//Reset the FirstRootSplitFlag
							FirstRootSplitFlag=0;
						}



					

					}
			//Indicating nothing new required, ready to take next Buffer into the Tree	
			return 0;
			}
		
		void CopyFromManageBufferToRootBuffer()
			{
				long long int a,b;
				double c,d;
				a=stxxl::stats_data(*ClassStats).get_reads();
				c=stxxl::stats_data(*ClassStats).get_pio_time();

				//Transfer Data from ManageBuffer to the Buffer of the Root Node
						for(long long int y=0;y<BufferSize;y++)
							{
							(*Root).NodeBuffer.content.push_back(ManageBuffer.content.at(y));
							(*Root).NodeBufferSize++;
							}
				(*Root).NodeBufferFullFlag=1;
				std::cout<<endl<<"Transfered data from ManageBuffer to NodeBuffer ";
				cout<<stxxl::stats_data(*ClassStats).get_reads()<<endl;
				
				b=stxxl::stats_data(*ClassStats).get_reads();
				d=stxxl::stats_data(*ClassStats).get_pio_time();

				NonTreeIOs+=(b-a);
				NonTreeIOTime+=(d-c);

			}




		/* There are 2 types of Split:
		 1. Split of a Root Node:: 
			In this case, we need 2 new Nodes, one new Internal Node('a') will be a new Root Node and (Old Root Node+New Leaf Node('InNode')) will be
			its 2 initial Child such that Old Node's second half of the Child will be assigned(or passed) to new Child Node. 
		 2. Split of an Internal Node(this will contain both Internal and Leaf Node): Actually, it is just an addition of a new sibling to the Calling Node
			In this case, we need 1 new node(Same as the Calling Node i.e. either Leaf Node OR Internal Node), which will be attached adjacent to the CallingNode as its Sibling with its second half assigned 
			to the new Node.
		*/
		Node* SplitRootNode(Node *a,Node *InNode)
			{
			Leaf *tempLeafPointer;
			std::cout<<endl<<"				::In SplitRootNode::"<<endl;
			std::cout<<endl<<"Address of new nodes: a="<<a<<endl;
			std::cout<<endl<<"Address of new nodes: InNode="<<InNode<<endl;
						
			// In case of splitting the Root Node, 2 cases are: either Root will be Leaf Node OR Internal Node. We need to handle
			// both the cases separately.
			

			// Case 1: Root is a Leaf Node :: Check it with Leaf Child Flag(=1, for Leaf Node)
			if((*Root).LeafChildFlag==1)
				{
				/* Actions to be performed: 
				1. Pass second half of Child Elements of Root to new 'InNode' Leaf Node.
				   
				*/

					// Pass second half of ValArray of Root to ValArray of InNode 
					for(int f=(1+((*Root).ValArraySize/2));f<(*Root).ValArraySize;f++)
						{
							(*InNode).ValArray.push_back((*Root).ValArray.at(f));
							(*InNode).ValArraySize++;

							

						}
					
					//Copy data of second half of PointersToLeaf of Root to PointersToLeaf of InNode 
					for(int f=((*Root).NumberOFChild/2),g=0;f<(*Root).NumberOFChild;f++,g++)
						{
							/*for(long long int x=0;x<(*(*Root).LeafChild[f]).Size;x++)
								{
								(*(*InNode).LeafChild[g]).LeafArr.push_back((*(*Root).LeafChild[f]).LeafArr.at(x));
								}
							*/
							tempLeafPointer=(*InNode).LeafChild[g];
							(*InNode).LeafChild[g]=(*Root).LeafChild[f];
							(*Root).LeafChild[f]=tempLeafPointer;
							(*InNode).NumberOFChild++;
							
						

						}

					
					// Delete the ValArray from current Root Node accordingly
					int TempValArraySize=(*Root).ValArraySize;
					for(int f=(1+((*Root).ValArraySize/2));f<TempValArraySize;f++)
						{
							(*Root).ValArray.pop_back();
							(*Root).ValArraySize--;

						}
					

					// Delete the Data of Leaf Pointers from current Root Node accordingly
					int TempNumberOfChild=(*Root).NumberOFChild;
					for(int f=((*Root).NumberOFChild/2);f<TempNumberOfChild;f++)
						{
							(*(*Root).LeafChild[f]).LeafArr.clear();
							(*Root).NumberOFChild--;
						}


					//Make current 'Root' and 'InNode' as Internal Child Node of 'a'
					(*a).NodeChild[0]=Root;
					(*a).NumberOFChild++;
					(*a).EmptyNumberOfChildRequired--;

					(*a).NodeChild[1]=InNode;
					(*a).NumberOFChild++;
					(*a).EmptyNumberOfChildRequired--;

					//Push in ValArray of 'a'					
					(*a).ValArray.push_back((*Root).ValArray.at((*Root).ValArraySize-1));
					(*a).ValArraySize++;

					(*a).ValArray.push_back((*InNode).ValArray.at((*InNode).ValArraySize-1));
					(*a).ValArraySize++;

					//Update Parent Of Old Root Node and InNode
					(*Root).Parent=a;
					(*InNode).Parent=a;

				}

			// Case 2: Root is an Internal Node
			else if((*Root).LeafChildFlag==0)
				{
				}
			splitRootNodeCallFlag=1; //Indicates SplitRootNode Called once
			return a;
			}
		
		// Two parameters: n, the Node whose Child node at index 'NodeChildIndex' in NodeChild Array is supposed to split
		// OR to whose right , the new Internal Node will be added
		void SplitInternalNode(Node *n,long long int NodeChildIndex)
			{
				/*
				TWO CASES
				1. *n's NodeChild can be an Internal Node
				2. *n's NodeChild can be a Leaf Node 
				(As per Base case of EmptyInternalNode at "Level 2", we don't need this case.
				This case will will be required only if base case in EmptyInternalNode is changed to "Level 1")
				*/
			
			// CASE 1. *n's NodeChild is an Internal Node	
				if((*(*n).NodeChild[NodeChildIndex]).LeafChildFlag==0)
					{
					long long int FirstEmptyNodeChildIndex;
					Node *tempNodePointer=NULL;
					FirstEmptyNodeChildIndex=(*n).NumberOFChild;
					//Test
					cout<<endl<<"In Split Internal Node: before swapping"<<endl;
					//PrintNode(n);
					cout<<endl<<"1"<<endl;
					// Starting at First Empty Node Child, shift all Node Child by 1 to the right to make a space to the right of Node Child
					for(long long int u=FirstEmptyNodeChildIndex;u>(NodeChildIndex+1);u--)
						{
							// Go on swapping 
							tempNodePointer=(*n).NodeChild[u];
							(*n).NodeChild[u]=(*n).NodeChild[u-1];
							(*n).NodeChild[u-1]=tempNodePointer;
						}
					//Added one more child to n's family
					(*n).NumberOFChild++;
					
			
					// Check for number of childs being Full for n because we just added one more child to n's family
					if((*n).NumberOFChild==mTree)
						{
							(*n).ChildFullFlag=1;
						}
					else
						{
							(*n).ChildFullFlag=0;
						}
					cout<<endl<<"2"<<endl;
					
					// SPLIT
					long long int tempNumberOfChild=(*(*n).NodeChild[NodeChildIndex]).NumberOFChild;
			
					// Pass Second half of NodeChild's Children to the new Node
					for(long long int u=((*(*n).NodeChild[NodeChildIndex]).NumberOFChild/2),v=0;u<tempNumberOfChild;u++,v++)
						{
							tempNodePointer=(*(*n).NodeChild[NodeChildIndex+1]).NodeChild[v];
							(*(*n).NodeChild[NodeChildIndex+1]).NodeChild[v]=(*(*n).NodeChild[NodeChildIndex]).NodeChild[u];
							(*(*n).NodeChild[NodeChildIndex]).NodeChild[u]=tempNodePointer;


							(*(*n).NodeChild[NodeChildIndex]).NumberOFChild--;
							(*(*n).NodeChild[NodeChildIndex+1]).NumberOFChild++;


							//Update the new Parents
							(*(*(*n).NodeChild[NodeChildIndex+1]).NodeChild[v]).Parent= (*n).NodeChild[NodeChildIndex+1];
							(*(*(*n).NodeChild[NodeChildIndex]).NodeChild[u]).Parent=(*n).NodeChild[NodeChildIndex];
							tempNodePointer=NULL;
						}
					cout<<endl<<"3"<<endl;
					
					//Reset ChildFullFlag for NodeChild and its sibling
					if((*(*n).NodeChild[NodeChildIndex]).NumberOFChild==mTree)
						{
							(*(*n).NodeChild[NodeChildIndex]).ChildFullFlag=1;
						}
					else
						{
							(*(*n).NodeChild[NodeChildIndex]).ChildFullFlag=0;
						}


					if((*(*n).NodeChild[NodeChildIndex+1]).NumberOFChild==mTree)
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ChildFullFlag=1;
						}
					else
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ChildFullFlag=0;
						}


					// Pass second half of ValArray of NodeChild to ValArray of NodeChild's Sibling 
					for(long long int f=(1+((*(*n).NodeChild[NodeChildIndex]).ValArraySize/2));f<(*(*n).NodeChild[NodeChildIndex]).ValArraySize;f++)
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ValArray.push_back((*(*n).NodeChild[NodeChildIndex]).ValArray.at(f));
							(*(*n).NodeChild[NodeChildIndex+1]).ValArraySize++;
							

						}
					cout<<endl<<"4"<<endl;
					
					//Delete contents of ValArray of NodeChild that has been transfered to its new sibling
					long long int TempValArraySize=(*(*n).NodeChild[NodeChildIndex]).ValArraySize;

					for(long long int f=(1+((*(*n).NodeChild[NodeChildIndex]).ValArraySize/2));f<TempValArraySize;f++)
						{
							(*(*n).NodeChild[NodeChildIndex]).ValArray.pop_back();
							(*(*n).NodeChild[NodeChildIndex]).ValArraySize--;
						}

					cout<<endl<<"5"<<endl;
					
					// Shift the elements in Buffer of NodeChild which have greater value than the highest value in ValArray of NodeChild to its sibling

					//Find the highest value in ValArray
					float HighestValArray=(*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1);


					// For passing NodeChild Buffer should have atleast one element
					if((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize>0)
						{
						//Find the value greater than HighestValArray value in the NodeChild Buffer 
						long long int SplitValBufferIndex=-1;

						for(long long int f=0;f<(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;f++)
							{
								if((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at(f).Lat>HighestValArray)
									{
										SplitValBufferIndex=f;
										break;
									}
							}
						cout<<endl<<"6"<<endl;
					
						// Found the Index in NodeChild Buffer
						if(SplitValBufferIndex>=0)
						{
						//pass the Buffer values into the Buffer of new NodeChild sibling
						for(long long int f=SplitValBufferIndex;f<(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;f++)
						{
						(*(*n).NodeChild[NodeChildIndex+1]).NodeBuffer.content.push_back((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at(f));
						(*(*n).NodeChild[NodeChildIndex+1]).NodeBufferSize++;
						}
						
						long long int tempNodeBufferSize=(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;
						//Remove the elements from NodeChild's Buffer
						for(long long int f=SplitValBufferIndex;f<tempNodeBufferSize;f++)
							{
							(*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.pop_back();
							(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize--;
							}
						
						}



						}
					cout<<endl<<"7"<<endl;
					
					for(long long int y=0;y<(*n).ValArraySize;y++)
						{
							cout<<" "<<(*n).ValArray.at(y);		
						}
					//Update 'n' according to the new split occured
					//Only ValArray related details are remaining to be updated for 'n'

					// From this Index, we need to shift ValArray values in some other Buffer temporarily
					long long int IndexToStartInValArr;
					IndexToStartInValArr=NodeChildIndex+1;
					cout<<endl<<(*n).ValArray.at(IndexToStartInValArr);	
					// This value would be MAX(Last Value in ValArray of NodeChild,Last Value in Buffer of NodeChild)
					float ValueToUpdateInValArr;

					cout<<endl<<"8.1"<<endl;
					//PrintNode((*n).NodeChild[NodeChildIndex]);
					int UpdateByBufferFlag=0;
					//Check this only if Buffer and valArray are not empty
					if((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize>0)
						{
						// Checking if Highest val in ValArray of NodeChild IS LESS THAN Highest val in Buffer of NodeChild
						if(((*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1))<((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize-1).Lat))
							{
							ValueToUpdateInValArr=(*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize-1).Lat;
							UpdateByBufferFlag=1;
							}
					
						}
					if(UpdateByBufferFlag==0)
						{
							ValueToUpdateInValArr=(*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1);
						}
						
					//PostOrderTraversal(Root);
					cout<<endl<<"8.2"<<endl;
					cout<<ValueToUpdateInValArr<<endl;	
					// Shift ValArray value of 'n' in temporary vector from index 'IndexToStartInValArr'
					
					std::vector<float> tempVector;
					long long int tempVectorSize=0;
					
					//Copy values in ValArray from IndexToStartinValArr index till the end to tempVector
					for(long long int y=IndexToStartInValArr;y<(*n).ValArraySize;y++)
						{
							tempVector.push_back((*n).ValArray.at(y));
							tempVectorSize++;
						}

					//Remove the elements of ValArray of 'n' from IndexToStartInValArr
					for(long long int y=IndexToStartInValArr;y<(*n).ValArraySize;y++)
						{
							(*n).ValArray.pop_back();
							
						}
					
					//Add new Value to ValArray of 'n'
					(*n).ValArray.push_back(ValueToUpdateInValArr);
					(*n).ValArraySize++;

					//Again pushback all the elements of tempVector to ValArr of 'n'
					for(long long int y=0;y<tempVectorSize;y++)
						{
							(*n).ValArray.push_back(tempVector.at(y));
							cout<<" "<<(*n).ValArray.at(y);		
						}
					
					//Reset tempVector
					tempVector.clear();
					tempVectorSize=0;

					
					
					cout<<endl<<"After swapping and split"<<endl;
					//PrintNode(n);
					}


			//CASE 2. *n's NodeChild is a Leaf Node which have Leaves as its Child
			else if((*(*n).NodeChild[NodeChildIndex]).LeafChildFlag==1)
				{
					long long int FirstEmptyNodeChildIndex;
					Node *tempNodePointer=NULL;
					FirstEmptyNodeChildIndex=(*n).NumberOFChild;
					//Test
					cout<<endl<<"In Split Internal Node: before swapping"<<endl;
					PrintNode(n);
					
					// Starting at First Empty Leaf Child, shift all Leaf Child by 1 to the right to make a space to the right of Leaf Child
					for(long long int u=FirstEmptyNodeChildIndex;u>(NodeChildIndex+1);u--)
						{
							// Go on swapping 
							tempNodePointer=(*n).NodeChild[u];
							(*n).NodeChild[u]=(*n).NodeChild[u-1];
							(*n).NodeChild[u-1]=tempNodePointer;
						}
					//Added one more child to n's family
					(*n).NumberOFChild++;
					
			
					// Check for number of childs being Full for n because we just added one more child to n's family
					if((*n).NumberOFChild==mTree)
						{
							(*n).ChildFullFlag=1;
						}
					else
						{
							(*n).ChildFullFlag=0;
						}

					// SPLIT
					long long int tempNumberOfChild=(*(*n).NodeChild[NodeChildIndex]).NumberOFChild;
					Leaf *tempLeafPointer;
					// Pass Second half of NodeChild's Leaf Children to the new Node
					for(long long int u=((*(*n).NodeChild[NodeChildIndex]).NumberOFChild/2),v=0;u<tempNumberOfChild;u++,v++)
						{
							tempLeafPointer=(*(*n).NodeChild[NodeChildIndex+1]).LeafChild[v];
							(*(*n).NodeChild[NodeChildIndex+1]).LeafChild[v]=(*(*n).NodeChild[NodeChildIndex]).LeafChild[u];
							(*(*n).NodeChild[NodeChildIndex]).LeafChild[u]=tempLeafPointer;


							(*(*n).NodeChild[NodeChildIndex]).NumberOFChild--;
							(*(*n).NodeChild[NodeChildIndex+1]).NumberOFChild++;


							/*
							//No parent Pointer for Leaf struct
							//Update the new Parents
							(*(*(*n).NodeChild[NodeChildIndex+1]).LeafChild[v]).Parent= (*n).NodeChild[NodeChildIndex+1];
							(*(*(*n).NodeChild[NodeChildIndex]).NodeChild[u]).Parent=(*n).NodeChild[NodeChildIndex];
							*/

							tempNodePointer=NULL;
						}

					//Reset ChildFullFlag for NodeChild and its sibling
					if((*(*n).NodeChild[NodeChildIndex]).NumberOFChild==mTree)
						{
							(*(*n).NodeChild[NodeChildIndex]).ChildFullFlag=1;
						}
					else
						{
							(*(*n).NodeChild[NodeChildIndex]).ChildFullFlag=0;
						}


					if((*(*n).NodeChild[NodeChildIndex+1]).NumberOFChild==mTree)
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ChildFullFlag=1;
						}
					else
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ChildFullFlag=0;
						}


					// Pass second half of ValArray of NodeChild to ValArray of NodeChild's Sibling 
					for(long long int f=(1+((*(*n).NodeChild[NodeChildIndex]).ValArraySize/2));f<(*(*n).NodeChild[NodeChildIndex]).ValArraySize;f++)
						{
							(*(*n).NodeChild[NodeChildIndex+1]).ValArray.push_back((*(*n).NodeChild[NodeChildIndex]).ValArray.at(f));
							(*(*n).NodeChild[NodeChildIndex+1]).ValArraySize++;
							

						}
					
					//Delete contents of ValArray of NodeChild that has been transfered to its new sibling
					long long int TempValArraySize=(*(*n).NodeChild[NodeChildIndex]).ValArraySize;

					for(long long int f=(1+((*(*n).NodeChild[NodeChildIndex]).ValArraySize/2));f<TempValArraySize;f++)
						{
							(*(*n).NodeChild[NodeChildIndex]).ValArray.pop_back();
							(*(*n).NodeChild[NodeChildIndex]).ValArraySize--;
						}


					// Shift the elements in Buffer of NodeChild which have greater value than the highest value in ValArray of NodeChild to its sibling

					//Find the highest value in ValArray
					float HighestValArray=(*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1);


					// For passing NodeChild Buffer should have atleast one element
					if((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize>0)
						{
						//Find the value greater than equal to HighestValArray value in the NodeChild Buffer
						//From value at this index, we can pass the value into its new Sibling 
						long long int SplitValBufferIndex=-1;

						for(long long int f=0;f<(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;f=f++)
							{
								if((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at(f).Lat>HighestValArray)
									{
										SplitValBufferIndex=f;
										break;
									}
							}

						// Found the Index in NodeChild Buffer
						if(SplitValBufferIndex>0)
							{
							//pass the Buffer values into the Buffer of new NodeChild sibling
							for(long long int f=SplitValBufferIndex;f<(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;f++)
							{
							(*(*n).NodeChild[NodeChildIndex+1]).NodeBuffer.content.push_back((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at(f));
							(*(*n).NodeChild[NodeChildIndex+1]).NodeBufferSize++;
							}
						
							long long int tempNodeBufferSize=(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize;
							//Remove the elements from NodeChild's Buffer
							for(long long int f=SplitValBufferIndex;f<tempNodeBufferSize;f++)
								{
								(*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.pop_back();
								(*(*n).NodeChild[NodeChildIndex]).NodeBufferSize--;
								}
						
							}



						}

					//Update 'n' according to the new split occured
					//Only ValArray related details are remaining to be updated for 'n'

					// From this Index, we need to shift ValArray values in some other Buffer temporarily
					long long int IndexToStartInValArr;
					IndexToStartInValArr=NodeChildIndex+1;

					// This value would be MAX(Last Value in ValArray of NodeChild,Last Value in Buffer of NodeChild)
					float ValueToUpdateInValArr;


					// Checking if Highest val in ValArray of NodeChild IS LESS THAN Highest val in Buffer of NodeChild
					if(((*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1))<((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize-1).Lat))
						{
						ValueToUpdateInValArr=((*(*n).NodeChild[NodeChildIndex]).NodeBuffer.content.at((*(*n).NodeChild[NodeChildIndex]).NodeBufferSize-1).Lat);
						}
					else
						{
						ValueToUpdateInValArr=(*(*n).NodeChild[NodeChildIndex]).ValArray.at((*(*n).NodeChild[NodeChildIndex]).ValArraySize-1);
						}

					// Shift ValArray value of 'n' in temporary vector from index 'IndexToStartInValArr'
					
					std::vector<float> tempVector;
					long long int tempVectorSize=0;
					
					//Copy values in ValArray from IndexToStartinValArr index till the end to tempVector
					for(long long int y=IndexToStartInValArr;y<(*n).ValArraySize;y++)
						{
							tempVector.push_back((*n).ValArray.at(y));
							tempVectorSize++;
						}

					//Remove the elements of ValArray of 'n' from IndexToStartInValArr
					for(long long int y=IndexToStartInValArr;y<(*n).ValArraySize;y++)
						{
							(*n).ValArray.pop_back();
							
						}
					
					//Add new Value to ValArray of 'n'
					(*n).ValArray.push_back(ValueToUpdateInValArr);
					(*n).ValArraySize++;

					//Again pushback all the elements of tempVector to ValArr of 'n'
					for(long long int y=0;y<tempVectorSize;y++)
						{
							(*n).ValArray.push_back(tempVector.at(y));
							
						}

					//Reset tempVector
					tempVector.clear();
					tempVectorSize=0;


					cout<<endl<<"After swapping and split"<<endl;
					PrintNode(n);
				}
			}
		void EmptyManageBuffer()
			{
				// Function is called only when ManageBuffer is Full, So empty the ManageBuffer and make ManageBufferSize=0
				ManageBuffer.content.clear();
				ManageBufferSize=0;

				
			}

		void EmptyInternalBuffer(Node *n)
			{

			
			//Buffer tempBlock;

			// Base Case: Check if *n is a "Level 2" Node, Empty the Buffer here in the corresponding Leaf Nodes for each element in n's Buffer 
			
			if(((*n).Level==2))
					{

					//PrintBuffer(&(*n).NodeBuffer,(*n).NodeBufferSize);
					
					// Buffer to hold children Data of Node 'n'
					Buffer HoldDataBuffer;
					long long int HoldDataBufferSize=0;
					//Add data of Leaf Node Children of 'n' to 'HoldDataBuffer' temporarily
					cout<<endl<<"1";
					//Parse Children of 'n'
					for(long long int x=0;x<(*n).NumberOFChild;x++)
						{
							// Parse leaves of each Children of 'n'
							for(long long int y=0;y<(*(*n).NodeChild[x]).NumberOFChild;y++)
								{
									//Copy data from Leaf at 'y' to HoldDataBuffer
									for(long long int z=0;z<(*(*(*n).NodeChild[x]).LeafChild[y]).Size;z++)
										{
										HoldDataBuffer.content.push_back((*(*(*n).NodeChild[x]).LeafChild[y]).LeafArr.at(z));
										HoldDataBufferSize++;
										}

									//Reset the Leaf at 'y' index
									(*(*(*n).NodeChild[x]).LeafChild[y]).FullFlag=0;
									(*(*(*n).NodeChild[x]).LeafChild[y]).LeafArr.clear();
									(*(*(*n).NodeChild[x]).LeafChild[y]).Size=0;
								}

							//Reset the Children of 'n' at Index 'x'
							(*(*n).NodeChild[x]).ChildFullFlag=0;
							(*(*n).NodeChild[x]).NumberOFChild=0;
							(*(*n).NodeChild[x]).ValArray.clear();
							(*(*n).NodeChild[x]).ValArraySize=1;

							//Push '0' into valArray
							(*(*n).NodeChild[x]).ValArray.push_back(0);
						}
					cout<<" 2";

					//Now add Data in the Buffer of 'n' to 'HoldDataBuffer'
					for(long long int x=0;x<(*n).NodeBufferSize;x++)
						{
							HoldDataBuffer.content.push_back((*n).NodeBuffer.content.at(x));
							HoldDataBufferSize++;
						}
					cout<<endl<<"Hold Buffer Size "<<HoldDataBufferSize<<endl;

					//Clear n's Data
					(*n).NodeBuffer.content.clear();
					(*n).NodeBufferFullFlag=0;
					(*n).NodeBufferSize=0;
					(*n).NumberOFChild=0;
					(*n).ValArray.clear();
					(*n).ValArray.push_back(0);
					(*n).ValArraySize=1;

					//Sort HoldDataBuffer to be added into n's Children now with the new Data added
					
					
					HoldDataBuffer=SortBufferItems(HoldDataBuffer,HoldDataBufferSize);


					
					
					//PrintBuffer(&HoldDataBuffer,HoldDataBufferSize);
					//Index of n's child to whose Buffer data in HoldDataBuffer will go which will iterate after adding BufferSize elements in each
					//child's Buffer AND Index of Leaf Child is used to iterate while filling up leaves of Leaf Nodes
					long long int IndexOfNChild=0,CurBufferSize=0,IndexOfLeafChild=0,CurLeafSize=0;


					//Add initial one Child to 'n'
					(*n).NumberOFChild++;
					cout<<" 3";

					//Flood data in HoldDataBuffer into n's children's Buffer which in turn will be added to their leaves
					for(long long int x=0;x<HoldDataBufferSize;x++)
						{
							// Move to the next n's Child
							if(CurBufferSize==BufferSize)
								{
									//cout<<" 3."<<IndexOfNChild<<" Buffer Size"<<(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize<<endl;
									//cout<<"TreeB "<<TreeB<<endl;
									//Reset
									CurBufferSize=0;
									CurLeafSize=0;

									//Buffer of n's Children at Current Index is Full
									(*(*n).NodeChild[IndexOfNChild]).NodeBufferFullFlag=1;
									
									//Before moving to next child of n, Empty the Buffer of Node Child at 'IndexofNChild' to its Leaves
									for(long long int y=0;y<(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize;y++)
										{
										//Move to next Leaf
										if(CurLeafSize==TreeB)
											{
											//cout<<endl<<" 3."<<IndexOfNChild<<"."<<IndexOfLeafChild;
											CurLeafSize=0;
											(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).FullFlag=1;

											IndexOfLeafChild++;
											
											}
										//cout<<":::"<<(*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.at(y).Lat;
										(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).LeafArr.push_back((*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.at(y));

										(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).Size++;
										CurLeafSize++;

										}
									//Now, n's Child at Current Index have all its Children
									(*(*n).NodeChild[IndexOfNChild]).ChildFullFlag=1;
									(*(*n).NodeChild[IndexOfNChild]).NumberOFChild=mTree;

									//Empty the Buffer
									(*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.clear();
									(*(*n).NodeChild[IndexOfNChild]).NodeBufferFullFlag=0;
									(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize=0;


									//Reset variables
									CurLeafSize=0;
									IndexOfLeafChild=0;

									//Move to next child
									IndexOfNChild++;

									//Increase Child Count for 'n'
									(*n).NumberOFChild++;
									
								}

							(*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.push_back(HoldDataBuffer.content.at(x));
							(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize++;
							CurBufferSize++;
						}
					cout<<" 4";
					//Empty the remaining elements in "(*(*n).NodeChild[IndexOfNChild])"s Buffer
						{
									//Reset
									CurBufferSize=0;
									//cout<<" 4."<<IndexOfNChild<<" Buffer Size"<<(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize<<endl;
									
									//Empty the Buffer of Node Child at 'IndexofNChild' to its Leaves
									for(long long int y=0;y<=(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize;y++)
										{
										//Move to next Leaf
										if(CurLeafSize==TreeB)
											{
											//cout<<endl<<" 4."<<IndexOfNChild<<"."<<IndexOfLeafChild;
											CurLeafSize=0;
											(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).FullFlag=1;

											IndexOfLeafChild++;

											//Update the Number of Leaf Child of "(*(*n).NodeChild[IndexOfNChild])"
											(*(*n).NodeChild[IndexOfNChild]).NumberOFChild++;
											if(y==(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize)
												break;
											}
										
										//cout<<":::"<<(*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.at(y).Lat;
										
										(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).LeafArr.push_back((*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.at(y));

										(*(*(*n).NodeChild[IndexOfNChild]).LeafChild[IndexOfLeafChild]).Size++;
										CurLeafSize++;

										}
									//Check for n's Child at Current Index have all its Children
									if((*(*n).NodeChild[IndexOfNChild]).NumberOFChild==mTree)
										{
										(*(*n).NodeChild[IndexOfNChild]).ChildFullFlag=1;
										}
									
									//Empty the Buffer
									(*(*n).NodeChild[IndexOfNChild]).NodeBuffer.content.clear();
									(*(*n).NodeChild[IndexOfNChild]).NodeBufferFullFlag=0;
									(*(*n).NodeChild[IndexOfNChild]).NodeBufferSize=0;


									//Reset variables
									CurLeafSize=0;
									IndexOfLeafChild=0;

									
									
						}

					//Update ValArrays
					for(int i=0;i<(*n).NumberOFChild;i++)
						{
							for(int j=0;j<(*(*n).NodeChild[i]).NumberOFChild;j++)
								{
									(*(*n).NodeChild[i]).ValArray.push_back((*(*(*n).NodeChild[i]).LeafChild[j]).LeafArr.at((*(*(*n).NodeChild[i]).LeafChild[j]).Size-1).Lat);
									(*(*n).NodeChild[i]).ValArraySize++;
								}

							//PrintNode((*n).NodeChild[i]);
							
							(*n).ValArray.push_back((*(*n).NodeChild[i]).ValArray.at((*(*n).NodeChild[i]).ValArraySize-1));
							(*n).ValArraySize++;
						}

					//PrintNode(n);
					//Check number of Child of 'n'
					if((*n).NumberOFChild==mTree)
						{
							(*n).ChildFullFlag=1;
						}
					cout<<" 5.1"<<endl;
					cout<<endl<<" 5.2"<<endl;
					
					//Data has been moved, Empty HoldDataBuffer
					HoldDataBuffer.content.clear();
					HoldDataBufferSize=0;

					}

				
				//  n is an Internal Node
				//Caution: We are not sorting Buffer after each insertion,sort it once the Buffer gets Full 
				else
					{
					long long int tempBufferSize;
					float CurrentLat;
					Data CurrentData;
					tempBufferSize=(*n).NodeBufferSize;
					//We need to empty 'm*BlockSize' number of elements from current Node n's Buffer
					for(long long int h=0;h<tempBufferSize;h++)
						{

						/*
						//Fill the 'tempBlock' with Block in n's Buffer
						for(long long int w=((*n).NodeBufferSize-TreeB);w<(*n).NodeBufferSize;w++)
								{
									tempBlock.content.push_back((*n).NodeBuffer.content.at(w));
								}

						// We have got Block Sized Data to be passed down the Tree in 'tempBlock'
						*/
						
						// Take the last value from NodeBuffer of 'n' into CurrentLat to move down the Tree into its corresponding Node Child's Buffer
						CurrentLat=(*n).NodeBuffer.content.at((*n).NodeBufferSize-1).Lat;
						CurrentData=(*n).NodeBuffer.content.at((*n).NodeBufferSize-1);

						long long int ChildIndex=-1;
						//First find the corresponding child of 'n' to which 'CurrentLat' should go
						// Parse ValArray of (*n) to find Internal Child Node for 'CurrentLat'
							for(long long int u=1;u<(*n).ValArraySize;u++)
								{
									if((*n).ValArray.at(u-1)<CurrentLat && (*n).ValArray.at(u)>=CurrentLat)
										{
										ChildIndex=u-1;
										break;
										}

									
								}
							
						// ChildIndex=-1 indicates that 'CurrentLat' can go in the highest value Child in valArray 
							if(ChildIndex==-1)
								{
									// If correct position is not found, set ChildIndex to last Child
									ChildIndex=(*n).NumberOFChild-1;
								}	
									

						//Update ValArray of (*n) for ChildIndex with MAX(CURRENT ValArr Value for ChildIndex, CurrentLat)
							if((*n).ValArray.at(ChildIndex+1)<CurrentLat)
								{
									(*n).ValArray[ChildIndex+1]=CurrentLat;
								}
		



						// Check for the child's ChildFullFlag=1, in this case we need to call SplitInternalNode which will add a sibling
						// to the corresponding child node
							if((*(*n).NodeChild[ChildIndex]).ChildFullFlag==1)
										{
										SplitInternalNode(n,ChildIndex);  //WRITE CODE FOR THIS CALL = DONE
										//PostOrderTraversal(Root);
										
										//Since, ValArray of 'n' has been updated, we need to reset the ChildIndex
										
										// ChildIndex should remain the same but updation of ValArray at 'ChildIndex+1' is required
										if((*n).ValArray.at(ChildIndex+1)>=CurrentLat)
											{
											//Update ValArray of (*n) for ChildIndex with MAX(CURRENT ValArr Value for ChildIndex, Currentlat)
												if((*n).ValArray.at(ChildIndex+1)<CurrentLat)
													{
														(*n).ValArray[ChildIndex+1]=CurrentLat;
													}
											}

										//Else, ChildIndex should move to next NodeChild
										else
											{
											ChildIndex+=1;
											//Update ValArray of (*n) for ChildIndex with MAX(CURRENT ValArr Value for ChildIndex, CurrentLat)
												if((*n).ValArray.at(ChildIndex+1)<CurrentLat)
													{
														(*n).ValArray[ChildIndex+1]=CurrentLat;
													}
											}


										}


							// Now, we can insert the 'CurrentLat' in the corresponding Child Node's Buffer
							(*(*n).NodeChild[ChildIndex]).NodeBuffer.content.push_back(CurrentData);
							(*(*n).NodeChild[ChildIndex]).NodeBufferSize++;
							
									
										/*
										//Insert tempBlock in corresponding Child Node's Buffer
										for(long long int o=0;o<TreeB;o++)
											{
											// Emptying the tempBlock into the corresponding Child Node's Buffer
											(*(*n).NodeChild[ChildIndex]).NodeBuffer.content.push_back(tempBlock.content.at(o));
											(*(*n).NodeChild[ChildIndex]).NodeBufferSize++;

											}

										*/
										
										/*
										//Sort corresponding Child Node's Buffer
										(*(*n).NodeChild[ChildIndex]).NodeBuffer=SortBufferItems((*(*n).NodeChild[ChildIndex]).NodeBuffer);
										*/



						// Delete last element from n's Buffer since it is moved down the Tree in its correct Buffer
							(*n).NodeBuffer.content.pop_back();
							(*n).NodeBufferSize--;
								
							
						//n's Buffer is no more Full
						(*n).NodeBufferFullFlag=0;

						/*
						//Clear tempBlock, since its content are moved and we need to move to next iteration
						tempBlock.content.clear();
						*/

						//Check if corresponding Child Node's Buffer is full, in that case call recursively for that Node
						if((*(*n).NodeChild[ChildIndex]).NodeBufferSize==BufferSize)
								{
								(*(*n).NodeChild[ChildIndex]).NodeBufferFullFlag=1;

								//Sort the Buffer of corresponding Child Node's Buffer before passing it recursively
								(*(*n).NodeChild[ChildIndex]).NodeBuffer=SortBufferItems((*(*n).NodeChild[ChildIndex]).NodeBuffer,(*(*n).NodeChild[ChildIndex]).NodeBufferSize);
								

								// Recursive call
								EmptyInternalBuffer((*n).NodeChild[ChildIndex]);
										
								}
						}
					}

			}



		int PostOrderTraversal(Node *q)
			{
				if((*q).LeafChildFlag==1)
					{
						cout<<endl<<"BASE CASE"<<endl;
						PrintNode(q);
						NumberOfNodes++;
						return 0;
					}
				for(long long int i=0;i<(*q).NumberOFChild;i++)
					{
						//cout<<endl<<"Recursive Iteration: "<<(*q).NumberOFChild<<endl;
						PostOrderTraversal((*q).NodeChild[i]);
						//cout<<endl<<"After Recursive Iteration:"<<endl;
					}

				PrintNode(q);
				NumberOfNodes++;
				return 0;
			}

		int DeleteEmptyNodes(Node *q)
			{
			if((*q).LeafChildFlag==1)
					{
						cout<<endl<<"BASE CASE: Before"<<endl;
						//PrintNode(q);

						for(int g=(*q).NumberOFChild;g<mTree;g++)
							{
								delete (*q).LeafChild[g];
								(*q).LeafChild[g]=NULL;
							}
						cout<<"After"<<endl;
						//PrintNode(q);
						return 0;
					}
				for(long long int i=0;i<(*q).NumberOFChild;i++)
					{
						//cout<<endl<<"Recursive Iteration: "<<(*q).NumberOFChild<<endl;
						DeleteEmptyNodes((*q).NodeChild[i]);
						//cout<<endl<<"After Recursive Iteration:"<<endl;
					}

				//cout<<endl<<"BEFORE"<<endl;
				//PrintNode(q);

				for(int g=(*q).NumberOFChild;g<mTree;g++)
					{
						//cout<<"g="<<g<<endl;
						
						delete (*q).NodeChild[g];
						(*q).NodeChild[g]=NULL;
					}
				//cout<<endl<<"Error Test:"<<(*(*q).NodeChild[(*q).NumberOFChild]).Level;	
				
				//cout<<endl<<"AFTER"<<endl;
				//PrintNode(q);

				return 0;
			}
		void EmptyLeafBuffer(Node *LeafNode)
			{
			//Store empty Leaf Index
			long long int EmptyLeafIndex=0,r=0;  //'r' Variable for denoting Leaf Index which increments after every Block(B) number of
												// elements
			std::cout<<endl<<"1";
			//Check for first empty Leaf
				for(long long int z=0;z<mTree;z++)
					{
						std::cout<<endl<<"Inside loop:"<<(*(*LeafNode).LeafChild[z]).FullFlag;
						if((*(*LeafNode).LeafChild[z]).FullFlag==0)
							{
							EmptyLeafIndex=z;
							std::cout<<endl<<"In Nested Loop"<<z;
							break;
							}
					}
				
				std::cout<<endl<<"2";
				// No leaf is Full for the Leaf Child of LeafNode
				
				if(EmptyLeafIndex==0)
					{
						std::cout<<endl<<"No leaf is Full...";
								
							//Transfer Data into Leaves(B items: because Leaves have the size of a block) sequentially from the LeafNode Buffer
							//Loop will run for the whole size of the Buffer to transfer all its data into the Leaves
							for(long long int y=0;y<(*LeafNode).NodeBufferSize;y++)
									{
										// Push the Data in the LeafNode Buffer into the 'r'th Leaf child of LeafNode
										(*(*LeafNode).LeafChild[r]).LeafArr.push_back((*LeafNode).NodeBuffer.content.at(y));
										
										(*(*LeafNode).LeafChild[r]).Size++; //Addition of element to Leaf
										
										//TEST of Leaf Node Buffer content =PASS
										//std::cout<<endl<<"r="<<r<<" In EmptyLeafNode: "<<(*LeafNode).NodeBuffer.content.at(y);
										//std::cout<<endl<<(*(*LeafNode).LeafChild[0]).LeafArr.at(y);

										//One Leaf got its 'B' number of elements. Move to next Leaf
										if((y+1)%TreeB==0)
											{

											//Leaf at 'r'  is FUll
											(*(*LeafNode).LeafChild[r]).FullFlag=1;
											
											//Increase the number of child of 'LeafNode'
											(*LeafNode).NumberOFChild++;
											//Leaf Data test =PASS
											
											//std::cout<<endl<<"For Leaf="<<r<<endl;
											// Print 'r'th Leaf Data
											/*for(long long int s=0;s<TreeB;s++)
												std::cout<<(*(*LeafNode).LeafChild[r]).LeafArr.at(s)<<" ";
											*/
											//Update ValArray = Contains the Largest element in its Leaf Child
											(*LeafNode).ValArray.push_back((*(*LeafNode).LeafChild[r]).LeafArr.at(TreeB-1).Lat);
											(*LeafNode).ValArraySize++;

											r++;
											
											}
									}
					
					//Make ChildFullFlag=1 indicating that it don't have space in its Leaves anymore
					(*LeafNode).ChildFullFlag=1;
							

					//Now, you can empty the Leaf Node Buffer
					(*LeafNode).NodeBuffer.content.clear();
					(*LeafNode).NodeBufferSize=0;
					(*LeafNode).NodeBufferFullFlag=0;
					}

				// Few of the leaves of LeafNode are Full 
				else
					{
					}
					
			}
		

		Buffer SortBufferItems(Buffer BufferToSort,long long int SortBufferSize)
			{
				Data temp;
				typedef stxxl::sorter<Data, DataComparator, 1*1024*1024> sorter_type;
				sorter_type float_sorter(DataComparator(), 64 * 1024 * 1024);
				stxxl::random_number32 rand32;
				stxxl::timer Timer1;
				
				Timer1.start();
				for (size_t i = 0; i < SortBufferSize; ++i)
				{
					
					float_sorter.push(BufferToSort.content[i]);    // fill sorter container
				}
				Timer1.stop();
				STXXL_MSG("push time: " << (Timer1.mseconds() / 1000));
				stxxl::timer Timer2;
				Timer2.start();
				float_sorter.sort();  // switch to output state and sort
				Timer2.stop();
				STXXL_MSG("sort time: " << (Timer2.mseconds() / 1000));
				
				//Empty BufferToSort to fill it up with sorted elements returned by 'float_Sorter'
				(BufferToSort).content.clear();
				
				// echo sorted elements
			   while (!float_sorter.empty())
				{
					temp.Lat=float_sorter->Lat;
					temp.Lon=float_sorter->Lon;
					(BufferToSort).content.push_back(temp);
					//std::cout << float_sorter->Lat << " "<<float_sorter->Lon<<std::endl;  // access value
					++float_sorter;
				}
			 return BufferToSort;
			}

		void PrintNode(Node* n)
			{
				std::cout<<endl<<"NODE DETAILS of:"<<n<<endl;
				std::cout<<"Child Full Flag="<<(*n).ChildFullFlag<<endl;
				std::cout<<"Node Buffer Full Flag="<<(*n).NodeBufferFullFlag<<endl;
				for(long long int j=0;j<mTree;j++)
					std::cout<<j<<"th Leaf Child Address="<<(*n).LeafChild[j]<<endl;
				std::cout<<endl;
				std::cout<<"Leaf Child Flag="<<(*n).LeafChildFlag<<endl;
				/*
				for(long long int j=0;j<(*n).NodeBufferSize;j++)
					std::cout<<j<<"th value in NodeBuffer ="<<(*n).NodeBuffer.content.at(j)<<endl;
				std::cout<<endl;
				*/
				std::cout<<"Node Buffer Size="<<(*n).NodeBufferSize<<endl;
				PrintBuffer(&(*n).NodeBuffer,(*n).NodeBufferSize);
				for(long long int j=0;j<mTree;j++)	
					std::cout<<j<<"th Node Child Address="<<(*n).NodeChild[j]<<endl;
				std::cout<<endl;
				
				std::cout<<"Number OF Children="<<(*n).NumberOFChild<<endl;
				std::cout<<"Parent Address="<<(*n).Parent<<endl;
				std::cout<<"EmptyNumberOfChildRequired="<<(*n).EmptyNumberOfChildRequired<<endl;
				std::cout<<"Level Of Node(Bottom Up)="<<(*n).Level<<endl;
				std::cout<<"ValArraySize="<<(*n).ValArraySize<<endl;

				for(long long int j=0;j<(*n).ValArraySize;j++)
					std::cout<<j<<"th element in ValArray"<<(*n).ValArray.at(j)<<endl;
				
				std::cout<<"YTreeRoot="<<(*n).YTreeRoot<<endl;

				std::cout<<"--------------------------------------------------------------"<<endl;
			}
		
		void PrintBuffer(Buffer* b,long long int size)


			{
			std::cout<<endl<<"PRINT BUFFER BEGIN: Size="<<size<<endl;
			for(long long int y=0;y<size;y++)
				std::cout<<"Index:"<<y<<" Lat="<<(*b).content.at(y).Lat<<" Lon="<<(*b).content.at(y).Lon<<endl;
			std::cout<<"PRINT BUFFER END"<<endl;
			}

		void PrintClassData()
			{
			std::cout<<endl<<"---------------------------------------------------------------------"<<endl<<"::CLASS DATA BEGIN::"<<endl<<endl;
			
			//std::std::cout<<" :BufferRangeTree Class Data: "<<endl<<endl;
			std::cout<<"Root Create Flag="<<RootCreateFlag<<endl;
			std::cout<<"TreeN="<<TreeN<<" TreeM="<<TreeM<<" TreeB="<<TreeB<<endl;
			std::cout<<"mTree="<<mTree<<" nTreeM="<<nTree<<endl;
			std::cout<<"Node::Root="<<Root<<" Node::temp="<<temp<<endl;
			std::cout<<"Node::a="<<a<<" Node::b="<<b<<" Node::c="<<c<<endl;
			std::cout<<"Buffer::ManageBuffer "<<&ManageBuffer<<endl;
			std::cout<<"FirstRootSplitFlag="<<FirstRootSplitFlag<<endl;
			std::cout<<"int::p ="<<p<<endl;
			std::cout<<"ManageBufferSize="<<ManageBufferSize<<endl;
			std::cout<<endl<<"::CLASS DATA END::"<<endl<<"---------------------------------------------------------------------"<<endl;
			}
		
		void Call(int flag)
			{
			if(flag==1)
				{
				cout<<endl<<"POSTORDER TRAVERSAL BEGIN"<<endl;
				PostOrderTraversal(Root);
				cout<<endl<<"Number Of Nodes:"<<NumberOfNodes<<endl<<"POSTORDER TRAVERSAL END"<<endl;
				NumberOfNodes=0;
				}
			else if(flag==2)
				{
				cout<<endl<<"DELETE EMPTY NODES BEGIN"<<endl;
				DeleteEmptyNodes(Root);
				cout<<endl<<"DELETE EMPTY NODES END"<<endl;
				}
			else if(flag==3)
				{
				cout<<endl<<"SEARCH BEGIN"<<endl;
				//PrintNode(Root);
				ofstream CurrentFile;
				CurrentFile.open ("1-DimensionSearchResult.txt");
				Search(Root,4,43,&CurrentFile);
				CurrentFile.close();
				cout<<endl<<"SEARCH END"<<endl;
				}
			//2-Dimension Search
			else if(flag==4)
				{
				cout<<endl<<"2-D SEARCH BEGIN"<<endl;

				//X-Search
				ofstream CurrentXFile;
				CurrentXFile.open ("XSearchResult.txt");
				//PrintNode(Root);
				Search(Root,7250,325000,&CurrentXFile);
				CurrentXFile.close();

				//Y-Search
				ofstream CurrentYFile;
				CurrentYFile.open ("YSearchResult.txt");
				//PrintNode(Root);
				Search((*NodeForYDimensionSearch).YTreeRoot,4500,350500,&CurrentYFile);
				CurrentYFile.close();
				
				cout<<endl<<"2-D SEARCH END"<<endl;
				}
			}

		int SearchPostOrderTraversal(Node *q,vector_Data *SearchData,long long int *SearchDataSize,ofstream *SearchFile)
			{
				if((*q).LeafChildFlag==1)
					{
						
						//Store all its Leaves in SearchData
						for(int k=0;k<(*q).NumberOFChild;k++)
							{
							//For each Leaf
								for(int f=0;f<(*(*q).LeafChild[k]).Size;f++)
								{
								(*SearchData).push_back((*(*q).LeafChild[k]).LeafArr.at(f));
								(*SearchDataSize)++;
								(*SearchFile)<<(*(*q).LeafChild[k]).LeafArr.at(f).Lat<<" "<<(*(*q).LeafChild[k]).LeafArr.at(f).Lon<<endl;
								}

							}
						return 0;
					}

				for(long long int i=0;i<(*q).NumberOFChild;i++)
					{
						SearchPostOrderTraversal((*q).NodeChild[i],SearchData,SearchDataSize,SearchFile);
					}

				return 0;
			}

		int Search(Node *n,float x1,float x2,ofstream *SearchFile)
			{
			cout<<" 0 ";
			int x1_Index=-1,x2_Index=-1,x2_NotFound=0;
			cout<<" 1 ";
			//Store the Result into it
			vector_Data SearchData;
			long long int SearchDataSize=0;
			Node *tempNdPtr=NULL;
			stack LeftSearchStack,RightSearchStack;
			

			
			//To find the Split Node
			while(x1_Index==x2_Index)
				{
				//Parse ValArray to find x1 ChildIndex
					for(int i=1;i<(*n).ValArraySize;i++)
						{
							if((*n).ValArray.at(i-1)<x1 && (*n).ValArray.at(i)>=x1)
								{
								x1_Index=i-1;
								break;
								}
						}
					cout<<" 2 ";

					//x1 not found,it means, it is greater than the highest value in ValArray or Negative, which is invalid
					if(x1_Index==-1)
						{
						cout<<endl<<x1<<" not in range,Invalid Input."<<endl;
						return 0;
						
						}

					//Parse ValArray to find x2 ChildIndex
					for(int i=1;i<(*n).ValArraySize;i++)
						{
							if((*n).ValArray.at(i-1)<x2 && (*n).ValArray.at(i)>=x2)
								{
								x2_Index=i-1;
								break;
								}
						}

					//x2 not found
					if(x2_Index==-1)
						{
						cout<<endl<<x2<<" not in range,Invalid Input"<<endl;
					
						return 0;
						}
					
					// If we do not find the Split Node, move to the corresponding Node Child
					if(x1_Index==x2_Index)
						{
							//Recursive call
							if((*n).Level>1)
								n=(*n).NodeChild[x1_Index];

							//BASE CASE
							//Handle Base case when *n is a Leaf Node 
							if((*n).Level==1)
								{
									//Parse ValArray to find x1 ChildIndex
									for(int i=1;i<(*n).ValArraySize;i++)
										{
											if((*n).ValArray.at(i-1)<x1 && (*n).ValArray.at(i)>=x1)
												{
												x1_Index=i-1;
												break;
												}
										}
									cout<<" 2.1 ";
									//x1 not found,it means, it is greater than the highest value in ValArray or Negative, which is invalid
									if(x1_Index==-1)
										{
										cout<<endl<<x1<<" not in range,Invalid Input."<<endl;
										return 0;
						
										}

									//Parse ValArray to find x2 ChildIndex
									for(int i=1;i<(*n).ValArraySize;i++)
										{
											if((*n).ValArray.at(i-1)<x2 && (*n).ValArray.at(i)>=x2)
												{
												x2_Index=i-1;
												break;
												}
										}
									//x2 not found
									if(x2_Index==-1)
										{
										cout<<endl<<x2<<" not in range,Invalid Input"<<endl;
					
										return 0;
										}
									//Parse Leaf for x1
									for(long long int y=0;y<(*(*n).LeafChild[x1_Index]).Size;y++)
										{
											if((*(*n).LeafChild[x1_Index]).LeafArr.at(y).Lat>=x1)
												{
													SearchData.push_back((*(*n).LeafChild[x1_Index]).LeafArr.at(y));
													SearchDataSize++;
													(*SearchFile)<<(*(*n).LeafChild[x1_Index]).LeafArr.at(y).Lat<<" "<<(*(*n).LeafChild[x1_Index]).LeafArr.at(y).Lon<<endl;
												}

										}
									cout<<" 2.1.1 "<<x1_Index<<" "<<x2_Index<<endl;
									//Move all Leaves in between x1-x2 to SearchData
									for(int y=x1_Index+1;y<x2_Index;y++)
										{
										//For each Leaf
											for(int f=0;f<(*(*n).LeafChild[y]).Size;f++)
											{
													(SearchData).push_back((*(*n).LeafChild[y]).LeafArr.at(f));
													(SearchDataSize)++;
													(*SearchFile)<<(*(*n).LeafChild[y]).LeafArr.at(f).Lat<<" "<<(*(*n).LeafChild[y]).LeafArr.at(f).Lon<<endl;
											}
											cout<<" 3.1.5";
										}
									cout<<" 2.1.2 ";
									//Parse Leaf for x2
									for(long long int y=0;y<(*(*n).LeafChild[x2_Index]).Size;y++)
										{
											if((*(*n).LeafChild[x2_Index]).LeafArr.at(y).Lat<=x2)
												{
													SearchData.push_back((*(*n).LeafChild[x2_Index]).LeafArr.at(y));
													SearchDataSize++;
													(*SearchFile)<<(*(*n).LeafChild[x2_Index]).LeafArr.at(y).Lat<<" "<<(*(*n).LeafChild[x2_Index]).LeafArr.at(y).Lon<<endl;
												}
										}
									cout<<" 2.1.3 ";
									//Print Search Data
									cout<<endl<<"IN SEARCH DATA: BEGIN"<<endl;
									for(long long int h=0;h<SearchDataSize;h++)
										{
											cout<<SearchData.at(h).Lat<<" "<<SearchData.at(h).Lon<<"::::";
										}
									cout<<endl<<"IN SEARCH DATA: END"<<endl;

									//Set Node for Y-Dimension Search
									NodeForYDimensionSearch=n;

									return 0;
								  }
							
							
						}
				}

			//Got the Split Node
			NodeForYDimensionSearch=n;

			//If Split Node is a Level 1 Node (Case for Root being at Level 1)
				if((*n).Level==1)
					{
					//Parse the Leaf at x1_Index
						Leaf *tempLeafPtr=(*n).LeafChild[x1_Index];
						for(int f=0;f<(*tempLeafPtr).Size;f++)
							{
								if((*tempLeafPtr).LeafArr.at(f).Lat>=x1)
									{
										(SearchData).push_back((*tempLeafPtr).LeafArr.at(f));
										(SearchDataSize)++;
										(*SearchFile)<<(*tempLeafPtr).LeafArr.at(f).Lat<<" "<<(*tempLeafPtr).LeafArr.at(f).Lon<<endl;
									}
							}
							
					//Parse the other Right side leaves
					
					for(int i=x1_Index+1;i<x2_Index;i++)
						{
						//For each Leaf
							for(int f=0;f<(*(*n).LeafChild[i]).Size;f++)
							{
									(SearchData).push_back((*(*n).LeafChild[i]).LeafArr.at(f));
									(SearchDataSize)++;
									(*SearchFile)<<(*(*n).LeafChild[i]).LeafArr.at(f).Lat<<" "<<(*(*n).LeafChild[i]).LeafArr.at(f).Lon<<endl;
							}
							
						}

					//Parse the Leaf at x_2 Index
					tempLeafPtr=(*n).LeafChild[x2_Index];
						for(int f=0;f<(*tempLeafPtr).Size;f++)
							{
								if((*tempLeafPtr).LeafArr.at(f).Lat<=x2)
									{
										(SearchData).push_back((*tempLeafPtr).LeafArr.at(f));
										(SearchDataSize)++;
										(*SearchFile)<<(*tempLeafPtr).LeafArr.at(f).Lat<<" "<<(*tempLeafPtr).LeafArr.at(f).Lon<<endl;
									}
							}

					cout<<endl<<"IN SEARCH DATA: BEGIN"<<endl;
					for(long long int h=0;h<SearchDataSize;h++)
								{
								cout<<SearchData.at(h).Lat<<" "<<SearchData.at(h).Lon<<"::::";
								}
					cout<<endl<<"IN SEARCH DATA: END"<<endl;
					
					return 0;
					}


			//Before calling for LeftSearch, store all adjacent Node Pointers in the LeftSearchStack
			cout<<" 3 ";
			for(int z=x2_Index-1;z>x1_Index;z--)
				{
				//Push data into LeftSearchStack for SearchPostOrderTraversal later
					LeftSearchStack.push((*n).NodeChild[z]);
				}
			cout<<" 3.1 ";
				//Call LeftSearch and Add the Data in the x1_Index child into SearchData
				LeftSearch((*n).NodeChild[x1_Index],x1,&LeftSearchStack,&SearchData,&SearchDataSize,SearchFile);
				
				cout<<" 4 ";
				//Empty the Data using SearchPostOrder in LeftSearchStack
				while(!LeftSearchStack.empty())
					{
						SearchPostOrderTraversal(LeftSearchStack.top(),&SearchData,&SearchDataSize,SearchFile);
						LeftSearchStack.pop();
					}

				//Call RightSearch and add Data in x2_Index child into SearchData
				RightSearch((*n).NodeChild[x2_Index],x2,&RightSearchStack,&SearchData,&SearchDataSize,SearchFile);

				//Print Search Data
				cout<<endl<<"IN SEARCH DATA: BEGIN"<<endl;
				for(long long int h=0;h<SearchDataSize;h++)
					{
						cout<<SearchData.at(h).Lat<<":"<<SearchData.at(h).Lon<<" ";
					}
				cout<<endl<<"IN SEARCH DATA: END"<<endl;
				return 0;	
			}

		int RightSearch(Node *r,float x2,stack *RightSearchStack,vector_Data *SearchData,long long int *SearchDataSize,ofstream *SearchFile)
			{
			int x2_Index=-1;
			while((*r).Level>0)
			  {
			   //Parse ValArray to find x2 ChildIndex
				for(int i=1;i<(*r).ValArraySize;i++)
							{
								if((*r).ValArray.at(i-1)<x2 && (*r).ValArray.at(i)>=x2)
									{
									x2_Index=i-1;
									break;
									}
							}
					//x2 not found
					if(x2_Index==-1)
						{
						cout<<endl<<x2<<" not in range,Invalid Input"<<endl;
					
						return 0;
						}

				if((*r).Level>1)
					{
					//Before calling down the Tree for RightSearch, run PostOrder on the SubTrees to the Left of x2
						for(int z=0;z<x2_Index;z++)
						{
							//Run Postorder on the Nodes to the Left of x2 in Left To Right iteration
							SearchPostOrderTraversal((*r).NodeChild[z],SearchData,SearchDataSize,SearchFile);
						
						}
					r=(*r).NodeChild[x2_Index];		
					}
				else
					{

					/*
					//First, Empty the RightSearchStack Nodes
					while(!(*RightSearchStack).empty())
						{
							SearchPostOrderTraversal((*RightSearchStack).top(),SearchData,SearchDataSize,SearchFile);
							(*RightSearchStack).pop();
						}
					*/

					//Parse the Left side leaves of x2_Index
					
					for(int i=0;i<x2_Index;i++)
						{
						//For each Leaf
							for(int f=0;f<(*(*r).LeafChild[i]).Size;f++)
							{
									(*SearchData).push_back((*(*r).LeafChild[i]).LeafArr.at(f));
									(*SearchDataSize)++;
									(*SearchFile)<<(*(*r).LeafChild[i]).LeafArr.at(f).Lat<<" "<<(*(*r).LeafChild[i]).LeafArr.at(f).Lon<<endl;
							}

						}

					//Parse the Leaf at x2_Index
						Leaf *tempLeafPtr=(*r).LeafChild[x2_Index];
						for(int f=0;f<(*tempLeafPtr).Size;f++)
							{
								if((*tempLeafPtr).LeafArr.at(f).Lat<=x2)
									{
										(*SearchData).push_back((*tempLeafPtr).LeafArr.at(f));
										(*SearchDataSize)++;
										(*SearchFile)<<(*tempLeafPtr).LeafArr.at(f).Lat<<" "<<(*tempLeafPtr).LeafArr.at(f).Lon<<endl;
									}
							}

					break;

					}

			  
			  }


			}

		void LeftSearch(Node *r,float x1,stack *LeftSearchStack,vector_Data *SearchData,long long int *SearchDataSize,ofstream *SearchFile)
		{
			cout<<" 3.1.1";
			int x1_Index=-1;
			while((*r).Level>0)
			  {
			   //Parse ValArray to find x1 ChildIndex
				for(int i=1;i<(*r).ValArraySize;i++)
							{
								if((*r).ValArray.at(i-1)<x1 && (*r).ValArray.at(i)>=x1)
									{
									x1_Index=i-1;
									break;
									}
							}
					cout<<" 3.1.2";
				if((*r).Level>1)
					{
					//Before calling down the Tree for LeftSearch, store all adjacent Node Pointers in the LeftSearchStack from Right To Left
					for(int z=(*r).NumberOFChild-1;z>x1_Index;z--)
						{
						//Push data into LeftSearchStack for SearchPostOrderTraversal later
							(*LeftSearchStack).push((*r).NodeChild[z]);
						}
					r=(*r).NodeChild[x1_Index];		
						cout<<" 3.1.3";
					}
				else
					{
					//Parse the Leaf at x1_Index
						Leaf *tempLeafPtr=(*r).LeafChild[x1_Index];
						for(int f=0;f<(*tempLeafPtr).Size;f++)
							{
								if((*tempLeafPtr).LeafArr.at(f).Lat>=x1)
									{
										(*SearchData).push_back((*tempLeafPtr).LeafArr.at(f));
										(*SearchDataSize)++;
										(*SearchFile)<<(*tempLeafPtr).LeafArr.at(f).Lat<<" "<<(*tempLeafPtr).LeafArr.at(f).Lon<<endl;
									}
							}
							cout<<" 3.1.4";
					//Parse the other Right side leaves
					
					for(int i=x1_Index+1;i<(*r).NumberOFChild;i++)
						{
						//For each Leaf
							for(int f=0;f<(*(*r).LeafChild[i]).Size;f++)
							{
									(*SearchData).push_back((*(*r).LeafChild[i]).LeafArr.at(f));
									(*SearchDataSize)++;
									(*SearchFile)<<(*(*r).LeafChild[i]).LeafArr.at(f).Lat<<" "<<(*(*r).LeafChild[i]).LeafArr.at(f).Lon<<endl;
							}
							cout<<" 3.1.5";
						}

					 break;
					}

			  }


		}


		int YConstructPostOrder(Node *q,ofstream *CurrentFile)
			{
				if((*q).LeafChildFlag==1)
					{
						cout<<endl<<"BASE CASE"<<endl;
						//Store all its Leaves in CurrentFile

						for(int k=0;k<(*q).NumberOFChild;k++)
							{
							//For each Leaf
								for(long long int f=0;f<(*(*q).LeafChild[k]).Size;f++)
								{
								(*CurrentFile)<<(*(*q).LeafChild[k]).LeafArr.at(f).Lon<<" "<<(*(*q).LeafChild[k]).LeafArr.at(f).Lat<<endl;
								}
							/*cout<<endl<<(*(*q).LeafChild[k]).Size<<" "<<TreeB;
							//Add dummy Data if the Leaf is Not Full because we need input in multiples of Buffer Size
							if((*(*q).LeafChild[k]).Size<TreeB)
								{
								float s=1000000;
								cout<<"Coming here "<<s<<endl;
								for(long long int f=(*(*q).LeafChild[k]).Size;f<TreeB;f++)
									{
										(*CurrentFile)<<s<<" "<<s<<endl;
									}
								}
							*/
							}
						return 0;
					}

				for(long long int i=0;i<(*q).NumberOFChild;i++)
					{
						//cout<<endl<<"Recursive Iteration: "<<(*q).NumberOFChild<<endl;
						YConstructPostOrder((*q).NodeChild[i],CurrentFile);
						//cout<<endl<<"After Recursive Iteration:"<<endl;
					}

				
				return 0;
			}

		int YConstructPreorderTraversal(Node *YAssociateRoot,int FirstCallFlag)
			{
			
			
			//FirstCallFlag=1 indicates intitial Call to start the Y-Tree Construction with Pushing Current Tree's Root in the PreOrder Stack
			//if FirstCallFlag=0 indicates we are getting Root Nodes of Y-Trees to get attached to the current Tree
			if(FirstCallFlag==1)
				{
					cout<<endl<<"First Call"<<endl;
					PreorderStack.push(Root);
					return 1;
				}


			else
				{
				if( remove( "YInput.txt" ) != 0 )
					{
					perror( "Error deleting file" );
					}
				else
					{
					puts( "File successfully deleted" );
					}
				ofstream CurrentFile;
				CurrentFile.open ("YInput.txt");
			
				if(!PreorderStack.empty())

					{
					Node *temporaryNd,*temporaryNd2;
					temporaryNd=NULL;
					temporaryNd2=NULL;
					cout<<" 1 ";
					temporaryNd=PreorderStack.top();
					if((*temporaryNd).YTreeRoot==NULL)
						{
						// Attaching the New Y-Tree's Root the its calling Parent Node in current X-Tree
						(*temporaryNd).YTreeRoot=YAssociateRoot;
						PrintNode(temporaryNd);
						cout<<" 2 ";
						//Make current Node in X-tree as the parent of the Root of newly created Y-Tree
						(*YAssociateRoot).Parent=temporaryNd;
						
						
						}
					else
						{
						cout<<" 3 ";
						PreorderStack.pop();
						}


					if((*temporaryNd).Level>=2)
						{
							cout<<" 4 ";
							for(int z=(*temporaryNd).NumberOFChild-1;z>=0;z--)
								{
								PreorderStack.push((*temporaryNd).NodeChild[z]);
								}
						}
					else if((*temporaryNd).Level==1)
							{
							cout<<" 4.1 ";
							PreorderStack.pop();
							if(PreorderStack.empty())
								return 0;
							cout<<" 4.2 ";
							}


					while((*PreorderStack.top()).YTreeRoot!=NULL)
							{
							cout<<" 5 ";
							
							PreorderStack.pop();
							if(PreorderStack.empty())
								return 0;
							
							}
					//Check for Empty Stack
					if(PreorderStack.empty())
						return 0;

					temporaryNd2=PreorderStack.top();
					YConstructPostOrder(temporaryNd2,&CurrentFile);
					CurrentFile.close();
						
						
						

					return 1;
					}


					
				return 0;
			
				}
			
			}
		
		
};

int main()
{
	
	typedef stxxl::VECTOR_GENERATOR<float>::result vector;
	//vector x;
	
	long long int M,B,N,n,m;
	//B=x.block_size;
	//std::cout<<"Block size: "<<B<<endl;
	B=1000000; //For testing purpose
	M=4000000;
	N=48000000;
	n=N/B;
	m=M/B; 
	float InputX;
	int FullBufferFlag,OperatorFlag;
	Buffer InputBuffer;
	stxxl::random_number32 rand32;
	BufferRangeTree MyTree(M,N,B);
	std::cout<<"Enter input numbers: ";
	ofstream CurrentYFile;
	CurrentYFile.open ("YInput.txt");
	float x,y;
	//cout.precision(8);
	// generate stats instance
	stxxl::stats * Stats = stxxl::stats::get_instance();
	// start measurement here
	
	// some computation ...
	float in[48]={47,1,9,29,912,34,54,5,6,3,7,2,8,9,1,89,34,5,23,61,25,85,12,233,352,637,458,23,12,07,38,1,54,16,95,34,75,61,91,194
					,523,569,416,264,361,826,951,162};
	
	

	 std::random_device rd; // obtain a random number from hardware
     std::mt19937 eng(rd()); // seed the generator
     std::uniform_int_distribution<> distr(1000, 400000); // define the range


	stxxl::stats_data stats_begin(*Stats);
	for(long long int i=0;i<N;i++)  // Loop for N elements (Assuming N to be in multiples of Block Size, in case of ambiguity add dummy values
												//				in to input N to make it a multiple of B(Block Size))
							
		{
			//cin>>InputX;
			x=distr(eng);
			y=distr(eng);
			InputBuffer.content.push_back(Data(x,y));
			CurrentYFile<<y<<" "<<x<<endl;
			
			//Pushing Buffer into the Tree
			if((i+1)%(M)==0)  // Buffer have 'M' number of elements
				{
					//std::cout<<endl<<"Meets this condition for i="<<i<<endl;
					FullBufferFlag=MyTree.InsertBlock(InputBuffer);
					InputBuffer.content.clear();
					// FullBufferFlag=1 indicates ManageBuffer in the Class filled block by block is Full

					// All operations related to One Full Buffer can happen here before moving to the next iteration of input N elements
					if(FullBufferFlag==1)
						{
						// Here Buffer is ready to get into the Tree
						//std::cout<<"Coming here i="<<i<<endl;
						// First Empty call on filling up the Buffer to check the status of the Tree
						OperatorFlag=MyTree.Operate(NULL,0);
						//std::cout<<endl<<"CAME HERE!!!! "<<OperatorFlag<<endl;
						try
						{
						while(OperatorFlag!=0)
							{
							//std::cout<<"WHILE LOOP CHECK..."<<endl;
							//new Internal Node required
							if(OperatorFlag==1 || OperatorFlag==4)
								{
									Node* newNode=new Node();
									(*newNode).LeafChildFlag=0; // Because it will not have Leaves as its Child
									
									//Put '0' in first ValArray's Position because we will have Child Pointers as '1' more than the ValArray size
									(*newNode).ValArray.push_back(0);
									(*newNode).NumberOFChild=0;
									(*newNode).EmptyNumberOfChildRequired=m;
									(*newNode).NodeBufferSize=0;
									(*newNode).ChildFullFlag=0;
									(*newNode).NodeBufferFullFlag=0;
									(*newNode).Parent=NULL;
									(*newNode).YTreeRoot=NULL;
									(*newNode).ValArraySize=1;
									(*newNode).Level=0; // Initialized to 0, will increase with indicating the height of the Tree
									

									for(long long int h=0;h<m;h++)
										{
										(*newNode).LeafChild[h]=NULL;
										(*newNode).NodeChild[h]=NULL;
										}
									std::cout<<endl<<"Internal Node sent="<<newNode<<endl;
									if(OperatorFlag==1)
										OperatorFlag=MyTree.Operate(newNode,1);
									else if(OperatorFlag==4)
										OperatorFlag=MyTree.Operate(newNode,4);
									std::cout<<endl<<"New request="<<OperatorFlag<<endl;
								}
							//new Leaf required
							/*else if(OperatorFlag==2)
								{
								Leaf* newLeaf=new Leaf();
								(*newLeaf).FullFlag=0; //Initially, Leaf is Empty
								(*newLeaf).Size=0;
								OperatorFlag=MyTree.Operate(NULL,newLeaf,2);
								}
							*/
							//new Leaf Node required
							else if(OperatorFlag==3 || OperatorFlag==5)
								{
								Node* newLeafNode=new Node();
								(*newLeafNode).LeafChildFlag=1;
								(*newLeafNode).NumberOFChild=0;
								(*newLeafNode).EmptyNumberOfChildRequired=m;
								//Put '0' in first ValArray's Position because we will have Child Pointers as '1' more than the ValArray size
								(*newLeafNode).ValArray.push_back(0);
								
								(*newLeafNode).NodeBufferSize=0;
								(*newLeafNode).ChildFullFlag=0;
								(*newLeafNode).NodeBufferFullFlag=0;
								(*newLeafNode).Parent=NULL;
								(*newLeafNode).ValArraySize=1;  // '1' for intially pushed '0' into the ValArray
								(*newLeafNode).Level=1; // Always 1 for Leaf Node
								(*newLeafNode).YTreeRoot=NULL;
								std::cout<<endl<<"Creating new Leaves...";
								
								//Adding new Leaf to all Leaf Pointer
								//if(i<12)
								{
								for(long long int x=0;x<m;x++)
									{
										//std::cout<<endl<<"Line 1...";
										Leaf* newLeaf=new Leaf();
										//std::cout<<endl<<"Line 2...";
										
										(*newLeaf).FullFlag=0; //Initially, Leaf is Empty
										(*newLeaf).Size=0;
										(*newLeafNode).NodeChild[x]=NULL;
										(*newLeafNode).LeafChild[x]=newLeaf;
										(*newLeafNode).EmptyNumberOfChildRequired--;
										//std::cout<<endl<<"Line 5...";
										
										//std::cout<<endl<<"LeafChild: "<<x<<": "<<(*newLeafNode).LeafChild[x];
									}
								}
								// New LeafNode have 'm' number of Leaves created as its child in the above Loop
								// This will help while spliting the Nodes to hold the count of number of children
								//(*newLeafNode).NumberOFChild=0;
								std::cout<<endl<<"Leaf Node sent="<<newLeafNode<<endl;
								// Sending new Leaf node with leaves attached to it.

								if(OperatorFlag==3)
										OperatorFlag=MyTree.Operate(newLeafNode,3);
								else if(OperatorFlag==5)
										OperatorFlag=MyTree.Operate(newLeafNode,5);
								
								}
							}

						}
						catch(const std::exception& ex)
						{
							std::cout<<endl<<"Exception caught: "<<ex.what()<<endl;
						}
						//Empty ManageBuffer in the Class to get ready to take next set of input elements
						MyTree.EmptyManageBuffer();
						}
				}
			
		}
		CurrentYFile.close();

		MyTree.Operate(NULL,6);

		MyTree.Call(2);
		MyTree.Call(1);
		std::cout<<endl<<"---------------------FOR CONSTRUCTION----------------"<<endl;
		std::cout << (stxxl::stats_data(*Stats) - stats_begin);
		
		//MyTree.Call(3);
	
		
		std::cout<<endl<<"Size of Data: "<<sizeof(Data)<<endl;


	std::cout<<endl<<"Total I/O Time: "<<stxxl::stats_data(*Stats).get_pio_time()<<endl;
	std::cout<<endl<<"Total Read I/Os: "<<stxxl::stats_data(*Stats).get_reads()<<endl;
	std::cout<<endl<<"Non Tree I/O Time: "<<NonTreeIOTime<<endl;
	std::cout<<endl<<"Non Tree Read I/Os: "<<NonTreeIOs<<endl;
	

	
	//Y-Associate Tree Construction
	
	long long int NumberOFYTrees=0;

	int YTreeFlag;
	YTreeFlag=MyTree.YConstructPreorderTraversal(NULL,1);
	//For each Node in XTree, we create an auxiliary Y-Tree
	try
	{
	while(YTreeFlag==1)
		{
		//YTreeFlag=0;
		long long int i=0;
		std::string line;
		std::string::size_type sz;     // alias of size_t
		std::ifstream Input ("YInput.txt");
		BufferRangeTree *YTree=new BufferRangeTree(M,N,B);
		float YInputX;
		int YFullBufferFlag,YOperatorFlag;
		Buffer YInputBuffer;
		if (Input.is_open())
		{
		i=0;
		cout<<endl<<"Y Input Start:"<<endl;
		
		while (getline(Input,line))
			{
			
			//getline(Input,line);
			float a_1 = std::stof (line,&sz);
			float a_2 = std::stof (line.substr(sz));
			cout<<endl<<"a_1="<<a_1<<" a_2="<<a_2<<endl;





			//Get input here

			
			//for(long long int i=0;i<N;i++)  // Loop for N elements (Assuming N to be in multiples of Block Size, in case of ambiguity add dummy values
													//				in to input N to make it a multiple of B(Block Size))
							
			{
				//cin>>InputX;
				YInputBuffer.content.push_back(Data(a_1, a_2));
				//std::cout<<endl<<"**** "<<i;
				//std::cout<<InputBuffer.content[i]<<" ";
				//Pushing Buffer into the Tree
				if((i+1)%(M)==0)  // Buffer have 'B' number of elements
					{
						//std::cout<<endl<<"Meets this condition for i="<<i<<endl;
						YFullBufferFlag=(*YTree).InsertBlock(YInputBuffer);
						YInputBuffer.content.clear();
						// FullBufferFlag=1 indicates ManageBuffer in the Class filled block by block is Full

						// All operations related to One Full Buffer can happen here before moving to the next iteration of input N elements
						if(YFullBufferFlag==1)
							{
							// Here Buffer is ready to get into the Tree
							//std::cout<<"Coming here i="<<i<<endl;
							// First Empty call on filling up the Buffer to check the status of the Tree
							YOperatorFlag=(*YTree).Operate(NULL,0);
							//std::cout<<endl<<"CAME HERE!!!! "<<OperatorFlag<<endl;
							try
							{
							while(YOperatorFlag!=0)
								{
								//std::cout<<"WHILE LOOP CHECK..."<<endl;
								//new Internal Node required
								if(YOperatorFlag==1 || YOperatorFlag==4)
									{
										Node* newNode=new Node();
										(*newNode).LeafChildFlag=0; // Because it will not have Leaves as its Child
									
										//Put '0' in first ValArray's Position because we will have Child Pointers as '1' more than the ValArray size
										(*newNode).ValArray.push_back(0);
										(*newNode).NumberOFChild=0;
										(*newNode).EmptyNumberOfChildRequired=m;
										(*newNode).NodeBufferSize=0;
										(*newNode).ChildFullFlag=0;
										(*newNode).NodeBufferFullFlag=0;
										(*newNode).Parent=NULL;
										(*newNode).YTreeRoot=NULL;
										(*newNode).ValArraySize=1;
										(*newNode).Level=0; // Initialized to 0, will increase with indicating the height of the Tree
									

										for(long long int h=0;h<m;h++)
											{
											(*newNode).LeafChild[h]=NULL;
											(*newNode).NodeChild[h]=NULL;
											}
										std::cout<<endl<<"Internal Node sent="<<newNode<<endl;
										if(YOperatorFlag==1)
											YOperatorFlag=(*YTree).Operate(newNode,1);
										else if(YOperatorFlag==4)
											YOperatorFlag=(*YTree).Operate(newNode,4);
										std::cout<<endl<<"New request="<<YOperatorFlag<<endl;
									}
								
								//new Leaf Node required
								else if(YOperatorFlag==3 || YOperatorFlag==5)
									{
									Node* newLeafNode=new Node();
									(*newLeafNode).LeafChildFlag=1;
									(*newLeafNode).NumberOFChild=0;
									(*newLeafNode).EmptyNumberOfChildRequired=m;
									//Put '0' in first ValArray's Position because we will have Child Pointers as '1' more than the ValArray size
									(*newLeafNode).ValArray.push_back(0);
								
									(*newLeafNode).NodeBufferSize=0;
									(*newLeafNode).ChildFullFlag=0;
									(*newLeafNode).NodeBufferFullFlag=0;
									(*newLeafNode).Parent=NULL;
									(*newLeafNode).ValArraySize=1;  // '1' for intially pushed '0' into the ValArray
									(*newLeafNode).Level=1; // Always 1 for Leaf Node
									(*newLeafNode).YTreeRoot=NULL;
									std::cout<<endl<<"Creating new Leaves...";
								
									//Adding new Leaf to all Leaf Pointer
									//if(i<12)
									{
									for(long long int x=0;x<m;x++)
										{
											//std::cout<<endl<<"Line 1...";
											Leaf* newLeaf=new Leaf();
											//std::cout<<endl<<"Line 2...";
										
											(*newLeaf).FullFlag=0; //Initially, Leaf is Empty
											(*newLeaf).Size=0;
											(*newLeafNode).NodeChild[x]=NULL;
											(*newLeafNode).LeafChild[x]=newLeaf;
											(*newLeafNode).EmptyNumberOfChildRequired--;
											//std::cout<<endl<<"Line 5...";
										
											//std::cout<<endl<<"LeafChild: "<<x<<": "<<(*newLeafNode).LeafChild[x];
										}
									}
									// New LeafNode have 'm' number of Leaves created as its child in the above Loop
									// This will help while spliting the Nodes to hold the count of number of children
									//(*newLeafNode).NumberOFChild=0;
									std::cout<<endl<<"Leaf Node sent="<<newLeafNode<<endl;
									// Sending new Leaf node with leaves attached to it.

									if(YOperatorFlag==3)
											YOperatorFlag=(*YTree).Operate(newLeafNode,3);
									else if(YOperatorFlag==5)
											YOperatorFlag=(*YTree).Operate(newLeafNode,5);
								
									}
								}

							}
							catch(const std::exception& ex)
							{
								std::cout<<endl<<"Exception caught: "<<ex.what()<<endl;
							}
							//Empty ManageBuffer in the Class to get ready to take next set of input elements
							(*YTree).EmptyManageBuffer();
							}
					}
				i++; //Next Input iteration
				} //For loop

				} //While loop
			Input.close();

			(*YTree).Operate(NULL,6);
			(*YTree).Call(2);
			(*YTree).Call(1);
			//(*YTree).Call(3);
		
			
			}//If condition for File open

			else
				{
					std::cout <<endl<< "Unable to open file" << std::endl << std::endl;
					return 0;
				}

		YTreeFlag=MyTree.YConstructPreorderTraversal((*YTree).Root,0);
		NumberOFYTrees++;
		}
	}
	catch(const std::exception& ex)
		{
		std::cout<<endl<<"Exception caught: "<<ex.what()<<endl;
		}
	std:cout << (stxxl::stats_data(*Stats) - stats_begin);
	std::cout<<endl<< "Number Of Y Trees="<<NumberOFYTrees<<endl;
	

	//Search in 2 Dimension
	MyTree.Call(4);
	std::cout<<"N="<<N<<" M="<<M<<" B="<<B;

	return 0;
}

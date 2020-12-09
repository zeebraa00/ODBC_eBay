#define NUM_KEYS 10
#define ORDER 11 
#include <iostream>

using namespace std;

enum NodeType {
	ROOT,
	INTERNAL,
	LEAF
};

class BTreeNode{   
	protected:
		NodeType type;
   	public:
		BTreeNode() {};
		virtual ~BTreeNode() {}
		NodeType getNodeType() {return type;}

		int length;
		void setFirstNode(BTreeNode* node, long long value, BTreeNode *root);
		void setChildNode(BTreeNode* node, long long value, BTreeNode *root, int idx);
		void setCondChildNode(BTreeNode* node, long long value, int num);
		void setTypeRoot() {this -> type = ROOT;}
		void setTypeInternal() {this -> type = INTERNAL;}
		void setTypeLeaf() {this -> type = LEAF;}
		int isLengthZero();
		virtual BTreeNode* getChild(int index){}
		virtual long long getKey(int index){}
		virtual void setChild(int index, BTreeNode* node){}
		virtual void setKey(int index, long long value){}
};

class BTreeInternalNode:public BTreeNode{
   	private:
		long long keys[NUM_KEYS];
		BTreeNode* child[ORDER];
   	public:
		BTreeInternalNode() { this->type = INTERNAL; };
		~BTreeInternalNode() {};
		void setChild(int index, BTreeNode* node) {this->child[index] = node;} 
		BTreeNode* getChild(int index) {return child[index];}
		void setKey(int index, long long value) {this->keys[index] = value;}
		long long getKey(int index) {return keys[index];}
};

class BTreeLeafNode:public BTreeNode{
   	private:
		long long keys[NUM_KEYS];
		BTreeLeafNode* right_sibling;
   	public:
		BTreeLeafNode() {{ this->type = LEAF; }};
		~BTreeLeafNode() {};
		void printLeafNode() {}; // print all keys in the current leaf node, separated by comma.
		long long getKey(int index) {return keys[index];}
		void setKey(int index, long long value) {keys[index] = value;}
};

class BTree{  
    private:
		BTreeNode *root;
    public:
		BTree() { root = new BTreeLeafNode(); root -> setTypeRoot(); }; 
		~BTree() {}; 
		void insert(long long value);
		//	void remove(long long value);
		void printLeafNode(long long value);
		// find the leaf node that contains 'value' and print all values in the leaf node.
		void pointQuery(long long value); 
		// print the found value or "NOT FOUND" if there is no value in the index
		void rangeQuery(long long low, long long high); 
		// print all found keys (low <= keys < high), separated by comma (e.g., 10, 11, 13, 15\n)
		void putInternal(long long value, BTreeNode* node, BTreeInternalNode* child);
		BTreeNode* getParent(BTreeNode* node, BTreeNode* child);
};
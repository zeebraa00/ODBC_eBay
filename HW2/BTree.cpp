#include <iostream>
#include <fstream>
#include "BTree.h"

using namespace std;

int BTreeNode::isLengthZero() {
	if (length == 0) {
		return 1;
	} else {
		return 0;
	}
}

void quickSort(long long *data, int start, int end) {
	if(start >= end){
        // 원소가 1개인 경우
        return; 
    }
    
    int pivot = start;
    int i = pivot + 1; // 왼쪽 출발 지점 
    int j = end; // 오른쪽 출발 지점
    int temp;
    
    while(i <= j){
        // 포인터가 엇갈릴때까지 반복
        while(i <= end && data[i] <= data[pivot]){
            i++;
        }
        while(j > start && data[j] >= data[pivot]){
            j--;
        }
        
        if(i > j){
            // 엇갈림
            temp = data[j];
            data[j] = data[pivot];
            data[pivot] = temp;
        }else{
            // i번째와 j번째를 스왑
            temp = data[i];
            data[i] = data[j];
            data[j] = temp;
        }
    } 
    
    // 분할 계산
    quickSort(data, start, j - 1);
    quickSort(data, j + 1, end);
};

int arr_length;
long long arr[100000];

void rangeCheck(long long low, long long high) {
	bool flag = false;
	for (int i=0; i < arr_length; i++) {
		if (low <= arr[i] && arr[i] < high) {
			if (!flag) {
				flag = true;
			} else {
				cout << "," << arr[i];
				continue;
			}
			cout << arr[i];
		}
	}
}

void printNode(BTreeNode* node) {
	bool flag = false;
	for (int i=0; i<node->length; i++)  {
		if (!flag) {
			cout << node->getKey(i);
			flag = true;
		} else {
			cout << "," << node->getKey(i);
		}
	}
}

int getIndex(BTreeNode* node, long long value) {
	int index=0;

	while (true) {
		if (value < node->getKey(index) || index >= node->length  ) {
			break;
		}
		index++;	
	}
	
	return index;
}

void pointCheck(BTreeNode* node, long long value) {
	bool flag = false;
	for (int a=0; a < node->length; a++) {
		if (value == node->getKey(a)) {
			cout  << value << "\n";
			flag = true;
			break;
		}
	}
	if (!flag) {
		cout << "NOT FOUND\n";
	}
}

void BTree::insert(long long value) {
	arr[arr_length] = value;
	arr_length++;
	long long temp[ORDER];

	if (root->isLengthZero()) {
		// 처음이면 루트노드임
		BTreeNode* node = new BTreeNode();
		// node->setFirstNode(node, value, root);
		node = root;
		node->setKey(node->length, value);
		node->length++;
		node->setTypeLeaf();
		root = node;
	} else {
		// 처음 아니면 루트노드 아님
		BTreeNode* node = root;
		NodeType type = node->getNodeType();

		BTreeNode* parent;
		
		while (type != LEAF) {
			// 리프노드가 아니라면
			parent = node; // 부모노드 기억해주고
			for (int i=0; i<node->length; i++) {
				// node의 길이만큼 반복
				if ( i==0 ) {
					if (value < node->getKey(0)) {
						node = node->getChild(0);
						break;
					}

					if (i == node->length-1) {
						// 키 갯수만큼 검사했는데 안나오면 자동으로 맨 마지막
						node = node->getChild(i+1);
						break;
					}
				} else {
					if (value < node->getKey(i)  && value >= node->getKey(i-1) ) {
						node = node->getChild(i);
						break;
					}

					if (i == node->length-1) {
						// 키 갯수만큼 검사했는데 안나오면 자동으로 맨 마지막
						node = node->getChild(i+1);
						break;
					}
				}
			}
			type = node->getNodeType();
		}

		if (node->length < NUM_KEYS) {
			int idx = 0;
			int len = node->length;

			while (true) {
				if (value < node->getKey(idx) || idx >= node->length) {
					break;
				}
				idx++;			
			}

			for (int j = len; j>idx; j--) {
				node->setKey(j, node->getKey(j-1)); 	
			}

			node->length++;
			node->setKey(idx, value);

		} else {
			BTreeInternalNode* newnode = new BTreeInternalNode();
			newnode->setTypeLeaf();
			

			for(int a=0; a< NUM_KEYS; a++) {
				temp[a] = node->getKey(a);
			}

			temp[NUM_KEYS] = value;

			quickSort(temp,0,ORDER-1);


			for (int a =0; a<6; a++) {
				node->setKey(a, temp[a]);
			}
            node->length=6;

			for (int a=0; a<5; a++) {
				newnode->setKey(a, temp[a+6]);
			}
            newnode->length=5;

			if (node == root) {
				BTreeInternalNode* newRoot = new BTreeInternalNode();
				newRoot->length=1;
				newRoot->setKey(0, newnode->getKey(0));
				// 0이면 왼쪽으로 가고
				newRoot->setChild(0, node);
				// 1이면 오른쪽으로 가고
				newRoot->setChild(1, newnode);
				newRoot->setTypeRoot();
				root = newRoot;
			} else {
				putInternal(newnode->getKey(0), parent, newnode);
			}
		}
	}
}

BTreeNode* BTree::getParent(BTreeNode* node, BTreeNode* child) {
	BTreeNode* parent;
	if ( (node->getChild(0))->getNodeType() == LEAF) {
		return NULL;
	}

	if (node->getNodeType() == LEAF) {
		return NULL;
	}

	for (int i =0; i< node->length+1; i++) {
		if (node->getChild(i) != child) {
			parent = getParent(node->getChild(i), child);
			if (parent == NULL) {
				continue;
			} else {
				return parent;
			}
		} else {
			parent = node;
			return parent;	
		}
	}
	return parent;
}

void BTree::putInternal(long long value, BTreeNode* node, BTreeInternalNode* child) {
	int len = node->length;
	long long tmp[ORDER];

	if (NUM_KEYS <= len) {
		BTreeInternalNode* internal = new BTreeInternalNode();
		internal->setTypeInternal();
		
		for (int a=0; a< ORDER-1; a++) {
			tmp[a] = node->getKey(a);
		}
		tmp[ORDER-1] = value;

		quickSort(tmp,0,ORDER-1);
		BTreeNode* arr[ORDER+1];
	
		for (int i =0; i< ORDER; i++) {
			arr[i]=node->getChild(i); 
		}

		int idx = 0;
		while (true) {
			if (value < tmp[idx] || idx >= NUM_KEYS) {
				break;
			}
			idx++;			
		}

		for (int i=ORDER; i> idx; i--) {
			arr[i+1] = node->getChild(i);
		}
		arr[idx+1] = child;
		
		node->length=5;
		internal->length=5;

		for (int i=0, mid=6; i<6; i++, mid++) {
			if (i==6) {
				internal->setChild(i, arr[mid]);
			} else {
				internal->setKey(i, tmp[mid]);
				internal->setChild(i, arr[mid]);
			}
		}
		
		if (node == root) {
			BTreeInternalNode* newRoot = new BTreeInternalNode();
			newRoot->length=1;
			newRoot->setTypeRoot();
			newRoot->setChild(0, node);
			newRoot->setChild(1, internal);
			newRoot->setKey(0, node->getKey(node->length));
			root = newRoot;
		} else {
			putInternal(node->getKey(node->length), getParent(root, node), internal);
		} 
	} else {
		int idx = 0;
		int len = node->length;

		while (true) {
			if (value < node->getKey(idx) || idx >= node->length) {
				break;
			}
			idx++;			
		}

		for (int i=idx; i<len; i++) {
			node->setKey(i+1, node->getKey(i));
			node->setChild(i+2, node->getChild(i+1));
		}
		
		node->setKey(idx, value);
		node->setChild(idx+1, child);
		node->length++;
	}
}	

void BTree::printLeafNode(long long value) {
	BTreeNode* node = root;
	BTreeNode* parent;

	while (node->getNodeType() != LEAF) {
		int len = node->length;
		parent = node;

		for (int i=0; i<len; i++) {
			if (value < node->getKey(i)) {
				node = node->getChild(i);
				break;
			}
			if (i == len-1) {
				node = node->getChild(i+1);
				break;
			}
		}
	}
	printNode(node);
	cout << "\n";
}

void BTree::rangeQuery(long long low, long long high) {
	quickSort(arr,0,arr_length-1);
	rangeCheck(low, high);
	cout << "\n";
}

void BTree::pointQuery(long long value) {
	BTreeNode* node = root;

	while (node->getNodeType() != LEAF) {
		for (int i=0; i < node->length; i++) {
			if (value < node->getKey(i)) {
				node = node->getChild(i);
				break;
			}
			if (i == node->length-1) {
				node = node->getChild(i+1);
				break;
			}
		}
	}

	pointCheck(node, value);
}
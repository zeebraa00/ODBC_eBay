#include <iostream>
#include <fstream>
#include "BTree.h"

using namespace std;

int main(int argc, char* argv[]) {

  BTree bt;
  char command;
  long long value, value2;

  ifstream input(argv[1]);
  bool flag = true;

  while(flag){
	input >> command;

	switch(command){
		case 'i':
       			input >> value;
			bt.insert(value);
			break;
		case 's':
       			input >> value;
			bt.pointQuery(value);
			break;
		case 'r':
       			input >> value >> value2;
			bt.rangeQuery(value, value2);
			break;
//		case 'd':
//       		input >> value;
//			bt.remove(value);
//			break;
		case 'p':
       			input >> value;
			bt.printLeafNode(value);
			break;
		case 'q':
			flag = false;
			break;
	}
  }

  return 0;
}

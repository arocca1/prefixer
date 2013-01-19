#include <iostream>
#include <fstream>
#include <stack>

bool isANumber(char * charArray) {
	bool isNum = false;

	//must check each digit to determine if it is a number
	for (int i = 0 ; i < sizeof(charArray) ; ++i) {
		if (charArray[i] != 0) {
			if (isdigit(charArray[i])) {
				isNum = true;
			}
			else {
				//exit the loop nicely
				i = sizeof(charArray);
			}
		}
		else {
			//exit the loop nicely
			i = sizeof(charArray);
		}
	}

	return isNum;
}

bool isAVar(char* c) {
	//restricting the variable to one letter so the rest of the array should be empty
	return (c[1] == 0) && ((c[0] >= 'a' && c[0] <= 'z') || (c[0] >= 'A' && c[0] <= 'Z'));
}

bool isAnOpeningBracket(char* c) {
	return (c[1] == 0) && (c[0] == '(');
}

bool isAClosingBracket(char* c) {
	return (c[1] == 0) && (c[0] == ')');
}

bool isMultiplyOrDivide(char* c) {
	return (c[1] == 0) && (c[0] == '*' || c[0] == '/');
}

bool isMultiply(char c) {
	return c == '*';
}

bool isDivide(char c) {
	return c == '/';
}

bool isAdd(char c) {
	return c == '+';
}

bool isSubtract(char c) {
	return c == '-';
}

bool isAddOrSubtract(char* c) {
	return (c[1] == 0) && (c[0] == '+' || c[0] == '-');
}

int precedence(char* c) {
	if (isAnOpeningBracket(c) || isAClosingBracket(c)) {
		return 3;
	}
	else if (isMultiplyOrDivide(c)) {
		return 2;
	}
	else if (isAddOrSubtract(c)) {
		return 1;
	}

	return 0;
}

#include "Node.h"

using namespace std;

//allowing for extremely large integers
char* nextChar = (char *)calloc(100, sizeof(char));
//used to determine if all brackets match
int numBrackets = 0;
//ifstream is used because writing to the file is not required
ifstream file;
//use a stack for parsed elements and nodes to build parse tree
stack<Node*> nodeStack;

void getNextChar() {
	//read the next input from the file
	file >> nextChar;
}

void checkChar() {
	if (isANumber(nextChar)) {
		Node* num = new Node(nextChar, "num");
		nodeStack.push(num);
	}
	else if (isAVar(nextChar)) {
		//get the first character of the string and free next char to save memory
		char* varChar = (char *)malloc(sizeof(char));
		*varChar = 0;
		strcpy(varChar, nextChar);
		free(nextChar);
		//coefficient is 1 and exponent is 1 for the variable
		//casting of the objects is to ensure they are passed as the correct type because of the other constructor
		Node* var = new Node(varChar, 1, 1);
		nodeStack.push(var);
	}
	else if (isAnOpeningBracket(nextChar)) {
		/*don't need to push on stack because it doesn't matter.
		just check for new expressions within*/
		++numBrackets;
		getNextChar();
		checkChar();

		if (file.good()) {
			getNextChar();
			checkChar();
		}
	}
	else if (isAClosingBracket(nextChar)) {
		--numBrackets;
		return;
	}
	else if (isMultiplyOrDivide(nextChar)) {
		//get the first character of the string and free next char to save memory
		char* multOrDivChar = (char *)malloc(sizeof(char));
		*multOrDivChar = 0;
		strcpy(multOrDivChar, nextChar);
		free(nextChar);
		nextChar = (char *)calloc(100, sizeof(char));
		//get the next operand
		getNextChar();
		checkChar();

		//the objects on top of the stack are swapped simply to keep the order of the original expression
		Node* rightObject = (Node*)nodeStack.top();
		nodeStack.pop();
		Node* leftObject = (Node*)nodeStack.top();
		nodeStack.pop();

		Node* multOrDivExpr = new Node(multOrDivChar, "operator", leftObject, rightObject);
		nodeStack.push(multOrDivExpr);

		/*ensure that precedence of these operators is always maintained.
		this is important for a case such as 1 + 3 * ( 5 + 2 ) / 3.
		the first addition will be consumed before the final division which is incorrect.*/
		if (file.good()) {
			getNextChar();
			checkChar();
		}
	}
	else if (isAddOrSubtract(nextChar)) {
		//get the first character of the string and free next char to save memory
		char* addOrSubChar = (char *)malloc(sizeof(char));
		*addOrSubChar = 0;
		strcpy(addOrSubChar, nextChar);
		free(nextChar);
		nextChar = (char *)calloc(100, sizeof(char));
		//get the next operand
		getNextChar();
		checkChar();

		//check if there is another operator after the operand with higher precedence
		if (file.good()) {
			getNextChar();
			checkChar();
		}

		//the objects on top of the stack are swapped simply to keep the order of the original expression
		Node* rightObject = (Node*)nodeStack.top();
		nodeStack.pop();		
		Node* leftObject = (Node*)nodeStack.top();
		nodeStack.pop();

		Node* addOrSubExpr = new Node(addOrSubChar, "operator", leftObject, rightObject);
		nodeStack.push(addOrSubExpr);
	}
	nextChar = (char *)calloc(100, sizeof(char));
}

void handleInfixExpression(char* fileName) {
	file.open(fileName);

	if (file.is_open())
	{
		while ( file.good() )
		{
			getNextChar();
			checkChar();
		}
		//although not necessary, the file is closed after there is no more valid input
		file.close();
	}
}

void printPrefixExpression(bool simplify) {
	Node* root = (Node*)nodeStack.top();
	nodeStack.pop();

	//the root node should be the only object on the stack
	if (nodeStack.empty()) {
		if (simplify) {
			cout << root->simplify() << endl;
		}
		else {
			cout << root->print() << endl;
		}
	}
}

int main(int argc, char* argv[]) {
	//the incorrect number of arguments has been supplied
	if (argc < 2 || argc > 3)
	{
		cout << "Sorry, but you haven't entered the required number of arguments." << endl;
		cout << "Exiting now ..." << endl;
		return 0;
	}

	char* fileName = (argc == 3) ? argv[2] : argv[1];
	bool simplifyExpression = false;
	bool incorrectFlag = false;

	if (argc == 3) {
		if (argv[1][0] == '-' && argv[1][1] == 'r' && argv[1][2] == 0) {
			simplifyExpression = true;
		}
		else {
			incorrectFlag = true;
		}
	}
	
	cout << "Welcome to the coolest prefixer ever!" << endl;
	cout << "The expression contained in " << fileName << " will be put into prefix notation" << endl;

	if (incorrectFlag) {
		cout << "The incorrect flag has been passed. It will be ignored" << endl;
	}

	handleInfixExpression(fileName);

	//each opening bracket must have a corresponding closed bracket
	if (!numBrackets) {
		printPrefixExpression(simplifyExpression);
	}
	else {
		cout << "Please check your expression because all brackets do not match correctly" << endl;
	}
	
	return 0;
}

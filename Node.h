class Node {
	private:
		Node* left;
		Node* right;
		char* type;
		char* value;
		/*important for simplification where a variable can have a coefficient and exponent.
		not necessary for other objects*/
		int varCoeff;
		int varExp;
		
	public:
		Node(char* nodeValue, char* nodeType);
		Node(char* nodeValue, int coeff, int exp);
		Node(char* nodeValue, char* nodeType, Node* leftOperand, Node* rightOperand);
		~Node();
		Node* getLeft();
		void setLeft(Node* newLeft);
		Node* getRight();
		void setRight(Node* newRight);
		char* getType();
		char* getValue();
		void setValue(char* c);
		int getCoeff();
		void setCoeff(int coeff);
		int getExp();
		void setExp(int exp);
		char* print();
		char* printExpression(char* leftOutput, char* rightOutput);
		char* printExpression(char* leftOutput, char* rightOutput, char* nodeValue);
		char* simplify();
		Node* simplifyChildren();
		Node* simplifySelf();
		Node* simplifySelfLeftNum();
		Node* simplifySelfRightNum();
		Node* simplifySelfLeftVar();
		Node* simplifySelfRightVar();
		Node* checkAndPerformOperationInt(int leftInt, int rightInt, char* op);
		Node* checkAndPerformOperationVarOnLeft(Node* intObj, Node* var, char* operation);
		Node* checkAndPerformOperationVarOnRight(Node* intObj, Node* var, char* operation);
		Node* checkAndPerformOperationVars(Node* leftVar, Node* rightVar, char* operation);
};

Node::Node(char* nodeValue, char* nodeType) {
	value = nodeValue;
	type = nodeType;
	left = 0;
	right = 0;
}

Node::Node(char* nodeValue, int coeff, int exp) {
	value = nodeValue;
	type = "var";
	varCoeff = coeff;
	varExp = exp;
	left = 0;
	right = 0;
}

Node::Node(char* nodeValue, char* nodeType, Node* leftOperand, Node* rightOperand) {
	value = nodeValue;
	type = nodeType;
	left = leftOperand;
	right = rightOperand;
}

Node::~Node() {
	if (left != 0) {
		delete left;
		left = 0;
	}
	
	if (right != 0) {
		delete right;
		right = 0;
	}
}

Node* Node::getLeft() {
	return left;
}

void Node::setLeft(Node* newLeft) {
	left = newLeft;
}

Node* Node::getRight() {
	return right;
}

void Node::setRight(Node* newRight) {
	right = newRight;
}

char* Node::getType() {
	//3 possible types: num, operator, var
	return type;
}

char* Node::getValue() {
	return value;
}

void Node::setValue(char* c) {
	if (c[1] == 0) {
		value = c;
	}
}

int Node::getCoeff() {
	return varCoeff;
}

void Node::setCoeff(int coeff) {
	varCoeff = coeff;
}

int Node::getExp() {
	return varExp;
}

void Node::setExp(int exp) {
	varExp = exp;
}

char* Node::print() {
	if (type == "operator") {
		char* leftOutput = left->print();
		char* rightOutput = right->print();
		return this->printExpression(leftOutput, rightOutput);
	}
	else if (type == "var") {
		if (varExp == 1) {
			if (varCoeff == 1) {
				return value;
			}
			else {
				char* coeff = (char *)malloc(sizeof(varCoeff)/sizeof(int));
				sprintf(coeff, "%d", varCoeff);
				return this->printExpression(coeff, value, "*");
			}
		}
		else {
			char* exp = (char *)malloc(sizeof(varExp)/sizeof(int));
			sprintf(exp, "%d", varExp);
			char* varWithExp = (char *)malloc(sizeof(value) + 1 + sizeof(exp));
			char* expSymbol = "^";
			strcpy(varWithExp, value);
			strcat(varWithExp, expSymbol);
			strcat(varWithExp, exp);

			if (varCoeff == 1) {				
				return varWithExp;
			}
			else {
				char* coeff = (char *)malloc(sizeof(varCoeff)/sizeof(int));
				sprintf(coeff, "%d", varCoeff);
				return this->printExpression(coeff, varWithExp, "*");
			}
		}
	}
	else {
		return value;
	}
}

char* Node::printExpression(char* leftOutput, char* rightOutput) {
	return this->printExpression(leftOutput, rightOutput, value);
}

char* Node::printExpression(char* leftOutput, char* rightOutput, char* nodeValue) {
	char* output;
	char* openingBracket = "(";
	char* closingBracket = ")";
	char* space = " ";

	output = (char *)malloc(sizeof(leftOutput) + sizeof(rightOutput) + sizeof(openingBracket) 
			+ sizeof(closingBracket) + 2 * sizeof(space));
	strcpy(output, openingBracket);
	strcat(output, nodeValue);
	strcat(output, space);
	strcat(output, leftOutput);
	strcat(output, space);
	strcat(output, rightOutput);
	strcat(output, closingBracket);
	return output;
}

char* Node::simplify() {
	Node* simplifiedSelf = this->simplifyChildren();
	return simplifiedSelf->print();
}

Node* Node::simplifyChildren() {
	if (left != 0) {
		left = left->simplifyChildren();
	}
	if (right != 0) {
		right = right->simplifyChildren();
	}
	return this->simplifySelf();
}

Node* Node::simplifySelf() {
	if (left != 0 && right != 0) {
		if (left->getType() == "num") {
			return this->simplifySelfLeftNum();
		}
		//the left is not a number, but the right is
		else if (right->getType() == "num") {
			//the same checks as left, except for num on both sides
			return this->simplifySelfRightNum();
		}
		else if (left->getType() == "var") {
			return this->simplifySelfLeftVar();
		}
		//the left is not a var, but the right is
		else if (right->getType() == "var") {
			return this->simplifySelfRightVar();
		}
	}

	/*don't have to check for num and var because num is either the coefficient of var or being added/subtracted to/from var
	which can't be simplified*/
	//can't simplify
	return this;
}

Node* Node::simplifySelfLeftNum() {
	int leftInt = atoi(left->getValue());

	if (right->getType() == "num") {
		int rightInt = atoi(right->getValue());
		Node* newNum = checkAndPerformOperationInt(leftInt, rightInt, value);
		//should be deleting this
		//delete this;
		return newNum;
	}
	else if (right->getType() == "var") {
		if (isMultiplyOrDivide(value)) {
			char* op = (char *)malloc(sizeof(char));
			strcpy(op, value);
			return checkAndPerformOperationVarOnRight(left, right, op);
		}
	}
	/*must check if it is an operator that can be simplified.
	6 + 7 - x can definitely be simplified, for example */
	else if (right->getType() == "operator") {
		//must check precedence of the operator before accessing its children
		if (precedence(value) >= precedence(right->getValue())) {
			if (right->getLeft()->getType() == "num" && right->getRight()->getType() == "var") {
				int rightInt = atoi(right->getLeft()->getValue());
				Node* leftObject = checkAndPerformOperationInt(leftInt, rightInt, value);
				Node* rightObject;
				char* op = (char *)malloc(sizeof(char));
				//figure out what the operators are
				if (isMultiplyOrDivide(value)) {
					if (isMultiplyOrDivide(right->getValue())) {
						if (value == right->getValue()) {
							//change the coefficient of the var or keep an expression depending on the operation
							op = value;
						}
						else {
							//no matter what the order of these operations, the left num is dividing the variable
							op = "/";
						}
						rightObject = checkAndPerformOperationVarOnRight(leftObject, right->getRight(), op);
					}
					//right operation is addition or subtraction (no other possibilities)
					else {
						//value will be either multiplication or division which means it applies to both terms
						rightObject = checkAndPerformOperationVarOnRight(left, right->getRight(), value);
						/*should be deleting these nodes
						delete left;
						right->setRight(0);
						delete right;*/
						return new Node(right->getValue(), "operator", leftObject, rightObject);
					}
				}
				//they are both addition/subtraction operations
				else {
					//both operations are the same so the var on the right will be added
					if (strcmp(value, right->getValue()) == 0) {
						op = "+";
					}
					//the operations are different so the var on the right will be subtracted
					else {
						op = "-";
					}
					rightObject = checkAndPerformOperationVarOnRight(leftObject, right->getRight(), op);
				}

				/*should be deleting these nodes
				delete left;
				right->setRight(0);
				delete right;*/
				return rightObject;
			}
			else if (right->getRight()->getType() == "num" && right->getLeft()->getType() == "var") {
				int rightInt = atoi(right->getRight()->getValue());
				Node* leftObject = checkAndPerformOperationInt(leftInt, rightInt, value);
				Node* rightObject;
				char* op = (char *)malloc(sizeof(char));
				//figure out what the operators are
				if (isMultiplyOrDivide(value)) {
					if (isMultiplyOrDivide(right->getValue())) {
						if (value == right->getValue()) {
							//change the coefficient of the var or keep an expression depending on the operation
							op = value;
						}
						else {
							//no matter what the order of these operations, the left num is dividing the variable
							op = "/";
						}
						rightObject = checkAndPerformOperationVarOnRight(leftObject, right->getLeft(), op);
					}
					//right is addition or subtraction (no other possibilities)
					else {
						//value will be either multiplication or division which means it applies to both terms
						rightObject = checkAndPerformOperationVarOnRight(left, right->getLeft(), value);
						/*should be deleting these nodes
						delete left;
						right->setLeft(0);
						delete right;*/
						return new Node(right->getValue(), "operator", leftObject, rightObject);
					}
				}
				//both operations are addition or subtraction
				else {
					//both operations are the same so the var on the right will be added
					if (strcmp(value, right->getValue()) == 0) {
						op = "+";
					}
					//the operations are different so the var on the right will be subtracted
					else {
						op = "-";
					}
					rightObject = checkAndPerformOperationVarOnRight(leftObject, right->getLeft(), op);
				}

				/*should be deleting these nodes
				delete left;
				right->setLeft(0);
				delete right;*/
				return rightObject;
			}
			/*if both sides are both numbers but the operation would reduce to a value between 0 and 1 through division.
			multiplication would simplify the operation to a number*/
			else if (right->getLeft()->getType() == "num" && right->getRight()->getType() == "num" && isDivide(right->getValue()[0])) {
				Node* finalObj;
				if (isMultiply(value[0])) {
					int rightInt = atoi(right->getLeft()->getValue());
					Node* result = checkAndPerformOperationInt(leftInt, rightInt, value);
					int resultInt = atoi(result->getValue());
					int otherInt = atoi(right->getRight()->getValue());
					finalObj = checkAndPerformOperationInt(resultInt, otherInt, right->getValue());
					return finalObj;
				}
				//is division
				else {
					char* mult = "*";
					int rightInt = atoi(right->getRight()->getValue());
					Node* result = checkAndPerformOperationInt(leftInt, rightInt, mult);
					int resultInt = atoi(result->getValue());
					int otherInt = atoi(right->getLeft()->getValue());
					free(mult);
					finalObj = checkAndPerformOperationInt(resultInt, otherInt, right->getValue());
				}

				/*should be deleting this
				delete this;*/
				return finalObj;
			}
		}
	}

	//if left can't be simplified
	return this;
}

Node* Node::simplifySelfRightNum() {
	int rightInt = atoi(right->getValue());

	if (left->getType() == "var") {
		if (isMultiplyOrDivide(value)) {
			char* op = (char *)malloc(sizeof(char));
			strcpy(op, value);
			return checkAndPerformOperationVarOnLeft(right, left, op);
		}
	}
	/*must check if it is an operator that can be simplified.
	(7 - x) + 6 can definitely be simplified, for example */
	else if (left->getType() == "operator") {
		//must check precedence of the operator before accessing its children
		if (precedence(value) >= precedence(left->getValue())) {
			if (left->getLeft()->getType() == "num" && left->getRight()->getType() == "var") {
				int leftInt = atoi(left->getLeft()->getValue());
				Node* leftObject = checkAndPerformOperationInt(leftInt, rightInt, value);
				Node* rightObject;
				char* op = (char *)malloc(sizeof(char));
				//figure out what the operators are
				if (isMultiplyOrDivide(value)) {
					if (isMultiplyOrDivide(left->getValue())) {
						if (value == left->getValue()) {
							//change the coefficient of the var or keep an expression depending on the operation
							strcpy(op, value);
						}
						else {
							//no matter what the order of these operations, the left num is dividing the variable
							op = "/";
						}
						rightObject = checkAndPerformOperationVarOnLeft(leftObject, left->getRight(), op);
					}
					//left operation is addition or subtraction (no other possibilities)
					else {
						//value will be either multiplication or division which means it applies to both terms
						rightObject = checkAndPerformOperationVarOnLeft(right, left->getRight(), value);
						/*should be deleting these nodes
						delete left;
						right->setRight(0);
						delete right;*/
						return new Node(left->getValue(), "operator", leftObject, rightObject);
					}
				}
				//they are both addition/subtraction operations
				else {
					//both operations are the same so the var on the right will be added
					if (strcmp(value, left->getValue()) == 0) {
						op = "+";
					}
					//the operations are different so the var on the right will be subtracted
					else {
						op = "-";
					}
					rightObject = checkAndPerformOperationVarOnRight(leftObject, left->getRight(), op);
				}

				/*should be deleting these nodes
				delete left;
				right->setRight(0);
				delete right;*/
				return rightObject;
			}
			else if (left->getRight()->getType() == "num" && left->getLeft()->getType() == "var") {
				int leftInt = atoi(left->getRight()->getValue());
				Node* rightObject;
				Node* leftObject;
				char* op = (char *)malloc(sizeof(char));
				//figure out what the operators are
				if (isMultiply(value[0])) {
					if (isMultiply(left->getValue()[0])) {
						strcpy(op, value);
						rightObject = checkAndPerformOperationInt(leftInt, rightInt, op);
						char* mult = (char *)malloc(sizeof(char));
						strcpy(mult, left->getValue());
						leftObject = checkAndPerformOperationVarOnLeft(rightObject, left->getLeft(), mult);
						//delete rightObject;
						//the var has its coefficient set to the value of the constant in rightObject
						return leftObject;
					}
					else if (isDivide(left->getValue()[0])) {
						strcpy(op, left->getValue());
						leftObject = checkAndPerformOperationVarOnLeft(left->getRight(), left->getLeft(), op);
						char* mult = (char *)malloc(sizeof(char));
						strcpy(mult, value);
						rightObject = checkAndPerformOperationVarOnLeft(right, leftObject, mult);
						//delete right;
						/* there could be a case where the var in this simplified object didn't have a coefficient large enough
						to integer divide the left number, but by multiplying the right num, it does. ex/ (x / 7) * 7 */
						return rightObject->simplifySelf();
					}
					//addition/subtraction of left expression
					else {
						strcpy(op, value);
						char* mult = (char *)malloc(sizeof(char));
						strcpy(mult, value);
						//value will be either multiplication or division which means it applies to both terms
						rightObject = checkAndPerformOperationInt(leftInt, rightInt, op);
						leftObject = checkAndPerformOperationVarOnLeft(rightObject, right->getLeft(), mult);
						char* finalOp = (char *)malloc(sizeof(char));
						strcpy(finalOp, left->getValue());
						//DELETE UNNECESSARY OBJECTS
						return new Node(finalOp, "operator", leftObject, rightObject);
					}
				}
				else if (isDivide(value[0])) {
					if (isMultiply(left->getValue()[0])) {
						//simplify var with new coefficient
						leftObject = checkAndPerformOperationVarOnLeft(left->getRight(), left->getLeft(), left->getValue());
						strcpy(op, value);
						//not guaranteed simplification to a var with this division operation
						rightObject = checkAndPerformOperationVarOnLeft(right, leftObject, op);
						return rightObject;
					}
					else if (isDivide(left->getValue()[0])) {
						//this operation involves dividing the variable by both of the numbers so group the numbers
						rightObject = checkAndPerformOperationInt(leftInt, rightInt, "*");
						strcpy(op, value);
						leftObject = checkAndPerformOperationVarOnLeft(rightObject, left->getLeft(), op);
						return leftObject;
					}
					else {
						strcpy(op, value);
						char* div = (char *)malloc(sizeof(char));
						strcpy(div, value);
						//value will be either multiplication or division which means it applies to both terms
						rightObject = checkAndPerformOperationInt(leftInt, rightInt, op);
						leftObject = checkAndPerformOperationVarOnLeft(rightObject, right->getLeft(), div);
						char* finalOp = (char *)malloc(sizeof(char));
						strcpy(finalOp, left->getValue());
						//DELETE UNNECESSARY OBJECTS
						return new Node(finalOp, "operator", leftObject, rightObject);
					}
				}
				//both operations are addition/subtraction
				else {
					char* finalOp;
					//both operations are the same so the var on the right will be added
					if (strcmp(value, right->getValue()) == 0) {
						op = "+";
						rightObject = checkAndPerformOperationInt(leftInt, rightInt, op);
						finalOp = (char *)malloc(sizeof(char));
						strcpy(finalOp, left->getValue());
					}
					//the operations are different
					else {
						op = "-";
						if (isAdd(value[0])) {
							rightObject = checkAndPerformOperationInt(rightInt, leftInt, op);
						}
						else {
							rightObject = checkAndPerformOperationInt(leftInt, rightInt, op);
						}
						finalOp = "+";
					}
					return checkAndPerformOperationVarOnLeft(rightObject, left->getLeft(), finalOp);
				}

				/*should be deleting these nodes
				delete left;
				right->setLeft(0);
				delete right;*/
				return this;
			}
			/*if both sides are both numbers but the operation would reduce to a value between 0 and 1 through division.
			multiplication would simplify the operation to a number*/
			else if (left->getLeft()->getType() == "num" && left->getRight()->getType() == "num" && isDivide(left->getValue()[0])) {
				Node* finalObj;
				char* op = (char *)malloc(sizeof(char));
				strcpy(op, left->getValue());
				if (isMultiply(value[0])) {
					int leftInt = atoi(left->getLeft()->getValue());
					Node* result = checkAndPerformOperationInt(leftInt, rightInt, value);
					int resultInt = atoi(result->getValue());
					int otherInt = atoi(left->getRight()->getValue());
					finalObj = checkAndPerformOperationInt(resultInt, otherInt, op);
					//delete left;
					return finalObj;
				}
				//is division
				else {
					char* mult = "*";
					int leftInt = atoi(left->getRight()->getValue());
					//denominator of the quotient
					Node* denom = checkAndPerformOperationInt(leftInt, rightInt, mult);
					int denomInt = atoi(denom->getValue());
					int otherInt = atoi(left->getLeft()->getValue());
					free(mult);
					finalObj = checkAndPerformOperationInt(otherInt, denomInt, op);
				}

				/*should be deleting this
				delete this;*/
				return finalObj;
			}
		}
	}

	//if right can't be simplified
	return this;
}

Node* Node::simplifySelfLeftVar() {
	if (right->getType() == "var" && (strcmp(left->getValue(), right->getValue()) == 0)) {
		Node* newNode = checkAndPerformOperationVars(left, right, value);
		//what if new node is unchanged and i duplicated the object? maybe return null
		return newNode;
	}
	else if (right->getType() == "num") {
		return checkAndPerformOperationVarOnLeft(right, left, value);
	}
	else if (right->getType() == "operator") {
		if (right->getLeft()->getType() == "var" && right->getRight()->getType() == "var") {
			Node* leftVar;
			Node* rightVar;
			Node* finalNode;
			char* finalOp;

			if (strcmp(left->getValue(), right->getLeft()->getValue()) == 0) {
				//both variables are the same as the left object
				if (strcmp(left->getValue(), right->getRight()->getValue()) == 0) {
					//the right operation is addition or subtraction
					if (isAdd(value[0])) {
						if (left->getExp() == right->getLeft()->getExp()) {
							leftVar = checkAndPerformOperationVars(left, right->getLeft(), "+");
							finalOp = (char *)malloc(sizeof(char));
							strcpy(finalOp, right->getValue());
							finalNode = new Node(finalOp, "operator", leftVar, right->getRight());
						}
						else if (left->getExp() == right->getRight()->getExp()) {
							if (isAdd(right->getValue()[0])) {
								leftVar = checkAndPerformOperationVars(left, right->getRight(), "+");
								finalOp = "+";
								finalNode = (finalOp, "operator", leftVar, right->getLeft());
							}
							else {
								leftVar = checkAndPerformOperationVars(left, right->getRight(), "-");
								finalOp = (char *)malloc(sizeof(char));
								strcpy(finalOp, value);
								finalNode = new Node(finalOp, "operator", leftVar, right->getLeft());
								//this is useful if x + (x^3 - x) because then finalNode would be (0 + x^3)
								return finalNode->simplifySelf();
							}
						}
					}
					else if (isSubtract(value[0])) {
						if (left->getExp() == right->getLeft()->getExp()) {
							leftVar = checkAndPerformOperationVars(left, right->getLeft(), "-");
							finalOp = (char *)malloc(sizeof(char));
							strcpy(finalOp, right->getValue());
							finalNode = new Node(finalOp, "operator", leftVar, right->getRight());
						}
						else if (left->getExp() == right->getRight()->getExp()) {
							if (isAdd(right->getValue()[0])) {
								leftVar = checkAndPerformOperationVars(left, right->getRight(), "-");
								finalOp = "-";
								finalNode = (finalOp, "operator", leftVar, right->getLeft());
							}
							else {
								leftVar = checkAndPerformOperationVars(left, right->getRight(), "+");
								finalOp = (char *)malloc(sizeof(char));
								strcpy(finalOp, value);
								finalNode = new Node(finalOp, "operator", leftVar, right->getLeft());
								//this is useful if x + (x^3 - x) because then finalNode would be (0 + x^3)
								return finalNode->simplifySelf();
							}
						}
					}
					//the left operation is multiplication. division should just be returned
					else if (isMultiply(value[0])) {
						leftVar = checkAndPerformOperationVars(left, right->getLeft(), "*");
						rightVar = checkAndPerformOperationVars(left, right->getRight(), "*");
						finalOp = (char *)malloc(sizeof(char));
						strcpy(finalOp, right->getValue());
						finalNode = new Node(finalOp, "operator", leftVar, rightVar);
					}
					else {
						return this;
					}
				}
				//the right object within the right expression is a different var. ex/ x + (x + y)
				else {
					char* op = (char *)malloc(sizeof(char));
					strcpy(op, value);
					leftVar = checkAndPerformOperationVars(left, right->getLeft(), op);
					finalOp = (char *)malloc(sizeof(char));
					strcpy(finalOp, right->getValue());

					//returned operation could result in 0 ex/ x - (x + y)
					if (leftVar->getType() == "num") {
						int leftInt = atoi(leftVar->getValue());

						if (leftInt == 0) {
							/*this will only happen if addition/subtraction because cancellation of terms is not 
							possible with multiplication/division*/
							if (strcmp(value, right->getValue()) != 0) {
								right->setCoeff(0 - right->getCoeff());
							}
							return right->getRight();
						}
					}

					if (isMultiply(value[0])) {
						if (isMultiplyOrDivide(right->getValue())) {
							rightVar = right->getRight();
						}
						else {
							rightVar = new Node("*", "operator", left, right->getRight());
						}

						finalNode = new Node(finalOp, "operator", leftVar, rightVar);
					}
					else if (isAdd(value[0])) {
						finalNode = new Node(finalOp, "operator", leftVar, right->getRight());
					}
					else if (isSubtract(value[0])) {
						if (isSubtract(right->getValue()[0])) {
							finalNode = new Node("+", "operator", leftVar, right->getRight());
						}
						else {
							finalNode = new Node(op, "operator", leftVar, right->getRight());
						}
					}
					//is division
					else {
						if (isMultiply(right->getValue()[0])) {
							finalNode = new Node("/", "operator", leftVar, right->getRight());
						}
						else if (isDivide(right->getValue()[0])) {
							finalNode = new Node("*", "operator", leftVar, right->getRight());
						}
						else {
							return this;
						}
					}
				}

				return finalNode;
			}
		}
		//ex/ x + (x - 7)
		else if (right->getLeft()->getType() == "var" && right->getRight()->getType() == "num" 
					&& (strcmp(left->getValue(), right->getLeft()->getValue()) == 0)) {
			Node* varOpNode;
			Node* varNumOpNode;
			Node* finalNode;
			char* finalOp;

			if (isAddOrSubtract(right->getValue())) {
				varOpNode = checkAndPerformOperationVars(left, right->getLeft(), value);

				if (isMultiplyOrDivide(value)) {
					varNumOpNode = checkAndPerformOperationVarOnLeft(right->getRight(), left, value);
					finalOp = (char *)malloc(sizeof(char));
					strcpy(finalOp, right->getValue());
				}
				else {
					if (strcmp(left->getValue(), right->getValue()) == 0) {
						finalOp = "+";
					}
					else {
						finalOp = "-";
					}

					if (varOpNode->getType() == "var") {
						varNumOpNode = checkAndPerformOperationVarOnLeft(right->getRight(), varOpNode, finalOp);
					}
					else {
						varNumOpNode = new Node(finalOp, "operator", varOpNode, right->getRight());
					}
					return varNumOpNode;
				}
				finalNode = new Node(finalOp, "operator", varOpNode, varNumOpNode);
				return finalNode;
			}
			else {
				/*the only time that a var and a num will be multiplied/divided is if the coefficient of the var is not
				large enough to integer divide the num in the quotient*/
				varOpNode = checkAndPerformOperationVars(left, right->getLeft(), value);

				if (isMultiply(value[0])) {
					finalOp = (char *)malloc(sizeof(char));
					//finalOp will be "/"
					strcpy(finalOp, right->getValue());
					finalNode = new Node(finalOp, "operator", varOpNode, right->getRight());
					return finalNode->simplifySelf();
				}
				else {
					finalOp = "*";

					if (varOpNode->getType() == "var") {
						finalNode = checkAndPerformOperationVarOnLeft(right->getRight(), varOpNode, finalOp);
					}
					//will be a number because the variable division became just the resultant coefficient
					else {
						int rightInt = atoi(right->getRight()->getValue());

						if (varOpNode->getType() == "num") {
							int leftInt = atoi(varOpNode->getValue());
							finalNode = checkAndPerformOperationInt(leftInt, rightInt, finalOp);
						}
						else if (varOpNode->getLeft()->getType() == "num" && varOpNode->getRight()->getType() == "num") {
							//both numbers are nums that could not be integer divided to a number greater than 0
							int leftInt = atoi(varOpNode->getLeft()->getValue());
							Node* intMulNode = checkAndPerformOperationInt(leftInt, rightInt, finalOp);
							char* divOp = "/";
							//so i can delete varOpNode
							Node* rightNum = new Node(varOpNode->getRight()->getValue(), "num");
							finalNode = new Node(divOp, "operator", intMulNode, rightNum);
							return finalNode->simplifySelf();
						}
						//there is a var and a num
						else {
							Node* varNode = checkAndPerformOperationVarOnLeft(right->getRight(), varOpNode->getLeft(), finalOp);
							char* nodeOp = (char *)malloc(sizeof(char));
							strcpy(nodeOp, varOpNode->getValue());
							finalNode = checkAndPerformOperationVarOnLeft(varOpNode->getRight(), varNode, nodeOp);
							return finalNode->simplifySelf();
						}
					}
					return finalNode;
				}
			}
		}
	}

	//can't simplify
	return this;
}

Node* Node::simplifySelfRightVar() {
	if (right->getType() == "num") {
		return checkAndPerformOperationVarOnRight(right, left, value);
	}

	//can't simplify
	return this;
}

Node* Node::checkAndPerformOperationInt(int leftInt, int rightInt, char* op) {
	int result;

	if (op[0] == '*') {
		result = leftInt * rightInt;
	}
	else if (op[0] == '/') {
		/*because this is integer division, decimal digits will be ignored.
		rounding, in this case, is not that important*/
		if (abs(leftInt) > abs(rightInt) && rightInt != 0) {
			result = leftInt / rightInt;
		}
		else {
			char* leftValue = (char *)malloc(sizeof(leftInt)/sizeof(int));
			sprintf(leftValue, "%d", leftInt);
			Node* leftObject = new Node(leftValue, "num");
			char* rightValue = (char *)malloc(sizeof(rightInt)/sizeof(int));
			sprintf(rightValue, "%d", rightInt);
			Node* rightObject = new Node(rightValue, "num");
			char* oper = (char *)malloc(sizeof(op));
			strcpy(oper, op);
			return new Node(oper, "operator", leftObject, rightObject);
		}
	}
	else if (op[0] == '+') {
		result = leftInt + rightInt;
	}
	else if (op[0] == '-') {
		result = leftInt - rightInt;
	}

	char* numValue = (char *)malloc(sizeof(result)/sizeof(int));
	sprintf(numValue, "%d", result);
	return new Node(numValue, "num");
}

Node* Node::checkAndPerformOperationVarOnRight(Node* intObj, Node* var, char* operation) {
	int intObjValue = atoi(intObj->getValue());

	if (operation[0] == '*') {
		if (intObjValue != 0) {
			int newCoeff = intObjValue * var->getCoeff();
			var->setCoeff(newCoeff);
			return var;
		}
		else {
			delete var;
			return intObj;
		}
	}
	else if (operation[0] == '/' || operation[0] == '+' || operation[0] == '-') {
		return new Node(operation, "operator", intObj, var);
	}

	return var;
}

Node* Node::checkAndPerformOperationVarOnLeft(Node* intObj, Node* var, char* operation) {
	int intObjValue = atoi(intObj->getValue());

	if (operation[0] == '*') {
		if (intObjValue != 0) {
			int newCoeff = intObjValue * var->getCoeff();
			var->setCoeff(newCoeff);
			delete intObj;
			return var;
		}
		else {
			delete var;
			return intObj;
		}
	}
	else if (operation[0] == '/') {
		if ((abs(var->getCoeff()) > abs(intObjValue)) && intObjValue != 0) {
			int newCoeff = var->getCoeff() / intObjValue;
			var->setCoeff(newCoeff);
			delete intObj;
			return var;
		}
		else {
			return new Node(operation, "operator", var, intObj);
		}
	}
	//the operation is addition/subtraction
	else {
		return new Node(operation, "operator", var, intObj);
	}
}

Node* Node::checkAndPerformOperationVars(Node* leftVar, Node* rightVar, char* operation) {
	if (isMultiply(operation[0])) {
		int newCoeff = leftVar->getCoeff() * rightVar->getCoeff();
		int newExp = leftVar->getExp() + rightVar->getExp();
		char* varValue = (char *)malloc(sizeof(char));
		strcpy(varValue, leftVar->getValue());
		Node* newVar = new Node(varValue, newCoeff, newExp);
		/*delete leftVar;
		delete rightVar;*/
		return newVar;
	}
	else if (isDivide(operation[0])) {
		int newCoeff = leftVar->getCoeff() / rightVar->getCoeff();
		int newExp = leftVar->getExp() - rightVar->getExp();
		Node* newNode;

		if (newExp == 0) {
			if (abs(newCoeff) > 0) {
				//the new node is a constant with the value of the coefficient
				char* numValue = (char *)malloc(sizeof(newCoeff)/sizeof(int));
				sprintf(numValue, "%d", newCoeff);
				newNode = new Node(numValue, "num");
			}
			else {
				char* leftIntValue = (char *)malloc(sizeof(leftVar->getCoeff())/sizeof(int));
				sprintf(leftIntValue, "%d", leftVar->getCoeff());
				Node* leftIntNode = new Node(leftIntValue, "num");
				char* rightIntValue = (char *)malloc(sizeof(rightVar->getCoeff())/sizeof(int));
				sprintf(rightIntValue, "%d", rightVar->getCoeff());
				Node* rightIntNode = new Node(rightIntValue, "num");
				newNode = new Node("/", "operator", leftIntNode, rightIntNode);
			}
		}
		else if (newExp > 1) {
			if (abs(newCoeff) > 0) {
				char* varValue = (char *)malloc(sizeof(char));
				strcpy(varValue, leftVar->getValue());
				newNode = new Node(varValue, newCoeff, newExp);
			}
			else {
				leftVar->setExp(newExp);
				char* numValue = (char *)malloc(sizeof(rightVar->getCoeff())/sizeof(int));
				sprintf(numValue, "%d", rightVar->getCoeff());
				Node* numNode = new Node(numValue, "num");
				newNode = new Node("/", "operator", leftVar, numNode);
			}
		}
		else {
			char* varValue = (char *)malloc(sizeof(char));
			strcpy(varValue, leftVar->getValue());
			Node* newVar = new Node(varValue, 1, abs(newExp));
			char* numValue = (char *)malloc(sizeof(newCoeff)/sizeof(int));
			sprintf(numValue, "%d", newCoeff);
			Node* leftNum = new Node(numValue, "num");
			//don't want negative exponents so creating a division operation
			newNode = new Node("/", "operator", leftNum, newVar);
		}

		return newNode;
	}
	//the operation is addition/subtraction
	else {
		//check that the exponents are similar
		if (leftVar->getExp() == rightVar->getExp()) {
			int newCoeff;

			if (isAdd(operation[0])) {
				newCoeff = leftVar->getCoeff() + rightVar->getCoeff();
			}
			else {
				newCoeff = leftVar->getCoeff() - rightVar->getCoeff();
			}

			if (newCoeff == 0) {
				return new Node("0", "num");
			}
			else {
				char* varValue = (char *)malloc(sizeof(char));
				strcpy(varValue, leftVar->getValue());
				Node* newVar = new Node(varValue, newCoeff, leftVar->getExp());
				//DELETE UNNECESSARY OBJECTS!!
				return newVar;
			}
		}

		return new Node(operation, "operator", leftVar, rightVar);
	}
} 

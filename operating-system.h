struct Node
{
	struct Node *next;
	char *data;
}*start=NULL;

void add(char character, char *data){
	do{
		struct Node *newNode, *currentNode;
		newNode = (struct Node *) malloc(sizeof(struct Node));
		newNode->data = data;
		newNode->next = NULL;

		if(start == NULL){
			start = newNode;
			currentNode = newNode;
		} else {
			currentNode->next = newNode;
			currentNode = newNode;
		}
	}while(character != '\0');
}
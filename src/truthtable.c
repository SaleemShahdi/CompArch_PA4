#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum { AND, OR, NAND, NOR, XOR, NOT, PASS, DECODER, MULTIPLEXER } kind_t;

int getType(char * s) {
    if (strcmp(s, "AND") == 0) {
        return AND;
    } else if (strcmp(s, "OR") == 0) {
        return OR;
    } else if (strcmp(s, "NAND") == 0) {
        return NAND;
    } else if (strcmp(s, "NOR") == 0) {
        return NOR;
    } else if (strcmp(s, "XOR") == 0) {
        return XOR;
    } else if (strcmp(s, "NOT") == 0) {
        return NOT;
    } else if (strcmp(s, "PASS") == 0) {
        return PASS;
    } else if (strcmp(s, "DECODER") == 0) {
        return DECODER;
    } else if (strcmp(s, "MULTIPLEXER") == 0) {
        return MULTIPLEXER;
    } else {
        return -1;
    }
}

struct Gate
{
    kind_t kind;
    int indegree;
    int size;    // indicates size of DECODER and MULTIPLEXER
    int *params; // length determined by kind and size;
    // includes inputs and outputs, indicated by variable numbers
};

typedef struct Gate gate;

struct Node {
    char * name;
    int index;
    struct Node * next;
};

typedef struct Node node;

struct GateNode {
    gate * g;
    struct GateNode * next;
    int position;
    struct GateNode * nextqueue;
};

typedef struct GateNode gateNode;

void enqueue(gateNode ** front, gateNode * data) {
    gateNode * head = *front;
    if (head == NULL) {
        *front = data;
        return;
    }
    while ((*front)->nextqueue != NULL) {
        *front = (*front)->nextqueue;
    }
    (*front)->nextqueue = data;
    *front = head;
    return;
}

gateNode dequeue(gateNode ** front) {
    gateNode * head = *front;
    gateNode * prev = NULL;
    while ((*front)->nextqueue != NULL) {
        prev = *front;
        *front = (*front)->nextqueue;
    }
    gateNode result = **front;
    if (prev == NULL) {
        *front = NULL;
    } else {
        prev->nextqueue = (*front)->nextqueue;
        *front = head;
    }
    return result;

}

int getNumberFromName(node * front, char * s) {
    if (front == NULL) {
        return -1;
    }
    if (strcmp(front->name, s) == 0) {
        return front->index;
    }
    return getNumberFromName(front->next, s);
}

char * getNameFromNumber(node * front, int index) {
    if (front == NULL) {
        return "not in list";
    }
    if (front->index == index) {
        return front->name;
    }
    return getNameFromNumber(front->next, index);
}

int isLiteralNumber(char * s) {
    if (strcmp(s, "0") == 0) {
        return 1;
    }
    if (strcmp(s, "1") == 0) {
        return 1;
    }
    return 0;
}

void runThroughGateArray(int * variables, gate * gateArray, int gateArraySize,
                         int inputsize, int outputsize)
{
    for (int i = 0; i <= gateArraySize - 1; i++) {
        gate current = gateArray[i];
        int type = current.kind;
        if (type == AND) {
            int outputindex = current.params[2];
            int input1index = current.params[0];
            int input2index = current.params[1];
            int input1 = variables[input1index];
            int input2 = variables[input2index];
            int result = input1 && input2;
            variables[outputindex] = result;

        } else if (type == OR) {
            int outputindex = current.params[2];
            int input1index = current.params[0];
            int input2index = current.params[1];
            int input1 = variables[input1index];
            int input2 = variables[input2index];
            int result = input1 || input2;
            variables[outputindex] = result;

        } else if (type == NAND) {
            int outputindex = current.params[2];
            int input1index = current.params[0];
            int input2index = current.params[1];
            int input1 = variables[input1index];
            int input2 = variables[input2index];
            int result = input1 && input2;
            result = !result;
            variables[outputindex] = result;

        } else if (type == NOR) {
            int outputindex = current.params[2];
            int input1index = current.params[0];
            int input2index = current.params[1];
            int input1 = variables[input1index];
            int input2 = variables[input2index];
            int result = input1 || input2;
            result = !result;
            variables[outputindex] = result;

        } else if (type == XOR) {
            int outputindex = current.params[2];
            int input1index = current.params[0];
            int input2index = current.params[1];
            int input1 = variables[input1index];
            int input2 = variables[input2index];
            int result = input1^input2;
            variables[outputindex] = result;

        } else if (type == NOT) {
            int outputindex = current.params[1];
            int inputindex = current.params[0];
            int input = variables[inputindex];
            int result = !input;
            variables[outputindex] = result;


        } else if (type == PASS) {
            int outputindex = current.params[1];
            int inputindex = current.params[0];
            int input = variables[inputindex];
            int result = input;
            variables[outputindex] = result;

        } else if (type == DECODER) {
            int numberofinputs = current.size;
            int result = 0;
            for (int i = 0; i <= numberofinputs - 1; i++) {
                int inputindex = current.params[i];
                result = result + (variables[inputindex] << (numberofinputs-1-i));
            }
            int decoderoutput = result + numberofinputs;
            int numberofoutputs = 1 << numberofinputs;
            for (int i = numberofinputs; i <= numberofinputs + numberofoutputs - 1; i++) {
                int outputindex = current.params[i];
                if (i == decoderoutput) {
                    variables[outputindex] = 1;
                } else {
                    variables[outputindex] = 0;
                }

            }

        } else if (type == MULTIPLEXER) {
            int numberofselectors = current.size;
            int numberofinputs = 1 << numberofselectors;
            int result = 0;
            for (int i = numberofinputs; i <= numberofselectors + numberofinputs - 1; i++) {
                int currentindex = current.params[i];
                result = result + (variables[currentindex] << (abs(i-(numberofinputs+numberofselectors))-1));
            }
            result = current.params[result];
            int outputindex = current.params[numberofselectors+numberofinputs];
            variables[outputindex] = variables[result];
        }
        
    }
    for (int i = 0; i <= inputsize - 1; i++) {
            printf("%d ", variables[i]);
        }
        printf("| ");
        for (int i = inputsize; i <= outputsize + inputsize - 2; i++) {
            printf("%d ", variables[i]);
        }
        printf("%d\n", variables[inputsize+outputsize-1]);
    
}

void printTruthTable(int * variables, int inputsize, int current, gate* gateArray,
                     int gateArraySize, int outputsize) {
    for (int i = 0; i <= 1; i++) {
        variables[current] = i;
        if (current == inputsize - 1) {
            runThroughGateArray(variables, gateArray, gateArraySize, inputsize, outputsize);
        } else {
            printTruthTable(variables, inputsize, current+1, gateArray, gateArraySize, outputsize);
        }
    }
}

char * getNewString(char * tempstring) {
    int size = 0;
    while (tempstring[size] != '\0') {
        size++;
    }
    size++;
    char * result = malloc(size * sizeof(char));
    for (int i = 0; i <= size - 1; i++) {
        result[i] = tempstring[i];
    }
    return result;
}

int isNeighbor(gate a, gate b) {
    int kind = a.kind;
    int * aoutputs;
    int * binputs;
    int aoutputsize;
    int binputsize;
    int neighbors = 0;
    switch (kind) {
        case AND:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[2];
            aoutputsize = 1;
            break;
        case OR:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[2];
            aoutputsize = 1;
            break;
        case NAND:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[2];
            aoutputsize = 1;
            break;
        case NOR:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[2];
            aoutputsize = 1;
            break;
        case XOR:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[2];
            aoutputsize = 1;
            break;
        case NOT:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[1];
            aoutputsize = 1;
            break;
        case PASS:
            aoutputs = malloc(1 * sizeof(int));
            aoutputs[0] = a.params[1];
            aoutputsize = 1;
            break;
        case DECODER:
            int size = a.size;
            int sizeofoutputs = 1 << size;
            aoutputs = malloc(sizeofoutputs * sizeof(int));
            for (int i = size; i <= sizeofoutputs - 1; i++) {
                aoutputs[i-size] = a.params[i];
            }
            aoutputsize = sizeofoutputs;
            break;
        case MULTIPLEXER:
            size = a.size;
            aoutputsize = 1;
            aoutputs = malloc(1*sizeof(int));
            int tworaisedsize = 1 << size;
            int totalsize = size + tworaisedsize;
            aoutputs[0] = a.params[totalsize];
            break;
    }
    kind = b.kind;
    switch (kind) {
        case AND:
            binputs = malloc(2 * sizeof(int));
            binputsize = 2;
            for (int i = 0; i <= 2 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case OR:
            binputs = malloc(2 * sizeof(int));
            binputsize = 2;
            for (int i = 0; i <= 2 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case NAND:
            binputs = malloc(2 * sizeof(int));
            binputsize = 2;
            for (int i = 0; i <= 2 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case NOR:
            binputs = malloc(2 * sizeof(int));
            binputsize = 2;
            for (int i = 0; i <= 2 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case XOR:
            binputs = malloc(2 * sizeof(int));
            binputsize = 2;
            for (int i = 0; i <= 2 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case NOT:
            binputs = malloc(1 * sizeof(int));
            binputsize = 1;
            for (int i = 0; i <= 1 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case PASS:
            binputs = malloc(1 * sizeof(int));
            binputsize = 1;
            for (int i = 0; i <= 1 - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
        case DECODER:
            int size = b.size;
            binputs = malloc(size * sizeof(int));
            for (int i = 0; i <= size - 1; i++) {
                binputs[i] = b.params[i];
            }
            binputsize = size;
            break;
        case MULTIPLEXER:
            size = b.size;
            int tworaisedsize = 1 << size;
            binputsize = size + tworaisedsize;
            binputs = malloc(binputsize*sizeof(int));
            for (int i = 0; i <= binputsize - 1; i++) {
                binputs[i] = b.params[i];
            }
            break;
    }
    for (int i = 0; i <= aoutputsize - 1; i++) {
        for (int j = 0; j <= binputsize - 1; j++) {
            if (aoutputs[i] == binputs[j]) {
                neighbors = 1;
            }

        }
    }
    free(aoutputs);
    free(binputs);
    return neighbors;
}

void freelinkedlist(node * linkedlist) {
    if (linkedlist->next == NULL) {
        free(linkedlist->name);
        free(linkedlist);
        linkedlist = NULL;
        return;
    }
    freelinkedlist(linkedlist->next);
    free(linkedlist->name);
    free(linkedlist);
    linkedlist = NULL;
    return;
}

int main (int argc, char ** argv) {
    FILE * stream = fopen(argv[1], "r");
    char tempstring [17];
    int index = 0;
    int * variables;
    int inputsize = 0;
    int outputsize = 0;
    node * front;
    node * linkedlist; //= front;
    int gateArraySize = 0;
    int gateArrayIndex = 0;
    while (1) {
        int i = fscanf(stream, " %16s", tempstring);
        if (i == EOF) {
            break;
        }
        if (getType(tempstring) >=0 && getType(tempstring) <= 8) {
            gateArraySize = gateArraySize + 1;
        }
    }
    fclose(stream);
    gate * gateArray = malloc(gateArraySize * sizeof(gate));
    stream = fopen(argv[1], "r");
    while (1) {
        int i = fscanf(stream, " %16s", tempstring);
        if (i == EOF) {
            break;
        }
        if (strcmp(tempstring, "INPUT") == 0) {
            fscanf(stream, "%d", &inputsize);
            for (int j = 0; j <= inputsize - 1; j++) {
                fscanf(stream, " %16s", tempstring);
                char * copy = getNewString(tempstring);
                if (j == 0) {
                    front = malloc(sizeof(node));
                    (*front).index = index;
                    (*front).name = copy;
                    (*front).next = NULL;
                    linkedlist = front;
                } else {
                    node * current = malloc(sizeof(node));
                    (*current).index = index;
                    (*current).name = copy;
                    (*current).next = NULL;
                    (*front).next = current;
                    front = front->next;
                }
                index = index + 1;
            }
            

        } else if (strcmp(tempstring, "OUTPUT") == 0) {
            fscanf(stream, "%d", &outputsize);
            for (int j = 0; j <= outputsize - 1; j++) {
                fscanf(stream, " %16s", tempstring);
                char * copy = getNewString(tempstring);
                node * current = malloc(sizeof(node));
                (*current).index = index;
                (*current).name = copy;
                (*current).next = NULL;
                (*front).next = current;
                front = (*front).next;
                index = index + 1;
            }

        } else if ((getType(tempstring) == 6) || (getType(tempstring) == 5)) {
            int * parameters = malloc(2 * sizeof(int));
            gate temp = {getType(tempstring), 0, 2, NULL};
            for (int j = 0; j <= 2 - 1; j++) {
                fscanf(stream, " %16s", tempstring);
                if (getNumberFromName(linkedlist, tempstring) == -1) {
                    char * copy = getNewString(tempstring);
                    node * current = malloc(sizeof(node));
                    current->name = copy;
                    current->index = index;
                    current->next = NULL;
                    front->next = current;
                    front = front->next;
                    index = index + 1;
                }
                int number = getNumberFromName(linkedlist, tempstring);
                parameters[j] = number;

            }
            temp.params = parameters;
            gateArray[gateArrayIndex] = temp;
            gateArrayIndex = gateArrayIndex + 1;

            
            

        } else if (getType(tempstring) == 7) {
            gate temp;
            temp.kind = DECODER;
            fscanf(stream, " %16s", tempstring);
            int n = atoi(tempstring);
            int tworaisedn = 1 << n;
            temp.size = n;
            int * parameters = malloc((n+tworaisedn) * sizeof(int));
            for (int j = 0; j <= n+tworaisedn-1; j++) {
                fscanf(stream, " %16s", tempstring);
                if (getNumberFromName(linkedlist, tempstring) == -1) {
                    char * copy = getNewString(tempstring);
                    node * current = malloc(sizeof(node));
                    current->name = copy;
                    current->index = index;
                    current->next = NULL;
                    front->next = current;
                    front = front->next;
                    index = index + 1;
                }
                int number = getNumberFromName(linkedlist, tempstring);
                parameters[j] = number;
            }
            temp.params = parameters;
            gateArray[gateArrayIndex] = temp;
            gateArrayIndex = gateArrayIndex + 1;

            

        } else if (getType(tempstring) == 8) {
            gate temp;
            temp.kind = MULTIPLEXER;
            fscanf(stream, " %16s", tempstring);
            int n = atoi(tempstring);
            int tworaisedn = 1 << n;
            temp.size = n;
            int * parameters = malloc((n+tworaisedn+1) * sizeof(int));
            for (int j = 0; j <= n + tworaisedn; j++) {
                fscanf(stream, " %16s", tempstring);
                if (getNumberFromName(linkedlist, tempstring) == -1) {
                    char * copy = getNewString(tempstring);
                    node * current = malloc(sizeof(node));
                    current->name = copy;
                    current->index = index;
                    current->next = NULL;
                    front->next = current;
                    front = front->next;
                    index = index + 1;
                }
                int number = getNumberFromName(linkedlist, tempstring);
                parameters[j] = number;
            }
            temp.params = parameters;
            gateArray[gateArrayIndex] = temp;
            gateArrayIndex = gateArrayIndex + 1;

        } else {
            int * parameters = malloc(3 * sizeof(int));
            gate temp = {getType(tempstring), 0, 3, NULL};
            for (int j = 0; j <= temp.size - 1; j++) {
                fscanf(stream, " %16s", tempstring);
                if (getNumberFromName(linkedlist, tempstring) == -1) {
                    char * copy = getNewString(tempstring);
                    node * current = malloc(sizeof(node));
                    current->name = copy;
                    current->index = index;
                    current->next = NULL;
                    front->next = current;
                    front = front->next;
                    index = index + 1;
                }
                int number = getNumberFromName(linkedlist, tempstring);
                parameters[j] = number;

            }
            temp.params = parameters;
            gateArray[gateArrayIndex] = temp;
            gateArrayIndex = gateArrayIndex + 1;

        }

        
    }
    fclose(stream);
    stream = NULL;
    variables = malloc((index) * sizeof(int));
    front = linkedlist;
    while (front != NULL) {
        int currentindex = front->index;
        char * currentname = front->name;
        if (isLiteralNumber(currentname)  == 1) {
            variables[currentindex] = atoi(currentname);
        }
        front = front->next;
    }
    gateNode * adjmatrix = malloc(gateArraySize * sizeof(gateNode));
    gateNode * current;
    for (int i = 0; i <= gateArraySize - 1; i++) {
        gateNode temp = {&gateArray[i], NULL, i, NULL};
        current = &temp;
        for (int j = 0; j <= gateArraySize - 1; j++) {
            if (j == i) {
                continue;
            }
            if (isNeighbor(*(temp.g), gateArray[j]) == 1) {
                gateNode temp1 = {&gateArray[j], NULL, j, NULL};
                current->next = &temp1;
                current = current->next;
            }
        }
        adjmatrix[i] = temp;
    }
    for (int i = 0; i <= gateArraySize - 1; i++) {
        int indegree = 0;
        for (int j = 0; j <= gateArraySize - 1; j++) {
            if (j == i) {
                continue;
            }
            if (isNeighbor(gateArray[j], gateArray[i]) == 1) {
                indegree = indegree + 1;

            }
        }
        gateArray[i].indegree = indegree;
    }
    gate * gateArraySorted = malloc(gateArraySize * sizeof(gate));
    gateNode * queue = NULL;
    for (int i = 0; i <= gateArraySize - 1; i++) {
        if ((*(adjmatrix[i].g)).indegree == 0) {
            enqueue(&queue, &(adjmatrix[i]));
        }
    }
    int i = 0;
    while (queue != NULL) {
        gateNode removed = dequeue(&queue);
        gateNode * temp = removed.next;
        gateArraySorted[i] = *(removed.g);
        i = i + 1;
        while (temp != NULL) {
            (*((*temp).g)).indegree = (*((*temp).g)).indegree - 1;
            if ((*((*temp).g)).indegree == 0) {
                int position = (*temp).position;
                enqueue(&queue, &(adjmatrix[position]));
            }
            temp = temp->next;

        }
    }
    

    //printTruthTable(variables, inputsize, 0, gateArray, gateArraySize, outputsize);
    printTruthTable(variables, inputsize, 0, gateArraySorted, gateArraySize, outputsize);
    freelinkedlist(linkedlist);
    free(variables);
    for (int i = 0; i <= gateArraySize - 1; i++) {
        free(gateArray[i].params);
    }
    free(gateArray);

    return EXIT_SUCCESS;
    



}
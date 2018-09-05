#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef struct Customer {
  char *id;
  int arrival;
  int priority;
  int age;
  int times;
} Customer;

typedef struct Node {
  struct Customer *customer;
  struct Node *next;
} Node;

typedef struct Queue {
  struct Node *front;
  struct Node *rear;
} Queue;

Node *new_node(Customer *customer) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->customer = customer;
  node->next = NULL;
  return node;
}

Queue *new_queue() {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}

void enQueue(Queue *queue, Node *node) {
  if(queue->rear == NULL) {
    queue->front = node;
    queue->rear = node;
  }
  else {
    queue->rear->next = node;
    queue->rear = node;
  }
}

Node *deQueue(Queue *queue) {
  if(queue->front == NULL) {
    return NULL;
  }
  Node *node = queue->front;
  queue->front = queue->front->next;
  if(queue->front == NULL) {
    queue->rear = NULL;
  }
  return node;
}

int main(int argC, char *argV[]) {
  FILE *input = fopen(argV[1], "r");
  char buffer[255];
  char *token;
  char delim[] = " \t\n\v\f\r";
  int count;
  Queue *array = (Queue *)malloc(sizeof(Queue));
  while(fgets(buffer, sizeof(buffer), input) != NULL) {
    Customer *customer = (Customer *)malloc(sizeof(Customer));
    token = strtok(buffer, delim);
    count = 0;
    while(count < 5) {
      switch(count) {
        case 0:
          customer->id = token;
          break;

        case 1:
          customer->arrival = atoi(token);
          break;

        case 2:
          customer->priority = atoi(token);
          break;

        case 3:
          customer->age = atoi(token);
          break;

        case 4:
          customer->times = atoi(token);
          break;

        default:
          return -1;
      }
      token = strtok(NULL, delim);
      count++;
    }
    enQueue(array, new_node(customer));
  }
  Node *temp = array->front;
  while(temp != NULL) {
    printf("%s\n", temp->customer->id);
    temp = temp->next;
  }
  fclose(input);
  return 0;
}

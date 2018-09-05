#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>

typedef enum { false, true } bool;

typedef struct Customer {
  char id[256];
  int arrival;
  int priority;
  int age;
  int cpu_time;
  int end;
  int ready;
  int total_ticks;
  int run_ticks;
  int total_runs;
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
  node->next = NULL;
  return node;
}

void sort_by_arrival(Queue *queue) {
  bool check = false;
  Node *head, *curr, *next;
  while(!check) {
    check = true;
    head = NULL;
    curr = queue->front;
    if(curr == NULL) {
      return;
    }
    next = curr->next;
    while(next != NULL) {
      if(curr->customer->arrival > next->customer->arrival) {
        head->next = next;
        curr->next = next->next;
        next->next = curr;
        head = next;
        next = curr->next;
        check = false;
      }
      else {
        head = curr;
        curr = next;
        next = next->next;
      }
    }
  }
}

void sort_by_priority(Queue *queue) {
  bool check = false;
  Node *head, *curr, *next;
  while(!check) {
    check = true;
    head = NULL;
    curr = queue->front;
    if(curr == NULL) {
      return;
    }
    next = curr->next;
    while(next != NULL) {
      if(curr->customer->priority < next->customer->priority) {
        head->next = next;
        curr->next = next->next;
        next->next = curr;
        head = next;
        next = curr->next;
        check = false;
      }
      else {
        head = curr;
        curr = next;
        next = next->next;
      }
    }
  }
}

void customer_arrival(Queue *array, Queue *high, Queue *medium, Queue *low, int timer) {
  while(array->front->customer->arrival == timer) {
    if(array->front->customer->priority >= 5) {
      Node *node = deQueue(array);
      enQueue(high, node);
    }
    else if(array->front->customer->priority >= 3 && array->front->customer->priority < 5) {
      Node *node = deQueue(array);
      enQueue(medium, node);
    }
    else {
      Node *node = deQueue(array);
      enQueue(low, node);
    }
  }
}

void print_queue(Queue *queue) {
  Node *temp = queue->front;
  while(temp != NULL) {
    printf("%s\n", temp->customer->id);
    temp = temp->next;
  }
}

int main(int argC, char *argV[]) {
  FILE *input = fopen(argV[1], "r");
  char buffer[256];
  char *token;
  char delim[] = " \t\n\v\f\r";
  int count;
  Queue *array = new_queue();
  while(fgets(buffer, sizeof(buffer), input) != NULL) {
    Customer *customer = (Customer *)malloc(sizeof(Customer));
    customer->end = -1;
    customer->ready = -1;
    customer->total_ticks = 0;
    customer->run_ticks = 0;
    customer->total_runs = 0;
    token = strtok(buffer, delim);
    count = 0;
    while(count < 5) {
      switch(count) {
        case 0:
          strcpy(customer->id, token);
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
          customer->cpu_time = atoi(token);
          break;

        default:
          return -1;
      }
      token = strtok(NULL, delim);
      count++;
    }
    enQueue(array, new_node(customer));
  }
  sort_by_arrival(array);
  Queue *high = new_queue();
  Queue *medium = new_queue();
  Queue *low = new_queue();
  Queue *finish = new_queue();
  int timer = -1;
  int active = -1;
  while(array->front == NULL || high->front == NULL || medium->front == NULL || low->front == NULL) {
    timer++;
    customer_arrival(array, high, medium, low, timer);

    // To be continued...

    if(high->front != NULL) {
      if(active == 1 || active == 2) {
        if(active == 1) {
          Node *node = deQueue(medium);
          enQueue(medium, node);
        }
        else if(active == 2) {
          Node *node = deQueue(low);
          enQueue(low, node);
        }
      }
      Customer *customer = high->front->customer;
      if(customer->ready == -1) {
        customer->ready = timer;
      }
      customer->total_ticks++;
      if(customer->total_ticks == customer->cpu_time) {
        customer->end = timer;
        Node *node = deQueue(high);
        enQueue(finish, node);
        active = -1;
      }
      else {
        customer->run_ticks++;
        if(customer->run_ticks == 6) {
          customer->run_ticks = 0;
          customer->total_runs++;
          if(customer->total_runs % 5 == 0) {
            customer->priority--;
            if(customer->priority < 5) {
              Node *node = deQueue(high);
              enQueue(medium, node);
              active = -1;
            }
            else {
              Node *node = deQueue(high);
              enQueue(high, node);
              sort_by_priority(high);
              active -1;
            }
          }
          else {
            Node *node = deQueue(high);
            enQueue(high, node);
            sort_by_priority(high);
            active = -1;
          }
        }
      }
    }
    else if(medium->front != NULL) {
      Customer *customer = medium->front->customer;
      if(customer->ready == -1) {
        customer->ready = timer;
      }
    }
    else if(low->front != NULL) {
      Customer *customer = low->front->customer;
      if(customer->ready == -1) {
        customer->ready = timer;
      }
    }
    return 0;
  }
  fclose(input);
  return 0;
}

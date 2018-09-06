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

void age_process(Queue *medium, Queue *low, int active) {
  Node *temp = medium->front;
  while(temp != NULL) {
    if(active == 1 && temp->next == NULL) {
      temp = temp->next;
    }
    else {
      temp->customer->age++;
      temp = temp->next;
    }
  }
  temp = low->front;
  while(temp != NULL) {
    if(active == 2 && temp->next == NULL) {
      temp = temp->next;
    }
    else {
      temp->customer->age++;
      temp = temp->next;
    }
  }
}

void check_age(Queue *medium, Queue *low) {
  Node *temp = medium->front;
  while(temp != NULL) {
    if(temp->customer->age == 8) {
      temp->customer->age = 0;
      temp->customer->priority++;
      if(temp->customer->priority == 5) {
        // Do annoying stuff
      }
    }
    temp = temp->next;
  }
  temp = low->front;
  while(temp != NULL) {
    if(temp->customer->age == 8) {
      temp->customer->age = 0;
      temp->customer->priority++;
      if(temp->customer->priority == 3) {
        // Do annoying stuff
      }
    }
    temp = temp->next;
  }
}

void customer_arrival(Queue *array, Queue *high, Queue *medium, Queue *low, int timer) {
  bool high_check = false;
  while(array->front->customer->arrival == timer) {
    if(array->front->customer->priority >= 5) {
      Node *node = deQueue(array);
      enQueue(high, node);
      high_check = true;
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
  if(high_check) {
    sort_by_priority(high);
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
  Node *node;
  Customer *customer;
  int timer = -1;
  int active = -1;
  while(array->front == NULL || high->front == NULL || medium->front == NULL || low->front == NULL) {
    // Increment timer and check for customer arrival
    timer++;
    customer_arrival(array, high, medium, low, timer);

    // High priority processing
    if(high->front != NULL) {
      // Interupt any lower priority active processes
      if(active == 1 || active == 2) {
        // Active medium process
        if(active == 1) {
          // DeQueue and increment total runs
          node = deQueue(medium);
          node->customer->total_runs++;
          // Check for decrease in priority condition
          if(node->customer->total_runs == 2) {
            node->customer->total_runs = 0;
            node->customer->priority--;
            // Check if it should be in lower queue or not
            if(node->customer->priority < 3) {
              enQueue(low, node);
            }
            else {
              enQueue(medium, node);
            }
          }
          // Else enQueue to the bottom of medium
          else {
            enQueue(medium, node);
          }
        }
        // Active low process
        else {
          node = deQueue(low);
          node->customer->total_runs++;
          enQueue(low, node);
        }
      }
      active = 0;
      customer = high->front->customer;
      customer->total_ticks++;
      customer->run_ticks++;
      customer->age = 0;
      // Check to see if this is customers first process
      if(customer->ready == -1) {
        customer->ready = timer;
      }
      // Current process route
      // Process completed route
      if(customer->total_ticks == customer->cpu_time) {
        customer->end = timer;
        node = deQueue(high);
        enQueue(finish, node);
        active = -1;
        age_process(medium, low, 0);
      }
      else {
        // End of run process
        if(customer->run_ticks == 5) {
          customer->run_ticks = 0;
          customer->total_runs++;
          active = -1;
          // Reduce priority check
          if(customer->total_runs == 5) {
            customer->total_runs = 0;
            customer->priority--;
            if(customer->priority < 5) {
              node = deQueue(high);
              enQueue(medium, node);
              age_process(medium, low, 1);
            }
            else {
              node = deQueue(high);
              enQueue(high, node);
              sort_by_priority(high);
              age_process(medium, low, 0);
            }
          }
          else {
            node = deQueue(high);
            enQueue(high, node);
            sort_by_priority(high);
            age_process(medium, low, 0);
          }
        }
      }
    }
    else if(medium->front != NULL) {
      if(active == 2) {
        node = deQueue(low);
        node->customer->total_runs++;
        enQueue(low, node);
      }
      active = 1;
      customer = medium->front->customer;
      customer->total_ticks++;
      customer->run_ticks++;
      customer->age = 0;
      // Check to see if this is customers first process
      if(customer->ready == -1) {
        customer->ready = timer;
      }
      // Current process route
      // Process completed route
      if(customer->total_ticks == customer->cpu_time) {
        customer->end = timer;
        node = deQueue(medium);
        enQueue(finish, node);
        active = -1;
        age_process(medium, low, 0);
      }
      else {
        // End of run process
        if(customer->run_ticks == 10) {
          customer->run_ticks = 0;
          customer->total_runs++;
          active = -1;
          // Reduce priority check
          if(customer->total_runs == 2) {
            customer->total_runs = 0;
            customer->priority--;
            if(customer->priority < 3) {
              node = deQueue(medium);
              enQueue(low, node);
              age_process(medium, low, 2);
            }
            else {
              node = deQueue(medium);
              enQueue(medium, node);
              age_process(medium, low, 1);
            }
          }
          else {
            node = deQueue(medium);
            enQueue(medium, node);
            age_process(medium, low, 1);
          }
        }
      }
    }
    else if(low->front != NULL) {
      active = 2;
      customer = low->front->customer;
      customer->total_ticks++;
      customer->run_ticks++;
      customer->age = 0;
      // Check to see if this is customers first process
      if(customer->ready == -1) {
        customer->ready = timer;
      }
      // Current process route
      // Process completed route
      if(customer->total_ticks == customer->cpu_time) {
        customer->end = timer;
        node = deQueue(low);
        enQueue(finish, node);
        active = -1;
        age_process(medium, low, 0);
      }
      else {
        // End of run process
        if(customer->run_ticks == 20) {
          customer->run_ticks = 0;
          customer->total_runs++;
          active = -1;
          node = deQueue(low);
          enQueue(low, node);
          age_process(medium, low, 2);
        }
      }
    }
  }
  fclose(input);
  return 0;
}

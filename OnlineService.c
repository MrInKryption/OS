// Operating Systems Group Assignment 1
// Brett Shelley (a1645904@student.adelaide.edu.au)
// Raymond Habis (a1631834@student.adelaide.edu.au)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Define true and false booleans
typedef enum { false, true } bool;

// Customer structure
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


// Node structure for Queues
typedef struct Node {
  struct Customer *customer;
  struct Node *next;
} Node;

// Queue structure
typedef struct Queue {
  struct Node *front;
  struct Node *rear;
} Queue;

// Creates a new Node with a Customer
Node *new_node(Customer *customer) {
  Node *node = (Node *)malloc(sizeof(Node));
  node->customer = customer;
  node->next = NULL;
  return node;
}

// Creates a new Queue
Queue *new_queue() {
  Queue *queue = (Queue *)malloc(sizeof(Queue));
  queue->front = NULL;
  queue->rear = NULL;
  return queue;
}

// enQueues a Node to a Queue
void enQueue(Queue *queue, Node *node) {
  node->next = NULL;
  if(queue->rear == NULL) {
    queue->front = node;
    queue->rear = node;
  }
  else {
    queue->rear->next = node;
    queue->rear = node;
  }
}

// deQueues the front Node of a Queue
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

// Sorts a Queues by its Customers arrival times
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
        if(curr == queue->front) {
          queue->front = curr->next;
        }
        else if(curr->next == queue->rear) {
          queue->rear = curr;
        }
        if(head != NULL) {
          head->next = next;
        }
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

// Sorts a Queues by its Customers priority
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
        if(curr == queue->front) {
          queue->front = curr->next;
        }
        else if(curr->next == queue->rear) {
          queue->rear = curr;
        }
        if(head != NULL) {
          head->next = next;
        }
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

// Increments a Customers age in the medium and low Queues if they have not just arrived
void age_process(Queue *medium, Queue *low, int active, int timer) {
  Node *temp = medium->front;
  while(temp != NULL) {
    if((active == 1 && temp->next == NULL) || temp->customer->arrival == timer) {
      temp = temp->next;
    }
    else {
      temp->customer->age++;
      temp = temp->next;
    }
  }
  temp = low->front;
  while(temp != NULL) {
    if((active == 2 && temp->next == NULL) || temp->customer->arrival == timer) {
      temp = temp->next;
    }
    else {
      temp->customer->age++;
      temp = temp->next;
    }
  }
}

// Checks the ages of the medium and low Queues to see if any priority increases occur
void check_age(Queue *high, Queue *medium, Queue *low) {
  Node *temp = medium->front;
  Node *head = NULL;
  while(temp != NULL) {
    if(temp->customer->age == 8) {
      temp->customer->age = 0;
      temp->customer->priority++;
      if(temp->customer->priority == 5) {
        if(head == NULL) {
          temp = deQueue(medium);
          temp->customer->total_runs = 0;
          enQueue(high, temp);
          temp = medium->front;
          head = NULL;
        }
        else {
          head->next = temp->next;
          if(temp->next == NULL) {
            medium->rear = head;
          }
          temp->next = NULL;
          enQueue(high, temp);
          temp = head;
        }
      }
      else {
        head = temp;
        temp = temp->next;
      }
    }
    else {
      head = temp;
      temp = temp->next;
    }
  }
  temp = low->front;
  head = NULL;
  while(temp != NULL) {
    if(temp->customer->age == 8) {
      temp->customer->age = 0;
      temp->customer->priority++;
      if(temp->customer->priority == 3) {
        if(head == NULL) {
          temp = deQueue(low);
          temp->customer->total_runs = 0;
          enQueue(medium, temp);
          temp = low->front;
          head = NULL;
        }
        else {
          head->next = temp->next;
          if(temp->next == NULL) {
            low->rear = head;
          }
          temp->next = NULL;
          enQueue(medium, temp);
          temp = head;
        }
      }
      else {
        head = temp;
        temp = temp->next;
      }
    }
    else {
      head = temp;
      temp = temp->next;
    }
  }
}

// Checks to see if any new higher priority Customer has interrupted a lower priority Customer
void check_interrupts(Queue *medium, Queue *low, int current, int active) {
  Node *node;
  if(current == 0) {
    if(active == 1) {
      node = deQueue(medium);
      node->customer->total_runs++;
      if(node->customer->total_runs == 2) {
        node->customer->total_runs = 0;
        node->customer->priority--;
        if(node->customer->priority == 2) {
          enQueue(low, node);
        }
        else {
          enQueue(medium, node);
        }
      }
      else {
        enQueue(medium, node);
      }
    }
    else if(active == 2) {
      node = deQueue(low);
      enQueue(low, node);
    }
  }
  else if(current == 1) {
    if(active == 2) {
      node = deQueue(low);
      enQueue(low, node);
    }
  }
}

// Checks for new customer arrivals at a time
void customer_arrival(Queue *array, Queue *high, Queue *medium, Queue *low, int timer) {
  bool high_check = false;
  while(array->front != NULL && array->front->customer->arrival == timer) {
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

// Returns the next Customer to be processed
Node *find_next_customer(Queue *high, Queue *medium, Queue *low) {
  if(high->front != NULL) {
    return high->front;
  }
  else if(medium->front != NULL) {
    return medium->front;
  }
  else if(low->front != NULL) {
    return low->front;
  }
  else {
    return NULL;
  }
}

// Returns the Queue number which the Node is in
int find_active(Node *node) {
  if(node == NULL) {
    return -1;
  }
  else if(node->customer->priority >= 5) {
    return 0;
  }
  else if(node->customer->priority >= 3 && node->customer->priority < 5) {
    return 1;
  }
  else if(node->customer->priority < 3) {
    return 2;
  }
}

// Outputs the final results of the finish Queue
void output(Queue *finish) {
  Node *node = finish->front;
  printf("Index Priority Arrival End Ready CPU_Time Waiting\n");
  while(node != NULL) {
    printf("%s %d %d %d %d %d %d\n", node->customer->id, node->customer->priority, node->customer->arrival, node->customer->end, node->customer->ready, node->customer->cpu_time, (node->customer->end - node->customer->ready - node->customer->cpu_time));
    node = node->next;
  }
}

// Frees all allocated memory from Queues, Nodes, and Customers
void free_all(Queue *array, Queue *high, Queue *medium, Queue *low, Queue *finish) {
  Node *node = array->front;
  Node *temp;
  while(node != NULL) {
    temp = node;
    node = node->next;
    free(temp->customer);
    free(temp);
  }
  free(array);
  node = high->front;
  while(node != NULL) {
    temp = node;
    node = node->next;
    free(temp->customer);
    free(temp);
  }
  free(high);
  node = medium->front;
  while(node != NULL) {
    temp = node;
    node = node->next;
    free(temp->customer);
    free(temp);
  }
  free(medium);
  node = low->front;
  while(node != NULL) {
    temp = node;
    node = node->next;
    free(temp->customer);
    free(temp);
  }
  free(low);
  node = finish->front;
  while(node != NULL) {
    temp = node;
    node = node->next;
    free(temp->customer);
    free(temp);
  }
  free(finish);
}

int main(int argC, char *argV[]) {
  // Opens an input file and creates new Customers from said input
  // Stores those Customers in Nodes and are added to a Queue
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
  fclose(input);

  // Sorts the arrival Queue by Customer arrival
  // Sets up all new Queues that will be used
  sort_by_arrival(array);
  Queue *high = new_queue();
  Queue *medium = new_queue();
  Queue *low = new_queue();
  Queue *finish = new_queue();
  Node *node = NULL;
  int timer = -1;
  int active = -1;

  // Loops while there are still Customers to arrive and to be processed
  while(array->front != NULL || high->front != NULL || medium->front != NULL || low->front != NULL) {
    // Increment timer
    timer++;

    // Increment variables of current active process
    if(node != NULL) {
      node->customer->total_ticks++;
      node->customer->run_ticks++;
      node->customer->age = 0;
    }

    // Check for new Customer arrivals
    customer_arrival(array, high, medium, low, timer);

    // High Queue processing
    if(high->front != NULL) {
      // Check for interrupts
      check_interrupts(medium, low, 0, active);

      // Set current active Queue
      active = 0;

      // Get Node at front of Queue
      node = high->front;

      // Set ready for Node if it is its first time being processed
      if(node != NULL && node->customer->ready == -1) {
        node->customer->ready = timer;
      }

      // Completed process
      if(node->customer->total_ticks == node->customer->cpu_time) {
        node = deQueue(high);
        node->customer->end = timer;
        enQueue(finish, node);
        age_process(medium, low, -1, timer);
        check_age(high, medium, low);
        node = find_next_customer(high, medium, low);
        active = find_active(node);
        if(node != NULL && node->customer->ready == -1) {
          node->customer->ready = timer;
        }
      }
      else {
        // End of process run
        if(node->customer->run_ticks == 5) {
          node = deQueue(high);
          node->customer->run_ticks = 0;
          node->customer->total_runs++;

          // Reduce priority check
          if(node->customer->total_runs == 5) {
            node->customer->total_runs = 0;
            node->customer->priority--;
            if(node->customer->priority == 4) {
              enQueue(medium, node);
              age_process(medium, low, 1, timer);
              check_age(high, medium, low);
              node = find_next_customer(high, medium, low);
              active = find_active(node);
              if(node != NULL && node->customer->ready == -1) {
                node->customer->ready = timer;
              }
            }
            else {
              enQueue(high, node);
              sort_by_priority(high);
              age_process(medium, low, 0, timer);
              check_age(high, medium, low);
              node = find_next_customer(high, medium, low);
              active = find_active(node);
              if(node != NULL && node->customer->ready == -1) {
                node->customer->ready = timer;
              }
            }
          }
          else {
            enQueue(high, node);
            sort_by_priority(high);
            age_process(medium, low, 0, timer);
            check_age(high, medium, low);
            node = find_next_customer(high, medium, low);
            active = find_active(node);
            if(node != NULL && node->customer->ready == -1) {
              node->customer->ready = timer;
            }
          }
        }
      }
    }

    // Medium Queue processing
    else if(medium->front != NULL) {
      // Check for interrupts
      check_interrupts(medium, low, 1, active);

      // Set current active Queue
      active = 1;

      // Get node at front of Queue
      node = medium->front;

      // Set ready for Node if it its first time being processed
      if(node != NULL && node->customer->ready == -1) {
        node->customer->ready = timer;
      }

      // Completed process
      if(node->customer->total_ticks == node->customer->cpu_time) {
        node = deQueue(medium);
        node->customer->end = timer;
        enQueue(finish, node);
        age_process(medium, low, -1, timer);
        check_age(high, medium, low);
        node = find_next_customer(high, medium, low);
        active = find_active(node);
        if(node != NULL && node->customer->ready == -1) {
          node->customer->ready = timer;
        }
      }
      else {
        // End of process run
        if(node->customer->run_ticks == 10) {
          node = deQueue(medium);
          node->customer->run_ticks = 0;
          node->customer->total_runs++;

          // Reduce priority check
          if(node->customer->total_runs == 2) {
            node->customer->total_runs = 0;
            node->customer->priority--;
            if(node->customer->priority == 2) {
              enQueue(low, node);
              age_process(medium, low, 2, timer);
              check_age(high, medium, low);
              node = find_next_customer(high, medium, low);
              active = find_active(node);
              if(node != NULL && node->customer->ready == -1) {
                node->customer->ready = timer;
              }
            }
            else {
              enQueue(medium, node);
              age_process(medium, low, 1, timer);
              check_age(high, medium, low);
              node = find_next_customer(high, medium, low);
              active = find_active(node);
              if(node != NULL && node->customer->ready == -1) {
                node->customer->ready = timer;
              }
            }
          }
          else {
            enQueue(medium, node);
            age_process(medium, low, 1, timer);
            check_age(high, medium, low);
            node = find_next_customer(high, medium, low);
            active = find_active(node);
            if(node != NULL && node->customer->ready == -1) {
              node->customer->ready = timer;
            }
          }
        }
      }
    }

    // Low Queue processing
    else if(low->front != NULL) {
      // Set current active Queue
      active = 2;

      // Get Node at front of Queue
      node = low->front;

      // Set ready for Node if it is its first time being processed
      if(node != NULL && node->customer->ready == -1) {
        node->customer->ready = timer;
      }

      // Completed process
      if(node->customer->total_ticks == node->customer->cpu_time) {
        node = deQueue(low);
        node->customer->end = timer;
        enQueue(finish, node);
        age_process(medium, low, -1, timer);
        check_age(high, medium, low);
        node = find_next_customer(high, medium, low);
        active = find_active(node);
        if(node != NULL && node->customer->ready == -1) {
          node->customer->ready = timer;
        }
      }
      else {
        // End of process run
        if(node->customer->run_ticks == 20) {
          node = deQueue(low);
          node->customer->run_ticks = 0;
          enQueue(low, node);
          age_process(medium, low, 2, timer);
          check_age(high, medium, low);
          node = find_next_customer(high, medium, low);
          active = find_active(node);
          if(node != NULL && node->customer->ready == -1) {
            node->customer->ready = timer;
          }
        }
      }
    }
  }

  // Output results
  // Free all allocated memory
  output(finish);
  free_all(array, high, medium, low, finish);
  return 0;
}

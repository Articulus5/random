#include <stdio.h>
#include <stdlib.h>

struct Node {
     int data;
     struct Node *next;
};

void print_list(struct Node *head)
{
     struct Node *cur = head;
     unsigned int i = 0;
     
     while (cur != NULL)
     {
          i++;
          printf("Node %u: data = %d | next = %p\n", i, cur->data, cur->next);
          cur = cur->next;
     }
}

void del_list(struct Node *head)
{
     struct Node *cur = head;
     struct Node *next;

     while (cur != NULL)
     {
          next = cur->next;
          free(cur);
          cur = next;
     }
}

struct Node *init_list(int *elements, unsigned int size)
{
     struct Node *head = NULL;
     struct Node *cur = NULL;
     struct Node *element = NULL;
     int i;

     // Return immediately if there is no data
     if (size <= 0) return NULL;

     element = malloc(sizeof(struct Node));
     if (element == NULL)
     {
          printf("Unable to allocate memory for list node.\n");
          return NULL;
     }

     head = element;
     cur = element;
     cur->data = elements[0];
     cur->next = NULL;

     for (i = 1; i < size; i++)
     {
          element = malloc(sizeof(struct Node));
          if (element == NULL)
          {
               printf("Unable to allocate memory for list node.\n");
               del_list(head);
               return NULL;
          }
          cur->next = element;
          element->data = elements[i];
          element->next = NULL;
          cur = element;
     }

     return head;
}

int add_node(struct Node *head, int data)
{
     struct Node *cur = head;
     struct Node *element = NULL;

     while (cur->next != NULL)
          cur = cur->next;

     element = malloc(sizeof(struct Node));
     if (element == NULL)
     {
          printf("Unable to allocate memory for list node.\n");
          return -1;
     }

     cur->next = element;
     element->data = data;
     element->next = NULL;

     return 0;
}

// Delete the first node found with value
int del_node(struct Node *head, int value)
{
     struct Node *cur = head;
     struct Node *element = NULL;

     // Look for the first node with data == value
     while (cur->next != NULL && cur->next->data != value)
          cur = cur->next;

     // If node is not found
     if (cur->next == NULL)
          return -1;
     else element = cur->next;

     // Update indexes
     cur->next = cur->next->next;

     // Delete node
     free(element);

     return 0;
}

struct Node *reverse_list(struct Node *head)
{
     struct Node *prev = NULL;
     struct Node *cur = head;
     struct Node *next = head->next;

     while (next != NULL)
     {
          cur->next = prev;
          prev = cur;
          cur = next;
          next = cur->next;
     }
     cur->next = prev;

     return cur;
}

int main(void)
{
     int data[] = {1, 2, 3, 4, 5, 6, 7, 8};
     unsigned int size = sizeof(data)/sizeof(int);
     struct Node *head = init_list(data, size);

     // Reverse list
     head = reverse_list(head);

     // Add a node
     add_node(head, 9);

     // Print the list
     print_list(head);
     printf("\n");

     // Remove the first node with a value of 4
     del_node(head, 4);
     print_list(head);

     // Clean up
     if (head != NULL)
          del_list(head);

     return 0;
}

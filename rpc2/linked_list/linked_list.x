struct node
{
   int data;
   node *next;
};

struct list
{ 
   node *head1;
   node *head2;
};

program LINKEDLIST {
   version LINKEDLIST {
      node LINKED_LIST(list) = 1;
   } = 1;
} = 0x0000000A;

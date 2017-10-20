/*
*  uiplist.c - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#include "project.h"
#include "uiplist.h"

/* This creates and returns a new uipList element. */
uipList uiplNewList()
{
  uipList l;

  l = (uipList)malloc(sizeof(struct _uipList));
  l->first = NULL;
  l->last = NULL;
  l->current = NULL;
  l->numelts = 0;
  return l;
}

/*  This frees all memory associated with the list. */
void uiplFreeList(uipList list)
{
  uipNode n, last;

  n = list->first;
  while (n != NULL) {
    last = n;
    n = n->next;
    free(last);
  }
  free(list);
}


/* These Append/Prepend elt to list. */
void uiplAppend(uipList list, UIPoint *elt)
{
  uipNode n;

  n = (uipNode)malloc(sizeof(struct _uipNode));
  n->elt = elt;
  n->next = NULL;
  if (list->first == NULL) {
    list->first = n;
    list->last = n;
  } else {
    list->last->next = n;
    list->last = n;
  }
  list->numelts++;
}


void uiplPrepend(uipList list, UIPoint *elt)
{
  uipNode n;

  n = (uipNode)malloc(sizeof(struct _uipNode));
  n->elt = elt;
  n->next = list->first;
  if (list->first == NULL) {
    list->first = n;
    list->last = n;
  } else {
    list->first = n;
  }
  list->numelts++;
}

/* These return the first/last element of the list. */ 
/* These require that the list have a first/last element */
UIPoint *uiplFirst(uipList list)
{
  if (list->first == NULL)
    Error("uiplFirst received an empty list.");
  return(list->first->elt);
}


UIPoint *uiplLast(uipList list)
{
  if (list->first == NULL)
    Error("uiplLast received an empty list.");
  return(list->last->elt);
}



/* This removes the first element of the list and returns it. */ 
/* It requires that the list have a first element */
UIPoint *uiplDelFirst(uipList list)
{
  uipNode temp;
  UIPoint *elt;

  if (list->first == NULL)
    Error("uiplDelFirst received an empty list.");
  temp = list->first;
  list->first = list->first->next;
  if (list->last == temp)
    list->last = NULL;
  elt = temp->elt;
  free(temp);
  list->numelts--;
  return(elt);
}


/* This appends the elements of second to first and removes them 
   from second (it does not destroy second). */
void uiplCat(uipList first, uipList second)
{
  first->last->next = second->first;
  first->last = second->last;
  second->first = NULL;
  second->last = NULL;
  first->numelts += second->numelts;
  second->numelts = 0;
}

/* These are standard iterator commands to be used in a for loop.
   They iterate over all elements of the list.  (DO NOT NEST THESE!) */
UIPoint *uiplIterFirst(uipList list)
{
  list->current = list->first;
  if (list->current)
    return(list->current->elt);
  else
    return(0);
}


int uiplIterMore(uipList list)
{
  if (list->current)
    return(1);
  else
    return(0);
}


UIPoint *uiplIterNext(uipList list)
{
  list->current = list->current->next;
  if (list->current)
    return(list->current->elt);
  else
    return(0);
}

/* This is a test that returns true iff there are no elements in the list */
int uiplEmpty(uipList list)
{
  return(list->first == NULL);
}



/* This returns the number of elements in "list." */
int uiplNumElts(uipList list)
{
  return list->numelts;
}

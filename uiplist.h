/*
*  uiplist.h - for HBSURF hierarchical B-Spline Surface Modeler
*
*  copyright 1996 by  Richard C. Davis and Yizhou Yu
*
*  All rights reserved.  You are free to copy, dissect, reuse, or do
*  whatever you want with this code except for selling it for profit
*  without our consent.  Please understand that it comes with NO
*  WARRANTY!  Use at your own risk!
*/

#ifndef __UIPLIST_H__
#define __UIPLIST_H__

#include <stdlib.h>
#include "project.h"
#include "error.h"

typedef struct _uipNode {
  UIPoint *elt;
  struct _uipNode *next;
} *uipNode;

typedef struct _uipList {
  uipNode first;
  uipNode last;
  uipNode current;
  int numelts;
} *uipList;

/* This creates and returns a new uipList element. */
uipList uiplNewList();

/*  This frees all memory associated with list. */
void uiplFreeList(uipList list);


/* These Append/Prepend elt to list. */
void uiplAppend(uipList list, UIPoint *elt);
void uiplPrepend(uipList list, UIPoint *elt);

/* These return the first/last element of the list. */ 
/* These require that the list have a first/last element */
UIPoint *uiplFirst(uipList list);
UIPoint *uiplLast(uipList list);

/* This removes the first element of the list and returns it. */ 
/* It requires that the list have a first element */
UIPoint *uiplDelFirst(uipList list);

/* This appends the elements of second to first and removes them 
   from second (it does not destroy second). */
void uiplCat(uipList first, uipList second);

/* These are standard iterator commands to be used in a for loop.
   They iterate over all elements of the list.  (DO NOT NEST THESE!) */
UIPoint *uiplIterFirst(uipList list);
int uiplIterMore(uipList list);
UIPoint *uiplIterNext(uipList list);

/* This is a test that returns true iff there are no elements in the list */
int uiplEmpty(uipList list);

/* This returns the number of elements in "list." */
int uiplNumElts(uipList list);




#endif /* __UIPLIST_H__ */

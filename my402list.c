/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 warmup programming assignments at USC.       */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: listtest.c,v 1.1 2015/08/21 03:52:20 william Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "cs402.h"

#include "my402list.h"


/*---------------------Function Declarations--------------*/

/*----------Length() -------------*/
/* Returns the length of the list*/
int  My402ListLength(My402List *pList){

	
	return pList->num_members;

	
}


/*-----------Empty()-------------*/
/* Returns TRUE if the list is empty . Returns FALSE otherwise. */
int  My402ListEmpty(My402List *pList){
	
	if(pList->num_members==0){
	return TRUE;
	}
	
	else {
	return FALSE;
	}
}

/*--------------Append()------------*/
/* If List is empty just add Obj to the List. Otherwise add obj after Last(). 
 * Return TRUE if operation performed sucessfully, FALSE otherwise.*/
 
int  My402ListAppend(My402List *pList, void *obj){
	
	My402ListElem *elem;
	My402ListElem *lastElem;
	
	My402ListElem *anchorElem= &(pList->anchor);

	if(pList!=NULL){
	if(My402ListEmpty(pList)){
			
		elem=(My402ListElem*)(malloc(sizeof(My402ListElem)));
		pList->anchor.next=elem;
		pList->anchor.prev=elem;
		elem->next=anchorElem;
		elem->prev=anchorElem;
		elem->obj=obj;
		pList->num_members++;
		
	}
	else{
		
			lastElem= My402ListLast(pList);
			return My402ListInsertAfter(pList,obj,lastElem);		
	}
	
	return TRUE;
}
else{
		
	return FALSE;
}
}

/*---------------Prepend()-------------------------*/
/*If List is empty just obj to the list .Otherwise add obj before First().
 * Returns TRUE if operation performed sucessfuly else return FALSE */

int  My402ListPrepend(My402List *pList, void *obj){

	My402ListElem *elem;
	My402ListElem *firstElem;
	
	My402ListElem *anchorElem= &(pList->anchor);

	if(pList!=NULL){
	if(My402ListEmpty(pList)){
			
		elem=(My402ListElem*)(malloc(sizeof(My402ListElem)));
		pList->anchor.next=elem;
		elem->next=anchorElem;
		elem->obj=obj;
		pList->num_members++;
		
	}
	else{
		
		elem=(My402ListElem*)malloc(sizeof(My402ListElem));
			firstElem= My402ListFirst(pList);
			return My402ListInsertBefore(pList,obj,firstElem);		
	}
	
	return TRUE;
}
else{
		
	return FALSE;
}	
	
}

/*---------------Unlink------------------*/
/*Unlink and delete element from list. Please do not delete the object pointed to by the element. 
 * Do not check if elem is on the list */
void My402ListUnlink(My402List *pList, My402ListElem *elem){
	
	

	My402ListElem *prevElem=elem->prev;
	My402ListElem *nextElem=elem->next;
	prevElem->next=nextElem;
	nextElem->prev=prevElem;
	
	pList->num_members--;
	free(elem);
	
}

/*-----------------UnlinkAll-------------*/
/*Unlink and delete all the elements from the list and make the list empty. 
 * Do not delete the objects pointed to by the list elements.*/
void My402ListUnlinkAll(My402List *pList){
	
	My402ListElem *first;
	while(pList->num_members!=0){
			first=My402ListFirst(pList);
			My402ListUnlink(pList, first);
	}
	
	return;
}

/*----------InsertAfter()----------------*/
/*Insert obj between elem and elem->next. If elem is NULL then is same as Append().
 * Returns TRUE if sucessful FALSE otherwise*/
int My402ListInsertAfter(My402List *pList, void *obj, My402ListElem *elem){
	
	if(elem==NULL){
	My402ListAppend(pList,obj);
	return TRUE;
		
	}
	else{
	
	My402ListElem *insertElem;
	My402ListElem *nextElem=elem->next;;
	
	insertElem=(My402ListElem*)malloc(sizeof(My402ListElem));
	
	
	
	insertElem->obj=obj;
	elem->next=insertElem;
	insertElem->prev=elem;
	insertElem->next=nextElem;	
	nextElem->prev=insertElem;
	
	
	pList->num_members++;
	}

	return TRUE;
}

/*----------InsertBefore()----------------*/
/*Insert obj between elem and elem->prev. If elem is NULL then is same as Prepend().
 * Returns TRUE if sucessful FALSE otherwise*/

int  My402ListInsertBefore(My402List *pList, void *obj, My402ListElem *elem){
	
	if(elem==NULL){
	My402ListPrepend(pList,obj);
	return TRUE;
		
	}
	else{
	
	My402ListElem *insertElem;
	My402ListElem *prevElem=elem->prev;;
	
	insertElem=(My402ListElem*)malloc(sizeof(My402ListElem));
	

	insertElem->obj=obj;
	
	insertElem->prev=prevElem;
	insertElem->next=elem;
	
	elem->prev=insertElem;
	prevElem->next=insertElem;
	
	pList->num_members++;
	}

	return TRUE;
	
	
}

/*------------First()---------------------*/
/*Returns first element of the list or NULL if the list is empty*/
My402ListElem *My402ListFirst(My402List *pList){
	
	My402ListElem *elem=NULL;
	
	if(My402ListEmpty(pList)){
		return elem;	
	}
	
	else{
	 
	 
	 elem=pList->anchor.next;
		
	return elem;
	}
}

/*----------------Last()-------------------*/
/*Returns last element of the list or NULL if the list is empty*/
My402ListElem *My402ListLast(My402List *pList){
	
	
	My402ListElem *elem=NULL;
	//My402ListElem *anchorElem= &pList->anchor;
	if(My402ListEmpty(pList)){
	return elem;	
	}
	
	else{
	
		return pList->anchor.prev;
		
		
	}
}

/*-------------Next()----------------------------*/
/*Returns elem->next or NULL if elem is the last item on the list */
My402ListElem *My402ListNext(My402List *pList, My402ListElem *elem){
	
	My402ListElem *lastElem=My402ListLast(pList);
	if(elem==lastElem){
		return NULL;
	
	}
	else
	{
	
	return elem->next;		
	
	}
}
/*-------Prev()---------------*/
/*Returns elem->prev or NULL if elem is the first element on the list*/
My402ListElem *My402ListPrev(My402List *pList, My402ListElem *elem){
	
	My402ListElem *firstElem=My402ListFirst(pList);
	if(elem==firstElem){
		return NULL;
	}
	else
	{
	return elem->prev;
	}
}

/*--------------Find()-------------------*/
/* Returns the list element elem such that elem->obj ==obj. Returns NULL if no such element is found.*/
My402ListElem *My402ListFind(My402List *pList, void *obj){
	
	int i=0;
	My402ListElem *elem=My402ListFirst(pList);
	for (i=0;i<pList->num_members;i++){
		if(elem->obj==obj){
			return elem;
		}
		else{
			elem=elem->next;
	}
		
	}
	
	return NULL;
}

int My402ListInit(My402List *pList){
	
	if(pList!=NULL){
	
	My402ListElem *anchorElem= &pList->anchor;
	
	
	pList->anchor.obj=NULL;
	/*Make the anchor next and prev point to itself*/
	pList->anchor.next=anchorElem;
	pList->anchor.prev=anchorElem;
		
	/*Set num_members to zero*/
	pList->num_members=0;	
	
	/*Set fields of DS to known values*/
	
	
	return TRUE;
	}
	else 
	return FALSE;
}

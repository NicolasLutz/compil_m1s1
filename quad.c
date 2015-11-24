#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad.h"

Quad* Q_gen(char op, Symbol* arg1, Symbol* arg2, Symbol* res)
{
	Quad* new = malloc(sizeof(*new));
	new->op = op;
	new->arg1 = arg1;
	new->arg2 = arg2;
	new->res = res;
	new->next = NULL;
	return new;
}

void QL_add (QuadList *ql, Quad* new)
{
	assert(ql!=NULL);
	if(!ql->head)
	{
		ql->head=new;
		ql->tail=new;
	}
	else
	{
		ql->tail->next=new;
		ql->tail=new;
	}
	return;
}

void QL_print (QuadList *ql)
{
	Quad *q=ql->head;
	assert(ql!=NULL);
	while(q!=NULL)
	{
		printf("%c %7s %7s %7s\n",
						q->op,
						(q->arg1 ? q->arg1->name : "None"),
						(q->arg2 ? q->arg2->name : "None"),
						(q->res  ? q->res->name  : "None"));
		q=q->next;
	}
	return;
}

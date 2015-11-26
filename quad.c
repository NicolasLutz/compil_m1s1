#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "quad.h"

Quad* Q_gen(Instruction op, Symbol* arg1, Symbol* arg2, Symbol* res)
{
    Quad* q = malloc(sizeof(*q));
    q->op       = op;
    q->arg1     = arg1;
    q->arg2     = arg2;
    q->res      = res;
    q->next     = NULL;
    return q;
}

Quad *Q_genGoto()
{
    Quad* q = malloc(sizeof(*q));
    q->op       = GOTO_I;
    q->arg1     = NULL;
    q->arg2     = NULL;
    q->res      = NULL;
    return q;
}

Quad *Q_concat(Quad *q1, Quad *q2)
{
    assert(q1->next==NULL);
    q1->next=q2;
    return q1;
}

//================================================================================================

QuadList *QL_gen()
{
    QuadList *ql=malloc(sizeof(*ql));
    ql->head=NULL;
    ql->tail=NULL;
    return ql;
}

void QL_add (QuadList *ql, Quad* quad)
{
	assert(ql!=NULL);
	if(!ql->head)
	{
        ql->head=quad;
        ql->tail=quad;
	}
	else
	{
        ql->tail->next=quad;
        ql->tail=quad;
	}
	return;
}

void QL_print (QuadList *ql)
{
    assert(ql!=NULL);
	Quad *q=ql->head;
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

//================================================================================================

//Usage example
/*
int main()
{
    QuadList *ql=QL_gen();
    return 0;
}
*/

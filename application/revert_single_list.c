#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "utility.h"

struct list {
	const char *string;
	struct list *next;
};

struct list *l_add(struct list *head, const char *string)
{
	struct list *new, *p = head;

	GODIE_ON(string == NULL, "invalid string");

	new = MALLOC(sizeof(*new));
	new->string = string;
	new->next = NULL;

	if (!head)
		return new;

	while (p->next)
		p = p->next;

	p->next = new;

	return head;
}

void l_show(struct list *head)
{
	struct list *p = head;

	GODIE_ON(head == NULL, "invalid head");

	while (p) {
		P("--> %s ", p->string);
		p = p->next;
	}
	P("\n");
}

void l_free(struct list *head)
{
	struct list *p;

	GODIE_ON(head == NULL, "invalid head");

	p = head;
	while (head) {
		head = head->next;
		FREE(p);
		p = head;
	}
}

struct list *l_revert(struct list *head)
{
	struct list *new_head, *p;

	GODIE_ON(head == NULL, "invalid head");

	if (head->next == NULL)
		return head;

	new_head = head;
	p = head->next;
	head->next = NULL;

	while (p) {
		head = p->next;
		p->next = new_head;
		new_head = p;
		p = head;
	}
	return new_head;
}

struct list *build_list(struct list *head)
{
	const char *strings[] = {
		"hello",
		"world",
		"platform",
		"guys",
		"is",
		"good",
		NULL
	};
	const char **p = strings;

	while (*p) {
		head = l_add(head, *p);
		*p++;
	}

	return head;
}

int main(int argc, char *argv[])
{
	struct list *head = NULL;

	head = build_list(head);
	l_show(head);
	head = l_revert(head);
	l_show(head);
	l_free(head);
	return 0;
}

/* gcc revert_single_list.c -pthread -o revert_single_list */

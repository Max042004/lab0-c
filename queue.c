#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "queue.h"

/* Create an empty queue */
struct list_head *q_new()
{
    struct list_head *new_q = malloc(sizeof(struct list_head));

    if (new_q) {
        INIT_LIST_HEAD(new_q);
        return new_q;
    } else
        return new_q;
}

/* Free all storage used by queue */
void q_free(struct list_head *head)
{
    if (!head)
        return;

    element_t *curr = NULL, *n;
    list_for_each_entry_safe (curr, n, head, list) {
        list_del(&curr->list);
        q_release_element(curr);
    }
    free(head);
}

/* Insert an element to the queue */
bool q_insert(struct list_head *head,
              const char *s,
              void (*op)(struct list_head *, struct list_head *))
{
    if (!head || !s)
        return false;

    element_t *new_element = malloc(sizeof(element_t));
    if (!new_element)
        return false;

    new_element->value = strdup(s);
    if (!new_element->value) {
        free(new_element);
        return false;
    }
    op(&new_element->list, head);
    free(new_element);
    return true;
}

/* Insert an element at head of queue */
bool q_insert_head(struct list_head *head, const char *s)
{
    return q_insert(head, s, list_add);
}

/* Insert an element at tail of queue */
bool q_insert_tail(struct list_head *head, const char *s)
{
    return q_insert(head, s, list_add_tail);
}

/* Remove an element from head of queue */
element_t *q_remove_head(const struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp;
    tmp = list_first_entry(head, element_t, list);
    if (sp)
        snprintf(sp, bufsize, "%s", tmp->value);
    list_del(&tmp->list);
    return tmp;
}

/* Remove an element from tail of queue */
element_t *q_remove_tail(const struct list_head *head, char *sp, size_t bufsize)
{
    if (!head || list_empty(head))
        return NULL;
    element_t *tmp;
    tmp = list_last_entry(head, element_t, list);
    if (sp)
        snprintf(sp, bufsize, "%s", tmp->value);
    list_del(&tmp->list);
    return tmp;
}

/* Return number of elements in queue */
int q_size(struct list_head *head)
{
    if (!head)
        return 0;

    int len = 0;
    struct list_head *li;

    list_for_each (li, head)
        len++;
    return len;
}

/* Delete the middle node in queue */
bool q_delete_mid(struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *tmp;
    struct list_head *forward, *n_forward, *backward = head->prev;

    list_for_each_safe (forward, n_forward, head) {
        if (forward == backward) {
            tmp = list_entry(forward, element_t, list);
            list_del(forward);
            q_release_element(tmp);
            break;
        } else if (n_forward == backward) {
            tmp = list_entry(n_forward, element_t, list);
            list_del(n_forward);
            q_release_element(tmp);
            break;
        }
        backward = backward->prev;
    }
    return true;
}

/* Delete all nodes that have duplicate string */
bool q_delete_dup(const struct list_head *head)
{
    if (!head || list_empty(head))
        return false;

    element_t *curr = NULL, *next = NULL;
    bool dul = false;
    list_for_each_entry_safe (curr, next, head, list) {
        if (curr->list.next != head && strcmp(curr->value, next->value) == 0) {
            list_del(&curr->list);
            q_release_element(curr);
            dul = true;
        } else if (dul) {
            list_del(&curr->list);
            q_release_element(curr);
            dul = false;
        }
    }

    return true;
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    // https://leetcode.com/problems/swap-nodes-in-pairs/
}

/* Reverse elements in queue */
void q_reverse(struct list_head *head) {}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    // https://leetcode.com/problems/reverse-nodes-in-k-group/
}

/* Sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend) {}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    // https://leetcode.com/problems/remove-nodes-from-linked-list/
    return 0;
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    // https://leetcode.com/problems/merge-k-sorted-lists/
    return 0;
}

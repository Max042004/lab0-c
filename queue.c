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

/* Reverse elements in queue */
void q_reverse(struct list_head *head)
{
    if (!head || list_empty(head))
        return;

    struct list_head *curr, *next;
    list_for_each_safe (curr, next, head) {
        list_move(curr, head);
    }
}

/* Reverse the nodes of the list k at a time */
void q_reverseK(struct list_head *head, int k)
{
    if (!head || list_empty(head) || k < 1)
        return;
    struct list_head *curr, *next, *tmp = head;
    int i = 0;

    LIST_HEAD(dummy);

    list_for_each_safe (curr, next, head) {
        i++;
        if (i == k) {
            list_cut_position(&dummy, tmp, curr);
            q_reverse(&dummy);
            list_splice_init(&dummy, tmp);
            tmp = next->prev;
            i = 0;
        }
    }
}

/* Swap every two adjacent nodes */
void q_swap(struct list_head *head)
{
    q_reverseK(head, 2);
}

/* Merge two sorted lists li1 and li2 into li1.
 * When elements compare equal, the one from li1 is chosen first.
 * The 'descend' flag selects sort order.
 */
static void _merge(struct list_head *li1, struct list_head *li2, bool descend)
{
    LIST_HEAD(tmp);

    while (!list_empty(li1) && !list_empty(li2)) {
        element_t *ele_1 = list_first_entry(li1, element_t, list);
        element_t *ele_2 = list_first_entry(li2, element_t, list);
        int cmp = strcmp(ele_1->value, ele_2->value);

        if (descend) {
            if (cmp >= 0)
                list_move_tail(&ele_1->list, &tmp);
            else
                list_move_tail(&ele_2->list, &tmp);
        } else {
            if (cmp <= 0)
                list_move_tail(&ele_1->list, &tmp);
            else
                list_move_tail(&ele_2->list, &tmp);
        }
    }
    if (!list_empty(li1))
        list_splice_tail_init(li1, &tmp);
    if (!list_empty(li2))
        list_splice_tail_init(li2, &tmp);

    list_splice_init(&tmp, li1);
}

/* Merge sort elements of queue in ascending/descending order */
void q_sort(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return;

    int size = 0;
    struct list_head *pos;
    list_for_each (pos, head) {
        size++;
    }
    if (size < 2)
        return;

    int mid = size / 2;

    LIST_HEAD(left);
    LIST_HEAD(right);

    for (int i = 0; i < mid; i++) {
        list_move_tail(head->next, &left);
    }
    list_splice_init(head, &right);

    q_sort(&left, descend);
    q_sort(&right, descend);

    _merge(&left, &right, descend);

    list_splice_init(&left, head);
}

/* Remove every node which has a node with a strictly less value anywhere to
 * the right side of it */
int q_ascend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *curr = head->prev;
    while (curr->prev != head) {
        if (strcmp(list_entry(curr, element_t, list)->value,
                   list_entry(curr->prev, element_t, list)->value) < 0) {
            struct list_head *tmp = curr->prev;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        } else {
            curr = curr->prev;
        }
    }

    return q_size(head);
}

/* Remove every node which has a node with a strictly greater value anywhere to
 * the right side of it */
int q_descend(struct list_head *head)
{
    if (!head || list_empty(head))
        return 0;

    struct list_head *curr = head->prev;
    while (curr->prev != head) {
        if (strcmp(list_entry(curr, element_t, list)->value,
                   list_entry(curr->prev, element_t, list)->value) > 0) {
            struct list_head *tmp = curr->prev;
            list_del(tmp);
            q_release_element(list_entry(tmp, element_t, list));
        } else {
            curr = curr->prev;
        }
    }

    return q_size(head);
}

/* Merge all the queues into one sorted queue, which is in ascending/descending
 * order */
int q_merge(struct list_head *head, bool descend)
{
    if (!head || list_empty(head) || list_is_singular(head))
        return 0;

    queue_contex_t *queue_head;
    if (descend) {
        queue_head = container_of(head->prev, queue_contex_t, chain);

        for (struct list_head *curr = head->prev->prev; curr != head;
             curr = curr->prev) {
            queue_contex_t *queue = container_of(curr, queue_contex_t, chain);
            _merge(queue_head->q, queue->q, descend);
            INIT_LIST_HEAD(queue->q);
            queue->size = 0;
        }
    } else {
        queue_head = container_of(head->next, queue_contex_t, chain);
        for (struct list_head *curr = head->next->next; curr != head;
             curr = curr->next) {
            queue_contex_t *queue = container_of(curr, queue_contex_t, chain);
            _merge(queue_head->q, queue->q, descend);
            INIT_LIST_HEAD(queue->q);
            queue->size = 0;
        }
    }

    return q_size(queue_head->q);
}

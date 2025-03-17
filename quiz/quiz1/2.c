#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct block {
    size_t size;
    struct block *l, *r;
} block_t;

block_t **find_free_tree(block_t **root, block_t *target);
block_t *find_predecessor_free_tree(block_t **root, block_t *node);

/*
 * Structure representing a free memory block in the memory allocator.
 * The free tree is a binary search tree that organizes free blocks (of type
 * block_t) to efficiently locate a block of appropriate size during memory
 * allocation.
 */
void remove_free_tree(block_t **root, block_t *target)
{
    /* Locate the pointer to the target node in the tree. */
    block_t **node_ptr = find_free_tree(root, target);

    /* If the target node has two children, we need to find a replacement. */
    if ((*node_ptr)->l && (*node_ptr)->r) {
        /* Find the in-order predecessor:
         * This is the rightmost node in the left subtree.
         */
        block_t **pred_ptr = &(*node_ptr)->l;
        while ((*pred_ptr)->r)
            pred_ptr = &(*pred_ptr)->r;

        /* Verify the found predecessor using a helper function (for debugging).
         */
        /* cppcheck-suppress constVariablePointer */
        block_t *expected_pred = find_predecessor_free_tree(root, *node_ptr);
        assert(expected_pred == *pred_ptr);

        /* If the predecessor is the immediate left child. */
        if (*pred_ptr == (*node_ptr)->l) {
            block_t *old_right = (*node_ptr)->r;
            *node_ptr = *pred_ptr; /* Replace target with its left child. */
            (*node_ptr)->r = old_right; /* Attach the original right subtree. */
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        } else {
            /* The predecessor is deeper in the left subtree. */
            block_t *old_left = (*node_ptr)->l;
            block_t *old_right = (*node_ptr)->r;
            block_t *pred_node = *pred_ptr;
            /* Remove the predecessor from its original location. */
            remove_free_tree(&old_left, *pred_ptr);
            /* Replace the target node with the predecessor. */
            *node_ptr = pred_node;
            (*node_ptr)->l = old_left;
            (*node_ptr)->r = old_right;
            assert(*node_ptr != (*node_ptr)->l);
            assert(*node_ptr != (*node_ptr)->r);
        }
    }
    /* If the target node has one child (or none), simply splice it out. */
    else if ((*node_ptr)->l || (*node_ptr)->r) {
        block_t *child = ((*node_ptr)->l) ? (*node_ptr)->l : (*node_ptr)->r;
        *node_ptr = child;
    } else {
        /* No children: remove the node. */
        *node_ptr = NULL;
    }
    /* Clear the removed node's child pointers to avoid dangling references. */
    target->l = NULL;
    target->r = NULL;
}

/* cppcheck-suppress constParameterPointer */
block_t **find_free_tree(block_t **root, block_t *target)
{
    if (!(*root))
        return root;

    block_t **node_ptr = root;
    while (node_ptr != NULL && target->size != (*node_ptr)->size) {
        if (target->size > (*node_ptr)->size) {
            *node_ptr = (*node_ptr)->r;
        } else {
            *node_ptr = (*node_ptr)->l;
        }
    }
    return node_ptr;
}

block_t *find_predecessor_free_tree(block_t **root, block_t *node)
{
    /* 若 node 有左子樹，就直接往左子樹走到底，再一直往右走到最右。 */
    if (node->l) {
        block_t *p = node->l;
        while (p->r)
            p = p->r;
        return p;
    }

    /* 若沒有左子樹：從根開始找「最後一個比 node->size 小的節點」。 */
    block_t *predecessor = NULL;
    block_t *current = *root;
    while (current) {
        if (node->size > current->size) {
            /* candidate: current 可能是前驅 */
            predecessor = current;
            current = current->r; /* 嘗試在右子樹找到更接近 node->size 的值 */
        } else if (node->size < current->size) {
            current = current->l; /* 往左子樹找更小的值 */
        } else {
            /* current->size == node->size 時直接中斷 */
            break;
        }
    }
    return predecessor;
}


block_t *create_block(size_t size)
{
    block_t *b = malloc(sizeof(block_t));
    if (!b) {
        perror("malloc");
        exit(1);
    }
    b->size = size;
    b->l = b->r = NULL;
    return b;
}

void insert_block(block_t **root, block_t *node)
{
    if (!*root) {
        *root = node;
        return;
    }
    if (node->size < (*root)->size) {
        insert_block(&(*root)->l, node);
    } else {
        insert_block(&(*root)->r, node);
    }
}

void print_inorder(const block_t *root)
{
    if (!root)
        return;
    print_inorder(root->l);
    printf("%zu ", root->size);
    print_inorder(root->r);
}

int main(void)
{
    block_t *root = NULL;

    /* mock BST */
    size_t sizes[] = {10, 7, 15, 12, 18, 5};
    int n = sizeof(sizes) / sizeof(sizes[0]);
    for (int i = 0; i < n; i++) {
        block_t *b = create_block(sizes[i]);
        insert_block(&root, b);
    }

    printf("Initial : ");
    print_inorder(root);
    printf("\n");

    /* delete size = 10 */
    block_t dummy_target;
    dummy_target.size = 10;
    remove_free_tree(&root, &dummy_target);

    printf("after removing size=10: ");
    print_inorder(root);
    printf("\n");

    /* delete size=7 */
    dummy_target.size = 7;
    remove_free_tree(&root, &dummy_target);

    printf("after removing size=7: ");
    print_inorder(root);
    printf("\n");

    return 0;
}
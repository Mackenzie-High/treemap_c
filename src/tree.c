#include "tree.h"

typedef struct
{
    size_t allocated;

    size_t capacity;

    tree_node_t* free;

} tree_allocator_pooled_t;

typedef struct
{
    size_t capacity;

    tree_node_t* free;

    tree_node_t nodes[];

} tree_allocator_slab_t;

static int32_t height_of (tree_node_t* node)
{
    return NULL == node ? 0 : node->height;
}

static size_t size_of (tree_node_t* node)
{
    return NULL == node ? 0 : node->size;
}

static int8_t balance_of (tree_node_t* node)
{
    if (NULL == node)
    {
        return 0;
    }
    else
    {
        return height_of(node->left) - height_of(node->right);
    }
}

static tree_node_t* create_node (tree_t* self, key_t* key)
{
    tree_node_t* node = self->allocator->allocate(self->allocator);

    if (NULL == node)
    {
        return NULL;
    }
    else
    {
        ++self->size;
        node->key = *key;
        node->height = 0;
        node->size = 1;
        node->left = NULL;
        node->right = NULL;
        return node;
    }
}

static int8_t max (int8_t x, int8_t y)
{
    return x > y ? x : y;
}

static void update_size (tree_node_t* node)
{
    if (NULL != node)
    {
        node->size = 1 + size_of(node->left) + size_of(node->right);
    }
}

static void update_height (tree_node_t* node)
{
    if (NULL != node)
    {
        if ((NULL == node->left) && (NULL == node->right))
        {
            node->height = 0;
        }
        else if (NULL == node->left)
        {
            node->height = 1 + node->right->height;
        }
        else if (NULL == node->right)
        {
            node->height = 1 + node->left->height;
        }
        else
        {
            node->height = 1 + max(node->left->height, node->right->height);
        }
    }
}

static tree_node_t* rotate_right (tree_node_t* y)
{
    if (NULL == y || NULL == y->left)
    {
        return y;
    }

    tree_node_t* x = y->left;
    tree_node_t* z = x->right;

    x->right = y;
    y->left = z;

    update_height(z);
    update_height(y);
    update_height(x);

    update_size(z);
    update_size(y);
    update_size(x);

    return x;
}

static tree_node_t* rotate_left (tree_node_t* x)
{
    if (NULL == x || NULL == x->right)
    {
        return x;
    }

    tree_node_t* y = x->right;
    tree_node_t* z = y->left;

    y->left = x;
    x->right = z;

    update_height(z);
    update_height(x);
    update_height(y);

    update_size(z);
    update_size(x);
    update_size(y);

    return y;
}

static tree_node_t* rebalance (tree_t* self, tree_node_t* node, key_t* key)
{
    if (NULL == node)
    {
        return NULL;
    }

    update_height(node);
    update_size(node);

    const int8_t balance = balance_of(node);

    if (balance > 1)
    {
        const int32_t ordering = self->comparator(self, key, &node->left->key);

        if (ordering < 0)
        {
            node = rotate_right(node);
        }
        else if (ordering > 0)
        {
            node->left = rotate_left(node->left);
            node = rotate_right(node);
        }
    }
    else if (balance < -1)
    {
        const int32_t ordering = self->comparator(self, key, &node->right->key);

        if (ordering > 0)
        {
            node = rotate_left(node);
        }
        else if (ordering < 0)
        {
            node->right = rotate_right(node->right);
            node = rotate_left(node);
        }
    }

    return node;
}

static tree_node_t* insert_node (tree_t* self, tree_node_t* node, key_t* key)
{
    if (node == NULL)
    {
        return create_node(self, key);
    }
    else if (self->comparator(self, key, &node->key) < 0)
    {
        node->left = insert_node(self, node->left, key);
        return rebalance(self, node, key);
    }
    else if (self->comparator(self, key, &node->key) > 0)
    {
        node->right = insert_node(self, node->right, key);
        return rebalance(self, node, key);
    }
    else
    {
        return node;
    }
}

static tree_node_t* find_min_value_node (tree_node_t* node)
{
    tree_node_t* current = node;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

static tree_node_t* delete_node (tree_t* self, tree_node_t* node, key_t* key, tree_node_t** deleted)
{
    if (node == NULL)
    {
        return NULL; // Key Not Found
    }

    // Case: The node to delete is in the left subtree.
    if (self->comparator(self, key, &node->key) < 0)
    {
        node->left = delete_node(self, node->left, key, deleted);
    }
    // Case: The node to delete is in the right subtree.
    else if (self->comparator(self, key, &node->key) > 0)
    {
        node->right = delete_node(self, node->right, key, deleted);
    }
    // Case: The node to delete does not have any children.
    else if (node->size == 1)
    {
        *deleted = node;
        return NULL;
    }
    // Case: The node to delete only has a right subtree.
    else if (node->left == NULL)
    {
        *deleted = node;
        node = node->right;
    }
    // Case: The node to delete only has a left subtree.
    else if (node->right == NULL)
    {
        *deleted = node;
        node = node->left;
    }
    // Case: The node to delete has both a left subtree and a right subtree.
    else
    {
        *deleted = node;

        // Find the inorder successor, which has no left subtree by-definition.
        tree_node_t* successor = find_min_value_node(node->right);

        // Delete the inorder successor
        tree_node_t* deleted_successor = NULL;
        successor->left = node->left;
        successor->right = delete_node(self, node->right, &successor->key, &deleted_successor);
        node = successor;
    }

    return rebalance(self, node, key);
}

static tree_node_t* find_node (tree_t* self, tree_node_t* node, key_t* key)
{
    if (NULL == node)
    {
        return NULL;
    }
    else if (self->comparator(self, key, &node->key) < 0)
    {
        return find_node(self, node->left, key);
    }
    else if (self->comparator(self, key, &node->key) > 0)
    {
        return find_node(self, node->right, key);
    }
    else
    {
        return node;
    }
}

/**
 * The dynamic allocator does not need any resources;
 * therefore, statically allocate the dynamic allocator.
 */
static tree_allocator_t TREE_DYNAMIC_ALLOCATOR;

static tree_node_t* dynamic_allocate (tree_allocator_t* self)
{
    return calloc(1, sizeof(tree_node_t));
}

static void wipe (tree_node_t* self)
{

    if (NULL != self)
    {
        memset(self, 0, sizeof(tree_node_t));
    }

}

static void dynamic_release (tree_allocator_t* self, tree_node_t* node)
{
    if (NULL != node)
    {
        wipe(node);
        free(node);
    }
}

static void dynamic_destroy (tree_allocator_t* self)
{
    // Pass, because the dynamic allocator is statically allocated.
}

static tree_node_t* pool_allocate (tree_allocator_t* self)
{
    tree_allocator_pooled_t* context = (tree_allocator_pooled_t*) self->context;

    if (NULL == context->free)
    {
        if (context->allocated < context->capacity)
        {
            tree_node_t* node = (tree_node_t*) calloc(1, sizeof(tree_node_t));

            if (NULL == node)
            {
                return NULL;
            }
            else
            {
                ++context->allocated;
                return node;
            }
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        tree_node_t* node = context->free;
        context->free = context->free->right;
        return node;
    }
}

static void pool_release (tree_allocator_t* self, tree_node_t* node)
{
    wipe(node);
    tree_allocator_pooled_t* context = (tree_allocator_pooled_t*) self->context;
    node->right = context->free;
    context->free = node;
}

static void pool_destroy (tree_allocator_t* self)
{
    if (NULL != self)
    {
        tree_allocator_pooled_t* context = (tree_allocator_pooled_t*) self->context;

        if (NULL != context)
        {
            tree_node_t* p = context->free;
            tree_node_t* q = NULL;
            while (NULL != p)
            {
                q = p->right;
                free(p);
                p = q;
            }

            context->free = NULL;

            free(context);
            self->context = NULL;
        }

        free(self);
    }
}

static tree_node_t* slab_allocate (tree_allocator_t* self)
{
    tree_allocator_slab_t* context = (tree_allocator_slab_t*) self->context;

    if (NULL == context->free)
    {
        return NULL;
    }
    else
    {
        tree_node_t* node = context->free;
        context->free = context->free->right;
        return node;
    }
}

static void slab_release (tree_allocator_t* self, tree_node_t* node)
{
    wipe(node);
    tree_allocator_slab_t* context = (tree_allocator_slab_t*) self->context;
    node->right = context->free;
    context->free = node;
}

static void slab_destroy (tree_allocator_t* self)
{
    if (NULL != self)
    {
        if (NULL != self->context)
        {
            free(self->context);
            self->context = NULL;
        }

        free(self);
    }
}

/**
 * @brief Creates a new dynamic tree allocator.
 * @return Pointer to the newly created tree allocator.
 */
tree_allocator_t* tree_allocator_dynamic ()
{
    TREE_DYNAMIC_ALLOCATOR.allocate = &dynamic_allocate;
    TREE_DYNAMIC_ALLOCATOR.release = &dynamic_release;
    TREE_DYNAMIC_ALLOCATOR.destroy = &dynamic_destroy;
    return &TREE_DYNAMIC_ALLOCATOR;
}

/**
 * @brief Creates a new pooled tree allocator with preallocated nodes.
 * @param preallocated Number of nodes to preallocate.
 * @param capacity Maximum capacity of the pooled allocator.
 * @return Pointer to the newly created pooled tree allocator.
 */
tree_allocator_t* tree_allocator_pooled (size_t preallocated, size_t capacity)
{
    tree_allocator_t* self = (tree_allocator_t*) calloc(1, sizeof(tree_allocator_t));

    if (NULL == self)
    {
        goto cleanup;
    }
    else
    {
        self->allocate = pool_allocate;
        self->release = pool_release;
        self->destroy = pool_destroy;
    }

    tree_allocator_pooled_t* context = (tree_allocator_pooled_t*) calloc(1, sizeof(tree_allocator_pooled_t));

    if (NULL == self)
    {
        goto cleanup;
    }
    else
    {
        self->context = (void*) context;
        context->capacity = capacity;
        context->allocated = preallocated;
    }

    for (size_t i = 0; i < preallocated; i++)
    {
        tree_node_t* node = calloc(1, sizeof(tree_node_t));

        if (NULL == node)
        {
            goto cleanup;
        }
        else
        {
            node->right = context->free;
            context->free = node;
        }
    }

    return self;

cleanup:
    pool_destroy(self);
    return NULL;
}

/**
 * @brief Creates a new slab tree allocator with a specified capacity.
 * @param capacity Maximum capacity of the slab allocator.
 * @return Pointer to the newly created slab tree allocator.
 */
tree_allocator_t* tree_allocator_slab (size_t capacity)
{
    tree_allocator_t* self = (tree_allocator_t*) calloc(1, sizeof(tree_allocator_t) + capacity * sizeof(tree_node_t));

    if (NULL == self)
    {
        goto cleanup;
    }
    else
    {
        self->allocate = slab_allocate;
        self->release = slab_release;
        self->destroy = slab_destroy;
    }

    tree_allocator_slab_t* context = (tree_allocator_slab_t*) calloc(1, sizeof(tree_allocator_slab_t) + capacity * sizeof(tree_node_t));

    if (NULL == self)
    {
        goto cleanup;
    }
    else
    {
        self->context = (void*) context;
        context->capacity = capacity;
    }

    for (size_t i = 0; i < capacity; i++)
    {
        tree_node_t* node = &context->nodes[i];

        if (NULL == node)
        {
            goto cleanup;
        }
        else
        {
            node->right = context->free;
            context->free = node;
        }
    }

    return self;

cleanup:
    slab_destroy(self);
    return NULL;
}

/**
 * @brief Frees the resources associated with a tree allocator.
 * @param self Pointer to the tree allocator to free.
 */
void tree_allocator_free (tree_allocator_t* self)
{
    if (NULL != self)
    {
        self->destroy(self);
    }
}

static int tree_naturalOrder (tree_t* self, key_t* X, key_t* Y)
{

    return *X < *Y ? -1 : (*X > *Y ? +1 : 0);

}

static int tree_reverseOrder (tree_t* self, key_t* X, key_t* Y)
{
    return tree_naturalOrder(self, Y, X);
}

/**
 * @brief Returns the natural order comparator function for keys.
 * @return Function pointer to the natural order comparator.
 */
tree_comparator_t tree_comparator_naturalOrder ()
{
    return &tree_naturalOrder;
}

/**
 * @brief Returns the reverse order comparator function for keys.
 * @return Function pointer to the reverse order comparator.
 */
tree_comparator_t tree_comparator_reverseOrder ()
{
    return &tree_reverseOrder;
}

/**
 * @brief Returns the default key value.
 * @return Default key.
 */
key_t tree_defaultKey ()
{

    return NULL;

}

/**
 * @brief Returns the default data value.
 * @return Default data value.
 */
data_t tree_defaultValue ()
{

    return NULL;

}

/**
 * @brief Creates a new instance of the AVL tree.
 * @return Pointer to the newly created AVL tree.
 */
tree_t* tree_new ()
{
    tree_allocator_t* allocator = tree_allocator_dynamic();
    tree_comparator_t comparator = tree_comparator_naturalOrder();
    return tree_make(allocator, comparator);
}

/**
 * @brief Creates a new AVL tree with a specified allocator and comparator.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Pointer to the newly created AVL tree.
 */
tree_t* tree_make (tree_allocator_t* allocator, tree_comparator_t comparator)
{
    tree_t* result = calloc(1, sizeof(tree_t));

    if (NULL == result)
    {
        return NULL;
    }

    result->allocator = allocator;
    result->size = 0;
    result->root = NULL;
    result->comparator = comparator;

    return result;
}

/**
 * @brief Frees the resources of an AVL tree.
 * @param self Pointer to the AVL tree to free.
 */
void tree_free (tree_t* self)
{
    if (NULL != self)
    {
        tree_clear(self);
        self->allocator = NULL;
        free(self);
    }
}

/**
 * @brief Creates an AVL tree with stack-allocated properties.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Stack-allocated AVL tree structure.
 */
tree_t tree_make_stackalloc (tree_allocator_t* allocator, tree_comparator_t comparator)
{
    tree_t result;
    result.allocator = allocator;
    result.comparator = comparator;
    return result;
}

/**
 * @brief Frees the resources of a stack-allocated AVL tree.
 * @param self Pointer to the stack-allocated AVL tree.
 */
void tree_free_stackalloc (tree_t* self)
{
    if (NULL != self)
    {
        tree_clear(self);
        self->allocator = NULL;
        self->comparator = NULL;
    }
}

/**
 * @brief Creates a copy of an existing AVL tree.
 * @param self Pointer to the tree to be copied.
 * @return Pointer to the newly created copy of the tree.
 */
tree_t* tree_copy (tree_t* self)
{
    tree_t* copy = tree_make(self->allocator, self->comparator);

    if (NULL == copy)
    {
        return NULL;
    }
    else if (tree_putAll(copy, self))
    {
        return copy;
    }
    else
    {
        tree_free(copy);
        return NULL;
    }
}

/**
 * @brief Retrieves the number of nodes in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Number of nodes in the tree.
 */
size_t tree_size (tree_t* self)
{
    return self->size;
}

/**
 * @brief Checks if the AVL tree is empty.
 * @param self Pointer to the AVL tree.
 * @return true if the tree is empty, false otherwise.
 */
bool tree_isEmpty (tree_t* self)
{
    // This is implemented as a pointer comparison,
    // because isEmpty() is called by other functions,
    // wheren nodes are being added and/or removed.
    // In those cases, size() may be momentarily inaccurate.
    return (NULL == self) || (NULL == self->root);
}

/**
 * @brief Clears all nodes from the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_clear (tree_t* self)
{
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_isEmpty(self) == false)
        {
            tree_removeFirst(self);
        }
    }
    tree_iter_free(&iter);
}

/**
 * @brief Checks if a specific key exists in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to check for existence.
 * @return true if the key exists, false otherwise.
 */
bool tree_containsKey (tree_t* self, key_t key)
{
    return NULL != find_node(self, self->root, &key);
}

/**
 * @brief Checks if the current tree contains all elements of another tree.
 * @param self Pointer to the AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all keys in other are present in self, false otherwise.
 */
bool tree_containsValue (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);

            tree_node_t* node = tree_iter_node(&iter);

            if (predicate(self, node, context))
            {
                return true;
            }
        }
    }
    tree_iter_free(&iter);
    return false;
}

/**
 * @brief Inserts a node with a specified key and returns the inserted node.
 * @param self Pointer to the AVL tree.
 * @param key Key for the node to insert.
 * @return Pointer to the created node.
 */
tree_node_t* tree_putNode (tree_t* self, key_t key)
{
    tree_node_t* root = insert_node(self, self->root, &key);
    if (NULL == root)
    {
        return NULL;
    }
    else
    {
        self->root = root;
        return find_node(self, self->root, &key); // TODO: improve performance
    }
}

/**
 * @brief Retrieves a node by its key.
 * @param self Pointer to the AVL tree.
 * @param key Key of the node to find.
 * @return Pointer to the found node or NULL if not found.
 */
tree_node_t* tree_getNode (tree_t* self, key_t key)
{
    return find_node(self, self->root, &key);
}

/**
 * @brief Retrieves the root node of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the root node.
 */
tree_node_t* tree_rootNode (tree_t* self)
{
    return self->root;
}

/**
 * @brief Retrieves the first node (minimum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the first node or NULL if the tree is empty.
 */
tree_node_t* tree_firstNode (tree_t* self)
{
    tree_node_t* p = self->root;

    if (NULL == p)
    {
        return NULL;
    }
    else
    {
        while (true)
        {
            if (NULL == p->left)
            {
                return p;
            }
            else
            {
                p = p->left;
            }
        }
    }
}

/**
 * @brief Retrieves the last node (maximum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the last node or NULL if the tree is empty.
 */
tree_node_t* tree_lastNode (tree_t* self)
{
    tree_node_t* p = self->root;

    if (NULL == p)
    {
        return NULL;
    }
    else
    {
        while (true)
        {
            if (NULL == p->right)
            {
                return p;
            }
            else
            {
                p = p->right;
            }
        }
    }
}

/**
 * @brief Finds the successor node (next higher key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the higher node.
 * @return Pointer to the higher node or NULL if not found.
 */
tree_node_t* tree_higherNode (tree_t* self, key_t key)
{
    tree_node_t* current = self->root;
    tree_node_t* successor = NULL;

    while (current != NULL)
    {
        if (self->comparator(self, &key, &current->key) < 0)
        {
            // Update successor and go left
            successor = current;
            current = current->left;
        }
        else
        {
            // Go right
            current = current->right;
        }
    }

    return successor;

}

/**
 * @brief Finds the predecessor node (next lower key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the lower node.
 * @return Pointer to the lower node or NULL if not found.
 */
tree_node_t* tree_lowerNode (tree_t* self, key_t key)
{
    tree_node_t* current = self->root;
    tree_node_t* predecessor = NULL;

    // Traverse the tree to find the lower node
    while (current != NULL)
    {
        if (self->comparator(self, &key, &current->key) <= 0)
        {
            // Move left; we need to find a smaller key
            current = current->left;
        }
        else
        {
            // Current key is smaller than the searched key
            // This could be a candidate for lower node
            predecessor = current;
            // Move right to find larger keys
            current = current->right;
        }
    }

    return predecessor;
}

static tree_node_t* find_nth (size_t prior, tree_node_t* node, size_t index)
{
    if (NULL == node)
    {
        return NULL; // Base case: If the node is null, return null.
    }

    // Get the size of the left subtree.
    const size_t left_size = size_of(node->left);

    if (prior + left_size == index)
    {
        // If prior + left_size matches the index, we've found the nth node.
        return node;
    }
    else if (prior + left_size > index)
    {
        // If the index is less than the size of the left subtree,
        // we need to search in the left subtree.
        return find_nth(prior, node->left, index);
    }
    else
    {
        // If the index is greater, we search in the right subtree.
        // Update the prior value to include the current node and left subtree size.
        return find_nth(prior + left_size + 1, node->right, index);
    }
}

/**
 * @brief Finds the nth node (0-based index) in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param index The index of the node to find.
 * @return Pointer to the nth node or NULL if not found.
 */
tree_node_t* tree_nthNode (tree_t* self, size_t index)
{
    return find_nth(0, self->root, index);
}



/**
 * @brief Adds a value as the first element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool tree_addFirst (tree_t* self, data_t value)
{
    return tree_pushFirst(self, value);
}

/**
 * @brief Adds a value as the last element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool tree_addLast (tree_t* self, data_t value)
{
    return tree_pushLast(self, value);
}

/**
 * @brief Pushes a value to the front of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_pushFirst (tree_t* self, data_t value)
{
    tree_node_t* node = tree_firstNode(self);

    if (NULL == node)
    {
        return tree_put(self, 0, value);
    }
    else
    {
        return tree_put(self, node->key - 1, value);
    }
}

/**
 * @brief Pushes a value to the end of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_pushLast (tree_t* self, data_t value)
{
    tree_node_t* node = tree_lastNode(self);

    if (NULL == node)
    {
        return tree_put(self, 0, value);
    }
    else
    {
        return tree_put(self, node->key + 1, value);
    }
}

/**
 * @brief Pushes a value to the AVL tree (equivalent to pushLast).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_push (tree_t* self, data_t value)
{
    return tree_pushLast(self, value);
}



/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_peek (tree_t* self)
{
    return tree_peekLast(self);
}

/**
 * @brief Peeks at the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
data_t tree_peekFirst (tree_t* self)
{
    tree_node_t* node = tree_firstNode(self);

    if (NULL == node)
    {
        return tree_defaultValue();
    }
    else
    {
        return node->value;
    }
}

/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_peekLast (tree_t* self)
{
    tree_node_t* node = tree_lastNode(self);

    if (NULL == node)
    {
        return tree_defaultValue();
    }
    else
    {
        return node->value;
    }
}

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_pop (tree_t* self)
{
    return tree_popLast(self);
}

/**
 * @brief Pops the first value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
data_t tree_popFirst (tree_t* self)
{
    tree_node_t* node = tree_firstNode(self);

    if (NULL == node)
    {
        return tree_defaultValue();
    }
    else
    {
        data_t value = node->value;
        tree_removeFirst(self);
        return value;
    }
}

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_popLast (tree_t* self)
{
    tree_node_t* node = tree_lastNode(self);

    if (NULL == node)
    {
        return tree_defaultValue();
    }
    else
    {
        data_t value = node->value;
        tree_removeLast(self);
        return value;
    }
}

bool tree_containsAll (tree_t* self, tree_t* other)
{
    if (tree_size(self) < tree_size(other))
    {
        return false;
    }

    tree_iterator_t iter = tree_iter(other);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);

            tree_node_t* node = tree_iter_node(&iter);

            if (tree_containsKey(self, node->key) == false)
            {
                return false;
            }
        }
    }
    tree_iter_free(&iter);
    return true;
}

/**
 * @brief Copies keys from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store keys.
 * @param array_size Size of the provided array.
 * @return Number of keys copied into the array.
 */
size_t tree_keysToArray (tree_t* self, key_t* array, size_t array_size)
{
    size_t count = 0;
    tree_iterator_t iter = tree_iter(self);
    {
        for (count = 0; tree_iter_hasNext(&iter) && (count < array_size); count++)
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            array[count] = node->key;
        }
    }
    tree_iter_free(&iter);
    return count;
}

/**
 * @brief Copies values from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store values.
 * @param array_size Size of the provided array.
 * @return Number of values copied into the array.
 */
size_t tree_valuesToArray (tree_t* self, data_t* array, size_t array_size)
{
    size_t count = 0;
    tree_iterator_t iter = tree_iter(self);
    {
        for (count = 0; tree_iter_hasNext(&iter) && (count < array_size); count++)
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            array[count] = node->value;
        }
    }
    tree_iter_free(&iter);
    return count;
}

/**
 * @brief Allocates a new array and copies the keys from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of keys or NULL if empty.
 */
key_t* tree_keysToNewArray (tree_t* self)
{
    if (NULL == self)
    {
        return NULL;
    }
    else if (tree_isEmpty(self))
    {
        return NULL;
    }

    size_t size = tree_size(self);

    key_t* array = (key_t*) calloc(size, sizeof(key_t));

    if (NULL == array)
    {
        return NULL;
    }
    else
    {
        tree_keysToArray(self, array, size);
        return array;
    }
}

/**
 * @brief Allocates a new array and copies the values from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of values or NULL if empty.
 */
data_t* tree_valuesToNewArray (tree_t* self)
{
    if (NULL == self)
    {
        return NULL;
    }
    else if (tree_isEmpty(self))
    {
        return NULL;
    }

    size_t size = tree_size(self);

    data_t* array = (data_t*) calloc(size, sizeof(data_t));

    if (NULL == array)
    {
        return NULL;
    }
    else
    {
        tree_valuesToArray(self, array, size);
        return array;
    }
}

/**
 * @brief Reduces tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced double value.
 */
double tree_reduceToDouble (tree_t* self, double (*functor)(tree_t*, tree_node_t*, double, void*), double initial, void* context)
{
    double result = initial;
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            result = functor(self, node, result, context);
        }
    }
    tree_iter_free(&iter);
    return result;
}

/**
 * @brief Reduces tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced int64 value.
 */
int64_t tree_reduceToInt64 (tree_t* self, int64_t (*functor)(tree_t*, tree_node_t*, int64_t, void*), int64_t initial, void* context)
{
    int64_t result = initial;
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            result = functor(self, node, result, context);
        }
    }
    tree_iter_free(&iter);
    return result;
}

/**
 * @brief Sums tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as a double value.
 */
double tree_sumToDouble (tree_t* self, double (*functor)(tree_t*, tree_node_t*, void*), void* context)
{
    double result = 0;
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            result += functor(self, node, context);
        }
    }
    tree_iter_free(&iter);
    return result;
}

/**
 * @brief Sums tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as an int64 value.
 */
int64_t tree_sumToInt64 (tree_t* self, int64_t (*functor)(tree_t*, tree_node_t*, void*), void* context)
{
    int64_t result = 0;
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            result += functor(self, node, context);
        }
    }
    tree_iter_free(&iter);
    return result;
}

/**
 * @brief Puts all elements from another tree into this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all elements were successfully added, false otherwise.
 */
bool tree_putAll (tree_t* self, tree_t* other)
{
    tree_iterator_t iter = tree_iter(other);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);

            tree_node_t* node = tree_iter_node(&iter);

            if (tree_put(self, node->key, node->value) == false)
            {
                return false;
            }
        }
    }
    tree_iter_free(&iter);
    return true;
}

/**
 * @brief Removes all elements that exist in another tree from this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void tree_removeAll (tree_t* self, tree_t* other)
{
    tree_iterator_t iter = tree_iter(other);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            tree_remove(self, node->key);
        }
    }
    tree_iter_free(&iter);
}

/**
 * @brief Retains only elements that exist in another tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void tree_retainAll (tree_t* self, tree_t* other)
{
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);

            tree_node_t* node = tree_iter_node(&iter);

            if (tree_containsKey(other, node->key) == false)
            {
                tree_iter_next(&iter);
                tree_remove(self, node->key);
            }
        }
    }
    tree_iter_free(&iter);
}

/**
 * @brief Inserts a key-value pair into the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to insert.
 * @param value Data value to associate with the key.
 * @return true if insertion was successful, false otherwise.
 */
bool tree_put (tree_t* self, key_t key, data_t value)
{
    tree_node_t* node = tree_putNode(self, key);

    if (NULL == node)
    {
        return false;
    }
    else
    {
        node->value = value;
        return true;
    }
}

/**
 * @brief Retrieves the value associated with a key in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to search for.
 * @return The associated value or default value if key not found.
 */
data_t tree_get (tree_t* self, key_t key)
{
    tree_node_t* node = tree_getNode(self, key);

    if (NULL == node)
    {
        return tree_defaultValue();
    }
    else
    {
        return node->value;
    }
}

/**
 * @brief Removes a key and its value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to remove.
 */
void tree_remove (tree_t* self, key_t key)
{
    tree_node_t* deleted = NULL;
    self->root = delete_node(self, self->root, &key, &deleted);

    if (NULL != deleted)
    {
        self->allocator->release(self->allocator, deleted);
        --self->size;
    }
}

/**
 * @brief Removes the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_removeFirst (tree_t* self)
{
    tree_node_t* node = tree_firstNode(self);

    if (NULL != node)
    {
        tree_remove(self, node->key);
    }
}

/**
 * @brief Removes the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_removeLast (tree_t* self)
{
    tree_node_t* node = tree_lastNode(self);

    if (NULL != node)
    {
        tree_remove(self, node->key);
    }
}

/**
 * @brief Removes nodes from the tree based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 */
void tree_removeIf (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            if (predicate(self, node, context))
            {
                tree_iter_next(&iter);
                tree_remove(self, node->key);
            }
        }
    }
    tree_iter_free(&iter);
}

/**
 * @brief Applies a function to each node in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param functor Function to apply to each node.
 * @param context Additional context passed to the functor.
 */
void tree_forEach (tree_t* self, void (*functor)(tree_t*, tree_node_t*, void*), void* context)
{
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            functor(self, node, context);
        }
    }
    tree_iter_free(&iter);
}

/**
 * @brief Checks if any nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if any match found, false otherwise.
 */
bool tree_anyMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    return tree_count(self, predicate, context) > 0;
}

/**
 * @brief Checks if all nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if all nodes match, false otherwise.
 */
bool tree_allMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    size_t size = tree_size(self);
    return tree_count(self, predicate, context) == size;
}

/**
 * @brief Checks if none of the nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if none match, false otherwise.
 */
bool tree_noneMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    return tree_count(self, predicate, context) == 0;
}

/**
 * @brief Counts the number of nodes matching a given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return Count of nodes matching the predicate.
 */
size_t tree_count (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context)
{
    size_t count = 0;
    tree_iterator_t iter = tree_iter(self);
    {
        while (tree_iter_hasNext(&iter))
        {
            tree_iter_next(&iter);
            tree_node_t* node = tree_iter_node(&iter);
            if (predicate(self, node, context))
            {
                ++count;
            }
        }
    }
    tree_iter_free(&iter);
    return count;
}

/**
 * @brief Checks if two trees are equal based on a comparison predicate.
 * @param self Pointer to the first AVL tree.
 * @param other Pointer to the second AVL tree.
 * @param predicate Function pointer for node comparison.
 * @param context Additional context passed to the predicate.
 * @return true if the trees are equal, false otherwise.
 */
bool tree_isEqual (tree_t* self, tree_t* other, bool (*predicate)(tree_node_t*, tree_node_t*, void*), void* context)
{
    if (tree_size(self) != tree_size(other))
    {
        return false;
    }

    tree_iterator_t iterX = tree_iter(self);
    {
        tree_iterator_t iterY = tree_iter(other);
        {
            while (tree_iter_hasNext(&iterX))
            {
                tree_iter_next(&iterX);
                tree_iter_next(&iterY);

                tree_node_t* nodeX = tree_iter_node(&iterX);
                tree_node_t* nodeY = tree_iter_node(&iterY);

                if (predicate(nodeX, nodeY, context) == false)
                {
                    return false;
                }
            }
        }
        tree_iter_free(&iterY);
    }
    tree_iter_free(&iterX);
    return true;
}

/**
 * @brief Creates an iterator for the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Iterator structure for the tree.
 */
tree_iterator_t tree_iter (tree_t* self)
{
    return tree_iter_atNode(self, NULL);
}

/**
 * @brief Creates an iterator starting at a specific key in the tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to start the iteration at.
 * @return Iterator structure for the tree starting at the given key.
 */
tree_iterator_t tree_iter_at (tree_t* self, key_t key)
{
    tree_node_t* node = find_node(self, self->root, &key);
    return tree_iter_atNode(self, node);
}

/**
 * @brief Creates an iterator starting at a specific node in the tree.
 * @param self Pointer to the AVL tree.
 * @param node Node to start the iteration at.
 * @return Iterator structure for the tree starting at the given node.
 */
tree_iterator_t tree_iter_atNode (tree_t* self, tree_node_t* node)
{
    tree_iterator_t result;
    result.owner = self;
    result.node = node;
    return result;
}

/**
 * @brief Frees the resources associated with a tree iterator.
 * @param self Pointer to the tree iterator to free.
 */
void tree_iter_free (tree_iterator_t* self)
{
    if (NULL != self)
    {
        self->owner = NULL;
        self->node = NULL;
    }
}

/**
 * @brief Checks if the iterator has a next element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a next element, false otherwise.
 */
bool tree_iter_hasNext (tree_iterator_t* self)
{
    if (tree_isEmpty(self->owner))
    {
        return false;
    }
    else if (NULL == self->node)
    {
        return true;
    }
    else
    {
        key_t key = self->node->key;
        return NULL != tree_higherNode(self->owner, key);
    }
}

/**
 * @brief Checks if the iterator has a previous element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a previous element, false otherwise.
 */
bool tree_iter_hasPrev (tree_iterator_t* self)
{
    if (tree_isEmpty(self->owner))
    {
        return false;
    }
    else if (NULL == self->node)
    {
        return true;
    }
    else
    {
        key_t key = self->node->key;
        return NULL != tree_lowerNode(self->owner, key);
    }
}

/**
 * @brief Advances the iterator to the next element.
 * @param self Pointer to the tree iterator.
 */
void tree_iter_next (tree_iterator_t* self)
{
    if (tree_isEmpty(self->owner) == false)
    {
        if (NULL == self->node)
        {
            self->node = tree_firstNode(self->owner);
        }
        else
        {
            key_t key = self->node->key;
            self->node = tree_higherNode(self->owner, key);

            // Iterators can be circular.
            if (NULL == self->node)
            {
                self->node = tree_firstNode(self->owner);
            }
        }
    }
}

/**
 * @brief Moves the iterator to the previous element.
 * @param self Pointer to the tree iterator.
 */
void tree_iter_prev (tree_iterator_t* self)
{
    if (tree_isEmpty(self->owner) == false)
    {
        if (NULL == self->node)
        {
            self->node = tree_lastNode(self->owner);
        }
        else
        {
            key_t key = self->node->key;
            self->node = tree_lowerNode(self->owner, key);

            // Iterators can be circular.
            if (NULL == self->node)
            {
                self->node = tree_lastNode(self->owner);
            }
        }
    }
}

/**
 * @brief Retrieves the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Pointer to the current node.
 */
tree_node_t* tree_iter_node (tree_iterator_t* self)
{
    return self->node;
}

/**
 * @brief Retrieves the key of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Key of the current node.
 */
key_t tree_iter_key (tree_iterator_t* self)
{
    if (NULL == self->node)
    {
        return tree_defaultKey();
    }
    else
    {
        return self->node->key;
    }
}

/**
 * @brief Sets the data value of the current node in the iterator.
 * @param self Pointer to the tree iterator.
 * @param value Data value to set.
 */
void tree_iter_set (tree_iterator_t* self, data_t value)
{
    self->node->value = value;
}

/**
 * @brief Retrieves the data value of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Data value of the current node.
 */
data_t tree_iter_get (tree_iterator_t* self)
{
    if (NULL == self->node)
    {
        return tree_defaultValue();
    }
    else
    {
        return self->node->value;
    }
}

/**
 * @brief Retrieves the key of a given tree node.
 * @param self Pointer to the tree node.
 * @return Key of the node.
 */
key_t tree_node_key (tree_node_t* self)
{
    if (NULL == self)
    {
        return tree_defaultKey();
    }
    else
    {
        return self->key;
    }
}

/**
 * @brief Sets the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @param value The data value to set.
 */
void tree_node_set (tree_node_t* self, data_t value)
{
    self->value = value;
}

/**
 * @brief Retrieves the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @return Data value of the node.
 */
data_t tree_node_get (tree_node_t* self)
{
    if (NULL == self)
    {
        return tree_defaultValue();
    }
    else
    {
        return self->value;
    }
}
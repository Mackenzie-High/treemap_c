#ifndef tree_H
#define tree_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


#include "common.h"


/**
 * Forward declaration of the tree_node_t structure.
 */
typedef struct tree_node tree_node_t;

/**
 * @struct tree_node
 * @brief Represents a node in the AVL tree.
 */
typedef struct tree_node
{
    /**
     * Height of the node in the tree.
     */
    int8_t height;

    /**
     * Size (number of nodes) in the subtree rooted at this node.
     */
    size_t size;

    /**
     * Pointer to the left child node.
     */
    tree_node_t* left;

    /**
     * Pointer to the right child node.
     */
    tree_node_t* right;

    /**
     * The key that identifies this node in the tree.
     */
    key_t key;

    /**
     * The data stored in this node.
     */
    data_t value;

} tree_node_t;

/**
 * Forward declaration of the tree_allocator_t structure.
 */
typedef struct tree_allocator tree_allocator_t;

/**
 * @struct tree_allocator
 * @brief Memory allocator interface for tree nodes.
 */
typedef struct tree_allocator
{
    /**
     * Function pointer to allocate a new tree node.
     */
    tree_node_t* (*allocate)(tree_allocator_t* self);

    /**
     * Function pointer to release a tree node.
     */
    void (*release)(tree_allocator_t* self, tree_node_t* node);

    /**
     * Function pointer to destroy the allocator.
     */
    void (*destroy)(tree_allocator_t* self);

    /**
     * Context pointer used by the allocator.
     */
    void* context;

} tree_allocator_t;

/**
 * Forward declaration of the tree_t structure.
 */
typedef struct tree tree_t;

/**
 * @typedef tree_comparator_t
 * @brief Comparator function type for comparing tree keys.
 */
typedef int (*tree_comparator_t)(tree_t*, key_t*, key_t*);

/**
 * @struct tree
 * @brief Represents the AVL tree structure with its properties and operations.
 */
typedef struct tree
{
    /**
     * Number of nodes in the tree.
     */
    size_t size;

    /**
     * Pointer to the root node of the tree.
     */
    tree_node_t* root;

    /**
     * Pointer to the allocator used for tree nodes.
     */
    tree_allocator_t* allocator;

    /**
     * Comparator function for comparing keys.
     */
    tree_comparator_t comparator;

} tree_t;

/**
 * @struct tree_iterator
 * @brief Iterator structure for traversing the tree.
 */
typedef struct
{
    /**
     * Pointer to the owning tree.
     */
    tree_t* owner;

    /**
     * Pointer to the current node in iteration.
     */
    tree_node_t* node;

} tree_iterator_t;

/**
 * @brief Creates a new dynamic tree allocator.
 * @return Pointer to the newly created tree allocator.
 */
tree_allocator_t* tree_allocator_dynamic ();

/**
 * @brief Creates a new pooled tree allocator with preallocated nodes.
 * @param preallocated Number of nodes to preallocate.
 * @param capacity Maximum capacity of the pooled allocator.
 * @return Pointer to the newly created pooled tree allocator.
 */
tree_allocator_t* tree_allocator_pooled (size_t preallocated, size_t capacity);

/**
 * @brief Creates a new slab tree allocator with a specified capacity.
 * @param capacity Maximum capacity of the slab allocator.
 * @return Pointer to the newly created slab tree allocator.
 */
tree_allocator_t* tree_allocator_slab (size_t capacity);

/**
 * @brief Frees the resources associated with a tree allocator.
 * @param self Pointer to the tree allocator to free.
 */
void tree_allocator_free (tree_allocator_t* self);

/**
 * @brief Returns the natural order comparator function for keys.
 * @return Function pointer to the natural order comparator.
 */
tree_comparator_t tree_comparator_naturalOrder ();

/**
 * @brief Returns the reverse order comparator function for keys.
 * @return Function pointer to the reverse order comparator.
 */
tree_comparator_t tree_comparator_reverseOrder ();

/**
 * @brief Returns the default key value.
 * @return Default key.
 */
key_t tree_defaultKey ();

/**
 * @brief Returns the default data value.
 * @return Default data value.
 */
data_t tree_defaultValue ();

/**
 * @brief Retrieves the key of a given tree node.
 * @param self Pointer to the tree node.
 * @return Key of the node.
 */
key_t tree_node_key (tree_node_t* self);

/**
 * @brief Sets the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @param value The data value to set.
 */
void tree_node_set (tree_node_t* self, data_t value);

/**
 * @brief Retrieves the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @return Data value of the node.
 */
data_t tree_node_get (tree_node_t* self);

/**
 * @brief Creates a new instance of the AVL tree.
 * @return Pointer to the newly created AVL tree.
 */
tree_t* tree_new ();

/**
 * @brief Creates a new AVL tree with a specified allocator and comparator.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Pointer to the newly created AVL tree.
 */
tree_t* tree_make (tree_allocator_t* allocator, tree_comparator_t comparator);

/**
 * @brief Frees the resources of an AVL tree.
 * @param self Pointer to the AVL tree to free.
 */
void tree_free (tree_t* self);

/**
 * @brief Creates an AVL tree with stack-allocated properties.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Stack-allocated AVL tree structure.
 */
tree_t tree_make_stackalloc (tree_allocator_t* allocator, tree_comparator_t comparator);

/**
 * @brief Frees the resources of a stack-allocated AVL tree.
 * @param self Pointer to the stack-allocated AVL tree.
 */
void tree_free_stackalloc (tree_t* self);

/**
 * @brief Creates a copy of an existing AVL tree.
 * @param self Pointer to the tree to be copied.
 * @return Pointer to the newly created copy of the tree.
 */
tree_t* tree_copy (tree_t* self);

/**
 * @brief Retrieves the number of nodes in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Number of nodes in the tree.
 */
size_t tree_size (tree_t* self);

/**
 * @brief Checks if the AVL tree is empty.
 * @param self Pointer to the AVL tree.
 * @return true if the tree is empty, false otherwise.
 */
bool tree_isEmpty (tree_t* self);

/**
 * @brief Clears all nodes from the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_clear (tree_t* self);

/**
 * @brief Inserts a node with a specified key and returns the inserted node.
 * @param self Pointer to the AVL tree.
 * @param key Key for the node to insert.
 * @return Pointer to the created node.
 */
tree_node_t* tree_putNode (tree_t* self, key_t key);

/**
 * @brief Retrieves a node by its key.
 * @param self Pointer to the AVL tree.
 * @param key Key of the node to find.
 * @return Pointer to the found node or NULL if not found.
 */
tree_node_t* tree_getNode (tree_t* self, key_t key);

/**
 * @brief Retrieves the root node of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the root node.
 */
tree_node_t* tree_rootNode (tree_t* self);

/**
 * @brief Retrieves the first node (minimum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the first node or NULL if the tree is empty.
 */
tree_node_t* tree_firstNode (tree_t* self);

/**
 * @brief Retrieves the last node (maximum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the last node or NULL if the tree is empty.
 */
tree_node_t* tree_lastNode (tree_t* self);

/**
 * @brief Finds the successor node (next higher key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the higher node.
 * @return Pointer to the higher node or NULL if not found.
 */
tree_node_t* tree_higherNode (tree_t* self, key_t key);

/**
 * @brief Finds the predecessor node (next lower key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the lower node.
 * @return Pointer to the lower node or NULL if not found.
 */
tree_node_t* tree_lowerNode (tree_t* self, key_t key);

/**
 * @brief Finds the nth node (0-based index) in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param index The index of the node to find.
 * @return Pointer to the nth node or NULL if not found.
 */
tree_node_t* tree_nthNode (tree_t* self, size_t index);



/**
 * @brief Adds a value as the first element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool tree_addFirst (tree_t* self, data_t value);

/**
 * @brief Adds a value as the last element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool tree_addLast (tree_t* self, data_t value);

/**
 * @brief Pushes a value to the front of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_pushFirst (tree_t* self, data_t value);

/**
 * @brief Pushes a value to the end of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_pushLast (tree_t* self, data_t value);

/**
 * @brief Pushes a value to the AVL tree (equivalent to pushLast).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool tree_push (tree_t* self, data_t value);



/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_peek (tree_t* self);

/**
 * @brief Peeks at the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
data_t tree_peekFirst (tree_t* self);

/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_peekLast (tree_t* self);

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_pop (tree_t* self);

/**
 * @brief Pops the first value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
data_t tree_popFirst (tree_t* self);

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
data_t tree_popLast (tree_t* self);

/**
 * @brief Checks if a specific key exists in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to check for existence.
 * @return true if the key exists, false otherwise.
 */
bool tree_containsKey (tree_t* self, key_t key);

/**
 * @brief Checks if the tree contains a value based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if a value satisfying the predicate is found, false otherwise.
 */
bool tree_containsValue (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Checks if the current tree contains all elements of another tree.
 * @param self Pointer to the AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all keys in other are present in self, false otherwise.
 */
bool tree_containsAll (tree_t* self, tree_t* other);

/**
 * @brief Copies keys from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store keys.
 * @param array_size Size of the provided array.
 * @return Number of keys copied into the array.
 */
size_t tree_keysToArray (tree_t* self, key_t* array, size_t array_size);

/**
 * @brief Copies values from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store values.
 * @param array_size Size of the provided array.
 * @return Number of values copied into the array.
 */
size_t tree_valuesToArray (tree_t* self, data_t* array, size_t array_size);

/**
 * @brief Allocates a new array and copies the keys from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of keys or NULL if empty.
 */
key_t* tree_keysToNewArray (tree_t* self);

/**
 * @brief Allocates a new array and copies the values from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of values or NULL if empty.
 */
data_t* tree_valuesToNewArray (tree_t* self);

/**
 * @brief Reduces tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced double value.
 */
double tree_reduceToDouble (tree_t* self, double (*functor)(tree_t*, tree_node_t*, double, void*), double initial, void* context);

/**
 * @brief Reduces tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced int64 value.
 */
int64_t tree_reduceToInt64 (tree_t* self, int64_t (*functor)(tree_t*, tree_node_t*, int64_t, void*), int64_t initial, void* context);

/**
 * @brief Sums tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as a double value.
 */
double tree_sumToDouble (tree_t* self, double (*functor)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Sums tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as an int64 value.
 */
int64_t tree_sumToInt64 (tree_t* self, int64_t (*functor)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Puts all elements from another tree into this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all elements were successfully added, false otherwise.
 */
bool tree_putAll (tree_t* self, tree_t* other);

/**
 * @brief Removes all elements that exist in another tree from this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void tree_removeAll (tree_t* self, tree_t* other);

/**
 * @brief Retains only elements that exist in another tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void tree_retainAll (tree_t* self, tree_t* other);

/**
 * @brief Inserts a key-value pair into the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to insert.
 * @param value Data value to associate with the key.
 * @return true if insertion was successful, false otherwise.
 */
bool tree_put (tree_t* self, key_t key, data_t value);

/**
 * @brief Retrieves the value associated with a key in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to search for.
 * @return The associated value or default value if key not found.
 */
data_t tree_get (tree_t* self, key_t key);

/**
 * @brief Removes a key and its value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to remove.
 */
void tree_remove (tree_t* self, key_t key);

/**
 * @brief Removes the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_removeFirst (tree_t* self);

/**
 * @brief Removes the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void tree_removeLast (tree_t* self);

/**
 * @brief Removes nodes from the tree based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 */
void tree_removeIf (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Applies a function to each node in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param functor Function to apply to each node.
 * @param context Additional context passed to the functor.
 */
void tree_forEach (tree_t* self, void (*functor)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Checks if any nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if any match found, false otherwise.
 */
bool tree_anyMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Checks if all nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if all nodes match, false otherwise.
 */
bool tree_allMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Checks if none of the nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if none match, false otherwise.
 */
bool tree_noneMatch (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Counts the number of nodes matching a given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return Count of nodes matching the predicate.
 */
size_t tree_count (tree_t* self, bool (*predicate)(tree_t*, tree_node_t*, void*), void* context);

/**
 * @brief Checks if two trees are equal based on a comparison predicate.
 * @param self Pointer to the first AVL tree.
 * @param other Pointer to the second AVL tree.
 * @param predicate Function pointer for node comparison.
 * @param context Additional context passed to the predicate.
 * @return true if the trees are equal, false otherwise.
 */
bool tree_isEqual (tree_t* self, tree_t* other, bool (*predicate)(tree_node_t*, tree_node_t*, void*), void* context);

/**
 * @brief Creates an iterator for the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Iterator structure for the tree.
 */
tree_iterator_t tree_iter (tree_t* self);

/**
 * @brief Creates an iterator starting at a specific key in the tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to start the iteration at.
 * @return Iterator structure for the tree starting at the given key.
 */
tree_iterator_t tree_iter_at (tree_t* self, key_t key);

/**
 * @brief Creates an iterator starting at a specific node in the tree.
 * @param self Pointer to the AVL tree.
 * @param node Node to start the iteration at.
 * @return Iterator structure for the tree starting at the given node.
 */
tree_iterator_t tree_iter_atNode (tree_t* self, tree_node_t* node);

/**
 * @brief Frees the resources associated with a tree iterator.
 * @param self Pointer to the tree iterator to free.
 */
void tree_iter_free (tree_iterator_t* self);

/**
 * @brief Checks if the iterator has a next element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a next element, false otherwise.
 */
bool tree_iter_hasNext (tree_iterator_t* self);

/**
 * @brief Checks if the iterator has a previous element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a previous element, false otherwise.
 */
bool tree_iter_hasPrev (tree_iterator_t* self);

/**
 * @brief Advances the iterator to the next element.
 * @param self Pointer to the tree iterator.
 */
void tree_iter_next (tree_iterator_t* self);

/**
 * @brief Moves the iterator to the previous element.
 * @param self Pointer to the tree iterator.
 */
void tree_iter_prev (tree_iterator_t* self);

/**
 * @brief Retrieves the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Pointer to the current node.
 */
tree_node_t* tree_iter_node (tree_iterator_t* self);

/**
 * @brief Retrieves the key of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Key of the current node.
 */
key_t tree_iter_key (tree_iterator_t* self);

/**
 * @brief Sets the data value of the current node in the iterator.
 * @param self Pointer to the tree iterator.
 * @param value Data value to set.
 */
void tree_iter_set (tree_iterator_t* self, data_t value);

/**
 * @brief Retrieves the data value of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Data value of the current node.
 */
data_t tree_iter_get (tree_iterator_t* self);

#endif // tree_H
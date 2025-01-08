
#! /usr/bin/env python3.10
#
# Copyright (c) 2024 Mackenzie High. All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
# 1. Redistributions of source code must retain the above copyright notice,
# this list of conditions and the following disclaimer.
#
# 2. Redistributions in binary form must reproduce the above copyright notice,
# this list of conditions and the following disclaimer in the documentation
# and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
# OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# =============================================================================================== #
#
# =============================================================================================== #

import argparse
import os
import sys
import pathlib
import tornado.template


TREE_TEMPLATE_H = '''
{% autoescape None %}

{{COPYRIGHT_HEADER}}

#ifndef {{NAME}}_H
#define {{NAME}}_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

{% for path in INCLUDE_PATHS %}
#include "{{path[0]}}"
{% end %}

/**
 * Forward declaration of the tree_node_t structure.
 */
typedef struct {{NAME}}_node {{NAME}}_node_t;

/**
 * @struct tree_node
 * @brief Represents a node in the AVL tree.
 */
typedef struct {{NAME}}_node
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
    {{NAME}}_node_t* left;

    /**
     * Pointer to the right child node.
     */
    {{NAME}}_node_t* right;

    /**
     * The key that identifies this node in the tree.
     */
    {{KEY_TYPE}} key;

    /**
     * The data stored in this node.
     */
    {{VALUE_TYPE}} value;

} {{NAME}}_node_t;

/**
 * Forward declaration of the tree_allocator_t structure.
 */
typedef struct {{NAME}}_allocator {{NAME}}_allocator_t;

/**
 * @struct tree_allocator
 * @brief Memory allocator interface for tree nodes.
 */
typedef struct {{NAME}}_allocator
{
    /**
     * Function pointer to allocate a new tree node.
     */
    {{NAME}}_node_t* (*allocate)({{NAME}}_allocator_t* self);

    /**
     * Function pointer to release a tree node.
     */
    void (*release)({{NAME}}_allocator_t* self, {{NAME}}_node_t* node);

    /**
     * Function pointer to destroy the allocator.
     */
    void (*destroy)({{NAME}}_allocator_t* self);

    /**
     * Context pointer used by the allocator.
     */
    void* context;

} {{NAME}}_allocator_t;

/**
 * Forward declaration of the tree_t structure.
 */
typedef struct {{NAME}} {{NAME}}_t;

/**
 * @typedef tree_comparator_t
 * @brief Comparator function type for comparing tree keys.
 */
typedef int (*{{NAME}}_comparator_t)({{NAME}}_t*, {{KEY_TYPE}}*, {{KEY_TYPE}}*);

/**
 * @struct tree
 * @brief Represents the AVL tree structure with its properties and operations.
 */
typedef struct {{NAME}}
{
    /**
     * Number of nodes in the tree.
     */
    size_t size;

    /**
     * Pointer to the root node of the tree.
     */
    {{NAME}}_node_t* root;

    /**
     * Pointer to the allocator used for tree nodes.
     */
    {{NAME}}_allocator_t* allocator;

    /**
     * Comparator function for comparing keys.
     */
    {{NAME}}_comparator_t comparator;

} {{NAME}}_t;

/**
 * @struct tree_iterator
 * @brief Iterator structure for traversing the tree.
 */
typedef struct
{
    /**
     * Pointer to the owning tree.
     */
    {{NAME}}_t* owner;

    /**
     * Pointer to the current node in iteration.
     */
    {{NAME}}_node_t* node;

} {{NAME}}_iterator_t;

/**
 * @brief Creates a new dynamic tree allocator.
 * @return Pointer to the newly created tree allocator.
 */
{{NAME}}_allocator_t* {{NAME}}_allocator_dynamic ();

/**
 * @brief Creates a new pooled tree allocator with preallocated nodes.
 * @param preallocated Number of nodes to preallocate.
 * @param capacity Maximum capacity of the pooled allocator.
 * @return Pointer to the newly created pooled tree allocator.
 */
{{NAME}}_allocator_t* {{NAME}}_allocator_pooled (size_t preallocated, size_t capacity);

/**
 * @brief Creates a new slab tree allocator with a specified capacity.
 * @param capacity Maximum capacity of the slab allocator.
 * @return Pointer to the newly created slab tree allocator.
 */
{{NAME}}_allocator_t* {{NAME}}_allocator_slab (size_t capacity);

/**
 * @brief Frees the resources associated with a tree allocator.
 * @param self Pointer to the tree allocator to free.
 */
void {{NAME}}_allocator_free ({{NAME}}_allocator_t* self);

/**
 * @brief Returns the natural order comparator function for keys.
 * @return Function pointer to the natural order comparator.
 */
{{NAME}}_comparator_t {{NAME}}_comparator_naturalOrder ();

/**
 * @brief Returns the reverse order comparator function for keys.
 * @return Function pointer to the reverse order comparator.
 */
{{NAME}}_comparator_t {{NAME}}_comparator_reverseOrder ();

/**
 * @brief Returns the default key value.
 * @return Default key.
 */
{{KEY_TYPE}} {{NAME}}_defaultKey ();

/**
 * @brief Returns the default data value.
 * @return Default data value.
 */
{{VALUE_TYPE}} {{NAME}}_defaultValue ();

/**
 * @brief Retrieves the key of a given tree node.
 * @param self Pointer to the tree node.
 * @return Key of the node.
 */
{{KEY_TYPE}} {{NAME}}_node_key ({{NAME}}_node_t* self);

/**
 * @brief Sets the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @param value The data value to set.
 */
void {{NAME}}_node_set ({{NAME}}_node_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Retrieves the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @return Data value of the node.
 */
{{VALUE_TYPE}} {{NAME}}_node_get ({{NAME}}_node_t* self);

/**
 * @brief Creates a new instance of the AVL tree.
 * @return Pointer to the newly created AVL tree.
 */
{{NAME}}_t* {{NAME}}_new ();

/**
 * @brief Creates a new AVL tree with a specified allocator and comparator.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Pointer to the newly created AVL tree.
 */
{{NAME}}_t* {{NAME}}_make ({{NAME}}_allocator_t* allocator, {{NAME}}_comparator_t comparator);

/**
 * @brief Frees the resources of an AVL tree.
 * @param self Pointer to the AVL tree to free.
 */
void {{NAME}}_free ({{NAME}}_t* self);

/**
 * @brief Creates an AVL tree with stack-allocated properties.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Stack-allocated AVL tree structure.
 */
{{NAME}}_t {{NAME}}_make_stackalloc ({{NAME}}_allocator_t* allocator, {{NAME}}_comparator_t comparator);

/**
 * @brief Frees the resources of a stack-allocated AVL tree.
 * @param self Pointer to the stack-allocated AVL tree.
 */
void {{NAME}}_free_stackalloc ({{NAME}}_t* self);

/**
 * @brief Creates a copy of an existing AVL tree.
 * @param self Pointer to the tree to be copied.
 * @return Pointer to the newly created copy of the tree.
 */
{{NAME}}_t* {{NAME}}_copy ({{NAME}}_t* self);

/**
 * @brief Retrieves the number of nodes in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Number of nodes in the tree.
 */
size_t {{NAME}}_size ({{NAME}}_t* self);

/**
 * @brief Checks if the AVL tree is empty.
 * @param self Pointer to the AVL tree.
 * @return true if the tree is empty, false otherwise.
 */
bool {{NAME}}_isEmpty ({{NAME}}_t* self);

/**
 * @brief Clears all nodes from the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void {{NAME}}_clear ({{NAME}}_t* self);

/**
 * @brief Inserts a node with a specified key and returns the inserted node.
 * @param self Pointer to the AVL tree.
 * @param key Key for the node to insert.
 * @return Pointer to the created node.
 */
{{NAME}}_node_t* {{NAME}}_putNode ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Retrieves a node by its key.
 * @param self Pointer to the AVL tree.
 * @param key Key of the node to find.
 * @return Pointer to the found node or NULL if not found.
 */
{{NAME}}_node_t* {{NAME}}_getNode ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Retrieves the root node of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the root node.
 */
{{NAME}}_node_t* {{NAME}}_rootNode ({{NAME}}_t* self);

/**
 * @brief Retrieves the first node (minimum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the first node or NULL if the tree is empty.
 */
{{NAME}}_node_t* {{NAME}}_firstNode ({{NAME}}_t* self);

/**
 * @brief Retrieves the last node (maximum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the last node or NULL if the tree is empty.
 */
{{NAME}}_node_t* {{NAME}}_lastNode ({{NAME}}_t* self);

/**
 * @brief Finds the successor node (next higher key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the higher node.
 * @return Pointer to the higher node or NULL if not found.
 */
{{NAME}}_node_t* {{NAME}}_higherNode ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Finds the predecessor node (next lower key) of a given key.
 * @param self Pointer to the AVL tree.
 * @param key Key for which to find the lower node.
 * @return Pointer to the lower node or NULL if not found.
 */
{{NAME}}_node_t* {{NAME}}_lowerNode ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Finds the nth node (0-based index) in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param index The index of the node to find.
 * @return Pointer to the nth node or NULL if not found.
 */
{{NAME}}_node_t* {{NAME}}_nthNode ({{NAME}}_t* self, size_t index);

{% if DEQUE %}

/**
 * @brief Adds a value as the first element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool {{NAME}}_addFirst ({{NAME}}_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Adds a value as the last element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool {{NAME}}_addLast ({{NAME}}_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Pushes a value to the front of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_pushFirst ({{NAME}}_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Pushes a value to the end of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_pushLast ({{NAME}}_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Pushes a value to the AVL tree (equivalent to pushLast).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_push ({{NAME}}_t* self, {{VALUE_TYPE}} value);

{% end %}

/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_peek ({{NAME}}_t* self);

/**
 * @brief Peeks at the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_peekFirst ({{NAME}}_t* self);

/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_peekLast ({{NAME}}_t* self);

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_pop ({{NAME}}_t* self);

/**
 * @brief Pops the first value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_popFirst ({{NAME}}_t* self);

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_popLast ({{NAME}}_t* self);

/**
 * @brief Checks if a specific key exists in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to check for existence.
 * @return true if the key exists, false otherwise.
 */
bool {{NAME}}_containsKey ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Checks if the tree contains a value based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if a value satisfying the predicate is found, false otherwise.
 */
bool {{NAME}}_containsValue ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Checks if the current tree contains all elements of another tree.
 * @param self Pointer to the AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all keys in other are present in self, false otherwise.
 */
bool {{NAME}}_containsAll ({{NAME}}_t* self, {{NAME}}_t* other);

/**
 * @brief Copies keys from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store keys.
 * @param array_size Size of the provided array.
 * @return Number of keys copied into the array.
 */
size_t {{NAME}}_keysToArray ({{NAME}}_t* self, {{KEY_TYPE}}* array, size_t array_size);

/**
 * @brief Copies values from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store values.
 * @param array_size Size of the provided array.
 * @return Number of values copied into the array.
 */
size_t {{NAME}}_valuesToArray ({{NAME}}_t* self, {{VALUE_TYPE}}* array, size_t array_size);

/**
 * @brief Allocates a new array and copies the keys from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of keys or NULL if empty.
 */
{{KEY_TYPE}}* {{NAME}}_keysToNewArray ({{NAME}}_t* self);

/**
 * @brief Allocates a new array and copies the values from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of values or NULL if empty.
 */
{{VALUE_TYPE}}* {{NAME}}_valuesToNewArray ({{NAME}}_t* self);

/**
 * @brief Reduces tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced double value.
 */
double {{NAME}}_reduceToDouble ({{NAME}}_t* self, double (*functor)({{NAME}}_t*, {{NAME}}_node_t*, double, void*), double initial, void* context);

/**
 * @brief Reduces tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for reduction.
 * @param initial Initial value for reduction.
 * @param context Additional context passed to the functor.
 * @return Reduced int64 value.
 */
int64_t {{NAME}}_reduceToInt64 ({{NAME}}_t* self, int64_t (*functor)({{NAME}}_t*, {{NAME}}_node_t*, int64_t, void*), int64_t initial, void* context);

/**
 * @brief Sums tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as a double value.
 */
double {{NAME}}_sumToDouble ({{NAME}}_t* self, double (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Sums tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as an int64 value.
 */
int64_t {{NAME}}_sumToInt64 ({{NAME}}_t* self, int64_t (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Puts all elements from another tree into this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all elements were successfully added, false otherwise.
 */
bool {{NAME}}_putAll ({{NAME}}_t* self, {{NAME}}_t* other);

/**
 * @brief Removes all elements that exist in another tree from this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void {{NAME}}_removeAll ({{NAME}}_t* self, {{NAME}}_t* other);

/**
 * @brief Retains only elements that exist in another tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void {{NAME}}_retainAll ({{NAME}}_t* self, {{NAME}}_t* other);

/**
 * @brief Inserts a key-value pair into the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to insert.
 * @param value Data value to associate with the key.
 * @return true if insertion was successful, false otherwise.
 */
bool {{NAME}}_put ({{NAME}}_t* self, {{KEY_TYPE}} key, {{VALUE_TYPE}} value);

/**
 * @brief Retrieves the value associated with a key in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to search for.
 * @return The associated value or default value if key not found.
 */
{{VALUE_TYPE}} {{NAME}}_get ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Removes a key and its value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to remove.
 */
void {{NAME}}_remove ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Removes the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void {{NAME}}_removeFirst ({{NAME}}_t* self);

/**
 * @brief Removes the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void {{NAME}}_removeLast ({{NAME}}_t* self);

/**
 * @brief Removes nodes from the tree based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 */
void {{NAME}}_removeIf ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Applies a function to each node in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param functor Function to apply to each node.
 * @param context Additional context passed to the functor.
 */
void {{NAME}}_forEach ({{NAME}}_t* self, void (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Checks if any nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if any match found, false otherwise.
 */
bool {{NAME}}_anyMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Checks if all nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if all nodes match, false otherwise.
 */
bool {{NAME}}_allMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Checks if none of the nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if none match, false otherwise.
 */
bool {{NAME}}_noneMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Counts the number of nodes matching a given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return Count of nodes matching the predicate.
 */
size_t {{NAME}}_count ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Checks if two trees are equal based on a comparison predicate.
 * @param self Pointer to the first AVL tree.
 * @param other Pointer to the second AVL tree.
 * @param predicate Function pointer for node comparison.
 * @param context Additional context passed to the predicate.
 * @return true if the trees are equal, false otherwise.
 */
bool {{NAME}}_isEqual ({{NAME}}_t* self, {{NAME}}_t* other, bool (*predicate)({{NAME}}_node_t*, {{NAME}}_node_t*, void*), void* context);

/**
 * @brief Creates an iterator for the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Iterator structure for the tree.
 */
{{NAME}}_iterator_t {{NAME}}_iter ({{NAME}}_t* self);

/**
 * @brief Creates an iterator starting at a specific key in the tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to start the iteration at.
 * @return Iterator structure for the tree starting at the given key.
 */
{{NAME}}_iterator_t {{NAME}}_iter_at ({{NAME}}_t* self, {{KEY_TYPE}} key);

/**
 * @brief Creates an iterator starting at a specific node in the tree.
 * @param self Pointer to the AVL tree.
 * @param node Node to start the iteration at.
 * @return Iterator structure for the tree starting at the given node.
 */
{{NAME}}_iterator_t {{NAME}}_iter_atNode ({{NAME}}_t* self, {{NAME}}_node_t* node);

/**
 * @brief Frees the resources associated with a tree iterator.
 * @param self Pointer to the tree iterator to free.
 */
void {{NAME}}_iter_free ({{NAME}}_iterator_t* self);

/**
 * @brief Checks if the iterator has a next element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a next element, false otherwise.
 */
bool {{NAME}}_iter_hasNext ({{NAME}}_iterator_t* self);

/**
 * @brief Checks if the iterator has a previous element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a previous element, false otherwise.
 */
bool {{NAME}}_iter_hasPrev ({{NAME}}_iterator_t* self);

/**
 * @brief Advances the iterator to the next element.
 * @param self Pointer to the tree iterator.
 */
void {{NAME}}_iter_next ({{NAME}}_iterator_t* self);

/**
 * @brief Moves the iterator to the previous element.
 * @param self Pointer to the tree iterator.
 */
void {{NAME}}_iter_prev ({{NAME}}_iterator_t* self);

/**
 * @brief Retrieves the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Pointer to the current node.
 */
{{NAME}}_node_t* {{NAME}}_iter_node ({{NAME}}_iterator_t* self);

/**
 * @brief Retrieves the key of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Key of the current node.
 */
{{KEY_TYPE}} {{NAME}}_iter_key ({{NAME}}_iterator_t* self);

/**
 * @brief Sets the data value of the current node in the iterator.
 * @param self Pointer to the tree iterator.
 * @param value Data value to set.
 */
void {{NAME}}_iter_set ({{NAME}}_iterator_t* self, {{VALUE_TYPE}} value);

/**
 * @brief Retrieves the data value of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Data value of the current node.
 */
{{VALUE_TYPE}} {{NAME}}_iter_get ({{NAME}}_iterator_t* self);

#endif // {{NAME}}_H

{{COPYRIGHT_FOOTER}}
'''

TREE_TEMPLATE_C = '''
{% autoescape None %}

{{COPYRIGHT_HEADER}}

#include "{{HEADER}}"

typedef struct
{
    size_t allocated;

    size_t capacity;

    {{NAME}}_node_t* free;

} {{NAME}}_allocator_pooled_t;

typedef struct
{
    size_t capacity;

    {{NAME}}_node_t* free;

    {{NAME}}_node_t nodes[];

} {{NAME}}_allocator_slab_t;

static int32_t height_of ({{NAME}}_node_t* node)
{
    return NULL == node ? 0 : node->height;
}

static size_t size_of ({{NAME}}_node_t* node)
{
    return NULL == node ? 0 : node->size;
}

static int8_t balance_of ({{NAME}}_node_t* node)
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

static {{NAME}}_node_t* create_node ({{NAME}}_t* self, {{KEY_TYPE}}* key)
{
    {{NAME}}_node_t* node = self->allocator->allocate(self->allocator);

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

static void update_size ({{NAME}}_node_t* node)
{
    if (NULL != node)
    {
        node->size = 1 + size_of(node->left) + size_of(node->right);
    }
}

static void update_height ({{NAME}}_node_t* node)
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

static {{NAME}}_node_t* rotate_right ({{NAME}}_node_t* y)
{
    if (NULL == y || NULL == y->left)
    {
        return y;
    }

    {{NAME}}_node_t* x = y->left;
    {{NAME}}_node_t* z = x->right;

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

static {{NAME}}_node_t* rotate_left ({{NAME}}_node_t* x)
{
    if (NULL == x || NULL == x->right)
    {
        return x;
    }

    {{NAME}}_node_t* y = x->right;
    {{NAME}}_node_t* z = y->left;

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

static {{NAME}}_node_t* rebalance ({{NAME}}_t* self, {{NAME}}_node_t* node, {{KEY_TYPE}}* key)
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

static {{NAME}}_node_t* insert_node ({{NAME}}_t* self, {{NAME}}_node_t* node, {{KEY_TYPE}}* key)
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

static {{NAME}}_node_t* find_min_value_node ({{NAME}}_node_t* node)
{
    {{NAME}}_node_t* current = node;
    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}

static {{NAME}}_node_t* delete_node ({{NAME}}_t* self, {{NAME}}_node_t* node, {{KEY_TYPE}}* key, {{NAME}}_node_t** deleted)
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
        {{NAME}}_node_t* successor = find_min_value_node(node->right);

        // Delete the inorder successor
        {{NAME}}_node_t* deleted_successor = NULL;
        successor->left = node->left;
        successor->right = delete_node(self, node->right, &successor->key, &deleted_successor);
        node = successor;
    }

    return rebalance(self, node, key);
}

static {{NAME}}_node_t* find_node ({{NAME}}_t* self, {{NAME}}_node_t* node, {{KEY_TYPE}}* key)
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
static {{NAME}}_allocator_t TREE_DYNAMIC_ALLOCATOR;

static {{NAME}}_node_t* dynamic_allocate ({{NAME}}_allocator_t* self)
{
    return calloc(1, sizeof({{NAME}}_node_t));
}

static void wipe ({{NAME}}_node_t* self)
{
{% if WIPE %}
    if (NULL != self)
    {
        memset(self, 0, sizeof({{NAME}}_node_t));
    }
{% end %}
}

static void dynamic_release ({{NAME}}_allocator_t* self, {{NAME}}_node_t* node)
{
    if (NULL != node)
    {
        wipe(node);
        free(node);
    }
}

static void dynamic_destroy ({{NAME}}_allocator_t* self)
{
    // Pass, because the dynamic allocator is statically allocated.
}

static {{NAME}}_node_t* pool_allocate ({{NAME}}_allocator_t* self)
{
    {{NAME}}_allocator_pooled_t* context = ({{NAME}}_allocator_pooled_t*) self->context;

    if (NULL == context->free)
    {
        if (context->allocated < context->capacity)
        {
            {{NAME}}_node_t* node = ({{NAME}}_node_t*) calloc(1, sizeof({{NAME}}_node_t));

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
        {{NAME}}_node_t* node = context->free;
        context->free = context->free->right;
        return node;
    }
}

static void pool_release ({{NAME}}_allocator_t* self, {{NAME}}_node_t* node)
{
    wipe(node);
    {{NAME}}_allocator_pooled_t* context = ({{NAME}}_allocator_pooled_t*) self->context;
    node->right = context->free;
    context->free = node;
}

static void pool_destroy ({{NAME}}_allocator_t* self)
{
    if (NULL != self)
    {
        {{NAME}}_allocator_pooled_t* context = ({{NAME}}_allocator_pooled_t*) self->context;

        if (NULL != context)
        {
            {{NAME}}_node_t* p = context->free;
            {{NAME}}_node_t* q = NULL;
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

static {{NAME}}_node_t* slab_allocate ({{NAME}}_allocator_t* self)
{
    {{NAME}}_allocator_slab_t* context = ({{NAME}}_allocator_slab_t*) self->context;

    if (NULL == context->free)
    {
        return NULL;
    }
    else
    {
        {{NAME}}_node_t* node = context->free;
        context->free = context->free->right;
        return node;
    }
}

static void slab_release ({{NAME}}_allocator_t* self, {{NAME}}_node_t* node)
{
    wipe(node);
    {{NAME}}_allocator_slab_t* context = ({{NAME}}_allocator_slab_t*) self->context;
    node->right = context->free;
    context->free = node;
}

static void slab_destroy ({{NAME}}_allocator_t* self)
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
{{NAME}}_allocator_t* {{NAME}}_allocator_dynamic ()
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
{{NAME}}_allocator_t* {{NAME}}_allocator_pooled (size_t preallocated, size_t capacity)
{
    {{NAME}}_allocator_t* self = ({{NAME}}_allocator_t*) calloc(1, sizeof({{NAME}}_allocator_t));

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

    {{NAME}}_allocator_pooled_t* context = ({{NAME}}_allocator_pooled_t*) calloc(1, sizeof({{NAME}}_allocator_pooled_t));

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
        {{NAME}}_node_t* node = calloc(1, sizeof({{NAME}}_node_t));

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
{{NAME}}_allocator_t* {{NAME}}_allocator_slab (size_t capacity)
{
    {{NAME}}_allocator_t* self = ({{NAME}}_allocator_t*) calloc(1, sizeof({{NAME}}_allocator_t) + capacity * sizeof({{NAME}}_node_t));

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

    {{NAME}}_allocator_slab_t* context = ({{NAME}}_allocator_slab_t*) calloc(1, sizeof({{NAME}}_allocator_slab_t) + capacity * sizeof({{NAME}}_node_t));

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
        {{NAME}}_node_t* node = &context->nodes[i];

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
void {{NAME}}_allocator_free ({{NAME}}_allocator_t* self)
{
    if (NULL != self)
    {
        self->destroy(self);
    }
}

static int {{NAME}}_naturalOrder ({{NAME}}_t* self, {{KEY_TYPE}}* X, {{KEY_TYPE}}* Y)
{
{% if COMPARATOR != "" %}
    return {{COMPARATOR}};
{% elif STRNCMP != "" %}
    return strncmp(X, Y, {{STRNCMP}});
{% else %}
    return *X < *Y ? -1 : (*X > *Y ? +1 : 0);
{% end %}
}

static int {{NAME}}_reverseOrder ({{NAME}}_t* self, {{KEY_TYPE}}* X, {{KEY_TYPE}}* Y)
{
    return {{NAME}}_naturalOrder(self, Y, X);
}

/**
 * @brief Returns the natural order comparator function for keys.
 * @return Function pointer to the natural order comparator.
 */
{{NAME}}_comparator_t {{NAME}}_comparator_naturalOrder ()
{
    return &{{NAME}}_naturalOrder;
}

/**
 * @brief Returns the reverse order comparator function for keys.
 * @return Function pointer to the reverse order comparator.
 */
{{NAME}}_comparator_t {{NAME}}_comparator_reverseOrder ()
{
    return &{{NAME}}_reverseOrder;
}

/**
 * @brief Returns the default key value.
 * @return Default key.
 */
{{KEY_TYPE}} {{NAME}}_defaultKey ()
{
{% if DEFAULT_KEY == "" %}
    return 0;
{% else %}
    return {{DEFAULT_KEY}};
{% end %}
}

/**
 * @brief Returns the default data value.
 * @return Default data value.
 */
{{VALUE_TYPE}} {{NAME}}_defaultValue ()
{
{% if DEFAULT_VALUE == "" %}
    return 0;
{% else %}
    return {{DEFAULT_VALUE}};
{% end %}
}

/**
 * @brief Creates a new instance of the AVL tree.
 * @return Pointer to the newly created AVL tree.
 */
{{NAME}}_t* {{NAME}}_new ()
{
    {{NAME}}_allocator_t* allocator = {{NAME}}_allocator_dynamic();
    {{NAME}}_comparator_t comparator = {{NAME}}_comparator_naturalOrder();
    return {{NAME}}_make(allocator, comparator);
}

/**
 * @brief Creates a new AVL tree with a specified allocator and comparator.
 * @param allocator Pointer to the tree allocator.
 * @param comparator Function pointer for key comparison.
 * @return Pointer to the newly created AVL tree.
 */
{{NAME}}_t* {{NAME}}_make ({{NAME}}_allocator_t* allocator, {{NAME}}_comparator_t comparator)
{
    {{NAME}}_t* result = calloc(1, sizeof({{NAME}}_t));

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
void {{NAME}}_free ({{NAME}}_t* self)
{
    if (NULL != self)
    {
        {{NAME}}_clear(self);
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
{{NAME}}_t {{NAME}}_make_stackalloc ({{NAME}}_allocator_t* allocator, {{NAME}}_comparator_t comparator)
{
    {{NAME}}_t result;
    result.allocator = allocator;
    result.comparator = comparator;
    return result;
}

/**
 * @brief Frees the resources of a stack-allocated AVL tree.
 * @param self Pointer to the stack-allocated AVL tree.
 */
void {{NAME}}_free_stackalloc ({{NAME}}_t* self)
{
    if (NULL != self)
    {
        {{NAME}}_clear(self);
        self->allocator = NULL;
        self->comparator = NULL;
    }
}

/**
 * @brief Creates a copy of an existing AVL tree.
 * @param self Pointer to the tree to be copied.
 * @return Pointer to the newly created copy of the tree.
 */
{{NAME}}_t* {{NAME}}_copy ({{NAME}}_t* self)
{
    {{NAME}}_t* copy = {{NAME}}_make(self->allocator, self->comparator);

    if (NULL == copy)
    {
        return NULL;
    }
    else if ({{NAME}}_putAll(copy, self))
    {
        return copy;
    }
    else
    {
        {{NAME}}_free(copy);
        return NULL;
    }
}

/**
 * @brief Retrieves the number of nodes in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Number of nodes in the tree.
 */
size_t {{NAME}}_size ({{NAME}}_t* self)
{
    return self->size;
}

/**
 * @brief Checks if the AVL tree is empty.
 * @param self Pointer to the AVL tree.
 * @return true if the tree is empty, false otherwise.
 */
bool {{NAME}}_isEmpty ({{NAME}}_t* self)
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
void {{NAME}}_clear ({{NAME}}_t* self)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_isEmpty(self) == false)
        {
            {{NAME}}_removeFirst(self);
        }
    }
    {{NAME}}_iter_free(&iter);
}

/**
 * @brief Checks if a specific key exists in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to check for existence.
 * @return true if the key exists, false otherwise.
 */
bool {{NAME}}_containsKey ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    return NULL != find_node(self, self->root, &key);
}

/**
 * @brief Checks if the current tree contains all elements of another tree.
 * @param self Pointer to the AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all keys in other are present in self, false otherwise.
 */
bool {{NAME}}_containsValue ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);

            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);

            if (predicate(self, node, context))
            {
                return true;
            }
        }
    }
    {{NAME}}_iter_free(&iter);
    return false;
}

/**
 * @brief Inserts a node with a specified key and returns the inserted node.
 * @param self Pointer to the AVL tree.
 * @param key Key for the node to insert.
 * @return Pointer to the created node.
 */
{{NAME}}_node_t* {{NAME}}_putNode ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* root = insert_node(self, self->root, &key);
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
{{NAME}}_node_t* {{NAME}}_getNode ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    return find_node(self, self->root, &key);
}

/**
 * @brief Retrieves the root node of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the root node.
 */
{{NAME}}_node_t* {{NAME}}_rootNode ({{NAME}}_t* self)
{
    return self->root;
}

/**
 * @brief Retrieves the first node (minimum key) of the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the first node or NULL if the tree is empty.
 */
{{NAME}}_node_t* {{NAME}}_firstNode ({{NAME}}_t* self)
{
    {{NAME}}_node_t* p = self->root;

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
{{NAME}}_node_t* {{NAME}}_lastNode ({{NAME}}_t* self)
{
    {{NAME}}_node_t* p = self->root;

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
{{NAME}}_node_t* {{NAME}}_higherNode ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* current = self->root;
    {{NAME}}_node_t* successor = NULL;

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
{{NAME}}_node_t* {{NAME}}_lowerNode ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* current = self->root;
    {{NAME}}_node_t* predecessor = NULL;

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

static {{NAME}}_node_t* find_nth (size_t prior, {{NAME}}_node_t* node, size_t index)
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
{{NAME}}_node_t* {{NAME}}_nthNode ({{NAME}}_t* self, size_t index)
{
    return find_nth(0, self->root, index);
}

{% if DEQUE %}

/**
 * @brief Adds a value as the first element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool {{NAME}}_addFirst ({{NAME}}_t* self, {{VALUE_TYPE}} value)
{
    return {{NAME}}_pushFirst(self, value);
}

/**
 * @brief Adds a value as the last element in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param value Data value to add.
 * @return true if the addition was successful, false otherwise.
 */
bool {{NAME}}_addLast ({{NAME}}_t* self, {{VALUE_TYPE}} value)
{
    return {{NAME}}_pushLast(self, value);
}

/**
 * @brief Pushes a value to the front of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_pushFirst ({{NAME}}_t* self, {{VALUE_TYPE}} value)
{
    {{NAME}}_node_t* node = {{NAME}}_firstNode(self);

    if (NULL == node)
    {
        return {{NAME}}_put(self, 0, value);
    }
    else
    {
        return {{NAME}}_put(self, node->key - 1, value);
    }
}

/**
 * @brief Pushes a value to the end of the AVL tree (Deque operation).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_pushLast ({{NAME}}_t* self, {{VALUE_TYPE}} value)
{
    {{NAME}}_node_t* node = {{NAME}}_lastNode(self);

    if (NULL == node)
    {
        return {{NAME}}_put(self, 0, value);
    }
    else
    {
        return {{NAME}}_put(self, node->key + 1, value);
    }
}

/**
 * @brief Pushes a value to the AVL tree (equivalent to pushLast).
 * @param self Pointer to the AVL tree.
 * @param value Data value to push.
 * @return true if the push was successful, false otherwise.
 */
bool {{NAME}}_push ({{NAME}}_t* self, {{VALUE_TYPE}} value)
{
    return {{NAME}}_pushLast(self, value);
}

{% end %}

/**
 * @brief Peeks at the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_peek ({{NAME}}_t* self)
{
    return {{NAME}}_peekLast(self);
}

/**
 * @brief Peeks at the first value in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_peekFirst ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_firstNode(self);

    if (NULL == node)
    {
        return {{NAME}}_defaultValue();
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
{{VALUE_TYPE}} {{NAME}}_peekLast ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_lastNode(self);

    if (NULL == node)
    {
        return {{NAME}}_defaultValue();
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
{{VALUE_TYPE}} {{NAME}}_pop ({{NAME}}_t* self)
{
    return {{NAME}}_popLast(self);
}

/**
 * @brief Pops the first value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return First data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_popFirst ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_firstNode(self);

    if (NULL == node)
    {
        return {{NAME}}_defaultValue();
    }
    else
    {
        {{VALUE_TYPE}} value = node->value;
        {{NAME}}_removeFirst(self);
        return value;
    }
}

/**
 * @brief Pops the last value from the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Last data value in the tree or default value if empty.
 */
{{VALUE_TYPE}} {{NAME}}_popLast ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_lastNode(self);

    if (NULL == node)
    {
        return {{NAME}}_defaultValue();
    }
    else
    {
        {{VALUE_TYPE}} value = node->value;
        {{NAME}}_removeLast(self);
        return value;
    }
}

bool {{NAME}}_containsAll ({{NAME}}_t* self, {{NAME}}_t* other)
{
    if ({{NAME}}_size(self) < {{NAME}}_size(other))
    {
        return false;
    }

    {{NAME}}_iterator_t iter = {{NAME}}_iter(other);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);

            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);

            if ({{NAME}}_containsKey(self, node->key) == false)
            {
                return false;
            }
        }
    }
    {{NAME}}_iter_free(&iter);
    return true;
}

/**
 * @brief Copies keys from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store keys.
 * @param array_size Size of the provided array.
 * @return Number of keys copied into the array.
 */
size_t {{NAME}}_keysToArray ({{NAME}}_t* self, {{KEY_TYPE}}* array, size_t array_size)
{
    size_t count = 0;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        for (count = 0; {{NAME}}_iter_hasNext(&iter) && (count < array_size); count++)
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            array[count] = node->key;
        }
    }
    {{NAME}}_iter_free(&iter);
    return count;
}

/**
 * @brief Copies values from the tree into an array.
 * @param self Pointer to the AVL tree.
 * @param array Array to store values.
 * @param array_size Size of the provided array.
 * @return Number of values copied into the array.
 */
size_t {{NAME}}_valuesToArray ({{NAME}}_t* self, {{VALUE_TYPE}}* array, size_t array_size)
{
    size_t count = 0;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        for (count = 0; {{NAME}}_iter_hasNext(&iter) && (count < array_size); count++)
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            array[count] = node->value;
        }
    }
    {{NAME}}_iter_free(&iter);
    return count;
}

/**
 * @brief Allocates a new array and copies the keys from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of keys or NULL if empty.
 */
{{KEY_TYPE}}* {{NAME}}_keysToNewArray ({{NAME}}_t* self)
{
    if (NULL == self)
    {
        return NULL;
    }
    else if ({{NAME}}_isEmpty(self))
    {
        return NULL;
    }

    size_t size = {{NAME}}_size(self);

    {{KEY_TYPE}}* array = ({{KEY_TYPE}}*) calloc(size, sizeof({{KEY_TYPE}}));

    if (NULL == array)
    {
        return NULL;
    }
    else
    {
        {{NAME}}_keysToArray(self, array, size);
        return array;
    }
}

/**
 * @brief Allocates a new array and copies the values from the tree into it.
 * @param self Pointer to the AVL tree.
 * @return Pointer to the newly allocated array of values or NULL if empty.
 */
{{VALUE_TYPE}}* {{NAME}}_valuesToNewArray ({{NAME}}_t* self)
{
    if (NULL == self)
    {
        return NULL;
    }
    else if ({{NAME}}_isEmpty(self))
    {
        return NULL;
    }

    size_t size = {{NAME}}_size(self);

    {{VALUE_TYPE}}* array = ({{VALUE_TYPE}}*) calloc(size, sizeof({{VALUE_TYPE}}));

    if (NULL == array)
    {
        return NULL;
    }
    else
    {
        {{NAME}}_valuesToArray(self, array, size);
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
double {{NAME}}_reduceToDouble ({{NAME}}_t* self, double (*functor)({{NAME}}_t*, {{NAME}}_node_t*, double, void*), double initial, void* context)
{
    double result = initial;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            result = functor(self, node, result, context);
        }
    }
    {{NAME}}_iter_free(&iter);
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
int64_t {{NAME}}_reduceToInt64 ({{NAME}}_t* self, int64_t (*functor)({{NAME}}_t*, {{NAME}}_node_t*, int64_t, void*), int64_t initial, void* context)
{
    int64_t result = initial;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            result = functor(self, node, result, context);
        }
    }
    {{NAME}}_iter_free(&iter);
    return result;
}

/**
 * @brief Sums tree elements to a double value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as a double value.
 */
double {{NAME}}_sumToDouble ({{NAME}}_t* self, double (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    double result = 0;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            result += functor(self, node, context);
        }
    }
    {{NAME}}_iter_free(&iter);
    return result;
}

/**
 * @brief Sums tree elements to an int64 value using a provided function.
 * @param self Pointer to the AVL tree.
 * @param functor Function for summation.
 * @param context Additional context passed to the functor.
 * @return Sum as an int64 value.
 */
int64_t {{NAME}}_sumToInt64 ({{NAME}}_t* self, int64_t (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    int64_t result = 0;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            result += functor(self, node, context);
        }
    }
    {{NAME}}_iter_free(&iter);
    return result;
}

/**
 * @brief Puts all elements from another tree into this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 * @return true if all elements were successfully added, false otherwise.
 */
bool {{NAME}}_putAll ({{NAME}}_t* self, {{NAME}}_t* other)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(other);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);

            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);

            if ({{NAME}}_put(self, node->key, node->value) == false)
            {
                return false;
            }
        }
    }
    {{NAME}}_iter_free(&iter);
    return true;
}

/**
 * @brief Removes all elements that exist in another tree from this tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void {{NAME}}_removeAll ({{NAME}}_t* self, {{NAME}}_t* other)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(other);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            {{NAME}}_remove(self, node->key);
        }
    }
    {{NAME}}_iter_free(&iter);
}

/**
 * @brief Retains only elements that exist in another tree.
 * @param self Pointer to the current AVL tree.
 * @param other Pointer to the other AVL tree.
 */
void {{NAME}}_retainAll ({{NAME}}_t* self, {{NAME}}_t* other)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);

            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);

            if ({{NAME}}_containsKey(other, node->key) == false)
            {
                {{NAME}}_iter_next(&iter);
                {{NAME}}_remove(self, node->key);
            }
        }
    }
    {{NAME}}_iter_free(&iter);
}

/**
 * @brief Inserts a key-value pair into the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to insert.
 * @param value Data value to associate with the key.
 * @return true if insertion was successful, false otherwise.
 */
bool {{NAME}}_put ({{NAME}}_t* self, {{KEY_TYPE}} key, {{VALUE_TYPE}} value)
{
    {{NAME}}_node_t* node = {{NAME}}_putNode(self, key);

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
{{VALUE_TYPE}} {{NAME}}_get ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* node = {{NAME}}_getNode(self, key);

    if (NULL == node)
    {
        return {{NAME}}_defaultValue();
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
void {{NAME}}_remove ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* deleted = NULL;
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
void {{NAME}}_removeFirst ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_firstNode(self);

    if (NULL != node)
    {
        {{NAME}}_remove(self, node->key);
    }
}

/**
 * @brief Removes the last value in the AVL tree.
 * @param self Pointer to the AVL tree.
 */
void {{NAME}}_removeLast ({{NAME}}_t* self)
{
    {{NAME}}_node_t* node = {{NAME}}_lastNode(self);

    if (NULL != node)
    {
        {{NAME}}_remove(self, node->key);
    }
}

/**
 * @brief Removes nodes from the tree based on a predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 */
void {{NAME}}_removeIf ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            if (predicate(self, node, context))
            {
                {{NAME}}_iter_next(&iter);
                {{NAME}}_remove(self, node->key);
            }
        }
    }
    {{NAME}}_iter_free(&iter);
}

/**
 * @brief Applies a function to each node in the AVL tree.
 * @param self Pointer to the AVL tree.
 * @param functor Function to apply to each node.
 * @param context Additional context passed to the functor.
 */
void {{NAME}}_forEach ({{NAME}}_t* self, void (*functor)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            functor(self, node, context);
        }
    }
    {{NAME}}_iter_free(&iter);
}

/**
 * @brief Checks if any nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if any match found, false otherwise.
 */
bool {{NAME}}_anyMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    return {{NAME}}_count(self, predicate, context) > 0;
}

/**
 * @brief Checks if all nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if all nodes match, false otherwise.
 */
bool {{NAME}}_allMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    size_t size = {{NAME}}_size(self);
    return {{NAME}}_count(self, predicate, context) == size;
}

/**
 * @brief Checks if none of the nodes match the given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return true if none match, false otherwise.
 */
bool {{NAME}}_noneMatch ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    return {{NAME}}_count(self, predicate, context) == 0;
}

/**
 * @brief Counts the number of nodes matching a given predicate.
 * @param self Pointer to the AVL tree.
 * @param predicate Function pointer to the predicate to evaluate.
 * @param context Additional context passed to the predicate.
 * @return Count of nodes matching the predicate.
 */
size_t {{NAME}}_count ({{NAME}}_t* self, bool (*predicate)({{NAME}}_t*, {{NAME}}_node_t*, void*), void* context)
{
    size_t count = 0;
    {{NAME}}_iterator_t iter = {{NAME}}_iter(self);
    {
        while ({{NAME}}_iter_hasNext(&iter))
        {
            {{NAME}}_iter_next(&iter);
            {{NAME}}_node_t* node = {{NAME}}_iter_node(&iter);
            if (predicate(self, node, context))
            {
                ++count;
            }
        }
    }
    {{NAME}}_iter_free(&iter);
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
bool {{NAME}}_isEqual ({{NAME}}_t* self, {{NAME}}_t* other, bool (*predicate)({{NAME}}_node_t*, {{NAME}}_node_t*, void*), void* context)
{
    if ({{NAME}}_size(self) != {{NAME}}_size(other))
    {
        return false;
    }

    {{NAME}}_iterator_t iterX = {{NAME}}_iter(self);
    {
        {{NAME}}_iterator_t iterY = {{NAME}}_iter(other);
        {
            while ({{NAME}}_iter_hasNext(&iterX))
            {
                {{NAME}}_iter_next(&iterX);
                {{NAME}}_iter_next(&iterY);

                {{NAME}}_node_t* nodeX = {{NAME}}_iter_node(&iterX);
                {{NAME}}_node_t* nodeY = {{NAME}}_iter_node(&iterY);

                if (predicate(nodeX, nodeY, context) == false)
                {
                    return false;
                }
            }
        }
        {{NAME}}_iter_free(&iterY);
    }
    {{NAME}}_iter_free(&iterX);
    return true;
}

/**
 * @brief Creates an iterator for the AVL tree.
 * @param self Pointer to the AVL tree.
 * @return Iterator structure for the tree.
 */
{{NAME}}_iterator_t {{NAME}}_iter ({{NAME}}_t* self)
{
    return {{NAME}}_iter_atNode(self, NULL);
}

/**
 * @brief Creates an iterator starting at a specific key in the tree.
 * @param self Pointer to the AVL tree.
 * @param key Key to start the iteration at.
 * @return Iterator structure for the tree starting at the given key.
 */
{{NAME}}_iterator_t {{NAME}}_iter_at ({{NAME}}_t* self, {{KEY_TYPE}} key)
{
    {{NAME}}_node_t* node = find_node(self, self->root, &key);
    return {{NAME}}_iter_atNode(self, node);
}

/**
 * @brief Creates an iterator starting at a specific node in the tree.
 * @param self Pointer to the AVL tree.
 * @param node Node to start the iteration at.
 * @return Iterator structure for the tree starting at the given node.
 */
{{NAME}}_iterator_t {{NAME}}_iter_atNode ({{NAME}}_t* self, {{NAME}}_node_t* node)
{
    {{NAME}}_iterator_t result;
    result.owner = self;
    result.node = node;
    return result;
}

/**
 * @brief Frees the resources associated with a tree iterator.
 * @param self Pointer to the tree iterator to free.
 */
void {{NAME}}_iter_free ({{NAME}}_iterator_t* self)
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
bool {{NAME}}_iter_hasNext ({{NAME}}_iterator_t* self)
{
    if ({{NAME}}_isEmpty(self->owner))
    {
        return false;
    }
    else if (NULL == self->node)
    {
        return true;
    }
    else
    {
        {{KEY_TYPE}} key = self->node->key;
        return NULL != {{NAME}}_higherNode(self->owner, key);
    }
}

/**
 * @brief Checks if the iterator has a previous element.
 * @param self Pointer to the tree iterator.
 * @return true if there is a previous element, false otherwise.
 */
bool {{NAME}}_iter_hasPrev ({{NAME}}_iterator_t* self)
{
    if ({{NAME}}_isEmpty(self->owner))
    {
        return false;
    }
    else if (NULL == self->node)
    {
        return true;
    }
    else
    {
        {{KEY_TYPE}} key = self->node->key;
        return NULL != {{NAME}}_lowerNode(self->owner, key);
    }
}

/**
 * @brief Advances the iterator to the next element.
 * @param self Pointer to the tree iterator.
 */
void {{NAME}}_iter_next ({{NAME}}_iterator_t* self)
{
    if ({{NAME}}_isEmpty(self->owner) == false)
    {
        if (NULL == self->node)
        {
            self->node = {{NAME}}_firstNode(self->owner);
        }
        else
        {
            {{KEY_TYPE}} key = self->node->key;
            self->node = {{NAME}}_higherNode(self->owner, key);

            // Iterators can be circular.
            if (NULL == self->node)
            {
                self->node = {{NAME}}_firstNode(self->owner);
            }
        }
    }
}

/**
 * @brief Moves the iterator to the previous element.
 * @param self Pointer to the tree iterator.
 */
void {{NAME}}_iter_prev ({{NAME}}_iterator_t* self)
{
    if ({{NAME}}_isEmpty(self->owner) == false)
    {
        if (NULL == self->node)
        {
            self->node = {{NAME}}_lastNode(self->owner);
        }
        else
        {
            {{KEY_TYPE}} key = self->node->key;
            self->node = {{NAME}}_lowerNode(self->owner, key);

            // Iterators can be circular.
            if (NULL == self->node)
            {
                self->node = {{NAME}}_lastNode(self->owner);
            }
        }
    }
}

/**
 * @brief Retrieves the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Pointer to the current node.
 */
{{NAME}}_node_t* {{NAME}}_iter_node ({{NAME}}_iterator_t* self)
{
    return self->node;
}

/**
 * @brief Retrieves the key of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Key of the current node.
 */
{{KEY_TYPE}} {{NAME}}_iter_key ({{NAME}}_iterator_t* self)
{
    if (NULL == self->node)
    {
        return {{NAME}}_defaultKey();
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
void {{NAME}}_iter_set ({{NAME}}_iterator_t* self, {{VALUE_TYPE}} value)
{
    self->node->value = value;
}

/**
 * @brief Retrieves the data value of the current node from the iterator.
 * @param self Pointer to the tree iterator.
 * @return Data value of the current node.
 */
{{VALUE_TYPE}} {{NAME}}_iter_get ({{NAME}}_iterator_t* self)
{
    if (NULL == self->node)
    {
        return {{NAME}}_defaultValue();
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
{{KEY_TYPE}} {{NAME}}_node_key ({{NAME}}_node_t* self)
{
    if (NULL == self)
    {
        return {{NAME}}_defaultKey();
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
void {{NAME}}_node_set ({{NAME}}_node_t* self, {{VALUE_TYPE}} value)
{
    self->value = value;
}

/**
 * @brief Retrieves the data value of a given tree node.
 * @param self Pointer to the tree node.
 * @return Data value of the node.
 */
{{VALUE_TYPE}} {{NAME}}_node_get ({{NAME}}_node_t* self)
{
    if (NULL == self)
    {
        return {{NAME}}_defaultValue();
    }
    else
    {
        return self->value;
    }
}

{{COPYRIGHT_FOOTER}}
'''

def generate_tree_map (args):
    source = pathlib.Path(args.source[0])
    source = source.resolve()
    header = pathlib.Path(source.parent, source.stem + ".h")

    kwargs = dict()
    kwargs["COMPARATOR"] = args.comparator[0]
    kwargs["DEFAULT_KEY"] = args.default_key[0]
    kwargs["DEFAULT_VALUE"] = args.default_value[0]
    kwargs["DEQUE"] = args.deque
    kwargs["HEADER"] = header.name
    kwargs["INCLUDE_PATHS"] = args.include
    kwargs["KEY_TYPE"] = args.key_type[0]
    kwargs["NAME"] = args.name[0]
    kwargs["STRNCMP"] = args.strncmp[0]
    kwargs["VALUE_TYPE"] = args.value_type[0]
    kwargs["WIPE"] = args.wipe
    kwargs["COPYRIGHT_HEADER"] = ""
    kwargs["COPYRIGHT_FOOTER"] = ""

    if args.copyright_header:
        with open(args.copyright_header[0]) as fd:
            kwargs["COPYRIGHT_HEADER"] = fd.read()

    if args.copyright_footer:
        with open(args.copyright_footer[0]) as fd:
            kwargs["COPYRIGHT_FOOTER"] = fd.read()

    # Generate Header File
    template = tornado.template.Template(TREE_TEMPLATE_H);
    rendered = template.generate(**kwargs)
    rendered = rendered.decode("utf-8")
    rendered = rendered.strip()
    with open(header, 'w') as hdr_file:
        hdr_file.write(rendered)

    # Generate Source File
    template = tornado.template.Template(TREE_TEMPLATE_C);
    rendered = template.generate(**kwargs)
    rendered = rendered.decode("utf-8")
    rendered = rendered.strip()
    with open(source, 'w') as src_file:
        src_file.write(rendered)

def main():
    parser = argparse.ArgumentParser(description='Generate C data structures from templates.')

    kwargs = { }
    kwargs["prog"]        = "treemap_c"
    kwargs["usage"]       = None
    kwargs["description"] = "Generate AVL treemap implementation in C."
    kwargs["epilog"]      = None
    parser = argparse.ArgumentParser(**kwargs)

    name_or_flags      = ["--source", "--src", "-s"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = True
    kwargs["help"]     = "path to the source file to generate"
    kwargs["metavar"]  = "<file>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--name", "-n"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = True
    kwargs["help"]     = "name prefix for the tree map"
    kwargs["metavar"]  = "<name>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--key-type", "-k"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = True
    kwargs["help"]     = "datatype of the keys"
    kwargs["metavar"]  = "<typename>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--value-type", "-d"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = True
    kwargs["help"]     = "datatype of the values"
    kwargs["metavar"]  = "<typename>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--include", "-i"]
    kwargs = { }
    kwargs["action"]   = "append"
    kwargs["nargs"]    = 1
    kwargs["default"]  = []
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "path to include into the header file"
    kwargs["metavar"]  = "<path>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--deque"]
    kwargs = { }
    kwargs["action"]   = "store_true"
    kwargs["default"]  = False
    kwargs["required"] = False
    kwargs["help"]     = "generate the deque related functions"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--strncmp"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = [""]
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "use strncmp for the natural ordering"
    kwargs["metavar"]  = "<sizeof>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--comparator"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = [""]
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "use custom comparator code for the natural ordering"
    kwargs["metavar"]  = "<code>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--default-key"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = [""]
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "use custom code for generating the default key"
    kwargs["metavar"]  = "<code>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--default-value"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = [""]
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "use custom code for generating the default value"
    kwargs["metavar"]  = "<code>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--wipe"]
    kwargs = { }
    kwargs["action"]   = "store_true"
    kwargs["default"]  = False
    kwargs["required"] = False
    kwargs["help"]     = "use memset to wipe nodes on deallocation"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--copyright-header"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "path to the file containing the copyright header"
    kwargs["metavar"]  = "<file>"
    parser.add_argument(*name_or_flags, **kwargs)

    name_or_flags      = ["--copyright-footer"]
    kwargs = { }
    kwargs["action"]   = "store"
    kwargs["nargs"]    = 1
    kwargs["default"]  = None
    kwargs["type"]     = str
    kwargs["required"] = False
    kwargs["help"]     = "path to the file containing the copyright footer"
    kwargs["metavar"]  = "<file>"
    parser.add_argument(*name_or_flags, **kwargs)

    args = sys.argv[1:]
    args = parser.parse_args(args)

    generate_tree_map(args)

if __name__ == '__main__':
    main()

# =============================================================================================== #
# Copyright (c) 2024 Mackenzie High. All rights reserved.
# =============================================================================================== #

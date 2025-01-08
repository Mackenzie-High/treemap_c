#ifdef RUN_UNIT_TESTS
#include "../src/tree.h"
#include "../src/unit_test.h"

static void tree_print (tree_node_t* node, int indent)
{
    if (NULL != node)
    {
        for (int i = 0; i < indent; i++)
        {
            printf(" ");
        }

        printf("%d => %d\n", node->key, node->value);

        tree_print(node->left, indent + 4);
        tree_print(node->right, indent + 4);
    }
}

static size_t compute_tree_size (tree_node_t* node)
{
    if (NULL == node)
    {
        return 0;
    }
    else
    {
        const size_t size = 1 + compute_tree_size(node->left) + compute_tree_size(node->right);
        assertEqual(size, node->size, "%zu != %zu", size, node->size);
        return size;
    }
}

static int32_t max (int32_t x, int32_t y)
{
    return x > y ? x : y;
}

static void check_tree_node (tree_t* self, tree_node_t* node)
{
    if (NULL == node)
    {
        return;
    }

    check_tree_node(self, node->left);
    check_tree_node(self, node->right);

    assertTrue(node->height >= 0);
    assertImplies((NULL == node->left) && (NULL == node->right), node->height == 0, "key = %d, height = %d", node->key, node->height);
    assertImplies((NULL == node->left) && (NULL != node->right), node->height == 1 + node->right->height);
    assertImplies((NULL != node->left) && (NULL == node->right), node->height == 1 + node->left->height);
    assertImplies((NULL != node->left) && (NULL != node->right), node->height == 1 + max(node->left->height, node->right->height));

    const int32_t balance = (NULL == node->left ? 0 : node->left->height) - (NULL == node->right ? 0 : node->right->height);
    assertTrue(-1 <= balance && balance <= +1, "balance = %d, key = %d", balance, node->key);

    assertImplies(NULL != node->left, node->key > node->left->key);
    assertImplies(NULL != node->right, node->key < node->right->key);
}

static void check_tree (tree_t* self, size_t expected_size)
{
    if (NULL == self->root)
    {
        assertEqual(0, tree_size(self));
        assertTrue(tree_isEmpty(self))
        return;
    }

    assertFalse(tree_isEmpty(self))
    assertEqual(self->size, tree_size(self));
    assertEqual(self->size, compute_tree_size(self->root));
    assertEqual(expected_size, self->size, "%zu != %zu", expected_size, self->size);

    check_tree_node(self, self->root);
}

static void test_1 ()
{
    tree_iterator_t iter;
    size_t count = 0;

    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100)); check_tree(p, 1);
        assertTrue(tree_put(p, 202, 210)); check_tree(p, 2);
        assertTrue(tree_put(p, 303, 320)); check_tree(p, 3);
        assertTrue(tree_put(p, 404, 430)); check_tree(p, 4);
        assertTrue(tree_put(p, 505, 540)); check_tree(p, 5);
        assertTrue(tree_put(p, 606, 650)); check_tree(p, 6);
        assertTrue(tree_put(p, 707, 760)); check_tree(p, 7);
        assertTrue(tree_put(p, 808, 870)); check_tree(p, 8);
        assertTrue(tree_put(p, 909, 980)); check_tree(p, 9);

        assertNull(tree_getNode(p, 0));
        assertEqual(100, tree_get(p, 101)); check_tree(p, 9);
        assertEqual(210, tree_get(p, 202)); check_tree(p, 9);
        assertEqual(320, tree_get(p, 303)); check_tree(p, 9);
        assertEqual(430, tree_get(p, 404)); check_tree(p, 9);
        assertEqual(540, tree_get(p, 505)); check_tree(p, 9);
        assertEqual(650, tree_get(p, 606)); check_tree(p, 9);
        assertEqual(760, tree_get(p, 707)); check_tree(p, 9);
        assertEqual(870, tree_get(p, 808)); check_tree(p, 9);
        assertEqual(980, tree_get(p, 909)); check_tree(p, 9);

        assertEqual(101, tree_firstNode(p)->key);
        assertEqual(100, tree_firstNode(p)->value);

        assertEqual(909, tree_lastNode(p)->key);
        assertEqual(980, tree_lastNode(p)->value);

        // Ascending Iterator
        count = 0;
        iter = tree_iter(p);
        {
            for (int32_t i = 101; tree_iter_hasNext(&iter); i = i + 101)
            {
                ++count;
                tree_iter_next(&iter);
                key_t key = tree_iter_key(&iter);
                data_t value = tree_iter_get(&iter);
                assertEqual(i, key);
                assertEqual(value, tree_get(p, i))
                assertTrue(tree_containsKey(p, key));
                assertFalse(tree_containsKey(p, key + 1));
                check_tree(p, 9);
            }
        }
        tree_iter_free(&iter);
        assertEqual(9, count);
        check_tree(p, 9);

        // Descending Iterator
        count = 0;
        iter = tree_iter(p);
        {
            for (int32_t i = 909; tree_iter_hasPrev(&iter); i = i - 101)
            {
                ++count;
                tree_iter_prev(&iter);
                key_t key = tree_iter_key(&iter);
                data_t value = tree_iter_get(&iter);
                assertEqual(i, key);
                assertEqual(value, tree_get(p, i))
                assertTrue(tree_containsKey(p, key));
                assertFalse(tree_containsKey(p, key + 1));
                check_tree(p, 9);
            }
        }
        tree_iter_free(&iter);
        assertEqual(9, count);
        check_tree(p, 9);

        // Removal
        tree_remove(p, 303);                check_tree(p, 8);
        tree_remove(p, 707);                check_tree(p, 7);
        tree_remove(p, 505);                check_tree(p, 6);
        tree_remove(p, 101);                check_tree(p, 5);
        tree_remove(p, 909);                check_tree(p, 4);
        assertFalse(tree_containsKey(p, 101));      check_tree(p, 4);
        assertEqual(210, tree_get(p, 202)); check_tree(p, 4);
        assertFalse(tree_containsKey(p, 303));      check_tree(p, 4);
        assertEqual(430, tree_get(p, 404)); check_tree(p, 4);
        assertFalse(tree_containsKey(p, 505));      check_tree(p, 4);
        assertEqual(650, tree_get(p, 606)); check_tree(p, 4);
        assertFalse(tree_containsKey(p, 707));      check_tree(p, 4);
        assertEqual(870, tree_get(p, 808)); check_tree(p, 4);
        assertFalse(tree_containsKey(p, 909));      check_tree(p, 4);
        assertEqual(4, tree_size(p));
    }
    tree_free(p);
}

static void test_2 ()
{
    key_t A = 1;
    key_t B = 2;
    key_t C = 3;
    key_t D = 4;
    key_t E = 5;
    key_t F = 6;
    key_t G = 7;
    key_t H = 8;
    key_t I = 9;
    key_t J = 10;
    key_t K = 11;
    key_t L = 12;

    // See: https://stackoverflow.com/questions/3955680/how-to-check-if-my-avl-tree-implementation-is-correct
    tree_t* p = tree_new();
    {
        // Insertion Case: "abc" on the insert of "c" will require a 1L rotation
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, C, C * 17));
        check_tree(p, 3);
        tree_clear(p);

        // Insertion Case: "cba" on the insert of "a" will require a 1R rotation
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, A, A * 17));
        check_tree(p, 3);
        tree_clear(p);

        // Insertion Case: "acb" on the insert of "b" will require a 2L rotation
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, B, B * 17));
        check_tree(p, 3);
        tree_clear(p);

        // Insertion Case: "cab" on the insert of "b" will require a 2R rotation
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, B, B * 17));
        check_tree(p, 3);
        tree_clear(p);

        // Deletion Case: "bcad" on the deletion of "a" will require a 1L rotation
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, D, D * 17));
        check_tree(p, 4);
        tree_remove(p, A);
        check_tree(p, 3);
        tree_clear(p);

        // Deletion Case: "cbda" on the deletion of "d" will require a 1R rotation
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, A, A * 17));
        check_tree(p, 4);
        tree_remove(p, D);
        check_tree(p, 3);
        tree_clear(p);

        // Deletion Case: "bdac" on the deletion of "a" will require a 2L rotation
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, C, C * 17));
        check_tree(p, 4);
        tree_remove(p, A);
        check_tree(p, 3);
        tree_clear(p);

        // Deletion Case: "cadb" on the deletion of "d" will require a 2R rotation
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, B, B * 17));
        check_tree(p, 4);
        tree_remove(p, D);
        check_tree(p, 3);
        tree_clear(p);

        // Deletion Case: "cbedfag" on the delete of "a" or skipping "a" and the insert of "g" will require a 1L rotation at "c"
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, E, E * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, F, F * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, G, G * 17));
        check_tree(p, 7);
        tree_remove(p, A); // TODO
        check_tree(p, 6);
        tree_clear(p);

        // Deletion Case: "ecfbdga" on the delete of "g" or skipping "g" and the insert of "a" will require a 1R rotation at "e"
        assertTrue(tree_put(p, E, E * 17));
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, F, F * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, G, G * 17));
        assertTrue(tree_put(p, A, A * 17));
        check_tree(p, 7);
        tree_remove(p, G);
        check_tree(p, 6);
        tree_clear(p);

        // Deletion Case: "ecjadhkgilbf" on the delete of "b" or skipping "b" and the insert of "f" will require a 2L rotation at "j" then "e". The insert case can optionally skip inserting "d"
        assertTrue(tree_put(p, E, E * 17));
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, J, J * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, H, H * 17));
        assertTrue(tree_put(p, K, K * 17));
        assertTrue(tree_put(p, G, G * 17));
        assertTrue(tree_put(p, I, I * 17));
        assertTrue(tree_put(p, L, L * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, F, F * 17));
        check_tree(p, 12);
        tree_remove(p, B);
        check_tree(p, 11);
        tree_clear(p);

        // Deletion Case: "hckbeiladfjg" on the delete of "j" or skipping "j" and the insert of "g" will require a 2R rotation at "c" then "b". The insert case can optionally skip inserting "l"
        assertTrue(tree_put(p, H, H * 17));
        assertTrue(tree_put(p, C, C * 17));
        assertTrue(tree_put(p, K, K * 17));
        assertTrue(tree_put(p, B, B * 17));
        assertTrue(tree_put(p, E, E * 17));
        assertTrue(tree_put(p, I, I * 17));
        assertTrue(tree_put(p, L, L * 17));
        assertTrue(tree_put(p, A, A * 17));
        assertTrue(tree_put(p, D, D * 17));
        assertTrue(tree_put(p, F, F * 17));
        assertTrue(tree_put(p, J, J * 17));
        assertTrue(tree_put(p, G, G * 17));
        check_tree(p, 12);
        tree_remove(p, J);
        check_tree(p, 11);
        tree_clear(p);
    }
    tree_free(p);
}

static void test_addFirst ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_addFirst(p, 101)); check_tree(p, 1);
        assertTrue(tree_addFirst(p, 202)); check_tree(p, 2);
        assertTrue(tree_addFirst(p, 303)); check_tree(p, 3);
        assertTrue(tree_addFirst(p, 404)); check_tree(p, 4);
        assertTrue(tree_addFirst(p, 505)); check_tree(p, 5);

        assertEqual(505, tree_get(p, -4));
        assertEqual(404, tree_get(p, -3));
        assertEqual(303, tree_get(p, -2));
        assertEqual(202, tree_get(p, -1));
        assertEqual(101, tree_get(p,  0));
    }
    tree_free(p);
}

static void test_addLast ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_addLast(p, 101)); check_tree(p, 1);
        assertTrue(tree_addLast(p, 202)); check_tree(p, 2);
        assertTrue(tree_addLast(p, 303)); check_tree(p, 3);
        assertTrue(tree_addLast(p, 404)); check_tree(p, 4);
        assertTrue(tree_addLast(p, 505)); check_tree(p, 5);

        assertEqual(101, tree_get(p, 0));
        assertEqual(202, tree_get(p, 1));
        assertEqual(303, tree_get(p, 2));
        assertEqual(404, tree_get(p, 3));
        assertEqual(505, tree_get(p, 4));
    }
    tree_free(p);
}

static bool match_predicate (tree_t* tree, tree_node_t* node, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    check_tree(tree, tree_size(tree));
    return node->value % 7 == 0;
}

static void test_anyMatch ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;

        assertFalse(tree_anyMatch(p, &match_predicate, &context));

        tree_put(p, 101, 20); // 20 % 7 != 0
        assertFalse(tree_anyMatch(p, &match_predicate, &context));
        tree_put(p, 202, 22); // 22 % 7 != 0
        assertFalse(tree_anyMatch(p, &match_predicate, &context));
        tree_put(p, 303, 21); // 21 % 7 == 0
        assertTrue(tree_anyMatch(p, &match_predicate, &context));
    }
    tree_free(p);
}

static void test_allMatch ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;

        assertTrue(tree_allMatch(p, &match_predicate, &context));

        tree_put(p, 101, 14); // 14 % 7 == 0
        assertTrue(tree_allMatch(p, &match_predicate, &context));
        tree_put(p, 202, 21); // 21 % 7 == 0
        assertTrue(tree_allMatch(p, &match_predicate, &context));
        tree_put(p, 303, 22); // 21 % 7 != 0
        assertFalse(tree_allMatch(p, &match_predicate, &context));
    }
    tree_free(p);
}

static void test_clear ()
{
    tree_t* p = tree_new();
    {
        for (size_t i = 0; i < 10; i++)
        {
            for (size_t k = 0; k < i; k++)
            {
                assertTrue(tree_put(p, k, k * 1000));
                check_tree(p, k);
            }

            tree_clear(p);
            check_tree(p, 0);
        }
    }
    tree_free(p);
}

static void test_copy ()
{
    for (size_t size = 0; size < 50; size++)
    {
        tree_t* p = tree_new();
        {
            // Fill the original tree.
            for (size_t i = 0; i < size; i++)
            {
                assertTrue(tree_put(p, i * 100 + 13, i * 1000 + 17));
            }

            check_tree(p, size);

            // Create a copy of the original tree.
            tree_t* q = tree_copy(p);
            {
                assertEqual(size, tree_size(q));
                check_tree(q, size);

                // Verify that the two trees are equal.
                tree_iterator_t iterP = tree_iter(p);
                tree_iterator_t iterQ = tree_iter(q);
                {
                    while (tree_iter_hasNext(&iterP))
                    {
                        assertTrue(tree_iter_hasNext(&iterP));
                        assertTrue(tree_iter_hasNext(&iterQ));

                        tree_iter_next(&iterP);
                        tree_iter_next(&iterQ);

                        tree_node_t* nodeP = tree_iter_node(&iterP);
                        tree_node_t* nodeQ = tree_iter_node(&iterQ);

                        assertTrue(nodeP != nodeQ);
                        assertEqual(nodeP->key, nodeQ->key);
                        assertEqual(nodeP->value, nodeQ->value);
                    }

                    assertFalse(tree_iter_hasNext(&iterP));
                    assertFalse(tree_iter_hasNext(&iterQ));
                }
                tree_iter_free(&iterP);
                tree_iter_free(&iterQ);
            }
            tree_free(q);
        }
        tree_free(p);
    }
}

static void test_copy_allocation_failure_special_case ()
{
    const size_t capacity = 5;
    tree_allocator_t* allocator = tree_allocator_slab(capacity);
    {
        tree_t* p = tree_make(allocator, tree_comparator_naturalOrder());
        {
            tree_put(p, 101, 100);
            tree_put(p, 202, 200);
            tree_put(p, 303, 300);
            assertEqual(3, tree_size(p))

            // The original tree contains three nodes.
            // Therefore, creating a copy requires three new nodes.
            // In total, the allocator would need capacity for six nodes,
            // namely the three original plus the three new copies.
            // The allocator only has a capacity of five nodes.
            // Therefore, the copy will fail part of the way through.
            // The copy tree and any nodes in the copy will be freed automatically.
            assertNull(tree_copy(p));
        }
        tree_free(p);
    }
    tree_allocator_free(allocator);
}

static void test_count ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;

        assertEquals(0, tree_count(p, &match_predicate, &context));

        tree_put(p, 101, 14); // 14 % 7 == 0
        assertEquals(1, tree_count(p, &match_predicate, &context));
        tree_put(p, 202, 15); // 15 % 7 != 0
        assertEquals(1, tree_count(p, &match_predicate, &context));

        tree_put(p, 303, 21); // 21 % 7 == 0
        assertEquals(2, tree_count(p, &match_predicate, &context));
        tree_put(p, 404, 22); // 22 % 7 != 0
        assertEquals(2, tree_count(p, &match_predicate, &context));

        tree_put(p, 505, 28); // 28 % 7 == 0
        assertEquals(3, tree_count(p, &match_predicate, &context));
        tree_put(p, 606, 29); // 29 % 7 != 0
        assertEquals(3, tree_count(p, &match_predicate, &context));

        tree_put(p, 707, 35); // 35 % 7 == 0
        assertEquals(4, tree_count(p, &match_predicate, &context));
        tree_put(p, 808, 36); // 36 % 7 != 0
        assertEquals(4, tree_count(p, &match_predicate, &context));
    }
    tree_free(p);
}

static void test_firstNode ()
{
    tree_t* p = tree_new();
    {
        // Check if the first node is NULL in an empty tree
        assertNull(tree_firstNode(p));

        // Populate the Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));

        // The first node should be the smallest key
        assertEqual(101, tree_firstNode(p)->key);
        assertEqual(100, tree_firstNode(p)->value);

        // Add more elements
        assertTrue(tree_put(p, 303, 320));

        // First node still remains the smallest key
        assertEqual(101, tree_firstNode(p)->key);
        assertEqual(100, tree_firstNode(p)->value);

        tree_remove(p, 101); // Remove the smallest element

        // Now the first node should be the next smallest
        assertEqual(202, tree_firstNode(p)->key);
        assertEqual(210, tree_firstNode(p)->value);
    }
    tree_free(p);
}

static void forEach_functor (tree_t* tree, tree_node_t* node, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    int32_t* output = (int32_t*) context;
    *output = *output * node->value;
}

static void test_forEach ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 0;

        context = 13;
        assertEqual(0, tree_size(p));
        tree_forEach(p, forEach_functor, &context);
        assertEqual(13, context);

        tree_put(p, 101, 23);
        context = 13;
        assertEqual(1, tree_size(p));
        tree_forEach(p, forEach_functor, &context);
        assertEqual(13 * 23, context);

        tree_put(p, 202, 27);
        context = 13;
        assertEqual(2, tree_size(p));
        tree_forEach(p, forEach_functor, &context);
        assertEqual(13 * 23 * 27, context);

        tree_put(p, 303, 31);
        context = 13;
        assertEqual(3, tree_size(p));
        tree_forEach(p, forEach_functor, &context);
        assertEqual(13 * 23 * 27 * 31, context);
    }
    tree_free(p);
}

static void test_free ()
{
    tree_t* p = NULL;

    // Case: null argument is harmless
    tree_free(NULL);

    // Case: normal free
    p = tree_new();
    assertNotNull(p);
    tree_free(p);

    // Case: allocator is NOT freed
    tree_allocator_t* allocator = tree_allocator_slab(5);
    {
        p = tree_make(allocator, tree_comparator_naturalOrder());
        assertNotNull(p);
        tree_free(p);

        // This should cause valgrind to detect an invalid read/write,
        // if the tree_free() freed the allocator.
        allocator->allocate = NULL;
    }
    tree_allocator_free(allocator);
}

static void test_free_stackalloc ()
{
    tree_t p;

    // Case: null argument is harmless
    tree_free_stackalloc(NULL);

    // Case: allocator is NOT freed
    tree_allocator_t* allocator = tree_allocator_slab(5);
    {
        p = tree_make_stackalloc(allocator, tree_comparator_naturalOrder());
        tree_free_stackalloc(&p);

        assertNull(p.allocator);
        assertNull(p.comparator);
        assertNull(p.root);
        assertEqual(0, p.size);

        // This should cause valgrind to detect an invalid read/write,
        // if the tree_free_stackalloc() freed the allocator.
        allocator->allocate = NULL;
    }
    tree_allocator_free(allocator);
}

static void test_get ()
{
    tree_t* p = tree_new();
    {
        // Populate the Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));

        // Test the get function
        assertEqual(100, tree_get(p, 101)); // Expecting value 100
        assertEqual(210, tree_get(p, 202)); // Expecting value 210
        assertEqual(0, tree_get(p, 303)); // Not found key, expecting default value
    }
    tree_free(p);
}

static void test_getNode ()
{
    tree_t* p = tree_new();
    {
        // Populate the Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));

        // Test the getNode function
        assertNotNull(tree_getNode(p, 101)); // Should return the node with key 101
        assertEqual(100, tree_getNode(p, 101)->value); // Check value

        assertNotNull(tree_getNode(p, 202)); // Should return the node with key 202
        assertEqual(210, tree_getNode(p, 202)->value); // Check value

        assertNull(tree_getNode(p, 303)); // Not found key, should return NULL
    }
    tree_free(p);
}

static void test_has ()
{
    tree_t* p = tree_new();
    {
        assertFalse(tree_containsKey(p, 101)); // Should return false

        // Populate the Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));

        assertTrue(tree_containsKey(p, 101)); // Now it should return true
        assertTrue(tree_containsKey(p, 202)); // Also true
        assertFalse(tree_containsKey(p, 303)); // False for a non-existing key
    }
    tree_free(p);
}

static void test_higherNode ()
{
    tree_t* p = tree_new();
    {
        // Populate the Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));
        assertTrue(tree_put(p, 303, 320));

        // The node with the lowest key that is greater than the given key
        assertEqual(101, tree_higherNode(p, 0)->key);
        assertEqual(101, tree_higherNode(p, 100)->key);
        assertEqual(202, tree_higherNode(p, 101)->key);
        assertEqual(303, tree_higherNode(p, 202)->key);
        assertNull(tree_higherNode(p, 303)); // No higher node should return NULL
    }
    tree_free(p);
}

static void test_isEmpty ()
{
    tree_t* p = tree_new();
    {
        // Should be empty initially
        assertTrue(tree_isEmpty(p));

        // Add an element
        // Now it should not be empty
        assertTrue(tree_put(p, 101, 100));
        assertFalse(tree_isEmpty(p));

        // Remove the element
        // Should be empty again
        tree_remove(p, 101);
        check_tree(p, 0);
        assertTrue(tree_isEmpty(p));
    }
    tree_free(p);
}

static bool isEqual_predicate (tree_node_t* nodeX, tree_node_t* nodeY, void* context)
{
    assertNotNull(nodeX);
    assertNotNull(nodeY);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    return (nodeX->key == nodeY->key) && (nodeY->value == 10 * nodeX->value);
}

static void test_isEqual ()
{
    tree_t* p = tree_new();
    tree_t* q = tree_new();
    {
        int32_t context = 42;

        // Case: both trees are empty
        assertTrue(tree_isEqual(p, q, isEqual_predicate, &context));

        // Case: one tree is larger than the other
        tree_put(p, 101, 10);
        tree_put(p, 202, 20);
        tree_put(p, 303, 30);
        tree_put(q, 101, 100);
        tree_put(q, 202, 200);
        assertFalse(tree_isEqual(p, q, isEqual_predicate, &context));
        assertFalse(tree_isEqual(q, p, isEqual_predicate, &context));
        tree_clear(p);
        tree_clear(q);

        // Case: true
        tree_put(p, 101, 10);
        tree_put(p, 202, 20);
        tree_put(p, 303, 30);
        tree_put(q, 101, 100);
        tree_put(q, 202, 200);
        tree_put(q, 303, 300);
        assertTrue(tree_isEqual(p, q, isEqual_predicate, &context));
        tree_clear(p);
        tree_clear(q);

        // Case: false, due to key mismatch
        tree_put(p, 101, 10);
        tree_put(p, 202, 20);
        tree_put(p, 303, 30);
        tree_put(q, 101, 100);
        tree_put(q, 222, 200); // different
        tree_put(q, 303, 300);
        assertFalse(tree_isEqual(p, q, isEqual_predicate, &context));
        tree_clear(p);
        tree_clear(q);

        // Case: false, due to value mismatch
        tree_put(p, 101, 10);
        tree_put(p, 202, 20);
        tree_put(p, 303, 30);
        tree_put(q, 101, 100);
        tree_put(q, 202, 999); // different
        tree_put(q, 303, 300);
        assertFalse(tree_isEqual(p, q, isEqual_predicate, &context));
        tree_clear(p);
        tree_clear(q);
    }
    tree_free(p);
    tree_free(q);
}

static void test_iter ()
{
    tree_t* p = tree_new();
    {
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);

        tree_iterator_t iter = tree_iter(p);
        {
            tree_iter_next(&iter);
            assertEqual(tree_iter_key(&iter), 101);
            assertEqual(tree_iter_get(&iter), 100);

            tree_iter_next(&iter);
            assertEqual(tree_iter_key(&iter), 202);
            assertEqual(tree_iter_get(&iter), 200);

            tree_iter_next(&iter);
            assertEqual(tree_iter_key(&iter), 303);
            assertEqual(tree_iter_get(&iter), 300);
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_at ()
{
    tree_t* p = tree_new();
    {
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);

        tree_iterator_t iter = tree_iter_at(p, 202);
        {
            assertNotNull(tree_iter_node(&iter));
            assertEqual(tree_iter_key(&iter), 202);
            assertEqual(tree_iter_get(&iter), 200);

            tree_iter_prev(&iter);
            assertEqual(tree_iter_key(&iter), 101);
            assertEqual(tree_iter_get(&iter), 100);

            tree_iter_next(&iter);
            tree_iter_next(&iter);
            assertEqual(tree_iter_key(&iter), 303);
            assertEqual(tree_iter_get(&iter), 300);
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_atNode ()
{
    tree_t* p = tree_new();
    {
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);

        tree_node_t* nodeX = tree_getNode(p, 101);
        tree_node_t* nodeY = tree_getNode(p, 202);
        tree_node_t* nodeZ = tree_getNode(p, 303);
        tree_iterator_t iter = tree_iter_atNode(p, nodeY);
        {
            assertNotNull(tree_iter_node(&iter));
            assertEqual(tree_iter_key(&iter), 202);
            assertEqual(tree_iter_get(&iter), 200);

            tree_iter_prev(&iter);
            assertEqual(tree_iter_key(&iter), 101);
            assertEqual(tree_iter_get(&iter), 100);

            tree_iter_next(&iter);
            tree_iter_next(&iter);
            assertEqual(tree_iter_key(&iter), 303);
            assertEqual(tree_iter_get(&iter), 300);
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_free ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));

        tree_iterator_t iter = tree_iter(p);
        {
            tree_iter_next(&iter);
            assertNotNull(iter.owner);
            assertNotNull(iter.node);
        }
        tree_iter_free(&iter);

        assertNull(iter.owner);
        assertNull(iter.node);
    }
    tree_free(p);
}

static void test_iter_get ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));

        tree_iterator_t iter = tree_iter(p);
        {
            // Default Value
            assertEqual(tree_defaultValue(), tree_iter_get(&iter));

            // Fetch Value
            tree_iter_next(&iter);
            assertEqual(100, tree_iter_get(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_hasNext ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));

        tree_iterator_t iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasNext(&iter)); // Initial State

            // First Pass
            tree_iter_next(&iter); // First element
            assertEqual(101, tree_iter_key(&iter));
            assertEqual(100, tree_iter_get(&iter));
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); // Second element
            assertEqual(202, tree_iter_key(&iter));
            assertEqual(200, tree_iter_get(&iter));
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); // Third element
            assertEqual(303, tree_iter_key(&iter));
            assertEqual(300, tree_iter_get(&iter));
            assertFalse(tree_iter_hasNext(&iter));

            // Second Pass (Circular Iterator)
            tree_iter_next(&iter); // First element
            assertEqual(101, tree_iter_key(&iter));
            assertEqual(100, tree_iter_get(&iter));
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); // Second element
            assertEqual(202, tree_iter_key(&iter));
            assertEqual(200, tree_iter_get(&iter));
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); // Third element
            assertEqual(303, tree_iter_key(&iter));
            assertEqual(300, tree_iter_get(&iter));
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_hasPrev ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));

        tree_iterator_t iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasPrev(&iter)); // Initial State

            // First Pass
            tree_iter_prev(&iter); // First element
            assertEqual(303, tree_iter_key(&iter));
            assertEqual(300, tree_iter_get(&iter));
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); // Second element
            assertEqual(202, tree_iter_key(&iter));
            assertEqual(200, tree_iter_get(&iter));
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); // Third element
            assertEqual(101, tree_iter_key(&iter));
            assertEqual(100, tree_iter_get(&iter));
            assertFalse(tree_iter_hasPrev(&iter));

            // Second Pass (Circular Iterator)
            tree_iter_prev(&iter); // First element
            assertEqual(303, tree_iter_key(&iter));
            assertEqual(300, tree_iter_get(&iter));
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); // Second element
            assertEqual(202, tree_iter_key(&iter));
            assertEqual(200, tree_iter_get(&iter));
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); // Third element
            assertEqual(101, tree_iter_key(&iter));
            assertEqual(100, tree_iter_get(&iter));
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_key ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));

        tree_iterator_t iter = tree_iter(p);
        {
            // Default value, if called before next() or prev().
            assertEqual(tree_defaultKey(), tree_iter_key(&iter))

            tree_iter_next(&iter);
            assertEqual(101, tree_iter_key(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_next ()
{
    tree_t* p = tree_new();
    {
        tree_iterator_t iter;

        // Case: Empty Tree, simply do not SIGSEGV
        iter = tree_iter(p);
        {
            assertFalse(tree_iter_hasNext(&iter));
            tree_iter_next(&iter);
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);

        // Case: Tree of Size 1
        assertTrue(tree_put(p, 101, 100));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);


        // Case: Tree of Size 2
        assertTrue(tree_put(p, 202, 200));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(101, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(202, iter.node->key);
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);

        // Case: Tree of Size 3
        assertTrue(tree_put(p, 303, 300));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(101, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(303, iter.node->key);
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);

        // Case: Iterators are circular, if you ignore hasNext().
        // Case: Tree of Size 3
        iter = tree_iter(p);
        {
            // First Pass
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(101, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(303, iter.node->key);
            assertFalse(tree_iter_hasNext(&iter));

            // Second Pass
            tree_iter_next(&iter); assertEqual(101, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasNext(&iter));
            tree_iter_next(&iter); assertEqual(303, iter.node->key);
            assertFalse(tree_iter_hasNext(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_iter_node ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));

        tree_iterator_t iter = tree_iter(p);
        {
            assertNull(tree_iter_node(&iter)); // Null, if called before next() or prev()

            tree_iter_next(&iter);

            assertNotNull(tree_iter_node(&iter)); // There should be a node
            assertEqual(100, tree_node_get(tree_iter_node(&iter))); // Should get the correct value
        }
        tree_iter_free(&iter);
    }
    tree_free(p);

}

static void test_iter_prev ()
{
    tree_t* p = tree_new();
    {
        tree_iterator_t iter;

        // Case: Empty Tree, simply do not SIGSEGV
        iter = tree_iter(p);
        {
            assertFalse(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter);
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);

        // Case: Tree of Size 1
        assertTrue(tree_put(p, 101, 100));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);


        // Case: Tree of Size 2
        assertTrue(tree_put(p, 202, 200));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);

        // Case: Tree of Size 3
        assertTrue(tree_put(p, 303, 300));
        iter = tree_iter(p);
        {
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(303, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);

        // Case: Iterators are circular, if you ignore hasPrev().
        // Case: Tree of Size 3
        iter = tree_iter(p);
        {
            // First Pass
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(303, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasPrev(&iter));

            // Second Pass
            tree_iter_prev(&iter); assertEqual(303, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(202, iter.node->key);
            assertTrue(tree_iter_hasPrev(&iter));
            tree_iter_prev(&iter); assertEqual(101, iter.node->key);
            assertFalse(tree_iter_hasPrev(&iter));
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_removeFirst ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        tree_removeFirst(p);
        check_tree(p, 0);

        // Case: Non-Empty Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));
        assertTrue(tree_put(p, 404, 400));
        assertTrue(tree_put(p, 505, 500));
        check_tree(p, 5);
        assertEqual(101, tree_firstNode(p)->key); tree_removeFirst(p); check_tree(p, 4);
        assertEqual(202, tree_firstNode(p)->key); tree_removeFirst(p); check_tree(p, 3);
        assertEqual(303, tree_firstNode(p)->key); tree_removeFirst(p); check_tree(p, 2);
        assertEqual(404, tree_firstNode(p)->key); tree_removeFirst(p); check_tree(p, 1);
        assertEqual(505, tree_firstNode(p)->key); tree_removeFirst(p); check_tree(p, 0);
        assertTrue(tree_isEmpty(p));

        // Case: Empty Tree, Ex Post Facto
        tree_removeFirst(p);
        check_tree(p, 0);
    }
    tree_free(p);
}

static void test_removeIf ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;

        assertEquals(0, tree_count(p, &match_predicate, &context));

        tree_put(p, 101, 14); // 14 % 7 == 0
        tree_put(p, 202, 15); // 15 % 7 != 0
        tree_put(p, 303, 21); // 21 % 7 == 0
        tree_put(p, 404, 22); // 22 % 7 != 0
        tree_put(p, 505, 28); // 28 % 7 == 0
        tree_put(p, 606, 29); // 29 % 7 != 0
        tree_put(p, 707, 35); // 35 % 7 == 0
        tree_put(p, 808, 36); // 36 % 7 != 0
        tree_put(p, 909, 49); // 49 % 7 == 0

        // Remove the values that are evenly divisible by seven.
        tree_removeIf(p, &match_predicate, &context);
        assertEqual(4, tree_size(p), "size = %zu", tree_size(p));
        check_tree(p, 4);
        assertEqual(15, tree_get(p, 202));
        assertEqual(22, tree_get(p, 404));
        assertEqual(29, tree_get(p, 606));
        assertEqual(36, tree_get(p, 808));
    }
    tree_free(p);
}

static void test_removeLast ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        tree_removeLast(p);
        check_tree(p, 0);

        // Case: Non-Empty Tree
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));
        assertTrue(tree_put(p, 404, 400));
        assertTrue(tree_put(p, 505, 500));
        check_tree(p, 5);
        assertEqual(505, tree_lastNode(p)->key); tree_removeLast(p); check_tree(p, 4);
        assertEqual(404, tree_lastNode(p)->key); tree_removeLast(p); check_tree(p, 3);
        assertEqual(303, tree_lastNode(p)->key); tree_removeLast(p); check_tree(p, 2);
        assertEqual(202, tree_lastNode(p)->key); tree_removeLast(p); check_tree(p, 1);
        assertEqual(101, tree_lastNode(p)->key); tree_removeLast(p); check_tree(p, 0);
        assertTrue(tree_isEmpty(p));

        // Case: Empty Tree, Ex Post Facto
        tree_removeLast(p);
        check_tree(p, 0);
    }
    tree_free(p);
}

static void test_iter_set ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));

        tree_iterator_t iter = tree_iter(p);
        {
            tree_iter_next(&iter);
            tree_iter_set(&iter, 400);
            tree_iter_next(&iter);
            tree_iter_set(&iter, 500);
            tree_iter_next(&iter);
            tree_iter_set(&iter, 600);

            assertEqual(tree_get(p, 101), 400);
            assertEqual(tree_get(p, 202), 500);
            assertEqual(tree_get(p, 303), 600);
        }
        tree_iter_free(&iter);
    }
    tree_free(p);
}

static void test_lastNode ()
{
    tree_t* p = tree_new();
    {
        assertNull(tree_lastNode(p));

        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));
        assertTrue(tree_put(p, 303, 320));

        assertEqual(303, tree_lastNode(p)->key);
        assertEqual(320, tree_lastNode(p)->value);
    }
    tree_free(p);

}

static void test_lowerNode ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 210));
        assertTrue(tree_put(p, 303, 320));

        assertEqual(303, tree_lowerNode(p, 304)->key);
        assertEqual(202, tree_lowerNode(p, 303)->key);
        assertEqual(101, tree_lowerNode(p, 202)->key);
        assertNull(tree_lowerNode(p, 101)); // No lower node should return NULL
    }
    tree_free(p);
}

static void test_make ()
{
    tree_allocator_t* allocator = tree_allocator_dynamic();
    tree_t* p = NULL;

    // Case: set allocator
    allocator = tree_allocator_dynamic();
    p = tree_make(allocator, tree_comparator_naturalOrder());
    assertNotNull(p);
    assertEqual(allocator, p->allocator);
    tree_free(p);
    tree_allocator_free(allocator);
    allocator = tree_allocator_slab(5);
    p = tree_make(allocator, tree_comparator_naturalOrder());
    assertNotNull(p);
    assertEqual(allocator, p->allocator);
    tree_free(p);
    tree_allocator_free(allocator);

    // Case: set comparator
    allocator = tree_allocator_dynamic();
    p = tree_make(allocator, tree_comparator_naturalOrder());
    assertNotNull(p);
    assertEqual(tree_comparator_naturalOrder(), p->comparator);
    tree_free(p);
    p = tree_make(allocator, tree_comparator_reverseOrder());
    assertNotNull(p);
    assertEqual(tree_comparator_reverseOrder(), p->comparator);
    tree_free(p);
    tree_allocator_free(allocator);

    // Case: attributes
    allocator = tree_allocator_dynamic();
    p = tree_make(allocator, tree_comparator_naturalOrder());
    assertNotNull(p);
    assertNull(p->root);
    assertEqual(0, p->size);
    tree_free(p);
    tree_allocator_free(allocator);
}

static void test_make_comparators ()
{
    tree_t* p = NULL;

    // Verify that a non-default comparator actually has an effect on the tree.
    // In this case, the comparator must reverse the order of the tree.
    p = tree_make(tree_allocator_dynamic(), tree_comparator_reverseOrder());
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));

        assertEqual(300, tree_popFirst(p));
        assertEqual(200, tree_popFirst(p));
        assertEqual(100, tree_popFirst(p));
    }
    tree_free(p);

    // Contrast with the other comparator,
    // which should be in ascending order.
    p = tree_make(tree_allocator_dynamic(), tree_comparator_naturalOrder());
    {
        assertTrue(tree_put(p, 101, 100));
        assertTrue(tree_put(p, 202, 200));
        assertTrue(tree_put(p, 303, 300));

        assertEqual(100, tree_popFirst(p));
        assertEqual(200, tree_popFirst(p));
        assertEqual(300, tree_popFirst(p));
    }
    tree_free(p);
}

static void test_make_stackalloc ()
{
    tree_allocator_t* allocator = tree_allocator_dynamic();
    tree_t p;

    // Case: set allocator
    allocator = tree_allocator_dynamic();
    p = tree_make_stackalloc(allocator, tree_comparator_naturalOrder());
    assertEqual(allocator, p.allocator);
    tree_free_stackalloc(&p);
    tree_allocator_free(allocator);
    allocator = tree_allocator_slab(5);
    p = tree_make_stackalloc(allocator, tree_comparator_naturalOrder());
    assertEqual(allocator, p.allocator);
    tree_free_stackalloc(&p);
    tree_allocator_free(allocator);

    // Case: set comparator
    allocator = tree_allocator_dynamic();
    p = tree_make_stackalloc(allocator, tree_comparator_naturalOrder());
    assertEqual(tree_comparator_naturalOrder(), p.comparator);
    tree_free_stackalloc(&p);
    p = tree_make_stackalloc(allocator, tree_comparator_reverseOrder());
    assertEqual(tree_comparator_reverseOrder(), p.comparator);
    tree_free_stackalloc(&p);
    tree_allocator_free(allocator);

    // Case: attributes
    allocator = tree_allocator_dynamic();
    p = tree_make_stackalloc(allocator, tree_comparator_naturalOrder());
    assertNull(p.root);
    assertEqual(0, p.size);
    tree_free_stackalloc(&p);
    tree_allocator_free(allocator);
}

static void test_new ()
{
    // Case: attributes
    tree_t* p = tree_new();
    assertNotNull(p);
    assertNull(p->root);
    assertEqual(0, p->size);
    assertEqual(tree_allocator_dynamic(), p->allocator);
    assertEqual(tree_comparator_naturalOrder(), p->comparator);
    tree_free(p);
}

static void test_node_get ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        tree_node_t* node = tree_getNode(p, 101);
        assertEqual(tree_node_get(node), 100);
    }
    tree_free(p);
}

static void test_node_key ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        tree_node_t* node = tree_getNode(p, 101);
        assertEqual(tree_node_key(node), 101);
    }
    tree_free(p);
}

static void test_node_set ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 100));
        tree_node_t* node = tree_getNode(p, 101);
        tree_node_set(node, 200);

        assertEqual(tree_node_get(node), 200);
    }
    tree_free(p);
}

static void test_nthNode ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertNull(tree_nthNode(p, 0));
        assertNull(tree_nthNode(p, 1));
        assertNull(tree_nthNode(p, 2));

        // Populate the Tree
        assertTrue(tree_put(p, 101, 100)); check_tree(p, 1);
        assertTrue(tree_put(p, 202, 210)); check_tree(p, 2);
        assertTrue(tree_put(p, 303, 320)); check_tree(p, 3);
        assertTrue(tree_put(p, 404, 430)); check_tree(p, 4);
        assertTrue(tree_put(p, 505, 540)); check_tree(p, 5);
        assertTrue(tree_put(p, 606, 650)); check_tree(p, 6);
        assertTrue(tree_put(p, 707, 760)); check_tree(p, 7);
        assertTrue(tree_put(p, 808, 870)); check_tree(p, 8);
        assertTrue(tree_put(p, 909, 980)); check_tree(p, 9);

        // Case: Non-Empty Tree
        assertEqual(100, tree_nthNode(p, 0)->value);
        assertEqual(210, tree_nthNode(p, 1)->value);
        assertEqual(320, tree_nthNode(p, 2)->value);
        assertEqual(430, tree_nthNode(p, 3)->value);
        assertEqual(540, tree_nthNode(p, 4)->value);
        assertEqual(650, tree_nthNode(p, 5)->value);
        assertEqual(760, tree_nthNode(p, 6)->value);
        assertEqual(870, tree_nthNode(p, 7)->value);
        assertEqual(980, tree_nthNode(p, 8)->value);

        assertNull(tree_nthNode(p, 9));
        assertNull(tree_nthNode(p, 10));
        assertNull(tree_nthNode(p, 11));
    }
    check_tree(p, 9);
    tree_free(p);
}

static void test_push ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_push(p, 101));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 1);

        assertTrue(tree_push(p, 202));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(1, tree_lastNode(p)->key);
        assertEqual(202, tree_lastNode(p)->value);
        check_tree(p, 2);

        assertTrue(tree_push(p, 303));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(2, tree_lastNode(p)->key);
        assertEqual(303, tree_lastNode(p)->value);
        check_tree(p, 3);

        assertTrue(tree_push(p, 404));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(3, tree_lastNode(p)->key);
        assertEqual(404, tree_lastNode(p)->value);
        check_tree(p, 4);

        assertTrue(tree_push(p, 505));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(4, tree_lastNode(p)->key);
        assertEqual(505, tree_lastNode(p)->value);
        check_tree(p, 5);
    }
    tree_free(p);
}

static void test_pushFirst ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_pushFirst(p, 101));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 1);

        assertTrue(tree_pushFirst(p, 202));
        assertEqual(-1, tree_firstNode(p)->key);
        assertEqual(202, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 2);

        assertTrue(tree_pushFirst(p, 303));
        assertEqual(-2, tree_firstNode(p)->key);
        assertEqual(303, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 3);

        assertTrue(tree_pushFirst(p, 404));
        assertEqual(-3, tree_firstNode(p)->key);
        assertEqual(404, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 4);

        assertTrue(tree_pushFirst(p, 505));
        assertEqual(-4, tree_firstNode(p)->key);
        assertEqual(505, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 5);
    }
    tree_free(p);
}

static void test_pushLast ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_pushLast(p, 101));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(0, tree_lastNode(p)->key);
        assertEqual(101, tree_lastNode(p)->value);
        check_tree(p, 1);

        assertTrue(tree_pushLast(p, 202));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(1, tree_lastNode(p)->key);
        assertEqual(202, tree_lastNode(p)->value);
        check_tree(p, 2);

        assertTrue(tree_pushLast(p, 303));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(2, tree_lastNode(p)->key);
        assertEqual(303, tree_lastNode(p)->value);
        check_tree(p, 3);

        assertTrue(tree_pushLast(p, 404));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(3, tree_lastNode(p)->key);
        assertEqual(404, tree_lastNode(p)->value);
        check_tree(p, 4);

        assertTrue(tree_pushLast(p, 505));
        assertEqual(0, tree_firstNode(p)->key);
        assertEqual(101, tree_firstNode(p)->value);
        assertEqual(4, tree_lastNode(p)->key);
        assertEqual(505, tree_lastNode(p)->value);
        check_tree(p, 5);
    }
    tree_free(p);
}

static void test_put ()
{
    tree_t* tree = tree_new();
    {
        tree_node_t* p;
        tree_node_t* q;

        // Put a node.
        tree_put(tree, 101, 1001);
        p = tree_getNode(tree, 101);
        assertEqual(101, p->key);
        assertEqual(1001, p->value);
        check_tree(tree, 1);

        // Put, and replace, the same node.
        tree_put(tree, 101, 1002);
        q = tree_getNode(tree, 101);
        assertEqual(101, p->key);
        assertEqual(101, q->key);
        assertEqual(1002, p->value);
        assertEqual(p, q);
        check_tree(tree, 1);

        // Put another node.
        tree_put(tree, 202, 2001);
        p = tree_getNode(tree, 202);
        assertEqual(202, p->key);
        assertEqual(2001, p->value);
        check_tree(tree, 2);

        // Put, and replace, the same node.
        tree_put(tree, 202, 2002);
        q = tree_getNode(tree, 202);
        assertEqual(202, p->key);
        assertEqual(202, q->key);
        assertEqual(2002, p->value);
        assertEqual(p, q);
        check_tree(tree, 2);

        // Put yet another node.
        tree_put(tree, 303, 3001);
        p = tree_getNode(tree, 303);
        assertEqual(303, p->key);
        assertEqual(3001, p->value);
        check_tree(tree, 3);

        // Put, and replace, the same node.
        tree_put(tree, 303, 3002);
        q = tree_getNode(tree, 303);
        assertEqual(303, p->key);
        assertEqual(303, q->key);
        assertEqual(3002, p->value);
        assertEqual(p, q);
        check_tree(tree, 3);
    }
    tree_free(tree);
}

static void test_putNode ()
{
    tree_t* tree = tree_new();
    {
        tree_node_t* p;
        tree_node_t* q;

        // Put a node.
        p = tree_putNode(tree, 101);
        assertEqual(101, p->key);
        check_tree(tree, 1);

        // Put, but do not replace, the same node.
        q = tree_putNode(tree, 101);
        assertEqual(101, p->key);
        assertEqual(101, q->key);
        assertEqual(p, q);
        check_tree(tree, 1);

        // Put another node.
        p = tree_putNode(tree, 202);
        assertEqual(202, p->key);
        check_tree(tree, 2);

        // Put, but do not replace, the same node.
        q = tree_putNode(tree, 202);
        assertEqual(202, p->key);
        assertEqual(202, q->key);
        assertEqual(p, q);
        check_tree(tree, 2);

        // Put yet another node.
        p = tree_putNode(tree, 303);
        assertEqual(303, p->key);
        check_tree(tree, 3);

        // Put, but do not replace, the same node.
        q = tree_putNode(tree, 303);
        assertEqual(303, p->key);
        assertEqual(303, q->key);
        assertEqual(p, q);
        check_tree(tree, 3);
    }
    tree_free(tree);
}

static void test_remove ()
{
    tree_t* p = tree_new();
    {
        assertTrue(tree_put(p, 101, 1));
        assertTrue(tree_put(p, 202, 2));
        assertTrue(tree_put(p, 303, 3));

        check_tree(p, 3);

        tree_remove(p, 202);
        check_tree(p, 2);
        assertFalse(tree_containsKey(p, 202));

        tree_remove(p, 101);
        check_tree(p, 1);
        assertFalse(tree_containsKey(p, 101));

        tree_remove(p, 303);
        check_tree(p, 0);
    }
    tree_free(p);

}

static void test_rootNode ()
{
    tree_t* p = tree_new();
    {
        assertNull(tree_rootNode(p)); // Initial tree should have no root

        tree_put(p, 202, 200);
        assertNotNull(tree_rootNode(p)); // Root should now exist
        assertEqual(202, tree_rootNode(p)->key); // Root key should match the inserted key
    }
    tree_free(p);
}

static void test_size ()
{
    tree_t* p = tree_new();
    {
        assertEqual(tree_size(p), 0); // Initially should have size 0

        // Add elements
        assertTrue(tree_put(p, 101, 1));
        assertEqual(tree_size(p), 1);

        assertTrue(tree_put(p, 202, 2));
        assertEqual(tree_size(p), 2);

        // Remove one
        tree_remove(p, 101);
        assertEqual(tree_size(p), 1); // Size should decrease

        // Remove all
        tree_remove(p, 202);
        assertEqual(tree_size(p), 0); // Size should now be 0
    }
    tree_free(p);
}

static void test_allocator_dynamic ()
{
    tree_allocator_t* allocator = tree_allocator_dynamic();
    {
        assertNotNull(allocator);

        // Allocate a node
        tree_node_t* node = allocator->allocate(allocator);
        assertNotNull(node);

        // Cleanup
        allocator->release(allocator, node);
    }
    tree_allocator_free(allocator);
}

static void test_allocator_pooled ()
{
    const int preallocated = 25;
    const int capacity = 50;

    tree_node_t* nodes[capacity];
    tree_node_t* nodeX = NULL;
    tree_node_t* nodeY = NULL;

    tree_allocator_t* allocator = tree_allocator_pooled(preallocated, capacity);
    {
        assertNotNull(allocator);

        // Allocate all of the available nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            nodes[i] = allocator->allocate(allocator);
            assertNotNull(nodes[i]);
        }

        // The next allocation will fail, because no nodes are available.
        assertNull(allocator->allocate(allocator));

        nodeX = nodes[0];
        nodes[0] = NULL;
        allocator->release(allocator, nodeX);

        // Allocate a node.
        // The node that was just released will be the one allocated.
        nodeY = allocator->allocate(allocator);
        assertTrue(nodeX == nodeY);
        nodes[0] = nodeY; // put it back for next step

        // Release all of the allocated nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            allocator->release(allocator, nodes[i]);
            nodes[i] = NULL;
        }

        // Allocate all of the available nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            nodes[i] = allocator->allocate(allocator);
            assertNotNull(nodes[i]);
        }

        // The next allocation will fail, because no nodes are available.
        assertNull(allocator->allocate(allocator));

        // Release all of the allocated nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            allocator->release(allocator, nodes[i]);
            nodes[i] = NULL;
        }
    }
    tree_allocator_free(allocator);
}

static void test_allocator_slab ()
{
    const int capacity = 50;

    tree_node_t* nodes[capacity];
    tree_node_t* nodeX = NULL;
    tree_node_t* nodeY = NULL;

    tree_allocator_t* allocator = tree_allocator_slab(capacity);
    {
        assertNotNull(allocator);

        // Allocate all of the available nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            nodes[i] = allocator->allocate(allocator);
            assertNotNull(nodes[i]);
        }

        // The next allocation will fail, because no nodes are available.
        assertNull(allocator->allocate(allocator));

        nodeX = nodes[0];
        nodes[0] = NULL;
        allocator->release(allocator, nodeX);

        // Allocate a node.
        // The node that was just released will be the one allocated.
        nodeY = allocator->allocate(allocator);
        assertTrue(nodeX == nodeY);
        nodes[0] = nodeY; // put it back for next step

        // Release all of the allocated nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            allocator->release(allocator, nodes[i]);
            nodes[i] = NULL;
        }

        // Allocate all of the available nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            nodes[i] = allocator->allocate(allocator);
            assertNotNull(nodes[i]);
        }

        // The next allocation will fail, because no nodes are available.
        assertNull(allocator->allocate(allocator));

        // Release all of the allocated nodes.
        for (size_t i = 0; i < capacity; i++)
        {
            allocator->release(allocator, nodes[i]);
            nodes[i] = NULL;
        }
    }
    tree_allocator_free(allocator);
}

static void test_allocator_free ()
{
    tree_allocator_t* allocator = tree_allocator_dynamic();
    assertNotNull(allocator);

    // Free the allocator
    allocator->destroy(allocator);
}

static void test_comparator_naturalOrder ()
{
    tree_comparator_t comparator = tree_comparator_naturalOrder();

    key_t a = 10;
    key_t b = 20;

    assertTrue(comparator(NULL, &a, &b) < 0); // 10 < 20
    assertTrue(comparator(NULL, &b, &a) > 0); // 20 > 10
    assertTrue(comparator(NULL, &a, &a) == 0); // 10 == 10
}

static void test_comparator_reverseOrder ()
{
    tree_comparator_t comparator = tree_comparator_reverseOrder();

    key_t a = 10;
    key_t b = 20;

    assertTrue(comparator(NULL, &a, &b) > 0); // 10 is considered greater than 20
    assertTrue(comparator(NULL, &b, &a) < 0); // 20 is considered less than 10
    assertTrue(comparator(NULL, &a, &a) == 0); // 10 == 10
}

static void test_defaultKey ()
{
    key_t default_key = tree_defaultKey();
    assertEqual(0, default_key);
}

static void test_defaultValue ()
{
    data_t default_value = tree_defaultValue();
    assertEqual(0, default_value);
}

static void test_peek ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_peek(p));

        // Case: Non-Empty Trees
        for (int32_t i = 1; i < 10; i++)
        {
            tree_put(p, i * 100 + 1, i * 100);
            assertEqual(i * 100, tree_peek(p));
            assertEqual(i * 100, tree_lastNode(p)->value);
            check_tree(p, i);
        }
    }
    tree_free(p);
}

static void test_peekFirst ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_peekFirst(p));

        // Case: Non-Empty Trees
        for (int32_t i = 1; i < 10; i++)
        {
            tree_put(p, i * 100 + 1, i * 100);
            assertEqual(100, tree_peekFirst(p));
            assertEqual(100, tree_firstNode(p)->value);
            check_tree(p, i);
        }
    }
    tree_free(p);
}

static void test_peekLast ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_peekLast(p));

        // Case: Non-Empty Trees
        for (int32_t i = 1; i < 10; i++)
        {
            tree_put(p, i * 100 + 1, i * 100);
            assertEqual(i * 100, tree_peekLast(p));
            assertEqual(i * 100, tree_lastNode(p)->value);
            check_tree(p, i);
        }
    }
    tree_free(p);
}

static void test_pop ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_pop(p));

        // Case: Non-Empty Tree
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        check_tree(p, 3);
        assertEqual(300, tree_pop(p)); check_tree(p, 2);
        assertEqual(200, tree_pop(p)); check_tree(p, 1);
        assertEqual(100, tree_pop(p)); check_tree(p, 0);
    }
    tree_free(p);
}

static void test_popFirst ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_popFirst(p));

        // Case: Non-Empty Tree
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        check_tree(p, 3);
        assertEqual(100, tree_popFirst(p)); check_tree(p, 2);
        assertEqual(200, tree_popFirst(p)); check_tree(p, 1);
        assertEqual(300, tree_popFirst(p)); check_tree(p, 0);
    }
    tree_free(p);
}

static void test_popLast ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertEqual(tree_defaultValue(), tree_popLast(p));

        // Case: Non-Empty Tree
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        check_tree(p, 3);
        assertEqual(300, tree_popLast(p)); check_tree(p, 2);
        assertEqual(200, tree_popLast(p)); check_tree(p, 1);
        assertEqual(100, tree_popLast(p)); check_tree(p, 0);
    }
    tree_free(p);
}

static void test_containsKey ()
{
    tree_t* p = tree_new();
    {
        // Case: Empty Tree
        assertFalse(tree_containsKey(p, 101));

        // Case: Non-Empty Tree
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        check_tree(p, 3);
        assertTrue(tree_containsKey(p, 101));
        assertTrue(tree_containsKey(p, 202));
        assertTrue(tree_containsKey(p, 303));
        assertFalse(tree_containsKey(p, 404));
    }
    tree_free(p);
}

static bool containsValue_predicate (tree_t* tree, tree_node_t* node, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    const data_t value = *((data_t*) context);
    return value == node->value;
}

static void test_containsValue ()
{
    tree_t* p = tree_new();
    {
        data_t value = 42;

        // Case: Empty Tree
        assertFalse(tree_containsValue(p, &containsValue_predicate, &value));

        // Case: Non-Empty Tree
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        check_tree(p, 3);
        assertFalse(tree_containsValue(p, containsValue_predicate, &value));
        value = 100; assertTrue(tree_containsValue(p, &containsValue_predicate, &value));
        value = 200; assertTrue(tree_containsValue(p, &containsValue_predicate, &value));
        value = 300; assertTrue(tree_containsValue(p, &containsValue_predicate, &value));
    }
    tree_free(p);
}

static void test_containsAll ()
{
    tree_t* p1 = tree_new();
    tree_t* p2 = tree_new();
    {
        // Case: self is empty, other is empty.
        assertTrue(tree_containsAll(p1, p2));

        // Case: self is empty, other is self.
        assertTrue(tree_containsAll(p1, p1));

        // Case: self is non-empty, other is empty.
        tree_put(p1, 101, 100);
        assertTrue(tree_containsAll(p1, p2));
        tree_clear(p1);
        tree_clear(p2);

        // Case: self is non-empty, other is self.
        tree_put(p1, 101, 100);
        assertTrue(tree_containsAll(p1, p2));
        tree_clear(p1);
        tree_clear(p2);

        // Case: other is a proper subset of self.
        tree_put(p1, 101, 100);
        tree_put(p1, 202, 200);
        tree_put(p1, 303, 300);
        tree_put(p1, 404, 400);
        tree_put(p1, 505, 500);
        tree_put(p2, 101, 100);
        tree_put(p2, 202, 200);
        tree_put(p2, 404, 400);
        tree_put(p2, 505, 500);
        assertTrue(tree_containsAll(p1, p2));
        assertFalse(tree_containsAll(p2, p1));
        tree_clear(p1);
        tree_clear(p2);
    }
    tree_free(p1);
    tree_free(p2);
}

static void test_keysToArray ()
{
    tree_t* p = tree_new();
    {
        // Case: Null Array
        assertEqual(0, tree_keysToArray(p, NULL, 0));

        // Case: Empty Array
        key_t empty[3];
        assertEqual(0, tree_keysToArray(p, empty, 0));

        // Case: array is smaller than the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        key_t smaller[3];
        assertEqual(3, tree_keysToArray(p, smaller, 3));
        assertEqual(101, smaller[0]);
        assertEqual(202, smaller[1]);
        assertEqual(303, smaller[2]);
        tree_clear(p);

        // Case: array is same size as the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        key_t same[5];
        assertEqual(5, tree_keysToArray(p, same, 5));
        assertEqual(101, same[0]);
        assertEqual(202, same[1]);
        assertEqual(303, same[2]);
        assertEqual(404, same[3]);
        assertEqual(505, same[4]);
        tree_clear(p);

        // Case: array is larger tan the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        key_t larger[7];
        memset(&larger, 0, 7 * sizeof(key_t));
        assertEqual(5, tree_keysToArray(p, larger, 7));
        assertEqual(101, larger[0]);
        assertEqual(202, larger[1]);
        assertEqual(303, larger[2]);
        assertEqual(404, larger[3]);
        assertEqual(505, larger[4]);
        assertEqual(0, larger[5]);
        assertEqual(0, larger[6]);
        tree_clear(p);
    }
    tree_free(p);
}

static void test_valuesToArray ()
{
    tree_t* p = tree_new();
    {
        // Case: Null Array
        assertEqual(0, tree_valuesToArray(p, NULL, 0));

        // Case: Empty Array
        data_t empty[3];
        assertEqual(0, tree_valuesToArray(p, empty, 0));

        // Case: array is smaller than the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        data_t smaller[3];
        assertEqual(3, tree_valuesToArray(p, smaller, 3));
        assertEqual(100, smaller[0]);
        assertEqual(200, smaller[1]);
        assertEqual(300, smaller[2]);
        tree_clear(p);

        // Case: array is same size as the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        data_t same[5];
        assertEqual(5, tree_valuesToArray(p, same, 5));
        assertEqual(100, same[0]);
        assertEqual(200, same[1]);
        assertEqual(300, same[2]);
        assertEqual(400, same[3]);
        assertEqual(500, same[4]);
        tree_clear(p);

        // Case: array is larger tan the tree.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        data_t larger[7];
        memset(&larger, 0, 7 * sizeof(data_t));
        assertEqual(5, tree_valuesToArray(p, larger, 7));
        assertEqual(100, larger[0]);
        assertEqual(200, larger[1]);
        assertEqual(300, larger[2]);
        assertEqual(400, larger[3]);
        assertEqual(500, larger[4]);
        assertEqual(0, larger[5]);
        assertEqual(0, larger[6]);
        tree_clear(p);
    }
    tree_free(p);
}

static void test_keysToNewArray ()
{
    tree_t* p = tree_new();
    {
        // Case: Null Tree
        assertNull(tree_keysToNewArray(NULL));

        // Case: Empty Tree
        assertTrue(tree_isEmpty(p));
        assertNull(tree_keysToNewArray(p));

        // Case: Non-Empty Array.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        key_t* array = tree_keysToNewArray(p);
        assertEqual(101, array[0]);
        assertEqual(202, array[1]);
        assertEqual(303, array[2]);
        assertEqual(404, array[3]);
        assertEqual(505, array[4]);
        free(array);
        array = NULL;
    }
    tree_free(p);
}

static void test_valuesToNewArray ()
{
    tree_t* p = tree_new();
    {
        // Case: Null Tree
        assertNull(tree_valuesToNewArray(NULL));

        // Case: Empty Tree
        assertTrue(tree_isEmpty(p));
        assertNull(tree_valuesToNewArray(p));

        // Case: Non-Empty Array.
        tree_put(p, 101, 100);
        tree_put(p, 202, 200);
        tree_put(p, 303, 300);
        tree_put(p, 404, 400);
        tree_put(p, 505, 500);
        check_tree(p, 5);
        key_t* array = tree_valuesToNewArray(p);
        assertEqual(100, array[0]);
        assertEqual(200, array[1]);
        assertEqual(300, array[2]);
        assertEqual(400, array[3]);
        assertEqual(500, array[4]);
        free(array);
        array = NULL;
    }
    tree_free(p);
}

static double reduceToDouble_functor (tree_t* tree, tree_node_t* node, double identity, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    return identity * node->value;
}

static int64_t reduceToInt64_functor (tree_t* tree, tree_node_t* node, int64_t identity, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    return identity * node->value;
}

static void test_reduceToDouble ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;
        double initial = 13;
        assertEqual(initial, tree_reduceToDouble(p, reduceToDouble_functor, initial, &context));

        tree_put(p, 101, 23);
        assertEqual(13 * 23, tree_reduceToDouble(p, reduceToDouble_functor, initial, &context));

        tree_put(p, 202, 27);
        assertEqual(13 * 23 * 27, tree_reduceToDouble(p, reduceToDouble_functor, initial, &context));

        tree_put(p, 303, 31);
        assertEqual(13 * 23 * 27 * 31, tree_reduceToDouble(p, reduceToDouble_functor, initial, &context));
    }
    tree_free(p);
}

static void test_reduceToInt64 ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;
        int64_t initial = 13;
        assertEqual(initial, tree_reduceToInt64(p, reduceToInt64_functor, initial, &context));

        tree_put(p, 101, 23);
        assertEqual(13 * 23, tree_reduceToInt64(p, reduceToInt64_functor, initial, &context));

        tree_put(p, 202, 27);
        assertEqual(13 * 23 * 27, tree_reduceToInt64(p, reduceToInt64_functor, initial, &context));

        tree_put(p, 303, 31);
        assertEqual(13 * 23 * 27 * 31, tree_reduceToInt64(p, reduceToInt64_functor, initial, &context));
    }
    tree_free(p);
}

static double sumToDouble_functor (tree_t* tree, tree_node_t* node, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    return 10 * node->value + 17;
}

static int64_t sumToInt64_functor (tree_t* tree, tree_node_t* node, void* context)
{
    assertNotNull(tree);
    assertNotNull(node);
    assertNotNull(context);
    assertEqual(42, *((int32_t*) context));
    return 10 * node->value + 17;
}

static void test_sumToDouble ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;
        assertEqual(0, tree_sumToDouble(p, sumToDouble_functor, &context));

        tree_put(p, 101, 23);
        assertEqual((10 * 23 + 17), tree_sumToDouble(p, sumToDouble_functor, &context));

        tree_put(p, 202, 27);
        assertEqual((10 * 23 + 17) + (10 * 27 + 17), tree_sumToDouble(p, sumToDouble_functor, &context));

        tree_put(p, 303, 31);
        assertEqual((10 * 23 + 17) + (10 * 27 + 17) + (10 * 31 + 17), tree_sumToDouble(p, sumToDouble_functor, &context));
    }
    tree_free(p);
}

static void test_sumToInt64 ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;
        assertEqual(0, tree_sumToInt64(p, sumToInt64_functor, &context));

        tree_put(p, 101, 23);
        assertEqual((10 * 23 + 17), tree_sumToInt64(p, sumToInt64_functor, &context));

        tree_put(p, 202, 27);
        assertEqual((10 * 23 + 17) + (10 * 27 + 17), tree_sumToInt64(p, sumToInt64_functor, &context));

        tree_put(p, 303, 31);
        assertEqual((10 * 23 + 17) + (10 * 27 + 17) + (10 * 31 + 17), tree_sumToInt64(p, sumToInt64_functor, &context));
    }
    tree_free(p);
}

static void test_putAll ()
{
    tree_t* p1 = tree_new();
    tree_t* p2 = tree_new();
    {
        // Case: both trees are empty
        assertTrue(tree_putAll(p1, p2));
        check_tree(p1, 0);
        check_tree(p2, 0);

        // Case: destination is empty
        tree_put(p2, 101, 100);
        assertTrue(tree_putAll(p1, p2));
        check_tree(p1, 1);
        check_tree(p2, 1);
        assertEqual(101, tree_firstNode(p1)->key);
        assertEqual(100, tree_firstNode(p1)->value);
        assertEqual(101, tree_firstNode(p2)->key);
        assertEqual(100, tree_firstNode(p2)->value);
        tree_clear(p1);
        tree_clear(p2);

        // Case: source is empty
        tree_put(p1, 101, 100);
        assertTrue(tree_putAll(p1, p2));
        check_tree(p1, 1);
        check_tree(p2, 0);
        assertEqual(101, tree_firstNode(p1)->key);
        assertEqual(100, tree_firstNode(p1)->value);
        tree_clear(p1);
        tree_clear(p2);

        // Case: different value for same key
        tree_put(p1, 101, 200);
        tree_put(p2, 101, 300);
        assertTrue(tree_putAll(p1, p2));
        check_tree(p1, 1);
        check_tree(p2, 1);
        assertEqual(101, tree_firstNode(p1)->key);
        assertEqual(300, tree_firstNode(p1)->value);
        assertEqual(101, tree_firstNode(p2)->key);
        assertEqual(300, tree_firstNode(p2)->value);
        tree_clear(p1);
        tree_clear(p2);

        // Case: multiple elements
        tree_put(p1, 101, 100);
        tree_put(p1, 202, 200);
        tree_put(p2, 303, 300);
        tree_put(p2, 404, 400);
        tree_put(p2, 505, 500);
        assertTrue(tree_putAll(p1, p2));
        check_tree(p1, 5);
        check_tree(p2, 3);
        assertEqual(100, tree_get(p1, 101));
        assertEqual(200, tree_get(p1, 202));
        assertEqual(300, tree_get(p1, 303));
        assertEqual(400, tree_get(p1, 404));
        assertEqual(500, tree_get(p1, 505));
        assertEqual(300, tree_get(p2, 303));
        assertEqual(400, tree_get(p2, 404));
        assertEqual(500, tree_get(p2, 505));
        tree_clear(p1);
        tree_clear(p2);
    }
    tree_free(p1);
    tree_free(p2);
}

static void test_removeAll ()
{
    tree_t* p1 = tree_new();
    tree_t* p2 = tree_new();
    {
        tree_put(p1, 101, 100);
        tree_put(p1, 202, 200);
        tree_put(p1, 303, 300);

        tree_put(p2, 101, 100); // Same key
        tree_put(p2, 202, 200); // Same key
        tree_put(p2, 404, 400);

        tree_removeAll(p1, p2); // Remove all keys from p1 that are in p2

        assertEqual(1, tree_size(p1)); // Only one key should remain
        assertTrue(tree_containsKey(p1, 303)); // 303 remains
        assertFalse(tree_containsKey(p1, 101)); // 101 should be removed
        assertFalse(tree_containsKey(p1, 202)); // 202 should be removed
    }
    tree_free(p1);
    tree_free(p2);
}

static void test_retainAll ()
{
    tree_t* p1 = tree_new();
    tree_t* p2 = tree_new();
    {
        tree_put(p1, 101, 100);
        tree_put(p1, 202, 200);
        tree_put(p1, 303, 300);

        tree_put(p2, 101, 100); // Same key
        tree_put(p2, 202, 200); // Same key
        tree_put(p2, 404, 400);

        tree_retainAll(p1, p2); // Retain only keys from p1 that are in p2

        assertEqual(2, tree_size(p1)); // Only 101 and 202 should remain
        assertFalse(tree_containsKey(p1, 303)); // 303 should be removed
    }
    tree_free(p1);
    tree_free(p2);
}

static void test_noneMatch ()
{
    tree_t* p = tree_new();
    {
        int32_t context = 42;

        assertTrue(tree_noneMatch(p, &match_predicate, &context));

        tree_put(p, 101, 20);
        assertTrue(tree_noneMatch(p, &match_predicate, &context));
        tree_put(p, 202, 22);
        assertTrue(tree_noneMatch(p, &match_predicate, &context));
        tree_put(p, 303, 21); // 21 % 7 == 0
        assertFalse(tree_noneMatch(p, &match_predicate, &context));
    }
    tree_free(p);
}

void declare_tree_tests ()
{
    UNIT_TEST_CASE(TreeMap, test_1);
    UNIT_TEST_CASE(TreeMap, test_2);
    UNIT_TEST_CASE(TreeMap, test_addFirst);
    UNIT_TEST_CASE(TreeMap, test_addLast);
    UNIT_TEST_CASE(TreeMap, test_allMatch);
    UNIT_TEST_CASE(TreeMap, test_allocator_dynamic);
    UNIT_TEST_CASE(TreeMap, test_allocator_free);
    UNIT_TEST_CASE(TreeMap, test_allocator_pooled);
    UNIT_TEST_CASE(TreeMap, test_allocator_slab);
    UNIT_TEST_CASE(TreeMap, test_anyMatch);
    UNIT_TEST_CASE(TreeMap, test_comparator_naturalOrder);
    UNIT_TEST_CASE(TreeMap, test_comparator_reverseOrder);
    UNIT_TEST_CASE(TreeMap, test_containsAll);
    UNIT_TEST_CASE(TreeMap, test_containsKey);
    UNIT_TEST_CASE(TreeMap, test_containsValue);
    UNIT_TEST_CASE(TreeMap, test_copy);
    UNIT_TEST_CASE(TreeMap, test_copy_allocation_failure_special_case);
    UNIT_TEST_CASE(TreeMap, test_count);
    UNIT_TEST_CASE(TreeMap, test_defaultKey);
    UNIT_TEST_CASE(TreeMap, test_defaultValue);
    UNIT_TEST_CASE(TreeMap, test_firstNode);
    UNIT_TEST_CASE(TreeMap, test_forEach);
    UNIT_TEST_CASE(TreeMap, test_free);
    UNIT_TEST_CASE(TreeMap, test_free_stackalloc);
    UNIT_TEST_CASE(TreeMap, test_get);
    UNIT_TEST_CASE(TreeMap, test_getNode);
    UNIT_TEST_CASE(TreeMap, test_has);
    UNIT_TEST_CASE(TreeMap, test_higherNode);
    UNIT_TEST_CASE(TreeMap, test_isEmpty);
    UNIT_TEST_CASE(TreeMap, test_isEqual);
    UNIT_TEST_CASE(TreeMap, test_iter);
    UNIT_TEST_CASE(TreeMap, test_iter_at);
    UNIT_TEST_CASE(TreeMap, test_iter_atNode);
    UNIT_TEST_CASE(TreeMap, test_iter_free);
    UNIT_TEST_CASE(TreeMap, test_iter_get);
    UNIT_TEST_CASE(TreeMap, test_iter_hasNext);
    UNIT_TEST_CASE(TreeMap, test_iter_hasPrev);
    UNIT_TEST_CASE(TreeMap, test_iter_key);
    UNIT_TEST_CASE(TreeMap, test_iter_next);
    UNIT_TEST_CASE(TreeMap, test_iter_node);
    UNIT_TEST_CASE(TreeMap, test_iter_prev);
    UNIT_TEST_CASE(TreeMap, test_iter_set);
    UNIT_TEST_CASE(TreeMap, test_keysToArray);
    UNIT_TEST_CASE(TreeMap, test_keysToNewArray);
    UNIT_TEST_CASE(TreeMap, test_lastNode);
    UNIT_TEST_CASE(TreeMap, test_lowerNode);
    UNIT_TEST_CASE(TreeMap, test_make);
    UNIT_TEST_CASE(TreeMap, test_make_comparators);
    UNIT_TEST_CASE(TreeMap, test_make_stackalloc);
    UNIT_TEST_CASE(TreeMap, test_new);
    UNIT_TEST_CASE(TreeMap, test_node_get);
    UNIT_TEST_CASE(TreeMap, test_node_key);
    UNIT_TEST_CASE(TreeMap, test_node_set);
    UNIT_TEST_CASE(TreeMap, test_noneMatch);
    UNIT_TEST_CASE(TreeMap, test_nthNode);
    UNIT_TEST_CASE(TreeMap, test_peek);
    UNIT_TEST_CASE(TreeMap, test_peekFirst);
    UNIT_TEST_CASE(TreeMap, test_peekLast);
    UNIT_TEST_CASE(TreeMap, test_pop);
    UNIT_TEST_CASE(TreeMap, test_popFirst);
    UNIT_TEST_CASE(TreeMap, test_popLast);
    UNIT_TEST_CASE(TreeMap, test_push);
    UNIT_TEST_CASE(TreeMap, test_pushFirst);
    UNIT_TEST_CASE(TreeMap, test_pushLast);
    UNIT_TEST_CASE(TreeMap, test_put);
    UNIT_TEST_CASE(TreeMap, test_putAll);
    UNIT_TEST_CASE(TreeMap, test_putNode);
    UNIT_TEST_CASE(TreeMap, test_reduceToDouble);
    UNIT_TEST_CASE(TreeMap, test_reduceToInt64);
    UNIT_TEST_CASE(TreeMap, test_remove);
    UNIT_TEST_CASE(TreeMap, test_removeAll);
    UNIT_TEST_CASE(TreeMap, test_removeFirst);
    UNIT_TEST_CASE(TreeMap, test_removeIf);
    UNIT_TEST_CASE(TreeMap, test_removeLast);
    UNIT_TEST_CASE(TreeMap, test_retainAll);
    UNIT_TEST_CASE(TreeMap, test_rootNode);
    UNIT_TEST_CASE(TreeMap, test_size);
    UNIT_TEST_CASE(TreeMap, test_sumToDouble);
    UNIT_TEST_CASE(TreeMap, test_sumToInt64);
    UNIT_TEST_CASE(TreeMap, test_valuesToArray);
    UNIT_TEST_CASE(TreeMap, test_valuesToNewArray);
}
#endif


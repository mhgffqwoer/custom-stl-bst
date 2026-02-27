#include <gtest/gtest.h>
#include <sstream>
#include <vector>
#include "BST.hpp"

/**
 * @brief Tests the empty() method for both new and populated trees.
 */
TEST(BST, empty) {
    BST<int> bst1;
    EXPECT_TRUE(bst1.empty()); // New tree should be empty
    
    BST<int> bst2 = {1, 2, 3};
    EXPECT_FALSE(bst2.empty()); // Tree with elements should not be empty
}

/**
 * @brief Tests equality and inequality operators between two trees.
 */
TEST(BST, equal) {
    BST<int> bst1, bst2;
    EXPECT_TRUE(bst1 == bst2); // Two empty trees are equal
    
    bst1.insert(5);
    EXPECT_TRUE(bst1 != bst2); // Trees with different contents are not equal
    
    bst2.insert(5);
    EXPECT_TRUE(bst1 == bst2); // Trees with same contents are equal
}

/**
 * @brief Tests basic insertion and different traversal orders (In-order, Pre-order, Post-order).
 */
TEST(BST, insert) {
    BST<int> bst;
    // Constructing a specific tree structure
    bst.insert(4); bst.insert(2); bst.insert(6);
    bst.insert(1); bst.insert(3); bst.insert(5); bst.insert(7);
    EXPECT_FALSE(bst.empty());

    // In-order traversal: Left -> Root -> Right (Sorted)
    std::string expected = " 1 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);

    // Pre-order traversal: Root -> Left -> Right
    expected = " 4 2 1 3 6 5 7";
    actual = "";
    for (auto i = bst.begin(Preorder()); i != bst.end(Preorder()); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);

    // Post-order traversal: Left -> Right -> Root
    expected = " 1 3 2 5 7 6 4";
    actual = "";
    for (auto i = bst.begin(Postorder()); i != bst.end(Postorder()); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests range insertion using iterator pairs from another container.
 */
TEST(BST, insert_it) {
    BST<int> bst;
    std::vector<int> cont = {4, 2, 6, 1, 3, 5, 7};
    bst.insert(cont.begin(), cont.end());
    EXPECT_FALSE(bst.empty());

    // Verify correct structure via in-order traversal
    std::string expected = " 1 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests the insert_range() method which accepts a container directly.
 */
TEST(BST, insert_range) {
    BST<int> bst;
    std::vector<int> cont = {4, 2, 6, 1, 3, 5, 7};
    bst.insert_range(cont);
    EXPECT_FALSE(bst.empty());

    std::string expected = " 1 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests insertion via initializer list.
 */
TEST(BST, insert_iList) {
    BST<int> bst;
    bst.insert({4, 2, 6, 1, 3, 5, 7});
    EXPECT_FALSE(bst.empty());
    
    std::string expected = " 1 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests construction from an initializer list.
 */
TEST(BST, init_List) {
    BST<int> bst = {4, 2, 6, 1, 3, 5, 7};
    EXPECT_FALSE(bst.empty());
    
    std::string expected = " 1 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests various erase operations (by value, iterator, and range).
 */
TEST(BST, erase) {
    BST<int> bst = {4, 2, 6, 1, 3, 5, 7, 1, 0};
    
    // Erase all instances of value 1
    bst.erase(1);
    std::string expected = " 0 2 3 4 5 6 7";
    std::string actual;
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);

    // Erase element by mutable iterator
    bst.erase(bst.begin());
    expected = " 2 3 4 5 6 7";
    actual.clear();
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);

    // Erase element by constant iterator
    bst.erase(bst.cbegin());
    expected = " 3 4 5 6 7";
    actual.clear();
    for (auto i = bst.begin(); i != bst.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);

    // Erase entire range (clear the tree)
    bst.erase(bst.cbegin(), bst.cend());
    EXPECT_TRUE(bst.empty());
}

/**
 * @brief Tests the find() method for both mutable and constant trees.
 */
TEST(BST, find) {
    BST<int> bst1 = {4, 2, 6, 1, 3, 5, 7};
    EXPECT_EQ(bst1.find(1), bst1.begin()); // Should find 1 at the beginning
    EXPECT_EQ(bst1.find(8), bst1.end());   // Should return end() if not found
    
    const BST<int> bst2 = {4, 2, 6, 1, 3, 5, 7};
    EXPECT_EQ(bst2.find(1), bst2.cbegin());
    EXPECT_EQ(bst2.find(8), bst2.cend());
}

/**
 * @brief Tests the contains() method.
 */
TEST(BST, contains) {
    BST<int> bst = {4, 2, 6, 1, 3, 5, 7};
    EXPECT_TRUE(bst.contains(1));
    EXPECT_FALSE(bst.contains(8));
}

/**
 * @brief Tests extraction of a value (returns value and removes it from tree).
 */
TEST(BST, extract) {
    BST<int> bst;
    bst.insert(5);
    EXPECT_EQ(bst.extract(5), 5); // Should return 5
    EXPECT_TRUE(bst.empty());     // Tree should be empty after extraction
}

/**
 * @brief Tests the swap() method between two trees.
 */
TEST(BST, swap) {
    BST<int> bst1 = {4, 2, 6, 1, 3, 5, 7};
    BST<int> bst2 = {1, 2, 3, 4, 5, 6, 7};
    bst1.swap(bst2);
    EXPECT_EQ(bst1, bst2);
    EXPECT_EQ(bst2, bst1);
}

/**
 * @brief Tests merging one tree into another.
 */
TEST(BST, merge) {
    BST<int> bst1 = {4, 2, 6, 1, 3, 5, 7};
    BST<int> bst2 = {1, 2, 3, 4, 5, 6, 7};
    bst1.merge(bst2);
    
    // Result should contain duplicates from both trees
    std::string expected = " 1 1 2 2 3 3 4 4 5 5 6 6 7 7";
    std::string actual;
    for (auto i = bst1.begin(); i != bst1.end(); ++i) {
        actual += ' ' + std::to_string(*i);
    }
    EXPECT_EQ(expected, actual);
}

/**
 * @brief Tests equal_range() for different traversal strategies.
 */
TEST(BST, equal_range) {
    BST<int> bst = {4, 2, 6, 1, 3, 5, 7};
    
    // In-order equal_range for value 3
    auto [a, b] = bst.equal_range(3);
    EXPECT_EQ(*a, 3);
    EXPECT_EQ(*b, 4);

    // Pre-order equal_range for value 3
    auto [c, d] = bst.equal_range(3, Preorder());
    EXPECT_EQ(*c, 3);
    EXPECT_EQ(*d, 6);

    // Post-order equal_range for value 3
    auto [e, f] = bst.equal_range(3, Postorder());
    EXPECT_EQ(*e, 3);
    EXPECT_EQ(*f, 2);
}
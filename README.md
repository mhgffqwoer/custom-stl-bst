# Custom STL-compatible BST

A lightweight implementation of a Binary Search Tree (BST) as a template container, designed to be compatible with standard STL container requirements and conventions.

## Description

The package provides a `BST` container that can be parameterized by element type, a custom comparator, and an allocator. It is designed to support various traversal strategies through a flexible iterator system.

**Features:**

- **Multiple Traversal Orders**: Native support for in-order, pre-order, and post-order traversals via iterator tags.
- **Bidirectional Iterators**: Supports both forward (`++`) and backward (`--`) movement for all traversal types.
- **STL Integration**: Fully compatible with custom allocators and standard iterator traits.
- **Non-owning Sentinel**: Uses a sentinel root node to simplify boundary conditions and range logic.

## How it works

The tree stores elements in `Node<T>` structures containing pointers to parent, left child, and right child. Iterators are implemented as a nested class `Iterator<IsConst, Pick>`, where `Pick` is a tag defining the traversal logic.

1. **Inorder** - Visits elements in sorted order (Left -> Root -> Right).
2. **Preorder** - Visits the root before its subtrees (Root -> Left -> Right).
3. **Postorder** - Visits subtrees before the root (Left -> Right -> Root).
4. **ReverseIterator** - A bidirectional adapter that reverses any of the above traversal sequences.

The container provides standard methods such as `insert`, `erase`, `find`, and `equal_range`, ensuring it can be used as a drop-in replacement for sorted associative-like collections.

## Project structure

```
/ (root directory)
├─ CMakeLists.txt
├─ README.md                # this description (now in English)
├─ bin/
│   ├─ CMakeLists.txt
│   └─ main.cpp             # container demonstration
├─ lib/                     # the library itself
│   ├─ CMakeLists.txt
│   └─ BST/                 # implementation
│       └─ BST.hpp          # main template header
└─ tests/
├─ CMakeLists.txt
└─ tests.cpp                # unit tests and usage examples
```

## Usage examples

### Basic In-order Traversal

By default, the tree uses in-order traversal, which returns elements in ascending (sorted) order.

```cpp
BST<int> bst = {4, 2, 6, 1, 3, 5, 7};
// Expected output: 1 2 3 4 5 6 7
for (auto it = bst.begin(); it != bst.end(); ++it) {
    std::cout << *it << " ";
}

```

### Pre-order and Post-order Traversals

You can specify a different traversal strategy by passing the corresponding tag (`Preorder` or `Postorder`) to the `begin()` and `end()` methods.

```cpp
BST<int> bst = {4, 2, 6, 1, 3, 5, 7};

// Pre-order (Root -> Left -> Right)
// Output: 4 2 1 3 6 5 7
for (auto it = bst.begin(Preorder()); it != bst.end(Preorder()); ++it) {
    std::cout << *it << " ";
}

// Post-order (Left -> Right -> Root)
// Output: 1 3 2 5 7 6 4
for (auto it = bst.begin(Postorder()); it != bst.end(Postorder()); ++it) {
    std::cout << *it << " ";
}

```

### Search Operations

The library provides several ways to find elements, including checking for existence or finding specific iterators.

```cpp
BST<int> bst = {4, 2, 6, 1, 3, 5, 7};

// Check if an element exists
if (bst.contains(5)) { // returns true
    auto it = bst.find(5);
    std::cout << "Found element: " << *it << std::endl;
}

// Get a range of equal elements (useful if the tree contains duplicates)
auto [low, high] = bst.equal_range(3);

```

### Reverse Iteration

Any supported traversal order can be reversed using the `rbegin()` and `rend()` methods combined with a traversal tag.

```cpp
BST<int> bst = {1, 2, 3, 4, 5};
auto rev = bst.rbegin(Inorder());

// Output: 5 4 3 2 1
while (rev != bst.rend()) {
    std::cout << *rev << " ";
    ++rev;
}

```

### Modification (Erase and Extract)

Elements can be removed by value or by iterator. The `extract` method removes the element and returns its value.

```cpp
BST<int> bst = {10, 20, 30};

bst.erase(20); // Removes the element with value 20

int val = bst.extract(bst.begin()); // Removes the first element and returns its value

```
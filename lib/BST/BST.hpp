#pragma once
#include <iostream>
#include <limits>

using std::initializer_list;
using std::pair;

/**
 * @brief Tag type indicating in-order traversal (Left -> Root -> Right).
 */
struct Inorder {};

/**
 * @brief Tag type indicating pre-order traversal (Root -> Left -> Right).
 */
struct Preorder {};

/**
 * @brief Tag type indicating post-order traversal (Left -> Right -> Root).
 */
struct Postorder {};

/**
 * @brief Compile-time conditional type selection.
 * @tparam B Boolean condition.
 * @tparam U Type chosen if B is true.
 * @tparam V Type chosen if B is false.
 */
template <bool B, class U, class V>
struct conditional {
    using type = U;
};

/**
 * @brief Specialization of conditional for the false case.
 */
template <class U, class V>
struct conditional<false, U, V> {
    using type = V;
};

/**
 * @brief Shorthand alias for conditional selection.
 */
template <bool B, class U, class V>
using conditional_t = typename conditional<B, U, V>::type;

/**
 * @brief Bidirectional reverse iterator adapter.
 * @tparam Iterator The underlying base iterator type.
 */
template <typename Iterator>
class ReverseIterator {
   private:
    Iterator it_;

   public:
    /**
     * @brief Constructs a reverse iterator from a base iterator.
     * @param it The base iterator to wrap.
     */
    ReverseIterator(Iterator it) : it_(it) {}

    /**
     * @brief Dereferences the iterator to access the element.
     * @return Reference to the element.
     */
    auto operator*() const { return *it_; }

    /**
     * @brief Pre-increment: moves the reverse iterator forward by decrementing the base.
     * @return Reference to the updated iterator.
     */
    ReverseIterator<Iterator>& operator++() {
        --it_;
        return *this;
    }

    /**
     * @brief Pre-decrement: moves the reverse iterator backward by incrementing the base.
     * @return Reference to the updated iterator.
     */
    ReverseIterator<Iterator>& operator--() {
        ++it_;
        return *this;
    }

    /**
     * @brief Post-increment.
     * @return The state of the iterator before incrementing.
     */
    ReverseIterator<Iterator> operator++(int) {
        ReverseIterator<Iterator> tmp = *this;
        --it_;
        return tmp;
    }

    /**
     * @brief Post-decrement.
     * @return The state of the iterator before decrementing.
     */
    ReverseIterator<Iterator> operator--(int) {
        ReverseIterator<Iterator> tmp = *this;
        ++it_;
        return tmp;
    }

    /**
     * @brief Equality comparison.
     */
    bool operator==(const ReverseIterator<Iterator>& other) const {
        return it_ == other.it_;
    }

    /**
     * @brief Inequality comparison.
     */
    bool operator!=(const ReverseIterator<Iterator>& other) const {
        return it_ != other.it_;
    }

    /**
     * @brief Provides access to the underlying base iterator.
     * @return Reference to the base iterator.
     */
    Iterator& base() { return it_; }
};

/**
 * @brief Internal node structure for the Binary Search Tree.
 * @tparam T The type of data stored in the node.
 */
template <class T>
struct Node {
    T data_;
    Node<T>* parent_;
    Node<T>* left_;
    Node<T>* right_;
};

/**
 * @brief A Binary Search Tree (BST) container compatible with STL conventions.
 * @tparam T Type of the elements.
 * @tparam Compare Comparison functor for ordering.
 * @tparam Alloc Allocator for memory management.
 */
template <class T, class Compare = std::less<T>,
          class Alloc = std::allocator<T>>
class BST {
    using allocator_type = Alloc;
    using node_allocator_traits = std::allocator_traits<allocator_type>;
    using node_allocator_type =
        typename node_allocator_traits::template rebind_alloc<Node<T>>;

   private:
    /**
     * @brief Internal iterator implementation for BST traversal.
     * @tparam IsConst Boolean flag for constant iterators.
     * @tparam Pick Traversal strategy (Inorder, Preorder, Postorder).
     */
    template <bool IsConst, class Pick = Inorder>
    class Iterator {
        using conditional_ptr =
            conditional_t<IsConst, const Node<T>*, Node<T>*>;
        using conditional_ref = conditional_t<IsConst, const T&, T&>;
        friend class BST<T, Compare, Alloc>;

       private:
        conditional_ptr it_;

        /** @brief Internal logic to advance iterator for In-order traversal. */
        Iterator& Add(Inorder) {
            if (it_->right_ != nullptr) {
                it_ = it_->right_;
                while (it_->left_ != nullptr) {
                    it_ = it_->left_;
                }
            } else if (it_->parent_ != nullptr) {
                while (it_ == it_->parent_->right_) {
                    it_ = it_->parent_;
                    if (it_->parent_->parent_ == nullptr) {
                        break;
                    }
                }
                it_ = it_->parent_;
            }
            return *this;
        }

        /** @brief Internal logic to advance iterator for Pre-order traversal. */
        Iterator& Add(Preorder) {
            if (it_->left_ != nullptr) {
                it_ = it_->left_;
            } else {
                while (it_->parent_ != nullptr &&
                       (it_ == it_->parent_->right_ ||
                        it_->parent_->right_ == nullptr)) {
                    it_ = it_->parent_;
                }
                if (it_->parent_ != nullptr) {
                    it_ = it_->parent_;
                }
                if (it_->right_ != nullptr) {
                    it_ = it_->right_;
                }
            }
            return *this;
        }

        /** @brief Internal logic to advance iterator for Post-order traversal. */
        Iterator& Add(Postorder) {
            if (it_ == it_->parent_->left_) {
                it_ = it_->parent_;
                if (it_->right_ != nullptr) {
                    it_ = it_->right_;
                    while (it_->left_ != nullptr) {
                        it_ = it_->left_;
                    }
                }
            } else {
                it_ = it_->parent_;
            }
            return *this;
        }

        /** @brief Internal logic to retreat iterator for In-order traversal. */
        Iterator& Subtract(Inorder) {
            if (it_->left_ != nullptr) {
                it_ = it_->left_;
                while (it_->right_ != nullptr) {
                    it_ = it_->right_;
                }
            } else if (it_->parent_ != nullptr) {
                while (it_ == it_->parent_->left_) {
                    it_ = it_->parent_;
                    if (it_->parent_->parent_ == nullptr) {
                        break;
                    }
                }
                it_ = it_->parent_;
            }
            return *this;
        }

        /** @brief Internal logic to retreat iterator for Pre-order traversal. */
        Iterator& Subtract(Preorder) {
            if (it_->parent_ == nullptr) {
                it_ = it_->left_;
                while (it_->right_ != nullptr) {
                    it_ = it_->right_;
                }
                return *this;
            }
            if (it_ == it_->parent_->left_) {
                it_ = it_->parent_;
            } else {
                it_ = it_->parent_;
                if (it_->left_ != nullptr) {
                    it_ = it_->left_;
                    while (it_->right_ != nullptr) {
                        it_ = it_->right_;
                    }
                }
            }
            return *this;
        }

        /** @brief Internal logic to retreat iterator for Post-order traversal. */
        Iterator& Subtract(Postorder) {
            if (it_->parent_ == nullptr) {
                it_ = it_->left_;
                return *this;
            } else if (it_->right_ != nullptr) {
                it_ = it_->right_;
            } else if (it_->left_ != nullptr) {
                it_ = it_->left_;
            } else {
                while (it_->parent_->right_ == nullptr ||
                       it_ == it_->parent_->left_) {
                    it_ = it_->parent_;
                    if (it_->parent_ == nullptr) {
                        return *this;
                    }
                }
                it_ = it_->parent_;
                if (it_->left_ != nullptr) {
                    it_ = it_->left_;
                }
            }
            return *this;
        }

       public:
        /** @brief Constructor initializing iterator with a node. */
        Iterator(conditional_ptr current) : it_(current) {}

        /** @brief Dereference operator to access node data. */
        conditional_ref operator*() const { return it_->data_; }

        /** @brief Pre-increment operator. */
        Iterator& operator++() { return Add(Pick()); }

        /** @brief Pre-decrement operator. */
        Iterator& operator--() { return Subtract(Pick()); }

        /** @brief Post-increment operator. */
        Iterator operator++(int) {
            Iterator tmp = *this;
            Add(Pick());
            return tmp;
        }

        /** @brief Post-decrement operator. */
        Iterator operator--(int) {
            Iterator tmp = *this;
            Subtract(Pick());
            return tmp;
        }

        /** @brief Equality comparison. */
        bool operator==(const Iterator& other) const {
            return it_ == other.it_;
        }

        /** @brief Inequality comparison. */
        bool operator!=(const Iterator& other) const {
            return !(it_ == other.it_);
        }
    };

    Node<T>* root_;
    size_t size_;
    Compare comp_;
    node_allocator_type alloc_;

    /**
     * @brief Internal helper to allocate and initialize a new node.
     * @return Pointer to the newly allocated node.
     */
    Node<T>* alloc_node() {
        Node<T>* node = alloc_.allocate(1);
        new (node) Node<T>();
        return node;
    }

    /**
     * @brief Internal helper to destroy and deallocate a node.
     * @param node_ Pointer to the node to be removed.
     */
    void delete_node(Node<T>* node_) {
        if (!node_) {
            return;
        }
        node_->~Node<T>();
        alloc_.deallocate(node_, 1);
    }

    /**
     * @brief Recursively deletes all nodes in a subtree.
     * @param node The root node of the subtree to clear.
     */
    void ClearRecursive(Node<T>* node) {
        if (node != nullptr) {
            ClearRecursive(node->left_);
            ClearRecursive(node->right_);
            delete_node(node);
        }
    }

    /**
     * @brief Recursively deep-copies a subtree.
     * @param other The source node to copy.
     * @param parent The parent node to assign to the new copy.
     * @return Pointer to the cloned subtree root.
     */
    Node<T>* CopyRecursive(const Node<T>* other, Node<T>* parent = nullptr) {
        if (other == nullptr) {
            return nullptr;
        }
        Node<T>* node = alloc_node();
        node->data_ = other->data_;
        node->parent_ = parent;
        node->left_ = CopyRecursive(other->left_, node);
        node->right_ = CopyRecursive(other->right_, node);
        return node;
    }

    /**
     * @brief Locates the node with the minimum value in the tree.
     * @return Pointer to the leftmost node.
     */
    Node<T>* Leftmost() const {
        if (root_->left_ == nullptr) {
            return root_;
        }
        Node<T>* current = root_->left_;
        while (current->left_ != nullptr) {
            current = current->left_;
        }
        return current;
    }

    /**
     * @brief Locates the leftmost node starting from a specific node.
     * @param node Start node.
     * @return Pointer to the leftmost descendant.
     */
    Node<T>* Leftmost(Node<T>* node) const {
        while (node->left_ != nullptr) {
            node = node->left_;
        }
        return node;
    }

    /**
     * @brief Locates the node with the maximum value in the tree.
     * @return Pointer to the rightmost node.
     */
    Node<T>* Rightmost() const {
        Node<T>* current = root_->left_;
        while (current->right_ != nullptr) {
            current = current->right_;
        }
        return current;
    }

    /**
     * @brief Internal logic to remove a node and maintain BST properties.
     * @param node Pointer to the node to delete.
     */
    void DeleteNode(Node<T>* node) {
        if (node->left_ == nullptr && node->right_ == nullptr) {
            if (node == node->parent_->left_) {
                node->parent_->left_ = nullptr;
            } else {
                node->parent_->right_ = nullptr;
            }
        } else if (node->left_ == nullptr) {
            if (node == node->parent_->left_) {
                node->parent_->left_ = node->right_;
                node->right_->parent_ = node->parent_;
            } else {
                node->parent_->right_ = node->right_;
                node->right_->parent_ = node->parent_;
            }
        } else if (node->right_ == nullptr) {
            if (node == node->parent_->left_) {
                node->parent_->left_ = node->left_;
                node->left_->parent_ = node->parent_;
            } else {
                node->parent_->right_ = node->left_;
                node->left_->parent_ = node->parent_;
            }
        } else {
            Node<T>* temp = Leftmost(node->right_);
            if (node == node->parent_->left_) {
                node->parent_->left_ = node->right_;
                node->right_->parent_ = node->parent_;
            } else {
                node->parent_->right_ = node->right_;
                node->right_->parent_ = node->parent_;
            }
            temp->left_ = node->left_;
            node->left_->parent_ = temp;
        }
        delete_node(node);
    }

   public:
    /**
     * @brief Default constructor. Initializes an empty tree with a sentinel root.
     */
    BST() : root_(alloc_node()), size_(0), comp_(Compare()), alloc_(Alloc()) {}

    /**
     * @brief Copy constructor. Performs a deep copy of another tree.
     * @param other The tree to copy.
     */
    BST(const BST& other)
        : root_(CopyRecursive(other.root_)),
          size_(other.size_),
          comp_(Compare()),
          alloc_(Alloc()) {}

    /**
     * @brief Copy assignment operator.
     * @param other The source tree.
     * @return Reference to this tree.
     */
    BST& operator=(const BST& other) {
        if (this != &other) {
            ClearRecursive(root_);
            root_ = CopyRecursive(other.root_);
            size_ = other.size_;
        }
        return *this;
    }

    /**
     * @brief Move assignment operator.
     * @param other Rvalue reference to the source tree.
     * @return Reference to this tree.
     */
    BST& operator=(BST&& other) {
        if (this != &other) {
            ClearRecursive(root_);
            root_ = other.root_;
            size_ = other.size_;
        }
        return *this;
    }

    /**
     * @brief Constructor that initializes the tree from a list of values.
     * @param ilist Initializer list of elements.
     */
    BST(initializer_list<T> ilist) : BST() {
        insert(ilist.begin(), ilist.end());
    }

    /**
     * @brief Replaces contents with values from an initializer list.
     * @param ilist Initializer list.
     * @return Reference to this tree.
     */
    BST& operator=(initializer_list<T> ilist) {
        clear();
        insert(ilist.begin(), ilist.end());
        return *this;
    }

    /**
     * @brief Destructor. Clears all nodes from the tree.
     */
    ~BST() { ClearRecursive(root_); }

    /** @brief Returns an In-order iterator to the beginning. */
    Iterator<false, Inorder> begin(Inorder = Inorder()) {
        return Iterator<false, Inorder>(Leftmost());
    }

    /** @brief Returns an In-order iterator to the end (sentinel). */
    Iterator<false, Inorder> end(Inorder = Inorder()) {
        return Iterator<false, Inorder>(root_);
    }

    /** @brief Returns a constant In-order iterator to the beginning. */
    Iterator<true, Inorder> cbegin(Inorder = Inorder()) const {
        return Iterator<true, Inorder>(Leftmost());
    }

    /** @brief Returns a constant In-order iterator to the end (sentinel). */
    Iterator<true, Inorder> cend(Inorder = Inorder()) const {
        return Iterator<true, Inorder>(root_);
    }

    /** @brief Returns a Pre-order iterator to the beginning. */
    Iterator<false, Preorder> begin(Preorder) {
        return Iterator<false, Preorder>(root_->left_);
    }

    /** @brief Returns a Pre-order iterator to the end (sentinel). */
    Iterator<false, Preorder> end(Preorder) {
        return Iterator<false, Preorder>(root_);
    }

    /** @brief Returns a constant Pre-order iterator to the beginning. */
    Iterator<true, Preorder> cbegin(Preorder) const {
        return Iterator<true, Preorder>(root_->left_);
    }

    /** @brief Returns a constant Pre-order iterator to the end (sentinel). */
    Iterator<true, Preorder> cend(Preorder) const {
        return Iterator<true, Preorder>(root_);
    }

    /** @brief Returns a Post-order iterator to the beginning. */
    Iterator<false, Postorder> begin(Postorder) {
        return Iterator<false, Postorder>(Leftmost(root_));
    }

    /** @brief Returns a Post-order iterator to the end (sentinel). */
    Iterator<false, Postorder> end(Postorder) {
        return Iterator<false, Postorder>(root_);
    }

    /** @brief Returns a constant Post-order iterator to the beginning. */
    Iterator<true, Postorder> cbegin(Postorder) const {
        return Iterator<true, Postorder>(Leftmost(root_));
    }

    /** @brief Returns a constant Post-order iterator to the end (sentinel). */
    Iterator<true, Postorder> cend(Postorder) const {
        return Iterator<true, Postorder>(root_);
    }

    /**
     * @brief Returns a reverse iterator starting from the logical end.
     * @tparam Pick Traversal strategy.
     */
    template <class Pick = Inorder>
    ReverseIterator<Iterator<false, Pick>> rbegin(Pick = Inorder()) {
        return ++(ReverseIterator<Iterator<false, Pick>>(end(Pick())));
    }

    /**
     * @brief Returns a reverse iterator pointing to the logical beginning.
     * @tparam Pick Traversal strategy.
     */
    template <class Pick = Inorder>
    ReverseIterator<Iterator<false, Pick>> rend(Pick = Inorder()) {
        return ++(ReverseIterator<Iterator<false, Pick>>(begin(Pick())));
    }

    /** @brief Constant version of rbegin. */
    template <class Pick = Inorder>
    ReverseIterator<Iterator<true, Pick>> crbegin(Pick = Inorder()) const {
        return ++(ReverseIterator<Iterator<true, Pick>>(cend(Pick())));
    }

    /** @brief Constant version of rend. */
    template <class Pick = Inorder>
    ReverseIterator<Iterator<true, Pick>> crend(Pick = Inorder()) const {
        return ++(ReverseIterator<Iterator<true, Pick>>(cbegin(Pick())));
    }

    /** @brief Checks if the tree contains no user data. */
    bool empty() const { return root_->left_ == nullptr; }

    /** @brief Returns the total number of elements in the tree. */
    size_t size() const { return size_; }

    /** @brief Returns the theoretical maximum number of nodes. */
    size_t max_size() const {
        return (std::numeric_limits<size_t>::max() - sizeof(BST)) / sizeof(Node<T>);
    }

    /** @brief Removes all user elements from the tree. */
    void clear() {
        ClearRecursive(root_->left_);
        root_->left_ = nullptr;
        size_ = 0;
    }

    /** @brief Swaps contents with another BST instance. */
    void swap(BST& other) {
        Node<T>* temp = root_;
        root_ = other.root_;
        other.root_ = temp;
    }

    /**
     * @brief Inserts a new value into the BST.
     * @param value The value to insert.
     * @return A pair containing the iterator to the element and a success flag.
     */
    template <class Pick = Inorder>
    pair<Iterator<false, Pick>, bool> insert(const T& value, Pick = Inorder()) {
        Node<T>* current = root_->left_;
        Node<T>* parent = root_;
        while (current != nullptr) {
            parent = current;
            if (comp_(value, current->data_)) {
                current = current->left_;
            } else {
                current = current->right_;
            }
        }
        Node<T>* newNode = alloc_node();
        newNode->data_ = value;
        newNode->parent_ = parent;
        newNode->left_ = nullptr;
        newNode->right_ = nullptr;
        if (parent->parent_ == nullptr) {
            root_->left_ = newNode;
        } else if (comp_(value, parent->data_)) {
            parent->left_ = newNode;
        } else {
            parent->right_ = newNode;
        }
        size_++;
        return {Iterator<false, Pick>(newNode), true};
    }

    /** @brief Inserts a range of elements. */
    template <class InputIt>
    void insert(InputIt first, InputIt last) {
        while (first != last) {
            insert(*first);
            ++first;
        }
    }

    /** @brief Inserts elements from an initializer list. */
    void insert(initializer_list<T> il) { insert(il.begin(), il.end()); }

    /** @brief Inserts elements from any container range. */
    template <class Container>
    void insert_range(const Container& c) {
        insert(c.cbegin(), c.cend());
    }

    /**
     * @brief Erases an element pointed to by an iterator.
     * @return Iterator to the next element.
     */
    template <class Pick>
    Iterator<false, Pick> erase(Iterator<false, Pick> it) {
        if (it == end(Pick())) {
            return it;
        }
        Node<T>* node = it.it_;
        ++it;
        DeleteNode(node);
        --size_;
        return it;
    }

    /** @brief Constant version of erase. */
    template <class Pick>
    Iterator<true, Pick> erase(Iterator<true, Pick> it) {
        if (it == cend(Pick())) {
            return it;
        }
        Node<T>* node = const_cast<Node<T>*>(it.it_);
        ++it;
        DeleteNode(node);
        --size_;
        return it;
    }

    /**
     * @brief Erases elements matching a specific value.
     * @return The number of elements removed.
     */
    size_t erase(const T& value) {
        size_t count = 0;
        auto it = find(value);
        while (it != end()) {
            erase(it);
            ++count;
            it = find(value);
        }
        return count;
    }

    /** @brief Erases a range of elements. */
    template <class Pick>
    void erase(Iterator<false, Pick> first, Iterator<false, Pick> last) {
        while (first != last) {
            erase(first++);
        }
    }

    /** @brief Constant range erase. */
    template <class Pick>
    void erase(Iterator<true, Pick> first, Iterator<true, Pick> last) {
        while (first != last) {
            erase(first++);
        }
    }

    /** @brief Extracts and returns a value while removing it from the tree. */
    template <class Pick>
    T extract(Iterator<false, Pick> it) {
        T value = *it;
        erase(it);
        return value;
    }

    /** @brief Constant extract. */
    template <class Pick>
    T extract(Iterator<true, Pick> it) {
        T value = *it;
        erase(it);
        return value;
    }

    /** @brief Extracts an element matching the value. */
    T extract(const T& value) {
        auto it = find(value);
        return extract(it);
    }

    /** @brief Merges another tree's contents into this one. */
    void merge(const BST& other) { insert_range(other); }

    /** @brief Counts occurrences of a specific value. */
    size_t count(const T& value) const {
        Node<T>* current = root_->left_;
        size_t count = 0;
        while (current != nullptr) {
            if (current->data_ == value) {
                count++;
            }
            if (comp_(value, current->data_)) {
                current = current->left_;
            } else {
                current = current->right_;
            }
        }
        return count;
    }

    /**
     * @brief Finds the first occurrence of a value.
     * @return Iterator to the element or end() if not found.
     */
    template <class Pick = Inorder>
    Iterator<false, Pick> find(const T& value, Pick = Inorder()) {
        auto it = begin(Pick());
        while (it != end(Pick()) && *it != value) {
            ++it;
        }
        return it;
    }

    /** @brief Constant version of find. */
    template <class Pick = Inorder>
    Iterator<true, Pick> find(const T& value, Pick = Inorder()) const {
        auto it = cbegin(Pick());
        while (it != cend(Pick()) && *it != value) {
            ++it;
        }
        return it;
    }

    /** @brief Finds the last occurrence of a value. */
    template <class Pick = Inorder>
    Iterator<false, Pick> rfind(const T& value, Pick = Inorder()) {
        auto it = find(value, Pick());
        while (it != end(Pick()) && *it == value) {
            ++it;
        }
        if (it != end(Pick())) {
            --it;
        }
        return it;
    }

    /** @brief Constant version of rfind. */
    template <class Pick = Inorder>
    Iterator<true, Pick> rfind(const T& value, Pick = Inorder()) const {
        auto it = find(value, Pick());
        while (it != cend(Pick()) && *it == value) {
            ++it;
        }
        if (it != cend(Pick())) {
            --it;
        }
        return it;
    }

    /** @brief Checks if the value exists in the tree. */
    bool contains(const T& value) const { return find(value) != cend(); }

    /** @brief Returns a range of elements matching the value. */
    template <class Pick = Inorder>
    pair<Iterator<false, Pick>, Iterator<false, Pick>> equal_range(
        const T& value, Pick = Inorder()) {
        return {lower_bound(value, Pick()), upper_bound(value, Pick())};
    }

    /** @brief Constant version of equal_range. */
    template <class Pick = Inorder>
    pair<Iterator<true, Pick>, Iterator<true, Pick>> equal_range(
        const T& value, Pick = Inorder()) const {
        return {lower_bound(value, Pick()), upper_bound(value, Pick())};
    }

    /** @brief Returns the lower bound for a value. */
    template <class Pick = Inorder>
    Iterator<false, Pick> lower_bound(const T& value, Pick = Inorder()) {
        return find(value, Pick());
    }

    /** @brief Constant lower bound. */
    template <class Pick = Inorder>
    Iterator<true, Pick> lower_bound(const T& value, Pick = Inorder()) const {
        return find(value, Pick());
    }

    /** @brief Returns the upper bound for a value. */
    template <class Pick = Inorder>
    Iterator<false, Pick> upper_bound(const T& value, Pick = Inorder()) {
        auto it = rfind(value, Pick());
        if (it != end(Pick())) {
            ++it;
        }
        return it;
    }

    /** @brief Constant upper bound. */
    template <class Pick = Inorder>
    Iterator<true, Pick> upper_bound(const T& value, Pick = Inorder()) const {
        auto it = rfind(value, Pick());
        if (it != cend(Pick())) {
            ++it;
        }
        return it;
    }

    /** @brief Returns the comparison functor. */
    Compare value_comp() const { return comp_; }

    /** @brief STL compatibility alias for value_comp. */
    Compare key_comp() const { return comp_; }

    /** @brief Returns the allocator instance. */
    Alloc get_allocator() const { return alloc_; }

    /** @brief Checks if two trees are equal based on size and sequence. */
    bool operator==(const BST& other) const {
        if (size_ != other.size_) {
            return false;
        }
        auto other_it = other.cbegin();
        for (auto it = cbegin(); it != cend(); ++it) {
            if (*it != *other_it) {
                return false;
            }
            ++other_it;
        }
        return true;
    }

    /** @brief Checks if two trees are not equal. */
    bool operator!=(const BST& other) const { return !(*this == other); }
};

/**
 * @brief Non-member swap for BST.
 */
template <class T, class Compare, class Alloc>
void swap(BST<T, Compare, Alloc>& lhs, BST<T, Compare, Alloc>& rhs) {
    lhs.swap(rhs);
}

/**
 * @brief Non-member equality operator for BST.
 */
template <class T, class Compare, class Alloc>
bool operator==(const BST<T, Compare, Alloc>& lhs,
                const BST<T, Compare, Alloc>& rhs) {
    return lhs.operator==(rhs);
}

/**
 * @brief Non-member inequality operator for BST.
 */
template <class T, class Compare, class Alloc>
bool operator!=(const BST<T, Compare, Alloc>& lhs,
                const BST<T, Compare, Alloc>& rhs) {
    return lhs.operator!=(rhs);
}

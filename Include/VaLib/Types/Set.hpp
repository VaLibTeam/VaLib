// VaLib - Vast Library
// Licensed under GNU GPL v3 License. See LICENSE file.
// (C) 2025 VaLibTeam
#pragma once

#include <VaLib/Meta/BasicDefine.hpp>

#include <VaLib/Types/BasicTypedef.hpp>
#include <VaLib/Types/Pair.hpp>

#include <functional>

enum class VaSetTreeColor { Red, Black };

template <typename T>
struct VaSetNode {
    using Color = VaSetTreeColor;

    const T key;

    VaSetNode<T>* left;
    VaSetNode<T>* right;
    VaSetNode<T>* parent;

    Color color;

    VaSetNode(const T& k, VaSetNode<T>* p = nullptr, Color c = Color::Red)
        : key(k), left(nullptr), right(nullptr) {
        parent = p;
        color = c;
    }
};

template <typename T, typename Compare = std::less<T>>
class VaSet {
  public:
    using Color = VaSetTreeColor;
    using Node = VaSetNode<T>;

  public:
    class NodeHandle {
      protected:
        Node* node;

      protected friends:
        friend class VaSet;

      public:
        NodeHandle(Node* n = nullptr) : node(n) {}
        NodeHandle(NodeHandle&& other) noexcept : node(other.node) { other.node = nullptr; }

        NodeHandle& operator=(NodeHandle&& other) noexcept {
            if (this != &other) {
                node = other.node;
                other.node = nullptr;
            }
            return *this;
        }

        // non-copyable
        NodeHandle(const NodeHandle&) = delete;
        NodeHandle& operator=(const NodeHandle&) = delete;

        bool isEmpty() const { return node == nullptr; }
        const T& key() const { return node->key; }

        inline operator bool() { return !isEmpty(); }

        Node* getRaw() const { return node; }
    };

    class Iterator {
      protected:
        Node* current;

      protected friends:
        friend class VaSet;

      public:
        Iterator(Node* p = nullptr) : current(p) {}

        const T& operator*() const { return current->key; }

        Iterator& operator++() {
            if (current->right) {
                current = current->right;
                while (current->left) {
                    current = current->left;
                }

            } else {
                Node* p = current->parent;
                while (p && current == p->right) {
                    current = p;
                    p = p->parent;
                }

                current = p;
            }
            return *this;
        }

        Iterator operator++(int) {
            Iterator tmp = *this;
            ++(*this);
            return tmp;
        }

        friend inline bool operator!=(const Iterator& lhs, const Iterator& rhs) {
            return lhs.current != rhs.current;
        }
        friend inline bool operator==(const Iterator& lhs, const Iterator& rhs) {
            return lhs.current == rhs.current;
        }
    };

  protected friends:
    friend class NodeHandle;
    friend class Iterator;

  protected:
    Node* root;
    Compare comp;

    Size len;

    void leftRotate(Node* x) {
        Node* y = x->right;
        x->right = y->left;
        if (y->left) y->left->parent = x;
        y->parent = x->parent;
        if (x->parent == nullptr) {
            root = y;
        } else if (x == x->parent->left) {
            x->parent->left = y;
        } else {
            x->parent->right = y;
        }

        y->left = x;
        x->parent = y;
    }

    void rightRotate(Node* x) {
        Node* y = x->left;
        x->left = y->right;
        if (y->right) y->right->parent = x;
        y->parent = x->parent;
        if (!x->parent) {
            root = y;
        } else if (x == x->parent->right) {
            x->parent->right = y;
        } else {
            x->parent->left = y;
        }

        y->right = x;
        x->parent = y;
    }

    void insertFixup(Node* z) {
        while (z->parent && z->parent->color == Color::Red) {

            if (z->parent == z->parent->parent->left) {
                Node* y = z->parent->parent->right;
                if (y && y->color == Color::Red) {

                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;

                } else {
                    if (z == z->parent->right) {
                        z = z->parent;
                        leftRotate(z);
                    }

                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;

                    rightRotate(z->parent->parent);
                }
            } else {
                Node* y = z->parent->parent->left;
                if (y && y->color == Color::Red) {
                    z->parent->color = Color::Black;
                    y->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    z = z->parent->parent;
                } else {
                    if (z == z->parent->left) {
                        z = z->parent;
                        rightRotate(z);
                    }

                    z->parent->color = Color::Black;
                    z->parent->parent->color = Color::Red;
                    leftRotate(z->parent->parent);
                }
            }
        }

        root->color = Color::Black;
    }

    void deleteFixup(Node* x) {
        while (x != root && (!x || x->color == Color::Black)) {
            // case 1: x is the left child of its parent
            if (x == x->parent->left) {
                Node* w = x->parent->right; // w is x's sibling

                // case 1.1: sibling w is red
                if (w->color == Color::Red) {
                    w->color = Color::Black;
                    x->parent->color = Color::Red;
                    leftRotate(x->parent);
                    w = x->parent->right;
                }

                // Case 1.2: both of w's children are black
                if ((!w->left || w->left->color == Color::Black) &&
                    (!w->right || w->right->color == Color::Black)) {
                    w->color = Color::Red; // Recolor w to red
                    x = x->parent;         // Move the problem up the tree
                } else {
                    // case 1.3: w's right child is black
                    if (!w->right || w->right->color == Color::Black) {
                        w->left->color = Color::Black;
                        w->color = Color::Red;
                        rightRotate(w);
                        w = x->parent->right;
                    }

                    // case 1.4: w's right child is red
                    // final color changes and left rotation
                    w->color = x->parent->color;
                    x->parent->color = Color::Black;
                    w->right->color = Color::Black;
                    leftRotate(x->parent);
                    x = root;
                }
            } else { // case 2: x is the right child (symmetric to case 1)
                Node* w = x->parent->left;

                // case 2.1: sibling w is red
                if (w->color == Color::Red) {
                    w->color = Color::Black;
                    x->parent->color = Color::Red;
                    rightRotate(x->parent);
                    w = x->parent->left;
                }

                // case 2.2: both of w's children are black
                if ((!w->right || w->right->color == Color::Black) &&
                    (!w->left || w->left->color == Color::Black)) {
                    w->color = Color::Red;
                    x = x->parent;
                } else {
                    // case 2.3: w's left child is black
                    if (!w->left || w->left->color == Color::Black) {
                        w->right->color = Color::Black;
                        w->color = Color::Red;
                        leftRotate(w);
                        w = x->parent->left;
                    }

                    // case 2.4: w's left child is red
                    w->color = x->parent->color;
                    x->parent->color = Color::Black;
                    w->left->color = Color::Black;
                    rightRotate(x->parent);
                    x = root;
                }
            }
        }

        if (x) {
            x->color = Color::Black;
        }
    }

    void transplant(Node* u, Node* v) {
        if (!u->parent) {
            root = v;
        } else if (u == u->parent->left) {
            u->parent->left = v;
        } else {
            u->parent->right = v;
        }

        if (v) {
            v->parent = u->parent;
        }
    }

    Node* minimum(Node* z) const {
        while (z->left) {
            z = z->left;
        }

        return z;
    }

    void clear(Node* z) {
        if (z) {
            clear(z->left);
            clear(z->right);
            delete z;
        }
    }

  public:
    VaSet() : root(nullptr), len(0) {}
    VaSet(std::initializer_list<T> init) : VaSet() {
        for (const T& val: init) {
            add(val);
        }
    }

    ~VaSet() { clear(root); }

    Iterator begin() const {
        Node* z = root;
        while (z && z->left) z = z->left;
        return Iterator(z);
    }

    Iterator end() const { return Iterator(nullptr); }

    VaPair<Iterator, bool> insert(const T& key) {
        Node* y = nullptr;
        Node* x = root;
        while (x) {
            y = x;

            if (comp(key, x->key)) {
                x = x->left;
            } else if (comp(x->key, key)) {
                x = x->right;
            } else {
                return {Iterator(x), false};
            }
        }

        Node* z = new Node(key, y);
        if (!y) {
            root = z;
        } else if (comp(z->key, y->key)) {
            y->left = z;
        } else {
            y->right = z;
        }

        insertFixup(z);
        len++;
        return {Iterator(z), true};
    }

    void add(const T& key) {
        Node* y = nullptr;
        Node* x = root;
        while (x) {
            y = x;

            if (comp(key, x->key)) {
                x = x->left;
            } else if (comp(x->key, key)) {
                x = x->right;
            } else {
                return;
            }
        }

        Node* z = new Node(key, y);
        if (!y) {
            root = z;
        } else if (comp(z->key, y->key)) {
            y->left = z;
        } else {
            y->right = z;
        }

        insertFixup(z);
        len++;
    }

    VaPair<Iterator, bool> insert(NodeHandle&& nh) {
        if (nh.isEmpty()) return {end(), false};

        Node* z = nh.node;
        Node* y = nullptr;
        Node* x = root;

        while (x) {
            y = x;
            if (comp(z->key, x->key)) {
                x = x->left;
            } else if (comp(x->key, z->key)) {
                x = x->right;
            } else {
                delete z;
                nh.node = nullptr;

                return {Iterator(x), false};
            }
        }

        z->parent = y;
        if (!y) {
            root = z;
        } else if (comp(z->key, y->key)) {
            y->left = z;
        } else {
            y->right = z;
        }

        insertFixup(z);
        nh.node = nullptr;
        len++;
        return {Iterator(z), true};
    }

    Iterator find(const T& key) const {
        Node* x = root;
        while (x) {
            if (comp(key, x->key)) {
                x = x->left;
            } else if (comp(x->key, key)) {
                x = x->right;
            } else {
                return Iterator(x);
            }
        }

        return end();
    }

    void swap(VaSet& other) {
        std::swap(root, other.root);
        std::swap(len, other.len);
        std::swap(comp, other.comp);
    }

    void merge(VaSet& other) {
        for (auto it = other.begin(); it != other.end();) {
            auto current = it++;
            NodeHandle nh = other.extract(current);

            if (!insert(std::move(nh)).second) {
                /// insert failed, so we keep the element in @ref other
                other.add(std::move(nh));
            }
        }
    }

    void erase(Iterator pos) {
        Node* z = pos.current;
        if (!z) return;

        Node* y = z;
        Color yOriginalClr = y->color;
        Node* x = nullptr;

        if (!z->left) {
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalClr = y->color;
            x = y->right;
            if (y->parent == z) {
                if (x) x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        delete z;
        if (yOriginalClr == Color::Black && x) deleteFixup(x);

        len--;
    }

    NodeHandle extract(Iterator pos) {
        Node* z = pos.current;
        if (!z) return NodeHandle();

        Node* y = z;
        Color yOriginalClr = y->color;
        Node* x = nullptr;

        if (!z->left) {
            x = z->right;
            transplant(z, z->right);
        } else if (!z->right) {
            x = z->left;
            transplant(z, z->left);
        } else {
            y = minimum(z->right);
            yOriginalClr = y->color;
            x = y->right;
            if (y->parent == z) {
                if (x) x->parent = y;
            } else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }

        if (yOriginalClr == Color::Black && x) deleteFixup(x);

        len--;

        // detach the node from tree context
        z->left = z->right = z->parent = nullptr;
        return NodeHandle(z);
    }

    NodeHandle extract(const T& key) { return extract(find(key)); }

    bool isEmpty() const { return len == 0; }

  public friends:
    friend inline Size len(const VaSet& set) { return set.len; }

  public operators:
    friend bool operator==(const VaSet& lhs, const VaSet& rhs) {
        if (lhs.len != rhs.len) return false;
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();

        while (it1 != lhs.end() && it2 != rhs.end()) {
            if (*it1 != *it2) return false;

            it1++;
            it2++;
        }

        return true;
    }

    friend bool operator!=(const VaSet& lhs, const VaSet& rhs) { return !(lhs == rhs); }

    friend bool operator<(const VaSet& lhs, const VaSet& rhs) {
        auto it1 = lhs.begin();
        auto it2 = rhs.begin();

        while (it1 != lhs.end() && it2 != rhs.end()) {
            if (*it1 < *it2) return true;
            if (*it2 < *it1) return false;

            it1++;
            it2++;
        }

        return (it1 == lhs.end()) && (it2 != rhs.end());
    }

    friend inline bool operator>(const VaSet& lhs, const VaSet& rhs) { return rhs < lhs; }
    friend inline bool operator<=(const VaSet& lhs, const VaSet& rhs) { return !(rhs < lhs); }
    friend inline bool operator>=(const VaSet& lhs, const VaSet& rhs) { return !(lhs < rhs); }

    friend inline VaSet operator|(const VaSet& lhs, const VaSet& rhs) {
        VaSet result = lhs;
        for (auto& val: rhs) {
            result.insert(val);
        }
        return result;
    }
};

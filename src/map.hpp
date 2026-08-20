/**
* implement a container like std::map
*/
#ifndef SJTU_MAP_HPP
#define SJTU_MAP_HPP

// only for std::less<T>
#include <functional>
#include <cstddef>
#include "utility.hpp"
#include "exceptions.hpp"

namespace sjtu {

template<
   class Key,
   class T,
   class Compare = std::less <Key>
   > class map {
  public:
   typedef pair<const Key, T> value_type;

  private:
   struct Node {
       value_type data;
       Node *left, *right, *parent;
       int height;

       Node(const value_type& val)
           : data(val), left(nullptr), right(nullptr), parent(nullptr), height(1) {}
   };

   Node* root;
   size_t size_;
   Compare comp;

   int getHeight(Node* node) const {
       return (node == nullptr) ? 0 : node->height;
   }

   int getBalance(Node* node) const {
       return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
   }

   void swapNodes(Node* a, Node* b) {
       if (a == b) return;

       // Update root if necessary
       if (a->parent == nullptr) {
           root = b;
       } else if (a->parent->left == a) {
           a->parent->left = b;
       } else {
           a->parent->right = b;
       }

       if (b->parent == nullptr) {
           root = a;
       } else if (b->parent->left == b) {
           b->parent->left = a;
       } else {
           b->parent->right = a;
       }

       // Handle case where a is parent of b
       if (a->left == b) {
           a->left = b->left;
           b->left = a;
           if (a->left != nullptr) {
               a->left->parent = a;
           }
           Node* tempRight = a->right;
           a->right = b->right;
           b->right = tempRight;
           if (a->right != nullptr) {
               a->right->parent = a;
           }
           if (b->right != nullptr) {
               b->right->parent = b;
           }
       } else if (a->right == b) {
           a->right = b->right;
           b->right = a;
           if (a->right != nullptr) {
               a->right->parent = a;
           }
           Node* tempLeft = a->left;
           a->left = b->left;
           b->left = tempLeft;
           if (a->left != nullptr) {
               a->left->parent = a;
           }
           if (b->left != nullptr) {
               b->left->parent = b;
           }
       } else if (b->left == a) {
           b->left = a->left;
           a->left = b;
           if (b->left != nullptr) {
               b->left->parent = b;
           }
           Node* tempRight = b->right;
           b->right = a->right;
           a->right = tempRight;
           if (b->right != nullptr) {
               b->right->parent = b;
           }
           if (a->right != nullptr) {
               a->right->parent = a;
           }
       } else if (b->right == a) {
           b->right = a->right;
           a->right = b;
           if (b->right != nullptr) {
               b->right->parent = b;
           }
           Node* tempLeft = b->left;
           b->left = a->left;
           a->left = tempLeft;
           if (b->left != nullptr) {
               b->left->parent = b;
           }
           if (a->left != nullptr) {
               a->left->parent = a;
           }
       } else {
           // No parent-child relationship
           Node* tempLeft = a->left;
           Node* tempRight = a->right;
           Node* tempParent = a->parent;
           int tempHeight = a->height;

           a->left = b->left;
           a->right = b->right;
           a->parent = b->parent;
           a->height = b->height;

           b->left = tempLeft;
           b->right = tempRight;
           b->parent = tempParent;
           b->height = tempHeight;

           if (a->left != nullptr) a->left->parent = a;
           if (a->right != nullptr) a->right->parent = a;
           if (b->left != nullptr) b->left->parent = b;
           if (b->right != nullptr) b->right->parent = b;
       }
   }

   Node* rotateLeft(Node* z) {
       Node* y = z->right;
       Node* T2 = y->left;

       y->left = z;
       z->right = T2;

       y->parent = z->parent;
       z->parent = y;
       if (T2 != nullptr) {
           T2->parent = z;
       }

       z->height = 1 + std::max(getHeight(z->left), getHeight(z->right));
       y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

       return y;
   }

   Node* rotateRight(Node* z) {
       Node* y = z->left;
       Node* T2 = y->right;

       y->right = z;
       z->left = T2;

       y->parent = z->parent;
       z->parent = y;
       if (T2 != nullptr) {
           T2->parent = z;
       }

       z->height = 1 + std::max(getHeight(z->left), getHeight(z->right));
       y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

       return y;
   }

   Node* insertNode(Node* node, const value_type& val, Node* parent, Node*& insertedNode, bool& success) {
       if (node == nullptr) {
           insertedNode = new Node(val);
           insertedNode->parent = parent;
           success = true;
           size_++;
           return insertedNode;
       }

       if (comp(val.first, node->data.first)) {
           node->left = insertNode(node->left, val, node, insertedNode, success);
       } else if (comp(node->data.first, val.first)) {
           node->right = insertNode(node->right, val, node, insertedNode, success);
       } else {
           insertedNode = node;
           success = false;
           return node;
       }

       node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
       int balance = getBalance(node);

       if (balance > 1 && comp(val.first, node->left->data.first)) {
           Node* rotated = rotateRight(node);
           if (rotated->left != nullptr) rotated->left->parent = rotated;
           if (rotated->right != nullptr) rotated->right->parent = rotated;
           return rotated;
       }
       if (balance < -1 && comp(node->right->data.first, val.first)) {
           Node* rotated = rotateLeft(node);
           if (rotated->left != nullptr) rotated->left->parent = rotated;
           if (rotated->right != nullptr) rotated->right->parent = rotated;
           return rotated;
       }
       if (balance > 1 && comp(node->left->data.first, val.first)) {
           node->left = rotateLeft(node->left);
           if (node->left != nullptr) node->left->parent = node;
           Node* rotated = rotateRight(node);
           if (rotated->left != nullptr) rotated->left->parent = rotated;
           if (rotated->right != nullptr) rotated->right->parent = rotated;
           return rotated;
       }
       if (balance < -1 && comp(val.first, node->right->data.first)) {
           node->right = rotateRight(node->right);
           if (node->right != nullptr) node->right->parent = node;
           Node* rotated = rotateLeft(node);
           if (rotated->left != nullptr) rotated->left->parent = rotated;
           if (rotated->right != nullptr) rotated->right->parent = rotated;
           return rotated;
       }

       return node;
   }

   Node* findMin(Node* node) const {
       if (node == nullptr) return nullptr;
       while (node->left != nullptr) {
           node = node->left;
       }
       return node;
   }

   Node* findMax(Node* node) const {
       if (node == nullptr) return nullptr;
       while (node->right != nullptr) {
           node = node->right;
       }
       return node;
   }

   void updateHeightAndRebalance(Node* current) {
       while (current != nullptr) {
           current->height = 1 + std::max(getHeight(current->left), getHeight(current->right));
           int balance = getBalance(current);
           Node* nextParent = current->parent;

           if (balance > 1 && getBalance(current->left) >= 0) {
               Node* rotated = rotateRight(current);
               if (rotated->parent != nullptr) {
                   if (current == rotated->parent->left) {
                       rotated->parent->left = rotated;
                   } else {
                       rotated->parent->right = rotated;
                   }
               } else {
                   root = rotated;
               }
               current = rotated;
           } else if (balance > 1 && getBalance(current->left) < 0) {
               current->left = rotateLeft(current->left);
               if (current->left != nullptr) {
                   current->left->parent = current;
               }
               Node* rotated = rotateRight(current);
               if (rotated->parent != nullptr) {
                   if (current == rotated->parent->left) {
                       rotated->parent->left = rotated;
                   } else {
                       rotated->parent->right = rotated;
                   }
               } else {
                   root = rotated;
               }
               current = rotated;
           } else if (balance < -1 && getBalance(current->right) <= 0) {
               Node* rotated = rotateLeft(current);
               if (rotated->parent != nullptr) {
                   if (current == rotated->parent->left) {
                       rotated->parent->left = rotated;
                   } else {
                       rotated->parent->right = rotated;
                   }
               } else {
                   root = rotated;
               }
               current = rotated;
           } else if (balance < -1 && getBalance(current->right) > 0) {
               current->right = rotateRight(current->right);
               if (current->right != nullptr) {
                   current->right->parent = current;
               }
               Node* rotated = rotateLeft(current);
               if (rotated->parent != nullptr) {
                   if (current == rotated->parent->left) {
                       rotated->parent->left = rotated;
                   } else {
                       rotated->parent->right = rotated;
                   }
               } else {
                   root = rotated;
               }
               current = rotated;
           }

           current = nextParent;
       }
   }

   Node* deleteNode(Node* node, const Key& key) {
       if (node == nullptr) {
           return node;
       }

       if (comp(key, node->data.first)) {
           node->left = deleteNode(node->left, key);
           if (node->left != nullptr) {
               node->left->parent = node;
           }
       } else if (comp(node->data.first, key)) {
           node->right = deleteNode(node->right, key);
           if (node->right != nullptr) {
               node->right->parent = node;
           }
       } else {
           if (node->left == nullptr || node->right == nullptr) {
               Node* temp = node->left ? node->left : node->right;
               if (temp == nullptr) {
                   temp = node;
                   node = nullptr;
                   Node* parent = temp->parent;
                   if (parent != nullptr) {
                       if (parent->left == temp) {
                           parent->left = nullptr;
                       } else {
                           parent->right = nullptr;
                       }
                   }
                   delete temp;
                   size_--;
                   if (parent != nullptr) {
                       updateHeightAndRebalance(parent);
                   }
               } else {
                   temp->parent = node->parent;
                   Node* toDelete = node;
                   node = temp;
                   Node* parent = toDelete->parent;
                   if (parent != nullptr) {
                       if (parent->left == toDelete) {
                           parent->left = node;
                       } else {
                           parent->right = node;
                       }
                   }
                   delete toDelete;
                   size_--;
                   updateHeightAndRebalance(node->parent != nullptr ? node->parent : node);
               }
           } else {
               Node* temp = findMin(node->right);
               swapNodes(node, temp);
               // Now, node is in temp's original position (leaf, since temp was min of right subtree, so no left child)
               // Delete node
               Node* parent = node->parent;
               if (parent != nullptr) {
                   if (parent->left == node) {
                       parent->left = nullptr;
                   } else {
                       parent->right = nullptr;
                   }
               }
               delete node;
               size_--;
               // temp is now in node's original position, update height and rebalance from parent
               updateHeightAndRebalance(parent);
               // Now, temp is the node that should be returned
               node = temp;
           }
       }

       return node;
   }

   Node* findNode(Node* node, const Key& key) const {
       if (node == nullptr) return nullptr;
       if (comp(key, node->data.first)) {
           return findNode(node->left, key);
       } else if (comp(node->data.first, key)) {
           return findNode(node->right, key);
       } else {
           return node;
       }
   }

   Node* copyTree(Node* otherNode, Node* parent) {
       if (otherNode == nullptr) return nullptr;
       Node* newNode = new Node(otherNode->data);
       newNode->parent = parent;
       newNode->left = copyTree(otherNode->left, newNode);
       newNode->right = copyTree(otherNode->right, newNode);
       newNode->height = otherNode->height;
       return newNode;
   }

   void deleteTree(Node* node) {
       if (node == nullptr) return;
       deleteTree(node->left);
       deleteTree(node->right);
       delete node;
   }

  public:
   class const_iterator;
   class iterator {
      private:
       friend class map;
       friend class const_iterator;
       Node* node;
       const map* container;

      public:
       iterator() : node(nullptr), container(nullptr) {}
       iterator(const iterator &other) : node(other.node), container(other.container) {}
       iterator(Node* n, const map* c) : node(n), container(c) {}

       iterator operator++(int) {
           iterator temp = *this;
           ++(*this);
           return temp;
       }

       iterator &operator++() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           if (node->right != nullptr) {
               node = container->findMin(node->right);
           } else {
               Node* parent = node->parent;
               while (parent != nullptr && node == parent->right) {
                   node = parent;
                   parent = parent->parent;
               }
               node = parent;
           }
           return *this;
       }

       iterator operator--(int) {
           iterator temp = *this;
           --(*this);
           return temp;
       }

       iterator &operator--() {
           if (node == nullptr) {
               if (container->root == nullptr) {
                   throw invalid_iterator();
               }
               node = container->findMax(container->root);
               return *this;
           }
           if (node->left != nullptr) {
               node = container->findMax(node->left);
           } else {
               Node* parent = node->parent;
               while (parent != nullptr && node == parent->left) {
                   node = parent;
                   parent = parent->parent;
               }
               if (parent == nullptr) {
                   throw invalid_iterator();
               }
               node = parent;
           }
           return *this;
       }

       value_type &operator*() const {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           return node->data;
       }

       bool operator==(const iterator &rhs) const {
           return (node == rhs.node) && (container == rhs.container);
       }
       bool operator==(const const_iterator &rhs) const {
           return (node == rhs.node) && (container == rhs.container);
       }
       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }
       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       value_type *operator->() const noexcept {
           return &(node->data);
       }
   };

   class const_iterator {
      private:
       friend class map;
       friend class iterator;
       Node* node;
       const map* container;
      public:
       const_iterator() : node(nullptr), container(nullptr) {}
       const_iterator(const const_iterator &other) : node(other.node), container(other.container) {}
       const_iterator(const iterator &other) : node(other.node), container(other.container) {}
       const_iterator(Node* n, const map* c) : node(n), container(c) {}

       const_iterator operator++(int) {
           const_iterator temp = *this;
           ++(*this);
           return temp;
       }
       const_iterator &operator++() {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           if (node->right != nullptr) {
               node = container->findMin(node->right);
           } else {
               Node* parent = node->parent;
               while (parent != nullptr && node == parent->right) {
                   node = parent;
                   parent = parent->parent;
               }
               node = parent;
           }
           return *this;
       }
       const_iterator operator--(int) {
           const_iterator temp = *this;
           --(*this);
           return temp;
       }
       const_iterator &operator--() {
           if (node == nullptr) {
               if (container->root == nullptr) {
                   throw invalid_iterator();
               }
               node = container->findMax(container->root);
               return *this;
           }
           if (node->left != nullptr) {
               node = container->findMax(node->left);
           } else {
               Node* parent = node->parent;
               while (parent != nullptr && node == parent->left) {
                   node = parent;
                   parent = parent->parent;
               }
               if (parent == nullptr) {
                   throw invalid_iterator();
               }
               node = parent;
           }
           return *this;
       }

       const value_type &operator*() const {
           if (node == nullptr) {
               throw invalid_iterator();
           }
           return node->data;
       }
       bool operator==(const iterator &rhs) const {
           return (node == rhs.node) && (container == rhs.container);
       }
       bool operator==(const const_iterator &rhs) const {
           return (node == rhs.node) && (container == rhs.container);
       }
       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }
       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }
       const value_type *operator->() const noexcept {
           return &(node->data);
       }
   };

   map() : root(nullptr), size_(0), comp(Compare()) {}
   map(const map &other) : root(nullptr), size_(0), comp(other.comp) {
       root = copyTree(other.root, nullptr);
       size_ = other.size_;
   }
   map &operator=(const map &other) {
       if (this != &other) {
           clear();
           root = copyTree(other.root, nullptr);
           size_ = other.size_;
           comp = other.comp;
       }
       return *this;
   }
   ~map() {
       clear();
   }

   T &at(const Key &key) {
       Node* node = findNode(root, key);
       if (node == nullptr) {
           throw index_out_of_bound();
       }
       return node->data.second;
   }
   const T &at(const Key &key) const {
       Node* node = findNode(root, key);
       if (node == nullptr) {
           throw index_out_of_bound();
       }
       return node->data.second;
   }

   T &operator[](const Key &key) {
       Node* node = findNode(root, key);
       if (node != nullptr) {
           return node->data.second;
       }
       auto result = insert(value_type(key, T()));
       return result.first->second;
   }
   const T &operator[](const Key &key) const {
       return at(key);
   }

   iterator begin() {
       return iterator(findMin(root), this);
   }
   const_iterator cbegin() const {
       return const_iterator(findMin(root), this);
   }
   iterator end() {
       return iterator(nullptr, this);
   }
   const_iterator cend() const {
       return const_iterator(nullptr, this);
   }

   bool empty() const {
       return size_ == 0;
   }
   size_t size() const {
       return size_;
   }

   void clear() {
       deleteTree(root);
       root = nullptr;
       size_ = 0;
   }

   pair<iterator, bool> insert(const value_type &value) {
       Node* insertedNode = nullptr;
       bool success = false;
       root = insertNode(root, value, nullptr, insertedNode, success);
       if (root != nullptr) {
           root->parent = nullptr;
       }
       return pair<iterator, bool>(iterator(insertedNode, this), success);
   }

   void erase(iterator pos) {
       if (pos.node == nullptr || pos.container != this) {
           throw invalid_iterator();
       }
       root = deleteNode(root, pos.node->data.first);
       if (root != nullptr) {
           root->parent = nullptr;
       }
   }

   size_t count(const Key &key) const {
       return (findNode(root, key) != nullptr) ? 1 : 0;
   }

   iterator find(const Key &key) {
       Node* node = findNode(root, key);
       return iterator(node, this);
   }
   const_iterator find(const Key &key) const {
       Node* node = findNode(root, key);
       return const_iterator(node, this);
   }
};

}

#endif

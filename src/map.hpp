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
   /**
  * the internal type of data.
  * it should have a default constructor, a copy constructor.
  * You can use sjtu::map as value_type by typedef.
    */
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
           return rotateRight(node);
       }
       if (balance < -1 && comp(node->right->data.first, val.first)) {
           return rotateLeft(node);
       }
       if (balance > 1 && comp(node->left->data.first, val.first)) {
           node->left = rotateLeft(node->left);
           return rotateRight(node);
       }
       if (balance < -1 && comp(val.first, node->right->data.first)) {
           node->right = rotateRight(node->right);
           return rotateLeft(node);
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
                   // No child case
                   temp = node;
                   node = nullptr;
                   delete temp;
                   size_--;
               } else {
                   // One child case: replace node with temp
                   temp->parent = node->parent;
                   Node* toDelete = node;
                   node = temp;
                   delete toDelete;
                   size_--;
               }
           } else {
               // Two children case
               Node* temp = findMin(node->right);
               // Copy temp's data to node using placement new
               node->data.~value_type();
               new (&node->data) value_type(temp->data);
               // Delete temp
               node->right = deleteNode(node->right, temp->data.first);
               if (node->right != nullptr) {
                   node->right->parent = node;
               }
           }
       }

       if (node == nullptr) {
           return node;
       }

       node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));
       int balance = getBalance(node);

       if (balance > 1 && getBalance(node->left) >= 0) {
           Node* rotated = rotateRight(node);
           if (rotated->left != nullptr) {
               rotated->left->parent = rotated;
           }
           if (rotated->right != nullptr) {
               rotated->right->parent = rotated;
           }
           return rotated;
       }

       if (balance > 1 && getBalance(node->left) < 0) {
           node->left = rotateLeft(node->left);
           if (node->left != nullptr) {
               node->left->parent = node;
           }
           Node* rotated = rotateRight(node);
           if (rotated->left != nullptr) {
               rotated->left->parent = rotated;
           }
           if (rotated->right != nullptr) {
               rotated->right->parent = rotated;
           }
           return rotated;
       }

       if (balance < -1 && getBalance(node->right) <= 0) {
           Node* rotated = rotateLeft(node);
           if (rotated->left != nullptr) {
               rotated->left->parent = rotated;
           }
           if (rotated->right != nullptr) {
               rotated->right->parent = rotated;
           }
           return rotated;
       }

       if (balance < -1 && getBalance(node->right) > 0) {
           node->right = rotateRight(node->right);
           if (node->right != nullptr) {
               node->right->parent = node;
           }
           Node* rotated = rotateLeft(node);
           if (rotated->left != nullptr) {
               rotated->left->parent = rotated;
           }
           if (rotated->right != nullptr) {
               rotated->right->parent = rotated;
           }
           return rotated;
       }

       return node;
   }

   Node* findNode(Node* node, const Key& key) const {
       if (node == nullptr) {
           return nullptr;
       }
       if (comp(key, node->data.first)) {
           return findNode(node->left, key);
       } else if (comp(node->data.first, key)) {
           return findNode(node->right, key);
       } else {
           return node;
       }
   }

   Node* copyTree(Node* otherNode, Node* parent) {
       if (otherNode == nullptr) {
           return nullptr;
       }
       Node* newNode = new Node(otherNode->data);
       newNode->parent = parent;
       newNode->left = copyTree(otherNode->left, newNode);
       newNode->right = copyTree(otherNode->right, newNode);
       newNode->height = otherNode->height;
       return newNode;
   }

   void deleteTree(Node* node) {
       if (node == nullptr) {
           return;
       }
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
       return pair<iterator, bool>(iterator(insertedNode, this), success);
   }

   void erase(iterator pos) {
       if (pos.node == nullptr || pos.container != this) {
           throw invalid_iterator();
       }
       root = deleteNode(root, pos.node->data.first);
       // Update root's parent to nullptr (if root exists)
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

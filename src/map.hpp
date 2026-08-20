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

   // Helper functions
   int getHeight(Node* node) const {
       return (node == nullptr) ? 0 : node->height;
   }

   int getBalance(Node* node) const {
       return (node == nullptr) ? 0 : getHeight(node->left) - getHeight(node->right);
   }

   Node* rotateLeft(Node* z) {
       Node* y = z->right;
       Node* T2 = y->left;

       // Perform rotation
       y->left = z;
       z->right = T2;

       // Update parents
       y->parent = z->parent;
       z->parent = y;
       if (T2 != nullptr) {
           T2->parent = z;
       }

       // Update heights
       z->height = 1 + std::max(getHeight(z->left), getHeight(z->right));
       y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

       return y;
   }

   Node* rotateRight(Node* z) {
       Node* y = z->left;
       Node* T2 = y->right;

       // Perform rotation
       y->right = z;
       z->left = T2;

       // Update parents
       y->parent = z->parent;
       z->parent = y;
       if (T2 != nullptr) {
           T2->parent = z;
       }

       // Update heights
       z->height = 1 + std::max(getHeight(z->left), getHeight(z->right));
       y->height = 1 + std::max(getHeight(y->left), getHeight(y->right));

       return y;
   }

   Node* insertNode(Node* node, const value_type& val, Node* parent, Node*& insertedNode, bool& success) {
       // Base case
       if (node == nullptr) {
           insertedNode = new Node(val);
           insertedNode->parent = parent;
           success = true;
           size_++;
           return insertedNode;
       }

       // Compare keys
       if (comp(val.first, node->data.first)) {
           node->left = insertNode(node->left, val, node, insertedNode, success);
       } else if (comp(node->data.first, val.first)) {
           node->right = insertNode(node->right, val, node, insertedNode, success);
       } else {
           // Key already exists
           insertedNode = node;
           success = false;
           return node;
       }

       // Update height of current node
       node->height = 1 + std::max(getHeight(node->left), getHeight(node->right));

       // Get balance factor
       int balance = getBalance(node);

       // Left Left Case
       if (balance > 1 && comp(val.first, node->left->data.first)) {
           return rotateRight(node);
       }

       // Right Right Case
       if (balance < -1 && comp(node->right->data.first, val.first)) {
           return rotateLeft(node);
       }

       // Left Right Case
       if (balance > 1 && comp(node->left->data.first, val.first)) {
           node->left = rotateLeft(node->left);
           return rotateRight(node);
       }

       // Right Left Case
       if (balance < -1 && comp(val.first, node->right->data.first)) {
           node->right = rotateRight(node->right);
           return rotateLeft(node);
       }

       return node;
   }

   Node* findMin(Node* node) const {
       if (node == nullptr) {
           return nullptr;
       }
       while (node->left != nullptr) {
           node = node->left;
       }
       return node;
   }

   Node* findMax(Node* node) const {
       if (node == nullptr) {
           return nullptr;
       }
       while (node->right != nullptr) {
           node = node->right;
       }
       return node;
   }

   void deleteNodeByPointer(Node* node) {
       if (node == nullptr) {
           return;
       }

       Node* temp;
       Node* child;

       // Case 1: Node has 0 or 1 children
       if (node->left == nullptr || node->right == nullptr) {
           temp = node;
       } else {
           // Case 2: Node has two children, get successor (min of right subtree)
           temp = findMin(node->right);
       }

       // Get child of temp (temp has at most one child)
       if (temp->left != nullptr) {
           child = temp->left;
       } else {
           child = temp->right;
       }

       // Update child's parent
       if (child != nullptr) {
           child->parent = temp->parent;
       }

       // Update temp's parent
       if (temp->parent == nullptr) {
           root = child;
       } else if (temp == temp->parent->left) {
           temp->parent->left = child;
       } else {
           temp->parent->right = child;
       }

       // If temp is the successor (case 2), copy temp's data to node
       if (temp != node) {
           node->data.~value_type();
           new (&node->data) value_type(temp->data);
       }

       // Delete temp
       delete temp;
       size_--;

       // Now, rebalance the tree starting from temp's parent
       Node* current = (temp->parent != nullptr) ? temp->parent : child;
       while (current != nullptr) {
           // Update height
           current->height = 1 + std::max(getHeight(current->left), getHeight(current->right));
           int balance = getBalance(current);

           // Balance the tree
           Node* nextParent = current->parent;

           if (balance > 1 && getBalance(current->left) >= 0) {
               if (current->parent == nullptr) {
                   root = rotateRight(current);
               } else if (current == current->parent->left) {
                   current->parent->left = rotateRight(current);
               } else {
                   current->parent->right = rotateRight(current);
               }
           } else if (balance > 1 && getBalance(current->left) < 0) {
               current->left = rotateLeft(current->left);
               if (current->parent == nullptr) {
                   root = rotateRight(current);
               } else if (current == current->parent->left) {
                   current->parent->left = rotateRight(current);
               } else {
                   current->parent->right = rotateRight(current);
               }
           } else if (balance < -1 && getBalance(current->right) <= 0) {
               if (current->parent == nullptr) {
                   root = rotateLeft(current);
               } else if (current == current->parent->left) {
                   current->parent->left = rotateLeft(current);
               } else {
                   current->parent->right = rotateLeft(current);
               }
           } else if (balance < -1 && getBalance(current->right) > 0) {
               current->right = rotateRight(current->right);
               if (current->parent == nullptr) {
                   root = rotateLeft(current);
               } else if (current == current->parent->left) {
                   current->parent->left = rotateLeft(current);
               } else {
                   current->parent->right = rotateLeft(current);
               }
           }

           current = nextParent;
       }
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
   /**
  * see BidirectionalIterator at CppReference for help.
  *
  * if there is anything wrong throw invalid_iterator.
  *     like it = map.begin(); --it;
  *       or it = map.end(); ++end();
    */
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

       /**
    * TODO iter++
        */
       iterator operator++(int) {
           iterator temp = *this;
           ++(*this);
           return temp;
       }

       /**
    * TODO ++iter
        */
       iterator &operator++() {
           if (node == nullptr) {
               // end() iterator, can't increment
               throw invalid_iterator();
           }
           // Find successor
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

       /**
    * TODO iter--
        */
       iterator operator--(int) {
           iterator temp = *this;
           --(*this);
           return temp;
       }

       /**
    * TODO --iter
        */
       iterator &operator--() {
           if (node == nullptr) {
               // end() iterator, decrement to last element
               if (container->root == nullptr) {
                   throw invalid_iterator();
               }
               node = container->findMax(container->root);
               return *this;
           }
           // Find predecessor
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

       /**
    * a operator to check whether two iterators are same (pointing to the same memory).
        */
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

       /**
    * some other operator for iterator.
        */
       bool operator!=(const iterator &rhs) const {
           return !(*this == rhs);
       }

       bool operator!=(const const_iterator &rhs) const {
           return !(*this == rhs);
       }

       /**
    * for the support of it->first.
    * See <http://kelvinh.github.io/blog/2013/11/20/overloading-of-member-access-operator-dash-greater-than-symbol-in-cpp/> for help.
        */
       value_type *operator->() const
           noexcept {
           return &(node->data);
       }
   };
   class const_iterator {
       // it should has similar member method as iterator.
       //  and it should be able to construct from an iterator.
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

   /**
  * TODO two constructors
    */
   map() : root(nullptr), size_(0), comp(Compare()) {}

   map(const map &other) : root(nullptr), size_(0), comp(other.comp) {
       root = copyTree(other.root, nullptr);
       size_ = other.size_;
   }

   /**
  * TODO assignment operator
    */
   map &operator=(const map &other) {
       if (this != &other) {
           clear();
           root = copyTree(other.root, nullptr);
           size_ = other.size_;
           comp = other.comp;
       }
       return *this;
   }

   /**
  * TODO Destructors
    */
   ~map() {
       clear();
   }

   /**
  * TODO
  * access specified element with bounds checking
  * Returns a reference to the mapped value of the element with key equivalent to key.
  * If no such element exists, an exception of type `index_out_of_bound'
    */
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

   /**
  * TODO
  * access specified element
  * Returns a reference to the value that is mapped to a key equivalent to key,
  *   performing an insertion if such key does not already exist.
    */
   T &operator[](const Key &key) {
       Node* node = findNode(root, key);
       if (node != nullptr) {
           return node->data.second;
       }
       auto result = insert(value_type(key, T()));
       return result.first->second;
   }

   /**
  * behave like at() throw index_out_of_bound if such key does not exist.
    */
   const T &operator[](const Key &key) const {
       return at(key);
   }

   /**
  * return a iterator to the beginning
    */
   iterator begin() {
       return iterator(findMin(root), this);
   }

   const_iterator cbegin() const {
       return const_iterator(findMin(root), this);
   }

   /**
  * return a iterator to the end
  * in fact, it returns past-the-end.
    */
   iterator end() {
       return iterator(nullptr, this);
   }

   const_iterator cend() const {
       return const_iterator(nullptr, this);
   }

   /**
  * checks whether the container is empty
  * return true if empty, otherwise false.
    */
   bool empty() const {
       return size_ == 0;
   }

   /**
  * returns the number of elements.
    */
   size_t size() const {
       return size_;
   }

   /**
  * clears the contents
    */
   void clear() {
       deleteTree(root);
       root = nullptr;
       size_ = 0;
   }

   /**
  * insert an element.
  * return a pair, the first of the pair is
  *   the iterator to the new element (or the element that prevented the insertion),
  *   the second one is true if insert successfully, or false.
    */
   pair<iterator, bool> insert(const value_type &value) {
       Node* insertedNode = nullptr;
       bool success = false;
       root = insertNode(root, value, nullptr, insertedNode, success);
       return pair<iterator, bool>(iterator(insertedNode, this), success);
   }

   /**
  * erase the element at pos.
  *
  * throw if pos pointed to a bad element (pos == this->end() || pos points an element out of this)
    */
   void erase(iterator pos) {
       if (pos.node == nullptr || pos.container != this) {
           throw invalid_iterator();
       }
       deleteNodeByPointer(pos.node);
   }

   /**
  * Returns the number of elements with key
  *   that compares equivalent to the specified argument,
  *   which is either 1 or 0
  *     since this container does not allow duplicates.
  * The default method of check the equivalence is !(a < b || b > a)
    */
   size_t count(const Key &key) const {
       return (findNode(root, key) != nullptr) ? 1 : 0;
   }

   /**
  * Finds an element with key equivalent to key.
  * key value of the element to search for.
  * Iterator to an element with key equivalent to key.
  *   If no such element is found, past-the-end (see end()) iterator is returned.
    */
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

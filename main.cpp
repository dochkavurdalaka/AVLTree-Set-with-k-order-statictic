#include <algorithm>
#include <functional>
#include <iostream>
#include <stack>
#include <vector>

template <class Type>
class AVLTree {
    std::function<bool(Type, Type)> cmp_ =
        [](const Type& one, const Type& two) { return one < two; };

    struct Node {
        Type key = Type{};
        Node* left = nullptr;
        Node* right = nullptr;
        int height = 0;
        size_t weight = 1;
    };

    Node* root_ = nullptr;

    bool Contains(Node* node, const Type& key) const {
        if (node == nullptr) {
            return false;
        }
        if (cmp_(key, node->key)) {
            return Contains(node->left, key);
        }
        if (cmp_(node->key, key)) {
            return Contains(node->right, key);
        }
        return true;
    }

    int Height(Node* node) const {
        if (node == nullptr) {
            return -1;
        }
        return node->height;
    }

    size_t Weight(Node* node) const {
        if (node == nullptr) {
            return 0;
        }
        return node->weight;
    }

    int Diff(Node* node) { return Height(node->left) - Height(node->right); }

    void LeftRotate(Node*& node) {
        Node* root_subtree = node;
        node = node->right;
        root_subtree->right = node->left;
        node->left = root_subtree;
        node->left->height =
            std::max(Height(node->left->left), Height(node->left->right)) + 1;
        node->height = std::max(Height(node->left), Height(node->right)) + 1;

        node->left->weight =
            Weight(node->left->left) + Weight(node->left->right) + 1;
        node->weight = Weight(node->left) + Weight(node->right) + 1;
    }

    void RightRotate(Node*& node) {
        Node* root_subtree = node;
        node = node->left;
        root_subtree->left = node->right;
        node->right = root_subtree;
        node->right->height =
            std::max(Height(node->right->left), Height(node->right->right)) + 1;
        node->height = std::max(Height(node->left), Height(node->right)) + 1;

        node->right->weight =
            Weight(node->right->left) + Weight(node->right->right) + 1;
        node->weight = Weight(node->left) + Weight(node->right) + 1;
    }

    void CreateIterator(Node* node, const Type& key, std::stack<Node*>* stack) {
        stack->push(node);
        if (cmp_(key, node->key)) {
            CreateIterator(node->left, key, stack);
            return;
        }
        if (cmp_(node->key, key)) {
            CreateIterator(node->right, key, stack);
            return;
        }
    }

    void Insert(Node*& node, const Type& key) {
        if (node == nullptr) {
            node = new Node();
            node->key = key;
            return;
        }
        if (cmp_(key, node->key)) {
            Insert(node->left, key);
            node->height =
                std::max(Height(node->left), Height(node->right)) + 1;
            node->weight = Weight(node->left) + Weight(node->right) + 1;
            int diff_b = Diff(node);
            if (diff_b == 2) {
                int diff_a = Diff(node->left);
                if (diff_a == 0 or diff_a == 1) {
                    RightRotate(node);
                } else {
                    LeftRotate(node->left);
                    RightRotate(node);
                }
            }
            return;
        }
        if (cmp_(node->key, key)) {
            Insert(node->right, key);
            node->height =
                std::max(Height(node->left), Height(node->right)) + 1;
            node->weight = Weight(node->left) + Weight(node->right) + 1;
            int diff_a = Diff(node);
            if (diff_a == -2) {
                int diff_b = Diff(node->right);
                if (diff_b == 0 or diff_b == -1) {
                    LeftRotate(node);
                } else {
                    RightRotate(node->right);
                    LeftRotate(node);
                }
            }
            return;
        }
    }

    Node* WorkWithInorderSucessor(Node*& node) {
        if (node->left != nullptr) {
            Node* node_to_return = WorkWithInorderSucessor(node->left);
            node->height =
                std::max(Height(node->left), Height(node->right)) + 1;
            node->weight = Weight(node->left) + Weight(node->right) + 1;
            int diff_a = Diff(node);
            if (diff_a == -2) {
                int diff_b = Diff(node->right);
                if (diff_b == 0 or diff_b == -1) {
                    LeftRotate(node);
                } else {
                    RightRotate(node->right);
                    LeftRotate(node);
                }
            }
            return node_to_return;
        }
        Node* node_to_return = node;
        node = node->right;
        return node_to_return;
    }

    bool ReduceComplexivity(Node*& node) {
        if (node->left == nullptr and node->right == nullptr) {
            delete node;
            node = nullptr;
            return true;
        }
        if (node->left == nullptr) {
            Node* node_right = node->right;
            delete node;
            node = node_right;
            return true;
        }
        if (node->right == nullptr) {
            Node* node_left = node->left;
            delete node;
            node = node_left;
            return true;
        }

        return false;
    }

    void Remove(Node*& node, const Type& key) {
        if (node == nullptr) {
            return;
        }
        if (cmp_(key, node->key)) {
            Remove(node->left, key);
            node->height =
                std::max(Height(node->left), Height(node->right)) + 1;
            node->weight = Weight(node->left) + Weight(node->right) + 1;
            int diff_a = Diff(node);
            if (diff_a == -2) {
                int diff_b = Diff(node->right);
                if (diff_b == 0 or diff_b == -1) {
                    LeftRotate(node);
                } else {
                    RightRotate(node->right);
                    LeftRotate(node);
                }
            }
            return;
        }
        if (cmp_(node->key, key)) {
            Remove(node->right, key);
            node->height =
                std::max(Height(node->left), Height(node->right)) + 1;
            node->weight = Weight(node->left) + Weight(node->right) + 1;
            int diff_b = Diff(node);
            if (diff_b == 2) {
                int diff_a = Diff(node->left);
                if (diff_a == 0 or diff_a == 1) {
                    RightRotate(node);
                } else {
                    LeftRotate(node->left);
                    RightRotate(node);
                }
            }
            return;
        }
        if (ReduceComplexivity(node)) {
            return;
        }
        Node* node_returned = WorkWithInorderSucessor(node->right);
        node_returned->left = node->left;
        node_returned->right = node->right;
        delete node;
        node = node_returned;
        node->height = std::max(Height(node->left), Height(node->right)) + 1;
        node->weight = Weight(node->left) + Weight(node->right) + 1;
        int diff_b = Diff(node);
        if (diff_b == 2) {
            int diff_a = Diff(node->left);
            if (diff_a == 0 or diff_a == 1) {
                RightRotate(node);
            } else {
                LeftRotate(node->left);
                RightRotate(node);
            }
        }
    }

    void Print(Node* node) {
        if (node == nullptr) {
            return;
        }

        Print(node->left);
        std::cout << node->key << " ";
        Print(node->right);
    }

    void Destroy(Node* node) {
        if (node == nullptr) {
            return;
        }
        Destroy(node->left);
        Destroy(node->right);
        delete node;
    }

    Type KOrderStatistic(Node* node, size_t order) {
        if (node == nullptr) {
            return Type{};
        }
        size_t weight_left = Weight(node->left);
        if (order == weight_left + 1) {
            return node->key;
        }
        if (order <= weight_left) {
            return KOrderStatistic(node->left, order);
        }
        return KOrderStatistic(node->right, order - (weight_left + 1));
    }

public:
    AVLTree(std::function<bool(Type, Type)> cmp):cmp_(cmp) {
    }

    bool Contains(const Type& key) const { return Contains(root_, key); }

    void Print() { Print(root_); }
    void Remove(const Type& key) { Remove(root_, key); }

    void Clear() {
        Destroy(root_);
        root_ = nullptr;
    }

    bool IsEmpty() const { return root_ == nullptr; }

    size_t Size() const { return Weight(root_); }

    Type KOrderStatistic(size_t order) { return KOrderStatistic(root_, order); }

    ~AVLTree() { Destroy(root_); }

    class Iterator {
        std::stack<Node*> stack_of_nodes_;

    public:
        explicit Iterator(std::stack<Node*> stack_of_nodes)
            : stack_of_nodes_(stack_of_nodes) {}

        const Type& operator*() const { return stack_of_nodes_.top()->key; }

        Iterator& operator++() {
            if (stack_of_nodes_.top()->right != nullptr) {
                stack_of_nodes_.push(stack_of_nodes_.top()->right);
                while (stack_of_nodes_.top()->left != nullptr) {
                    stack_of_nodes_.push(stack_of_nodes_.top()->left);
                }
            } else {
                Node* node = stack_of_nodes_.top();
                stack_of_nodes_.pop();
                while (!stack_of_nodes_.empty() and
                       stack_of_nodes_.top()->right == node) {
                    node = stack_of_nodes_.top();
                    stack_of_nodes_.pop();
                }
            }
            return *this;
        }

        Iterator& operator--() {
            if (stack_of_nodes_.top()->left != nullptr) {
                stack_of_nodes_.push(stack_of_nodes_.top()->left);
                while (stack_of_nodes_.top()->right != nullptr) {
                    stack_of_nodes_.push(stack_of_nodes_.top()->right);
                }
            } else {
                Node* node = stack_of_nodes_.top();
                stack_of_nodes_.pop();
                while (!stack_of_nodes_.empty() and
                       stack_of_nodes_.top()->left == node) {
                    node = stack_of_nodes_.top();
                    stack_of_nodes_.pop();
                }
            }
            return *this;
        }

        bool operator==(const Iterator& other) const {
            return stack_of_nodes_ == other.stack_of_nodes_;
        }

        bool operator!=(const Iterator& other) const {
            return !operator==(other);
        }
    };

    Iterator Insert(const Type& key) {
        std::stack<Node*> stack_of_nodes;
        Insert(root_, key);
        CreateIterator(root_, key, &stack_of_nodes);
        return Iterator(stack_of_nodes);
    }

    Iterator Begin() const {
        std::stack<Node*> stack_of_nodes;
        Node* node = root_;
        while (node != nullptr) {
            stack_of_nodes.push(node);
            node = node->left;
        }
        return Iterator(stack_of_nodes);
    }

    Iterator RBegin() const {
        std::stack<Node*> stack_of_nodes;
        Node* node = root_;
        while (node != nullptr) {
            stack_of_nodes.push(node);
            node = node->right;
        }
        return Iterator(stack_of_nodes);
    }

    Iterator End() const { return Iterator(std::stack<Node*>()); }
};

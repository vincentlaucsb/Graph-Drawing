#include "tree_lp.h"

namespace tree {
    class BinarySearchTree {
        using Node = TreeNode;

    public:
        Node root;
        void insert(std::string data, Node* current) {
            if (data < current->data) {
                if (current->left) current = current->left.get();
                else {
                    current->left = std::make_unique<Node>();
                    current->left->data = data;
                    current = nullptr;
                }
            }
            else if (data > current->data) {
                if (current->right) current = current->right.get();
                else {
                    current->right = std::make_unique<Node>();
                    current->right->data = data;
                    current = nullptr;
                }
            }
            else return;

            if (current) this->insert(data, current);
        }

        void insert(std::string data) {
            insert(data, &root);
        }
    };

    BinarySearchTree make_random_tree(size_t num_items) {
        BinarySearchTree tree;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 100);

        for (int i = 0; i < num_items; i++) {
            int number = dis(gen);
            std::cout << "Inserting " << dis(gen) << std::endl;
            tree.insert(std::to_string(number));
        }
        
        return tree;
    }
}
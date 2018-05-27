#include "tree_lp.h"

namespace tree {
    class BinarySearchTree {
        using Node = TreeNode;

    public:
        Node root;
        void insert(std::string data, Node* current) {
            if (current->data.empty()) {
                current->data = data;
                return;
            }

            if (data < current->data) {
                if (!current->left) current->left = std::make_unique<Node>();
                current = current->left.get();
            }
            else if (data > current->data) {
                if (!current->right) current->right = std::make_unique<Node>();
                current = current->right.get();
            }
            else return;

            insert(data, current);
        }

        void insert(std::string data) {
            insert(data, &root);
        }
    };

    BinarySearchTree make_random_tree(size_t num_items) {
        BinarySearchTree tree;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, (int)num_items * 5);

        for (int i = 0; i < num_items; i++)
            tree.insert(std::to_string(dis(gen)));
        
        return tree;
    }
}
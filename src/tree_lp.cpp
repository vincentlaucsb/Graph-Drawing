#include "tree_lp.h"
#include "bst.hpp"

namespace tree {
    namespace paper {
        // Functions for writing the paper
        TreeNode fig2() {
            /** Replicate figure 2 from RT81 */
            TreeNode root;

            // Left subtree
            auto& d1_left = root.left = std::make_unique<TreeNode>();
            auto& d2_left = d1_left->left = std::make_unique<TreeNode>();
            d1_left->right = std::make_unique<TreeNode>();
            auto& d3_left = d2_left->left = std::make_unique<TreeNode>();
            d2_left->right = std::make_unique<TreeNode>();
            auto& d4_left = d3_left->left = std::make_unique<TreeNode>();
            d3_left->right = std::make_unique<TreeNode>();
            d4_left->left = std::make_unique<TreeNode>();
            auto& d5_right = d4_left->right = std::make_unique<TreeNode>();
            d5_right->left = std::make_unique<TreeNode>();
            auto& d6_right = d5_right->right = std::make_unique<TreeNode>();
            auto& d7_left = d6_right->right = std::make_unique<TreeNode>();
            d7_left->left = std::make_unique<TreeNode>();
            d7_left->right = std::make_unique<TreeNode>();

            // Right subtree
            auto& d1_right = root.right = std::make_unique<TreeNode>();
            auto& d2_right = d1_right->right = std::make_unique<TreeNode>();
            d1_right->left = std::make_unique<TreeNode>();
            auto& d3_right = d2_right->right = std::make_unique<TreeNode>();
            d2_right->left = std::make_unique<TreeNode>();
            auto& d4_right = d3_right->right = std::make_unique<TreeNode>();
            d3_right->left = std::make_unique<TreeNode>();
            d4_right->right = std::make_unique<TreeNode>();
            auto& d5_left = d4_right->left = std::make_unique<TreeNode>();
            d5_left->right = std::make_unique<TreeNode>();
            auto& d6_left = d5_left->left = std::make_unique<TreeNode>();
            auto& d7_right = d6_left->left = std::make_unique<TreeNode>();
            d7_right->left = std::make_unique<TreeNode>();
            d7_right->right = std::make_unique<TreeNode>();

            return root;
        }

        void level_order(TreeNode& root) {
            // Given a tree, label its nodes via a level-order traversal
            std::deque<TreeNode*> children = { &root };
            int id = 1;
            while (!children.empty()) {
                auto current = children.front();
                current->data = std::to_string(id++);

                if (current->left) children.push_back(current->left.get());
                if (current->right) children.push_back(current->right.get());

                children.pop_front();
            }
        }

        void preorder(TreeNode& root) {
            // Given a tree, label its nodes via a preorder traversal
            int id = 1;
            preorder(root, id);
        }

        void preorder(TreeNode& root, int& id) {
            // Given a tree, label its nodes via a preorder traversal
            root.data = std::to_string(id++);
            if (root.left) preorder(*(root.left.get()), id);
            if (root.right) preorder(*(root.right.get()), id);
        }
    }

    size_t TreeNode::height() {
        size_t l_height = 0, r_height = 0;
        if (left) l_height = left->height();
        if (right) r_height = right->height();
        return std::max(l_height + 1, r_height + 1);
    }

    size_t TreeNode::size() {
        // Return the number of nodes in the tree
        size_t nodes = 1; // Count self
        if (this->left) nodes += left->size();
        if (this->right) nodes += right->size();
        return nodes;
    }

    double factorial(const int n) {
        // Calculate n!
        int ret = 1;
        for (int i = 1; i <= n; i++) ret *= i;
        return ret;
    }

    int binary_trees(int n) {
        // Return the number of binary trees with n nodes
        return (factorial(2 * n) / pow(factorial(n), 2)) // 2n choose n
            / (double)(n + 1);
    }

    int g_jn(int j, int n) {
        // Return the number of binary trees with n nodes whose left-subtree has j nodes
        if (n == 0) {
            if (j == 0) return 1;
            else return 0;
        }

        return binary_trees(j) * binary_trees(n - j - 1);
    }

    int rank(TreeNode* tree, RankMap* cache) {
        // Calculate the rank of a tree
        // If cache is not null, also save computations there

        int ret = 0;
        if (!tree) ret = 1;
        else {
            int right_tree_size = tree->right ? (int)tree->right->size() : 0;
            ret = (binary_trees(right_tree_size)
                * (rank(tree->left.get(), cache) - 1))
                + rank(tree->right.get(), cache);

            for (int j = 0; tree->left && j < tree->left->size(); j++)
                ret += g_jn(j, (int)tree->size());
        }

        if (cache && tree && tree->size() > 1
            && ret > 0 // Why is rank = 0 in some cases?!?
        ) (*cache)[(int)tree->size()][ret].push_back(tree);
        return ret;
    }

    namespace helpers {
        void perfect_tree_helper(TreeNode& node, int height) {
            // Create a full binary tree of height h
            node.left = std::make_unique<TreeNode>();
            node.right = std::make_unique<TreeNode>();

            if (height) {
                perfect_tree_helper(*(node.left.get()), height - 1);
                perfect_tree_helper(*(node.right.get()), height - 1);
            }
        };

        TreeNode IncompleteBinaryTree::make_tree(int depth) {
            TreeNode tree;
            make_tree(tree, depth);
            return tree;
        }

        void IncompleteBinaryTree::make_tree(TreeNode& tree, int depth) {
            // Generate an incomplete binary tree recursively
            if (depth) {
                auto lchance = distribution(generator), rchance = distribution(generator);
                if (lchance > 0.5) tree.left = std::make_unique<TreeNode>();
                if (rchance > 0.5) tree.right = std::make_unique<TreeNode>();
                if (tree.left) this->make_tree(*(tree.left.get()), depth - 1);
                if (tree.right) this->make_tree(*(tree.right.get()), depth - 1);
            }
        }
    }

    TreeNode perfect_tree(int height) {
        TreeNode root;
        if (height) helpers::perfect_tree_helper(root, height - 1);
        return root;
    }

    TreeNode incomplete_tree(int height) {
        helpers::IncompleteBinaryTree tree_maker;
        while (true) {
            auto tree = tree_maker.make_tree(height);
            if (tree.height() >= height) return tree;
        }
    }

    SVG::SVG draw_tree(glp_prob* P, LevelMap& level) {
        // Given a solved linear program and the corresponding tree, draw it
        const double circle_radius = 15;

        SVG::SVG root;
        auto edges = root.add_child<SVG::Group>(),
            nodes = root.add_child<SVG::Group>(),
            text_labels = root.add_child<SVG::Group>();

        root.style("circle")
            .set_attr("stroke", "#000000")
            .set_attr("fill", "#ffffff");
        root.style("text")
            .set_attr("font-family", "sans-serif")
            .set_attr("font-size", "10pt")
            .set_attr("dominant-baseline", "central")
            .set_attr("text-anchor", "middle");
        root.style("line")
            .set_attr("stroke", "#000000");

        size_t current_level = 0, current_level_index = 0;
        const double scaling = 50;

        // Associated tree pointers with circle objects
        std::unordered_map<TreeNode*, SVG::Circle*> vertices;

        // Add vertices
        for (int i = 3; i <= glp_get_num_cols(P); i++) {
            if (current_level_index >= level[current_level].size()) {
                current_level++;
                current_level_index = 0;
            }

            auto cur_node = level[current_level][current_level_index];
            auto cur_vertex = vertices[cur_node] = nodes->add_child<SVG::Circle>(
                glp_get_col_prim(P, i) * scaling, // x-value
                (double)current_level * scaling,  // y-value
                circle_radius);

            current_level_index++;

            // Add text labels
            *text_labels << SVG::Text(*cur_vertex, cur_node->data);
        }

        // Add edges
        for (auto& elem : vertices) {
            // Left
            if (elem.first->left) edges->add_child<SVG::Line>(
                *(vertices[elem.first]),
                *(vertices[elem.first->left.get()])
                );

            // Right
            if (elem.first->right) edges->add_child<SVG::Line>(
                *(vertices[elem.first]),
                *(vertices[elem.first->right.get()])
                );
        }

        return root;
    }

    std::pair<glp_prob*, LevelMap> map_tree(TreeNode& root, const TreeOptions& options) {
        LevelMap levels; // Keep track of all nodes on a given level

        // Assign IDs (can't calculate any constraints before specifying IDs)
        using Level = int;
        using NodeInfo = std::pair<Level, TreeNode*>; // Level of node + node itself
        std::deque<NodeInfo> children = { std::make_pair(0, &root) };

        // Can't assign to 0; 1 = X, 2 = x
        int current_id = 3, current_row = 1, current_level = 0;

        // Information for constraint matrix
        int num_nodes = 0, left_sons = 0, right_sons = 0;

        while (!children.empty()) {
            NodeInfo current = children.front();
            auto& current_node = current.second;
            current_level = current.first;
            num_nodes++;

            current_node->id = current_id; // Assign ID to node
            if (current_node->left) {
                left_sons++;
                children.push_back(std::make_pair(current_level + 1, current_node->left.get()));
            }
            if (current_node->right) {
                right_sons++;
                children.push_back(std::make_pair(current_level + 1, current_node->right.get()));
            }

            // Update mapping of levels to nodes
            levels[current_level].push_back(current_node);

            // Loop update
            children.pop_front();
            current_id++;
        }

        // Calculate number of constraints
        int num_constraints = num_nodes * 2       // Width constraint
            + left_sons + right_sons              // 2nd constraint
            + std::min(left_sons, right_sons)     // Equal separation
            + 1;                                  // Edge case: Tree of height 0


        // Add constraint for each adjacent pair of nodes (aesthetic 3)
        for (auto& l : levels)
            if (l.second.size() > 1) num_constraints += l.second.size() - 1;

        // Add constraints for isomorphic trees (aesthetic 6)
        RankMap cache;
        rank(&root, &cache);

        for (auto& node_size : cache) {
            for (auto& nodes : node_size.second)
                num_constraints += (nodes.second.size() - 1);
        }

        glp_prob *P;
        P = glp_create_prob();

        // Variable for each node + X, x
        glp_add_cols(P, num_nodes + 2);
        for (int i = 1; i <= num_nodes + 2; i++) glp_set_col_bnds(P, i, GLP_FR, NULL, NULL); // Make columns unbounded
        glp_add_rows(P, num_constraints);

        // Set objective function
        glp_set_obj_dir(P, GLP_MIN);
        glp_set_obj_coef(P, 1, 1);  // X
        glp_set_obj_coef(P, 2, -1); // -x

        // Counts for constraints
        int width_aux_count = 0, aes2_count = 0, aes3_count = 0, aes4_count = 0, aes6_count = 0;

        for (auto& l : levels) {
            current_level = l.first;

            for (auto& node : l.second) {
                // Add width constraints: indices refer to X, current_id
                current_id = node->id;
                const int ind_up[] = { NULL, 1, current_id },
                    ind_down[] = { NULL, 2, current_id };
                const double val[] = { NULL, -1, 1 };
                glp_set_mat_row(P,
                    current_row++,  // Index of constraint
                    2,              // Number of values
                    &(ind_up[0]),   // Indices of values
                    &(val[0])       // Values
                );
                glp_set_row_bnds(P, current_row, GLP_LO, 0, NULL);
                glp_set_row_name(P, current_row,
                    ("Width auxiliary variable " + std::to_string(++width_aux_count)).c_str()
                );

                glp_set_mat_row(P,
                    current_row++,   // Index of constraint
                    2,               // Number of values
                    &(ind_down[0]),  // Indices of values
                    &(val[0])        // Values
                );
                glp_set_row_bnds(P, current_row, GLP_UP, NULL, 0);
                glp_set_row_name(P, current_row,
                    ("Width auxiliary variable " + std::to_string(++width_aux_count)).c_str()
                );

                // Calculate 2nd constraint (all left children strictly left of parent)
                if (node->left) {
                    const int ind[] = { NULL, node->id, node->left->id };
                    const double val[] = { NULL, 1, -1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        2,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_name(P, current_row,
                        ("Aesthetic 2 (left children) " + std::to_string(++aes2_count)).c_str()
                    );
                    glp_set_row_bnds(P, current_row++, GLP_LO, 1, 0);
                }

                // All right children strictly right of parent
                if (node->right) {
                    const int ind[] = { NULL, node->id, node->right->id };
                    const double val[] = { NULL, -1, 1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        2,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_name(P, current_row,
                        ("Aesthetic 2 (right children) " + std::to_string(++aes2_count)).c_str()
                    );
                    glp_set_row_bnds(P, current_row++, GLP_LO, 1, 0);
                }

                // Equal separation (parent centered over child)
                if (node->left && node->right) {
                    const int ind[] = { NULL, node->id, node->left->id, node->right->id };
                    const double val[] = { NULL, -2, 1, 1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        3,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_name(P, current_row,
                        ("Aesthetic 4 " + std::to_string(++aes4_count)).c_str()
                    );
                    glp_set_row_bnds(P, current_row++, GLP_FX, 0, 0);
                }
            }

            // Calculate separation constraints
            if (l.second.size() - 1) {
                int first_id = l.second.front()->id, second_id = l.second.back()->id;
                for (int i = first_id; i < second_id; i++) {
                    const int ind[] = { NULL, i, i + 1 };
                    const double val[] = { NULL, -1, 1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        2,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_name(P, current_row,
                        ("Aesthetic 3 " + std::to_string(++aes3_count)).c_str()
                    );
                    glp_set_row_bnds(P, current_row++, GLP_LO, 2, NULL);
                }
            }
        }

        // Sixth aesthetic
        if (options.aes6) {
            for (auto& node_size : cache) {
                for (auto& nodes : node_size.second) {
                    for (size_t i = 0; (i + 1) < nodes.second.size(); i++) {
                        auto& r1 = nodes.second[i], r2 = nodes.second[i + 1];

                        if (r1->right) {
                            const int ind[] = { NULL,
                                r1->right->id, r1->id,  r2->right->id, r2->id
                            };
                            const double val[] = { NULL, 1, -1, -1, 1 };

                            glp_set_mat_row(P,
                                current_row, // Index of constraint
                                4,           // Number of values
                                &(ind[0]),   // Indices of values
                                &(val[0])    // Values
                            );
                            glp_set_row_name(P, current_row,
                                ("Aesthetic 6 " + std::to_string(++aes6_count)).c_str()
                            );
                            glp_set_row_bnds(P, current_row++, GLP_FX, 0, 0);
                        }
                        else {
                            const int ind[] = { NULL,
                                r1->left->id, r1->id,  r2->left->id, r2->id
                            };
                            const double val[] = { NULL, 1, -1, -1, 1 };

                            glp_set_mat_row(P,
                                current_row, // Index of constraint
                                4,           // Number of values
                                &(ind[0]),   // Indices of values
                                &(val[0])    // Values
                            );
                            glp_set_row_name(P, current_row,
                                ("Aesthetic 6 " + std::to_string(++aes6_count)).c_str()
                            );
                            glp_set_row_bnds(P, current_row++, GLP_FX, 0, 0);
                        }
                    }
                }
            }
        }

        // Write model to file
        if (!options.filename.empty()) glp_write_lp(P, NULL, options.filename.c_str());

        glp_simplex(P, NULL); // Solve problem with default settings
        return std::make_pair(P, levels);
    }
}

int main(int argc, char** argv) {
    using namespace tree;
    cxxopts::Options options(argv[0], "Draw a full tree of height n");
    options.positional_help("[output file] [n]");
    options.add_options("required")
        ("f,file", "output file", cxxopts::value<std::string>())
        ("n,num", "n", cxxopts::value<int>());
    options.add_options("optional")
        ("b,bst", "Produce a random binary search tree with n items")
        ("i,incomp", "Produce an incomplete tree of height n")
        ("c,cplex", "Save model in CPLEX format to text file", cxxopts::value<std::string>()->default_value(""))
        ("l,level", "Illustrate a level order traversal on a perfect tree of height n")
        ("p,preorder", "Illustrate a preorder traversal on a perfect tree of height n");
    options.parse_positional({ "file", "num" });

    if (argc < 3) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    try {
        auto result = options.parse(argc, (const char**&)argv);

        std::string file = result["file"].as<std::string>();
        std::string cplex = result["cplex"].as<std::string>();
        int number = result["num"].as<int>();
        bool bst = result["bst"].as<bool>(),
            incomp = result["incomp"].as<bool>(),
            level = result["level"].as<bool>(),
            preorder = result["preorder"].as<bool>();

        TreeNode root;
        if (incomp) root = incomplete_tree(number);
        else if (bst) root = make_random_tree(number).root;
        else root = perfect_tree(number);

        // Label nodes
        if (level) paper::level_order(root);
        if (preorder) paper::preorder(root);

        auto mapping = map_tree(root, { true, cplex }),
            mapping_noaes6 = map_tree(root, { false, "" });
        SVG::SVG drawing = draw_tree(mapping.first, mapping.second);
        SVG::SVG drawing_noaes6 = draw_tree(mapping_noaes6.first, mapping_noaes6.second);
        drawing.autoscale();
        drawing_noaes6.autoscale();

        std::ofstream outfile(file);
        std::ofstream outfile2("noaes6_" + file);
        outfile << std::string(drawing);
        outfile2 << std::string(drawing_noaes6);
    }
    catch (cxxopts::OptionException&) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    return 0;
}
/* eof */

#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <deque>
#include <iostream>
// #include <chrono>
#include "cxxopts.hpp"
#include "svg.hpp"
#include "glpk.h"

namespace tree {
    struct TreeNode {
        int id;
        std::unique_ptr<TreeNode> left = nullptr;
        std::unique_ptr<TreeNode> right = nullptr;
    };

    // Map level numbers to lists of nodes at that level
    using LevelMap = std::unordered_map<int, std::vector<TreeNode*>>;

    void full_tree(TreeNode& root, int height) {
        // Create a full binary tree of height h
        root.left = std::make_unique<TreeNode>();
        root.right = std::make_unique<TreeNode>();

        if (height) {
            full_tree(*(root.left.get()), height - 1);
            full_tree(*(root.right.get()), height - 1);
        }
    }

    SVG::SVG draw_tree(glp_prob* P, LevelMap& level) {
        // Given a solved linear program and the corresponding tree, draw it
        SVG::SVG root;
        root.style("line").set_attr("stroke", "#000000");

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

            vertices[level[current_level][current_level_index]] = root.add_child<SVG::Circle>(
                glp_get_col_prim(P, i) * scaling, // x-value
                (double)current_level * scaling,  // y-value
                10 // circle radius
                );

            current_level_index++;
        }

        // Add edges
        for (auto& elem : vertices) {
            // Left
            if (elem.first->left) root.add_child<SVG::Line>(
                *(vertices[elem.first]),
                *(vertices[elem.first->left.get()])
                );

            // Right
            if (elem.first->right) root.add_child<SVG::Line>(
                *(vertices[elem.first]),
                *(vertices[elem.first->right.get()])
                );
        }

        return root;
    }

    std::pair<glp_prob*, LevelMap> map_tree(TreeNode& root) {
        LevelMap levels; // Keep track of all nodes on a given level

        // Assign IDs
        // First item of pair = parent of node, second item = node
        using NodeInfo = std::pair<TreeNode*, TreeNode*>;
        std::deque<NodeInfo> children = { std::make_pair(nullptr, &root) };

        // Can't assign to 0; 1 = X, 2 = x
        int current_id = 3, current_row = 1, current_level = 0;

        // Information for constraint matrix
        int num_nodes = 0, left_sons = 0, right_sons = 0;

        while (!children.empty()) {
            NodeInfo current = children.front();
            auto& current_node = current.second;
            num_nodes++;

            if ((current_level > 0)) {
                if (std::find(levels[current_level - 1].begin(), levels[current_level - 1].end(), current.first) == levels[current_level - 1].end()) {
                    current_level++;
                }
            }

            current_node->id = current_id;
            if (current_node->left) {
                left_sons++;
                children.push_back(std::make_pair(current_node, current_node->left.get()));
            }
            if (current_node->right) {
                right_sons++;
                children.push_back(std::make_pair(current_node, current_node->right.get()));
            }

            // Update mapping of levels to nodes
            levels[current_level].push_back(current_node);

            // Loop update
            children.pop_front();
            current_id++;
            if (current_level == 0) current_level++;
        }

        int num_constraints = num_nodes * 2 // Width constraint
            + left_sons + right_sons  // 2nd constraint
            + std::min(left_sons, right_sons); // Equal separation

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

                                    // Add width constraints: indices refer to X, current_id
        for (auto& l : levels) {
            current_level = l.first;

            for (auto& elem : l.second) {
                current_id = elem->id;
                const int ind_up[] = { NULL, 1, current_id },
                    ind_down[] = { NULL, 2, current_id };
                const double val[] = { NULL, -1, 1 };
                glp_set_mat_row(P,
                    current_row,    // Index of constraint
                    2,              // Number of values
                    &(ind_up[0]),   // Indices of values
                    &(val[0])       // Values
                );
                current_row++;
                glp_set_row_bnds(P, current_row, GLP_LO, 0, NULL);

                glp_set_mat_row(P,
                    current_row,     // Index of constraint
                    2,               // Number of values
                    &(ind_down[0]),  // Indices of values
                    &(val[0])        // Values
                );

                current_row++;
                glp_set_row_bnds(P, current_row, GLP_UP, NULL, 0);
            }
        }

        // Calculate 2nd constraint + equal separation constraints
        for (auto& l : levels) {
            for (auto& node : l.second) {
                if (node->left) {
                    const int ind[] = { NULL, node->id, node->left->id };
                    const double val[] = { NULL, 1, -1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        2,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_bnds(P, current_row, GLP_LO, 1, 0);
                    current_row++;
                }

                if (node->right) {
                    const int ind[] = { NULL, node->id, node->right->id };
                    const double val[] = { NULL, -1, 1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        2,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_bnds(P, current_row, GLP_LO, 1, 0);
                    current_row++;
                }

                // Equal separation
                if (node->left && node->right) {
                    const int ind[] = { NULL, node->id, node->left->id, node->right->id };
                    const double val[] = { NULL, -2, 1, 1 };

                    glp_set_mat_row(P,
                        current_row, // Index of constraint
                        3,           // Number of values
                        &(ind[0]),   // Indices of values
                        &(val[0])    // Values
                    );
                    glp_set_row_bnds(P, current_row, GLP_FX, 0, 0);
                    current_row++;
                }
            }
        }

        // Calculate separation constraints
        for (auto& l : levels) {
            // Add correct number of rows
            if (l.second.size() - 1) {
                glp_add_rows(P, l.second.size() - 1); // For each adjacent pair of nodes
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
                    glp_set_row_bnds(P, current_row, GLP_LO, 2, NULL);

                    current_row++;
                }
            }
        }

        glp_simplex(P, NULL); // Solve problem with default settings
        return std::make_pair(P, levels);
    }
}

int main(int argc, char** argv) {
    using namespace tree;
    cxxopts::Options options(argv[0], "Produces a full tree of specified depth");
    options.positional_help("[output file] [depth]");
    options.add_options("required")
        ("f,file", "output file", cxxopts::value<std::string>())
        ("d,depth", "depth", cxxopts::value<int>());
    options.parse_positional({ "file", "depth" });

    if (argc < 3) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    try {
        auto result = options.parse(argc, (const char**&)argv);

        std::string file = result["file"].as<std::string>();
        int depth = result["depth"].as<int>();

        TreeNode root;
        full_tree(root, depth);

        auto mapping = map_tree(root);
        SVG::SVG drawing = draw_tree(mapping.first, mapping.second);
        drawing.autoscale();

        std::ofstream outfile(file);
        outfile << std::string(drawing);
    }
    catch (cxxopts::OptionException&) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    return 0;
}

/* eof */

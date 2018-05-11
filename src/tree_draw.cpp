// This code contains routines for building and drawing trees

#include "tree.h"
#include <fstream>

namespace tree {
    namespace helpers {
        void binary_tree(TreeNode* tree, int depth) {
            /** A helper function that creates a complete binary tree recursively */
            TreeNode *left, *right;

            if (depth) {
                left = tree->add_left();
                right = tree->add_right();
                binary_tree(left, depth - 1);
                binary_tree(right, depth - 1);
            }
        }

        /*
        void nary_tree(NaryTreeNode* tree, int n, int depth) {
            ** A helper function that creates a complete nary tree recursively *
            NaryTreeNode* child;

            if (depth) {
                for (int i = 0; i < n; i++) {
                    child = tree->add_child();
                    nary_tree(child, n, depth - 1);
                }
            }
        }*/

        TreeDraw::TreeDraw(SVG::SVG* _tree_svg, const DrawOpts& _options) :
            tree_svg(_tree_svg), options(_options) {
            // Text styling
            tree_svg->style("text").set_attr("text-anchor", "middle")
                .set_attr("font-family", "sans-serif")
                .set_attr("font-size", "12px");

            // Thread styling
            tree_svg->style("line.thread").set_attr("stroke-dasharray", "5,5");
            if (!_options.show_threads)
                tree_svg->style("line.thread").set_attr("display", "none");

            // Edge and vertex styling
            edges = tree_svg->add_child<SVG::Group>();
            vertices = tree_svg->add_child<SVG::Group>();
            vertices->set_attr("class", "vertices");
            edges->set_attr("class", "edges")
                .set_attr("stroke", options.edge_color)
                .set_attr("stroke-width", options.edge_width);
        }

        void TreeDraw::draw_tree(TreeBase& tree) {
            /** Draw a SVG tree recursively */
            vertices->add_child<SVG::Circle>(tree.x, tree.y, options.node_size);

            auto children = tree.get_children();
            for (size_t i = 0; i < children.size(); i++) {
                auto& child = children[i];
                bool is_left = (i == 0), is_right = ((i + 1) == children.size());

                if (options.disp_label) {
                    child->label = SVG::to_string(child->displacement);
                }
                
                auto edge = edges->add_child<SVG::Line>(tree.x, child->x, tree.y, child->y);
                if (tree.thread_l == child || tree.thread_r == child) edge->set_attr("class", "thread");
                else this->draw_tree(*child); // Recursion

                // Add text labels
                if (!child->label.empty()) { // Hang from vertex
                    if (child->is_leaf()) {
                        auto label = vertices->add_child<SVG::Text>(edge->x2(), edge->y2(), child->label);
                        label->set_attr("transform") <<
                            "translate(" << 0 << "," << 20 << "), " <<
                            "rotate(" << 90 << "," << edge->x2() << "," << edge->y2() << ")";
                    }
                    else { // Place along edge
                        auto label = vertices->add_child<SVG::Text>(edge->x2(), edge->y2() - 10, child->label);
                        label->set_attr("transform") << "rotate(" << edge->angle() <<
                            "," << edge->x2() << "," << edge->y2() << ")";
                    }
                }
            }
        }
    }

    void update_map_right(std::map<double, SVG::Circle*>& contour, SVG::Circle* node);
    void update_map_left(std::map<double, SVG::Circle*>& contour, SVG::Circle* node);
    void highlight_contour(SVG::SVG& root, std::function<void(std::map<double, SVG::Circle*>&, SVG::Circle*)> func);

    void update_map_left(std::map<double, SVG::Circle*>& contour, SVG::Circle* node) {
        if ((contour.find(node->y()) == contour.end()) || (node->x() < contour[node->y()]->x()))
            contour[node->y()] = node;
    }

    void update_map_right(std::map<double, SVG::Circle*>& contour, SVG::Circle* node) {
        if ((contour.find(node->y()) == contour.end()) || (node->x() > contour[node->y()]->x()))
            contour[node->y()] = node;
    };

    void highlight_contour(SVG::SVG& root,
        std::function<void(std::map<double, SVG::Circle*>&, SVG::Circle*)> func) {
        /** Given a drawn tree, highlight the left or right contour */
        auto nodes = root.get_children<SVG::Circle>();

        // For every layer in the drawing (y-position), find left/right-most vertex
        std::map<double, SVG::Circle*> contour;
        for (auto& node : nodes) func(contour, node);

        // Create boundary for highlighted nodes
        for (auto& circ : contour) circ.second->set_attr("class", "contour");
        root.style("circle.contour").set_attr("fill", "red");
    }

    void left_contour(SVG::SVG& root) {
        /** Given a drawn tree, highlight the left contour */
        auto func = &update_map_left;
        highlight_contour(root, func);
    }

    void right_contour(SVG::SVG& root) {
        /** Given a drawn tree, highlight the right contour */
        auto func = &update_map_right;
        highlight_contour(root, func);
    }

    TreeNode binary_tree(int height) {
        // Create a complete binary tree of specified height
        TreeNode root;
        helpers::binary_tree(&root, height);
        return root;
    }

    /**
    NaryTreeNode nary_tree(int n, int height) {
        // Create a complete nary tree of specified height
        NaryTreeNode root;
        helpers::nary_tree(&root, n, height);
        return root;
    }
    */

    TreeNode IncompleteBinaryTree::make_tree(int depth) {
        TreeNode tree;
        make_tree(tree, depth);
        return tree;
    }

    void IncompleteBinaryTree::make_tree(TreeNode& tree, int depth) {
        // Generate an incomplete binary tree recursively
        if (depth) {
            auto lchance = distribution(generator), rchance = distribution(generator);
            if (lchance > 0.5) tree.add_left();
            if (rchance > 0.5) tree.add_right();
            if (tree.left()) this->make_tree(*(tree.left()), depth - 1);
            if (tree.right()) this->make_tree(*(tree.right()), depth - 1);
        }
    }

    /*void IncompleteNaryTree::make_tree_helper(NaryTreeNode& tree, int depth) {
        if (depth) {
            bool make_nodes = (tree_chance(generator) < 0.5);
            if (make_nodes) {
                int nodes = distribution(generator);
                for (int i = 0; i < (int)nodes; i++) {
                    auto child = tree.add_child();
                    this->make_tree_helper(*child, depth - 1);
                }
            }
        }
    }*/

    SVG::SVG draw_tree(TreeBase& tree, const DrawOpts& options) {
        /** Create an SVG drawing of the given binary or nary tree */
        SVG::SVG drawing;
        helpers::TreeDraw drawer(&drawing, options);
        tree.calculate_xy(options);            // Calculate coords of each point
        drawer.draw_tree(tree);
        drawing.autoscale({ 20, 20, 20, 20 }); // 20px margins on all sides
        return drawing;
    }
}
//
// Defines a general purpose tree data structure
//

#ifndef TREE_DRAWING_TREE_H
#define TREE_DRAWING_TREE_H
#define MINIMUM_SEPARATION 2

#include "svg.hpp"
#include <cassert>
#include <functional>
#include <vector>
#include <memory>
#include <map>
#include <math.h>
#include <random>

namespace tree {
    struct DrawOpts {
        double x_sep;
        double y_sep;
        int node_size;
        std::string edge_color;
        std::string edge_width;
        bool disp_label; /*< Create displacement labels */
        bool show_threads; /*< Show threads */
    };

    const DrawOpts DEFAULT_DRAWING_OPTIONS = { 10, 10, 3, "black", "1px", false, false };

    class TreeBase;
    using NodeList = std::vector<TreeBase*>;

    /** Used for positioning text labels */
    enum NodePosition { LEFT, MIDDLE, RIGHT };

    /** Abstract base class for trees */
    class TreeBase {
        struct Extreme {
            TreeBase* addr;
            double level;
        };

        struct ThreadInfo {
            TreeBase* left = nullptr;
            TreeBase* right = nullptr;
            TreeBase* lroot = nullptr;
            TreeBase* rroot = nullptr;
        };

    public:
        std::string label = "";
        double x = 0;
        double y = 0;
        double displacement = 1;
        TreeBase * thread_l = nullptr;
        TreeBase * thread_r = nullptr;

        virtual TreeBase* left() = 0;
        virtual TreeBase* right() = 0;
        virtual NodeList get_children() = 0;
        virtual bool is_leaf() = 0;

        size_t height();
        virtual void calculate_xy(const DrawOpts&, const unsigned int = 0, const double = 0) = 0;
        void calculate_displacement();
        std::pair<double, ThreadInfo> distance_between(TreeBase*, TreeBase*);
        void thread_left(ThreadInfo&);
        void thread_right(ThreadInfo&);
        Extreme left_most(Extreme&);
        Extreme right_most(Extreme&);

    protected:
        virtual void merge_subtrees() = 0;

    private:
        void left_most(Extreme, std::vector<Extreme>&);
        void right_most(Extreme, std::vector<Extreme>&);
    };

    /** A node for a binary tree */
    class TreeNode : public TreeBase {
    public:
        TreeNode* left() override { return this->left_ptr ? this->left_ptr.get() : (TreeNode*)this->thread_l; }
        TreeNode* right() override { return this->right_ptr ? this->right_ptr.get() : (TreeNode*)this->thread_r; }
        bool is_leaf() override { return !this->left() && !this->right(); }

        TreeNode* add_left(TreeNode node = TreeNode(), const std::string& label = "") {
            this->left_ptr = std::make_shared<TreeNode>(node);
            this->left_ptr->label = label;
            return this->left();
        };

        TreeNode* add_right(TreeNode node = TreeNode(), const std::string& label = "") {
            this->right_ptr = std::make_shared<TreeNode>(node);
            this->right_ptr->label = label;
            return this->right();
        };

        NodeList get_children() override {
            NodeList ret = {};
            if (left()) { ret.push_back(left()); }
            if (right()) { ret.push_back(right()); }
            return ret;
        }

        void calculate_xy(const DrawOpts&, const unsigned int = 0, const double = 0) override;

    private:
        std::shared_ptr<TreeNode> left_ptr = nullptr;
        std::shared_ptr<TreeNode> right_ptr = nullptr;
        void merge_subtrees() override;
    };

    /**
    class NaryTreeNode : public TreeBase {
    public:
    inline NaryTreeNode* left() override {
    if (this->children.empty()) return (NaryTreeNode*)this->thread_l;
    return this->children[0].get();
    }

    inline NaryTreeNode* right() override {
    if (this->children.empty()) return (NaryTreeNode*)this->thread_r;
    return this->children.back().get();
    }

    inline NaryTreeNode* add_child() {
    this->children.push_back(std::make_shared<NaryTreeNode>(NaryTreeNode()));
    return this->children.back().get();
    };

    bool is_leaf() override { return this->children.empty(); }

    NodeList get_children() override {
    NodeList ret;
    for (auto& child : children) ret.push_back(child.get());
    if (ret.empty()) {
    if (this->thread_l) ret.push_back(thread_l);
    if (this->thread_r) ret.push_back(thread_r);
    }
    return ret;
    }

    std::vector<std::shared_ptr<NaryTreeNode>> children;

    private:
    void merge_subtrees(double displacement) override;
    };
    **/

    //
    // Tree Drawing Stuff
    //

    namespace helpers {
        void binary_tree(TreeNode* tree, int depth);
        // void nary_tree(NaryTreeNode* tree, int n, int height);

        class TreeDraw {
            /** A class used for drawing trees */
        public:
            TreeDraw(SVG::SVG* _tree_svg, const DrawOpts& _options = DEFAULT_DRAWING_OPTIONS);
            void draw_tree(TreeBase& tree);
        protected:
            SVG::SVG* tree_svg;
            DrawOpts options;
            SVG::Group* edges = nullptr;
            SVG::Group* vertices = nullptr;
        };
    }

    class IncompleteBinaryTree {
        /** Class for building incomplete binary trees */
    public:
        IncompleteBinaryTree() : generator(std::random_device()()), distribution(0, 1) {};
        TreeNode make_tree(int depth);
        void make_tree(TreeNode& tree, int depth);

    private:
        std::default_random_engine generator;
        std::uniform_real_distribution<double> distribution;
    };

    /**class IncompleteNaryTree {
    ** Class for building complete nary trees *
    public:
    IncompleteNaryTree(double mean, double var) :
    generator(std::random_device()()),
    tree_chance(0, 1),
    distribution(mean, var) {};

    NaryTreeNode make_tree(int depth) {
    ** Generate an incomplete binary tree recursively *
    NaryTreeNode tree;
    make_tree_helper(tree, depth);
    return tree;
    }

    void make_tree_helper(NaryTreeNode& tree, int depth);

    private:
    std::default_random_engine generator;
    std::uniform_int_distribution<int> tree_chance;
    std::normal_distribution<double> distribution;
    };*/

    void left_contour(SVG::SVG& root);
    void right_contour(SVG::SVG& root);

    TreeNode binary_tree(int height);
    // NaryTreeNode nary_tree(int n, int height);
    SVG::SVG draw_tree(TreeBase& tree, const DrawOpts& options = DEFAULT_DRAWING_OPTIONS);
}

#endif //TREE_DRAWING_TREE_H
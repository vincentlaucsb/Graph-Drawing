// This file contains only the tree drawing algorithm itself

#include "tree.h"

namespace tree {
    void TreeNode::calculate_xy(const DrawOpts& options, const unsigned int depth, const double offset) {
        /** Calculate the x, y coordinates of each node via a preorder traversal */
        if (depth == 0) this->calculate_displacement();
        this->x = offset * options.x_sep;
        this->y = depth * options.y_sep;

        if (left() && !this->thread_l) left()->calculate_xy(
            options, depth + 1, offset - this->displacement);
        if (right() && !this->thread_r) right()->calculate_xy(
            options, depth + 1, offset + this->displacement);
    }

    size_t TreeBase::height() {
        /** Calculate the height of this tree */
        size_t height = 1;
        std::deque<TreeBase*> children;
        for (auto& child : this->get_children()) {
            height = std::max(height, child->height() + 1);
        }

        return height;
    }

    void TreeBase::calculate_displacement() {
        /** Calculate the displacement of each node via a postorder traversal */
        this->merge_subtrees();
    }

    std::pair<double, TreeBase::ThreadInfo> TreeBase::distance_between(TreeBase* left, TreeBase* right) {
        /* Return the minimum horizontal distance needed between two subtrees
         * such that they can be placed 2 units apart horizontally
         *
         * Precondition: All nodes except the top nodes have correct displacements set
         */

        TreeBase *lroot = left, *rroot = right;

        // cursep: The current separation between subtrees; used as a trigger to add extra distance
        double rootsep = MINIMUM_SEPARATION, cursep = MINIMUM_SEPARATION;

        // Accumulate displacements and terminate when either subtree runs out of height
        while (left && right) {
            // Add extra distance when subtrees get too close
            if (cursep < MINIMUM_SEPARATION) {
                rootsep += (MINIMUM_SEPARATION - cursep);
                cursep = MINIMUM_SEPARATION;
            }

            if (left->right()) {
                cursep -= left->displacement;
                left = left->right();
            }
            else {
                cursep += left->displacement;
                left = left->left();
            }

            if (right->left()) {
                cursep -= right->displacement;
                right = right->left();
            }
            else {
                cursep += right->displacement;
                right = right->right();
            }
        }

        return std::make_pair(rootsep/2, ThreadInfo({
            left, right, lroot, rroot
        }));
    }

    void TreeBase::thread_left(TreeBase::ThreadInfo& thread) {
        // Need to make threading part of another thing
        // Perform threading if necessary
        auto rmost = thread.rroot->right_most(Extreme({ this, 1 }));
        if (thread.left->thread_l != rmost.addr) rmost.addr->thread_l = thread.left;
    }

    void TreeBase::thread_right(TreeBase::ThreadInfo& thread) {
        auto lmost = thread.lroot->left_most(Extreme({ this, 1 }));
        if (thread.right->thread_r != lmost.addr) lmost.addr->thread_r = thread.right;
    }

    void TreeNode::merge_subtrees() {
        /* "Merge" the subtrees of this node such that they have a horizontal separation of 2
         *  by setting an appropriate displacement for this node
         */

        // Postorder traversal
        if (this->left()) this->left()->merge_subtrees();
        if (this->right()) this->right()->merge_subtrees();

        // Merge subtrees (if they exist)
        if (this->left() && this->right()) {
            // Because by default, this node has displacement zero,
            // it will be centered over its children
            auto dist = this->distance_between(this->left(), this->right());
            this->displacement = dist.first;

            // Thread if necessary
            if (dist.second.left) this->thread_left(dist.second);
            if (dist.second.right) this->thread_right(dist.second);
        }
    }

    TreeBase::Extreme TreeBase::left_most(Extreme& current) {
        /** Return the left most node in a subtree */
        std::vector<Extreme> lmost;
        this->left_most(current, lmost);
        return lmost.back();
    }

    void TreeBase::left_most(Extreme current, std::vector<Extreme>& lmost) {
        current.addr = this;
        current.level++;
        if (lmost.size() <= current.level) lmost.push_back(current);
        if (this->left()) this->left()->left_most(current, lmost);
        if (this->right()) this->right()->left_most(current, lmost);
    }

    TreeBase::Extreme TreeBase::right_most(Extreme& current) {
        /** Return a list with the right contour of a vertex */
        std::vector<Extreme> rmost;
        this->right_most(current, rmost);
        return rmost.back();
    }

    void TreeBase::right_most(Extreme current, std::vector<Extreme>& rmost) {
        current.addr = this;
        current.level++;
        if (rmost.size() <= current.level) rmost.push_back(current);
        if (this->right()) this->right()->right_most(current, rmost);
        if (this->left()) this->left()->right_most(current, rmost);
    }

    //void NaryTreeNode::merge_subtrees(double displacement) {
    /** "Merge" the subtrees of this node such that they have a horizontal separation of 2
    *  by setting an appropriate displacement for this node
    *
    *  displacement: Default displacement value if this is a leaf node
    *

    // Default displacement
    //this->displacement = displacement;

    // Postorder traversal
    double width = ((double)this->children.size() - 1) * MINIMUM_SEPARATION,
    current_width = -width / 2;
    for (size_t i = 0; i < this->children.size(); i++) {
    this->children[i]->merge_subtrees(current_width);
    current_width += width / (double)(this->children.size() - 1);
    }

    // Merge subtrees (if they exist) by moving from left to right, merging two
    // adjacent subtrees at a time
    if (!this->children.empty()) {
    // Because by default, this node has displacement zero,
    // it will be centered over its children
    for (size_t i = 0; (i + 1) < this->children.size(); i++) {
    if (this->label == "root") std::cout << "Merging trees" << std::endl;
    auto dist = this->distance_between(this->children[i].get(), this->children[i + 1].get());
    this->children[i + 1]->displacement = i * dist.first;

    // Thread if necessary
    if (dist.second.left) this->thread_left(dist.second);
    if (dist.second.right) this->thread_right(dist.second);
    }
    }
    }*/
}
// Drawing Trees with Linear Programming

#pragma once
#include <stdlib.h>
#include <algorithm>
#include <unordered_map>
#include <memory>
#include <deque>
#include <iostream>
#include <random>
// #include <chrono>
#include "cxxopts.hpp"
#include "svg.hpp"
#include "glpk.h"

namespace tree {
    struct TreeNode {
        std::string data;
        int id;
        std::unique_ptr<TreeNode> left = nullptr;
        std::unique_ptr<TreeNode> right = nullptr;
        size_t size();
        size_t height();
    };

    TreeNode fig2();

    // Map level numbers to lists of nodes at that level
    using LevelMap = std::unordered_map<int, std::vector<TreeNode*>>;
    struct TreeOptions {
        bool aes6;
        std::string filename;
    };
    const TreeOptions DEFAULT_TREE_OPTIONS = { true, "" };

    double factorial(const int n);
    int binary_trees(int n);
    int g_jn(int j, int n);

    using NumNodes = int;
    using Rank = int;
    using RankMap = std::unordered_map<NumNodes, std::unordered_map<Rank, std::vector<TreeNode*>>>;
    int rank(TreeNode* tree, RankMap* cache = nullptr);
    TreeNode full_tree(int height);
    TreeNode incomplete_tree(int height);
    SVG::SVG draw_tree(glp_prob* P, LevelMap& level);
    std::pair<glp_prob*, LevelMap> map_tree(
        TreeNode& root,
        const TreeOptions& options=DEFAULT_TREE_OPTIONS
    );

    namespace helpers {
        void full_tree_helper(TreeNode& node, int height);

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
    }
}
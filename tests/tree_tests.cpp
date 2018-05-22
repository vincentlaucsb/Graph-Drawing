#define CATCH_CONFIG_MAIN
#include "catch.hpp"
#include "tree_lp.h"

using namespace tree;

TreeNode rank2() {
    // Return a tree of rank 2
    TreeNode tree2;
    tree2.right = std::make_unique<TreeNode>();
    tree2.right->right = std::make_unique<TreeNode>();
    tree2.right->right->left = std::make_unique<TreeNode>();
    return tree2;
}

TEST_CASE("Factorial Test", "[factorial_test]") {
    REQUIRE(factorial(0) == 1);
    REQUIRE(factorial(3) == 6);
    REQUIRE(factorial(5) == 5 * 4 * 3 * 2);
}

TEST_CASE("Rank Helpers Test", "[rank_helper_test]") {
    REQUIRE(binary_trees(0) == 1);
    REQUIRE(g_jn(0, 0) == 1); // Edge Case
    REQUIRE(g_jn(0, 1) == 1);
}

TEST_CASE("tree_size() Test", "[tree_size_test]") {
    REQUIRE(rank2().size() == 4);
}

TEST_CASE("rank() Test 1", "[tree_test2]") {
    TreeNode tree1;
    int i = 0;
    for (auto tree = &tree1; i < 3; tree = tree->right.get()) {
        tree->right = std::make_unique<TreeNode>();
        i++;
    }

    REQUIRE(tree1.size() == 4);
    REQUIRE(rank(&tree1) == 1);
}

TEST_CASE("rank() Test 2", "[tree_test2]") {
    auto tree2 = rank2();
    int tree2_rank = rank(&tree2);

    REQUIRE(tree2_rank == 2);
}

TEST_CASE("rank() Test 3", "[tree_test3]") {
    TreeNode tree3;
    tree3.right = std::make_unique<TreeNode>();
    tree3.right->left = std::make_unique<TreeNode>();
    tree3.right->right = std::make_unique<TreeNode>();

    int tree3_rank = rank(&tree3);
    REQUIRE(tree3_rank == 3);
}

TEST_CASE("rank() Test 14", "[tree_test14]") {
    TreeNode tree14;
    int i = 0;
    for (auto tree = &tree14; i < 3; tree = tree->left.get()) {
        tree->left = std::make_unique<TreeNode>();
        i++;
    }

    REQUIRE(tree14.size() == 4);
    REQUIRE(rank(&tree14) == 14);
}
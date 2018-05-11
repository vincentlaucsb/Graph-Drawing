#include "cxxopts.hpp"
#include "tree.h"
#include <fstream>

// Tree drawing example of a binary tree with uniform width and height
int main(int argc, char** argv) {
    using namespace tree;
    cxxopts::Options options(argv[0], "Produces a tree of specified depth");
    options.positional_help("[output file] [depth]");
    options.add_options("required")
        ("f,file", "output file", cxxopts::value<std::string>())
        ("d,depth", "depth", cxxopts::value<int>());
    options.add_options("optional")
        ("n,nary", "Produce a n-ary tree", cxxopts::value<int>()->default_value("2"))
        ("i,incomp", "Produce an incomplete tree")
        ("x,xsep", "Seperation between nodes (x-axis)", cxxopts::value<int>()->default_value("10"))
        ("y,ysep", "Seperation between nodes (y-axis)", cxxopts::value<int>()->default_value("20"))
        ("s,nodesize", "Radius of nodes (in pixels)", cxxopts::value<int>()->default_value("3"))
        ;
    options.parse_positional({ "file", "depth" });

    if (argc < 3) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    try {
        auto result = options.parse(argc, (const char**&)argv);

        std::string file = result["file"].as<std::string>();
        bool incomp = result["incomp"].as<bool>();
        int depth = result["depth"].as<int>(),
            nodes = result["nary"].as<int>();

        DrawOpts opts = DEFAULT_DRAWING_OPTIONS;
        opts.x_sep = result["xsep"].as<int>();
        opts.y_sep = result["ysep"].as<int>();
        opts.show_threads = true;
        opts.node_size = result["nodesize"].as<int>();

        SVG::SVG tree_drawing;
        if (nodes == 2) {
            TreeNode tree;
            if (incomp) {
                auto maker = IncompleteBinaryTree();
                while (tree.height() < depth) tree = maker.make_tree(depth);
            }
            else tree = binary_tree(depth);

            tree_drawing = draw_tree(tree, opts);
        }
        /**
        else {
        NaryTreeNode tree;
        if (incomp) {
        auto maker = IncompleteNaryTree(nodes, 1);
        while (tree.height() < depth) tree = maker.make_tree(depth);
        }
        else tree = nary_tree(nodes, depth);

        tree_drawing = draw_tree(tree, opts);
        }*/

        std::ofstream outfile(file);
        outfile << std::string(tree_drawing);
    }
    catch (cxxopts::OptionException&) {
        std::cout << options.help({ "optional" }) << std::endl;
        return 1;
    }

    return 0;
}
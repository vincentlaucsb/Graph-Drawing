// Generate figures needed to write the paper

#include "tree.h"
#include <fstream>

namespace tree {
    SVG::SVG draw_fig2(const DrawOpts& options, bool disp_label = false);
    SVG::SVG draw_fig4(const DrawOpts& options);

    SVG::SVG draw_fig2(const DrawOpts& options, bool disp_label) {
        /** Replicate figure 2 from RT81 */
        TreeNode root;

        // Left subtree
        auto d1_left = root.add_left();
        auto d2_left = d1_left->add_left();
        d1_left->add_right();
        auto d3_left = d2_left->add_left();
        d2_left->add_right();
        auto d4_left = d3_left->add_left();
        d3_left->add_right();
        d4_left->add_left();
        auto d5_right = d4_left->add_right();
        d5_right->add_left();
        auto d6_right = d5_right->add_right();
        auto d7_left = d6_right->add_right();
        d7_left->add_left();
        d7_left->add_right();

        // Right subtree
        auto d1_right = root.add_right();
        auto d2_right = d1_right->add_right();
        d1_right->add_left();
        auto d3_right = d2_right->add_right();
        d2_right->add_left();
        auto d4_right = d3_right->add_right();
        d3_right->add_left();
        d4_right->add_right();
        auto d5_left = d4_right->add_left();
        d5_left->add_right();
        auto d6_left = d5_left->add_left();
        auto d7_right = d6_left->add_left();
        d7_right->add_left();
        d7_right->add_right();

        // Create SVG
        SVG::SVG fig2 = draw_tree(root, options);
        fig2.style("text").set_attr("font-family", "sans-serif");
        return fig2;
    }

    SVG::SVG draw_fig4(const DrawOpts& options) {
        TreeNode left, right;
        left.add_right();
        left.add_left()->add_right()->add_right()->add_right()->add_right();
        right.add_left();
        right.add_right()->add_left()->add_left()->add_left()->add_left();

        SVG::SVG fig4_left = draw_tree(left, options),
            fig4_right = draw_tree(right, options),
            fig4_drawing = merge(fig4_left, fig4_right);
        return fig4_drawing;
    }
}

int main(int argc, char** argv) {
    using namespace tree;
    std::ofstream fig2("figure2.svg"), fig2_threads("figure2_threads.svg"),
        fig2_labels("figure2_labels.svg"),
        fig2_contours("figure2_contours.svg"),
        fig4("figure4.svg");

    DrawOpts options = DEFAULT_DRAWING_OPTIONS, enlarged_options = DEFAULT_DRAWING_OPTIONS;
    options.x_sep = 20;
    options.y_sep = 40;
    options.node_size = 5;
    // TEMPORARY
    options.show_threads = true;
    options.disp_label = true;

    auto show_labels = options, show_threads = options;
    show_labels.disp_label = true;
    show_threads.show_threads = true;

    enlarged_options.x_sep = 30;
    enlarged_options.y_sep = 90;

    /*
    * Replicate Figure 2 and illustrate left/right contours
    */
    SVG::SVG fig2a = draw_fig2(options),
        fig2b = draw_fig2(show_threads),
        fig2c = draw_fig2(enlarged_options),
        fig2_temp1 = draw_fig2(options),
        fig2_temp2 = draw_fig2(options);

    // Highlight left & right contours respectively
    left_contour(fig2_temp1);
    right_contour(fig2_temp2);
    SVG::SVG fig2_contours_drawing = merge(fig2_temp1, fig2_temp2);

    fig2 << std::string(fig2a);
    fig2_threads << std::string(fig2b);
    fig2_labels << std::string(fig2c);
    fig2_contours << std::string(fig2_contours_drawing);

    /*
    * Replicate Figure 4
    */
    SVG::SVG fig4_drawing = draw_fig4(options);
    fig4 << std::string(fig4_drawing);

    /*
    * Incomplete Binary Tree Edge Cases
    *
    */
    TreeNode temp = binary_tree(2);
    TreeNode incomp_root = binary_tree(1);
    incomp_root.add_right(temp);
    auto incomp_fig = draw_tree(incomp_root, options);
    std::ofstream incomp_out("incomp1.svg");
    incomp_out << std::string(incomp_fig);

    TreeNode temp2 = binary_tree(2);
    TreeNode incomp_root2 = binary_tree(1);
    incomp_root2.add_left(temp2);
    auto incomp_fig2 = draw_tree(incomp_root2, options);
    std::ofstream incomp_out2("incomp2.svg");
    incomp_out2 << std::string(incomp_fig2);

    /*
    * Complete Binary Trees
    */
    for (int i = 1; i <= 5; i++) {
        TreeNode binary_root = binary_tree(i);
        SVG::SVG binary_svg = draw_tree(binary_root, show_labels),
            binary_svg2 = draw_tree(binary_root, options);
        std::ofstream binary_out("binary_tree_" + std::to_string(i) + ".svg"),
            binary_out2("binary_tree_no_label" + std::to_string(i) + ".svg");
        binary_out << std::string(binary_svg);
        binary_out2 << std::string(binary_svg2);
    }

    /*
    * Complete Ternary Trees
    *
    for (int i = 1; i <= 5; i++) {
        NaryTreeNode root = nary_tree(3, i);
        SVG::SVG ternary_svg = draw_tree(root, show_labels),
            ternary_svg2 = draw_tree(root, options);
        std::ofstream ternary_out("ternary_tree_" + std::to_string(i) + ".svg"),
            ternary_out2("ternary_tree_no_label" + std::to_string(i) + ".svg");
        ternary_out << std::string(ternary_svg);
        ternary_out2 << std::string(ternary_svg2);
    }*/

    /*
    * Complete Quarternary, Quinary, Senary, Septenary Tree
    *
    SVG::SVG four_svg = draw_tree(nary_tree(4, 3), options);
    std::ofstream four_out("quarternary_tree.svg");
    four_out << std::string(four_svg);

    SVG::SVG five_svg = draw_tree(nary_tree(5, 3), options);
    std::ofstream five_out("quinary_tree.svg");
    five_out << std::string(five_svg);

    SVG::SVG six_svg = draw_tree(nary_tree(6, 2), options);
    std::ofstream six_out("senary_tree.svg");
    six_out << std::string(six_svg);

    SVG::SVG seven_svg = draw_tree(nary_tree(7, 2), options);
    std::ofstream seven_out("septenary_tree.svg");
    seven_out << std::string(seven_svg);*/

    /*
    * Some Bullshit Tree
    *

    NaryTreeNode bs_root;
    for (int i = 0; i < 3; i++)
        bs_root.add_child();

    for (int i = 0; i < 3; i++) {
        bs_root.children[0]->add_child();
        bs_root.children[2]->add_child();
    }

    SVG::SVG bs_svg = draw_tree(bs_root, show_threads);
    std::ofstream bs_out("bs_tree.svg");
    bs_out << std::string(bs_svg);
    */

    return 0;
}
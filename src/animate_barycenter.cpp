#include "force_directed.h"
#include "cxxopts.hpp"
#include <iomanip> // setprecision
#include <sstream>

std::string latex(const Eigen::MatrixXd&);
std::string latex(const char name, const Eigen::VectorXd&);

std::string latex(const Eigen::MatrixXd& m) {
    // Print the LaTeX code for a matrix
    std::stringstream ret;
    ret << std::setprecision(3) << "\\begin{bmatrix}" << std::endl;

    for (int i = 0; i < m.rows(); i++) {
        ret << "\t";
        for (int j = 0; j < m.cols(); j++) {
            ret << m(i, j);
            if (j + 1 < m.cols()) ret << " & ";
            else ret << " \\\\ " << std::endl;
        }
    }

    ret << "\\end{bmatrix}" << std::endl;

    return ret.str();
}

std::string latex(const char name, const Eigen::VectorXd& v) {
    // Print the LaTeX code for a vector
    std::stringstream r1, r2;
    r1 << std::setprecision(3) << "\\begin{bmatrix}" << std::endl;
    r2 << std::setprecision(3) << "\\begin{bmatrix}" << std::endl;

    for (int i = 0; i < v.rows(); i++) {
        r1 << "\t" << name << "_" << i + 1;
        r2 << "\t" << v(i);

        r1 << " \\\\ " << std::endl;
        r2 << " \\\\ " << std::endl;
    }

    r1 << "\\end{bmatrix}" << std::endl;
    r2 << "\\end{bmatrix}" << std::endl;

    return r1.str() + "=" + r2.str();
}

int main(int argc, char** argv) {
    using namespace force_directed;

    cxxopts::Options options(argv[0],
        "Produces animations of a Newton-Raphson based version of the "
        "barycenter drawing algorithm or static images from a linear "
        "system based solver");
    options.positional_help("[output file]");
    options.add_options("required")
        ("f,file", "Output file", cxxopts::value<std::string>());
    options.add_options("optional")
        ("h,hypercube", "Animate drawing a hypercube of order 3")
        ("p,petersen", "Animate drawing the Petersen graph", cxxopts::value<bool>()->default_value("false"))
        ("r,prism", "Animate drawing the Prism graph on n vertices", cxxopts::value<int>()->default_value("0"))
        ("g,generalized", "Animate drawing the Generalized Petersen graph GP(n, k)",
            cxxopts::value<std::string>()->default_value(""))
        ("w,width", "Specify the width of the drawing", cxxopts::value<int>()->default_value("500"))
        ("s,static", "Draw a still image of the graph (uses linear solver)", cxxopts::value<bool>()->default_value("false"));

    options.parse_positional({ "file" });

    if (argc < 2) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }

    auto result = options.parse(argc, (const char**&)argv);

    std::string file = result["file"].as<std::string>(),
        gp = result["generalized"].as<std::string>();

    bool _static = result["static"].as<bool>();
    int width = result["width"].as<int>();
    TUNGraph graph;
    size_t vertices;
    
    if (result["hypercube"].as<bool>()) {
        graph = hypercube();
        vertices = 4;
    }
    else if (result["petersen"].as<bool>()) {
        graph = petersen();
        vertices = 5;
    }
    else if (!gp.empty()) {
        int n = -1, k = -1;
        std::stringstream ss(gp);
        std::string num;

        while (std::getline(ss, num, ',')) {
            if (n < 0) n = std::stoi(num);
            else k = std::stoi(num);
        }

        graph = generalized_petersen(n, k);
        vertices = n;
    }
    else if (result["complete"].as<int>()) {
        graph = complete(result["complete"].as<int>());
        vertices = 4;
        _static = true;
    }
    else if (result["prism"].as<int>()) {
        auto n = result["prism"].as<int>();
        graph = prism(n);
        vertices = n;
    }

    std::ofstream graph_out(file);
    if (_static) {
        auto output = barycenter_layout_la(graph, vertices, width);
        output.image.autoscale();

        graph_out << std::string(output.image);
        std::cout << "Matrix" << std::endl;
        std::cout << latex(output.matrix) << std::endl;

        std::cout << "Fixed vertex positions (x)" << std::endl;
        std::cout << latex('x', output.fixed_x) << std::endl;

        std::cout << "Fixed vertex positions (y)" << std::endl;
        std::cout << latex('y', output.fixed_y) << std::endl;

        std::cout << "Free vertex positions (x)" << std::endl;
        std::cout << latex('x', output.sol_x) << std::endl;

        std::cout << "Free vertex positions (y)" << std::endl;
        std::cout << latex('y', output.sol_y) << std::endl;
    }
    else {
        std::vector<SVG::SVG> frames = barycenter_layout(graph, vertices, width);
        auto final_svg = SVG::frame_animate(frames, 3);
        graph_out << std::string(final_svg);
    }

    return 0;
}

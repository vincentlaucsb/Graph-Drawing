#include "force_directed.h"
#include "cxxopts.hpp"

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
        ("k,complete", "Draw the complete graph on n vertices", cxxopts::value<int>()->default_value("0"))
        ("w,width", "Specify the width of the drawing", cxxopts::value<int>()->default_value("500"))
        ("s,static", "Draw a still image of the graph (uses linear solver)", cxxopts::value<bool>()->default_value("false"));

    options.parse_positional({ "file" });

    if (argc < 2) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }

    auto result = options.parse(argc, (const char**&)argv);

    std::string file = result["file"].as<std::string>();
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
        std::cout << output.matrix << std::endl << std::endl;

        std::cout << "Fixed vertex positions (x)" << std::endl;
        std::cout << output.fixed_x << std::endl << std::endl;

        std::cout << "Fixed vertex positions (y)" << std::endl;
        std::cout << output.fixed_y << std::endl << std::endl;

        std::cout << "Free vertex positions (x)" << std::endl;
        std::cout << output.sol_x << std::endl << std::endl;

        std::cout << "Free vertex positions (y)" << std::endl;
        std::cout << output.sol_y << std::endl;
    }
    else {
        std::vector<SVG::SVG> frames = barycenter_layout(graph, vertices, width);
        auto final_svg = SVG::frame_animate(frames, 3);
        graph_out << std::string(final_svg);
    }

    return 0;
}

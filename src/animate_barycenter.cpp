#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace force_directed;

    cxxopts::Options options(argv[0], "Animates drawing a graph with the barycenter method");
    options.positional_help("[output file]");
    options.add_options("required")
        ("f,file", "Output file", cxxopts::value<std::string>());
    options.add_options("optional")
        ("h,hypercube", "Animate drawing a hypercube of order 3")
        ("p,petersen", "Animate drawing the Petersen graph", cxxopts::value<bool>()->implicit_value("false"))
        ("r,prism", "Animate drawing the Prism graph on n vertices", cxxopts::value<int>()->default_value("0"))
        ("w,width", "Specify the width of the drawing", cxxopts::value<int>()->default_value("500"))
        ("s,static", "Draw a still image of the graph", cxxopts::value<bool>()->implicit_value("false"));

    options.parse_positional({ "file" });

    if (argc < 2) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }

    auto result = options.parse(argc, (const char**&)argv);

    std::string file = result["file"].as<std::string>();
    int width = result["width"].as<int>();
    TNEANet graph;
    size_t vertices;
    
    if (result["hypercube"].as<bool>()) {
        graph = hypercube();
        vertices = 4;
    }
    else if (result["petersen"].as<bool>()) {
        graph = petersen();
        vertices = 5;
    }
    else if (result["prism"].as<int>()) {
        auto n = result["prism"].as<int>();
        graph = prism(n);
        vertices = n;
    }
    bool _static = result["static"].as<bool>();

    prism_distances(3, 30);
    std::vector<SVG::SVG> frames = barycenter_layout(graph, vertices, width);
    std::ofstream graph_out(file);

    if (_static) {
        auto final_svg = SVG::frame_animate(frames, 3);
        graph_out << std::string(final_svg);
    }
    else {
        auto& final_svg = frames.back();
        final_svg.autoscale();
        graph_out << std::string(final_svg);
    }

    return 0;
}
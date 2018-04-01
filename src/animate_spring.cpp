#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace force_directed;

    cxxopts::Options options(argv[0], "Animates drawing a complete graph on n vertices");
    options.positional_help("[output file] [m]");
    options.add_options("required")
        ("f,file", "Output file", cxxopts::value<std::string>())
        ("m,vertices", "Animate a complete graph on m vertices", cxxopts::value<int>());
    options.add_options("optional")
        ("n,to", "Animate complete graphs for m, ..., n vertices",
            cxxopts::value<int>()->default_value("0"));

    options.parse_positional({ "file", "vertices" });

    if (argc < 3) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }
    
    auto result = options.parse(argc, (const char**&)argv);
    
    std::string file = result["file"].as<std::string>();
    int m = result["vertices"].as<int>(),
        n = result["to"].as<int>();

    ForceDirectedParams params = { 400, 2, 1 };

    if (n < m) n = m;
    try {
        for (int i = m; i <= n; i++) {
            auto graph = complete(i);
            std::vector<SVG::SVG> frames = force_directed_layout(params, graph);
            auto final_svg = SVG::frame_animate(frames, 5);

            std::ofstream graph_out(file + std::to_string(i) + ".svg");
            graph_out << std::string(final_svg);
        }
    }
    catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
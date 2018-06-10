#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace force_directed;

    cxxopts::Options options(argv[0], "Draw a star graph on n vertices");
    options.positional_help("[output file] [m]");
    options.add_options("required")
        ("f,file", "Output file", cxxopts::value<std::string>())
        ("m,vertices", "Animate a complete graph on m vertices", cxxopts::value<int>());

    options.parse_positional({ "file", "vertices" });

    if (argc < 3) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }

    auto result = options.parse(argc, (const char**&)argv);

    std::string file = result["file"].as<std::string>();
    int m = result["vertices"].as<int>();

    ForceDirectedParams params = { 400, 0, 1 };

    // auto graph = TSnap::GenStar<PUNGraph>(m, false);
    // auto graph = tree();
    // auto graph_ptr = TSnap::GenFull<PUNGraph>(m);
    auto graph_ptr = TSnap::GenRndDegK(m, 3);
    auto& graph = *graph_ptr;
    // auto graph = hypercube_un();
    // auto graph = wheel_un(m);
    // auto graph = tree(m);

    try {
        auto final_pos = eades84(graph);
        auto final_svg = draw_graph(graph, final_pos);
        final_svg.autoscale();
        std::ofstream graph_out(file);
        graph_out << std::string(final_svg);
    }
    catch (std::runtime_error& err) {
        std::cout << "Error" << std::endl << err.what() << std::endl;
    }

    return 0;
}
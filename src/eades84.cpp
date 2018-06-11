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
    // auto graph_ptr = TSnap::GenRndDegK(6, 3);
    // auto& graph = *graph_ptr;
    // auto graph = hypercube_un();
    // auto graph = wheel_un(m);
    // auto graph = tree(m);

    // Good results
    // auto graph = three_reg_6();
    auto graph = hypercube_4();

    /* For regular cube
    VertexPos pos = {
        { 0, { 0, 0 } },
        { 1, { 200, 0 } },
        { 2, { 200, -200 } },
        { 3, { 0, -200 } },
        { 4, { 100, 100 } },
        { 5, { 300, 100 } },
        { 6, { 300, -100 } },
        { 7, { 100, -100 } }
    };

    VertexPos pos = {
        { 0, { 0, 0 } },
        { 1, { 200, 0 } },
        { 2, { 180, -200 } },
        { 3, { 0, -200 } },
        { 4, { 100, 100 } },
        { 5, { 300, 100 } },
        { 6, { 300, -80 } },
        { 7, { 100, -100 } }
    };*/

    VertexPos pos = {
        { 0, { 0, 0 } },
        { 1, { 200, 0 } },
        { 2, { 200, -200 } },
        { 3, { 0, -200 } },
        { 4, { 100, 100 } },
        { 5, { 300, 100 } },
        { 6, { 300, -100 } },
        { 7, { 100, -100 } }, // 
        { 8, { 0 + 400, 0 + 100 } },
        { 9, { 200 + 400, 0 + 100 } },
        { 10, { 200 + 400, -200 + 100 } },
        { 11, { 0 + 400, -200 + 100 } },
        { 12, { 100 + 400, 100 + 100 } },
        { 13, { 300 + 400, 100 + 100 } },
        { 14, { 300 + 400, -100 + 100 } },
        { 15, { 100 + 400, -100 + 100 } }
    };

    /*
    VertexPos pos = {
        { 0,{ 0, 0 } },
        { 1,{ 200, 0 } },
        { 2,{ 200, -200 } },
        { 3,{ 0, -200 } },
        { 4,{ 100, 100 } },
        { 5,{ 300, 100 } },
        { 6,{ 300, -100 } },
        { 7,{ 100, -100 } }, // 
        { 8,{ -200, 0 } },
        { 9,{ 400, 0 } },
        { 10,{ 400, -400 } },
        { 11,{ -200, -400 } },
        { 12,{ -100, 200 } },
        { 13,{ 400, 200 } },
        { 14,{ 400, -300 } },
        { 15,{ -100, -300 } }
    };
    */
    
    /*
    auto points = SVG::util::polar_points(graph.GetNodes(), 0, 0, 100);
    VertexPos pos;
    for (int i = 0; i < graph.GetNodes(); i++) pos[i] = points[i];
    */

    try {
        auto final_pos = eades84(graph, pos);
        // auto final_pos = eades84(graph);
        auto final_svg = SVG::frame_animate(final_pos, 30);
        std::ofstream graph_out(file);
        graph_out << std::string(final_svg);
    }
    catch (std::runtime_error& err) {
        std::cout << "Error" << std::endl << err.what() << std::endl;
    }

    return 0;
}
#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace force_directed;

    cxxopts::Options options(argv[0], "Animates drawing a complete graph on n vertices");
    options.positional_help("[output file] [n]");
    options.add_options("required")
        ("f,file", "Output file", cxxopts::value<std::string>())
        ("n,vertices", "Animate a complete graph on m vertices", cxxopts::value<int>());
    options.add_options("optional")
        ("s,side", "Create an algorithm trace of the spring layout",
            cxxopts::value<bool>()->default_value("false"));

    options.parse_positional({ "file", "vertices" });

    if (argc < 3) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }
    
    auto result = options.parse(argc, (const char**&)argv);
    
    std::string file = result["file"].as<std::string>();
    bool side_by_side = result["side"].as<bool>();
    int n = result["vertices"].as<int>();

    ForceDirectedParams params = { 400, 0, 1 };

    try {
        auto graph_ptr = TSnap::GenFull<PUNGraph>(n);
        auto graph = *graph_ptr;

        std::vector<SVG::SVG> frames = eades84_2(params, graph);

        if (side_by_side) {
            const int excess_frames = (int)frames.size() - 16,
                n_frames = (int)frames.size();
                
            // Delete excess frames
            int i = 0;
            for (auto it = frames.begin(); it < frames.end();) {
                if (i > 0 && i%(n_frames/excess_frames) == 0) it = frames.erase(it);
                else ++it;

                i++;
            }

            auto final_svg = SVG::merge(frames, 1000, 250);
            std::ofstream graph_out(file + ".svg");
            graph_out << std::string(final_svg);
        }
        else {
            auto final_svg = SVG::frame_animate(frames, 5);
            std::ofstream graph_out(file + ".svg");
            graph_out << std::string(final_svg);
        }
    }
    catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
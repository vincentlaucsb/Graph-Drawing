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
        ("c,cube", "Draw the regular cube")
        ("k,tesseract", "Draw the 4-cube")
        ("t,three_reg", "Draw a three-regular graph")
        ("s,side", "Create an algorithm trace of the spring layout",
            cxxopts::value<bool>()->default_value("false"));

    options.parse_positional({ "file", "vertices" });

    if (argc < 3) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }
    
    auto result = options.parse(argc, (const char**&)argv);
    
    std::string file = result["file"].as<std::string>();
    bool side_by_side = result["side"].as<bool>(),
        cube = result["cube"].as<bool>(),
        tesseract = result["tesseract"].as<bool>(),
        three_reg = result["three_reg"].as<bool>();
    int n = result["vertices"].as<int>();
    ForceDirectedParams params = { 400, 2, 1 };

    try {
        auto graph_ptr = TSnap::GenFull<PUNGraph>(n);
        auto graph = *graph_ptr;
        VertexPos pos = random_layout(graph);

        if (cube) {
            graph = hypercube();
            pos = {
                { 4,{ 0, 0 } },
                { 5,{ 200, 0 } },
                { 6,{ 180, -200 } },
                { 7,{ 0, -200 } },
                { 0,{ 100, 100 } },
                { 1,{ 300, 100 } },
                { 2,{ 300, -80 } },
                { 3,{ 100, -100 } }
            };
        }
        if (tesseract) {
            graph = hypercube_4();
            pos = {
                { 0,{ 0, 0 } },
                { 1,{ 200, 0 } },
                { 2,{ 200, -200 } },
                { 3,{ 0, -200 } },
                { 4,{ 100, 100 } },
                { 5,{ 300, 100 } },
                { 6,{ 300, -100 } },
                { 7,{ 100, -100 } }, // 
                { 8,{ 0 + 800, 0 + 100 } },
                { 9,{ 200 + 800, 0 + 100 } },
                { 10,{ 200 + 800, -200 + 100 } },
                { 11,{ 0 + 800, -200 + 100 } },
                { 12,{ 100 + 800, 100 + 100 } },
                { 13,{ 300 + 800, 100 + 100 } },
                { 14,{ 300 + 800, -100 + 100 } },
                { 15,{ 100 + 800, -100 + 100 } }
                };
        }

        if (three_reg) {
            graph = three_reg_6();
            auto points = SVG::util::polar_points(graph.GetNodes(), 0, 0, 100);
            for (int i = 0; i < graph.GetNodes(); i++) pos[i] = points[i];
        }

        std::vector<SVG::SVG> frames = eades84_2(params, graph, pos);

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
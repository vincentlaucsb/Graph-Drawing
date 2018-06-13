#include "csv_parser.h"
#include "force_directed.h"
#include "cxxopts.hpp"

int main(int argc, char** argv) {
    using namespace csv;
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
        ("s,still", "Draw a still drawing of the graph")
        ("r,trace", "Create an algorithm trace of the spring layout")
        ("p,pos", "Read a CSV file containing positions for vertices",
            cxxopts::value<std::string>()->default_value(""));

    options.parse_positional({ "file", "vertices" });

    if (argc < 3) {
        std::cout << options.help({ "required", "optional" }) << std::endl;
        return 1;
    }
    
    auto result = options.parse(argc, (const char**&)argv);
    
    std::string file = result["file"].as<std::string>(),
        pos_file = result["pos"].as<std::string>();
    bool still = result["still"].as<bool>(),
        side_by_side = result["trace"].as<bool>(),
        cube = result["cube"].as<bool>(),
        tesseract = result["tesseract"].as<bool>(),
        three_reg = result["three_reg"].as<bool>();
    int n = result["vertices"].as<int>();
    ForceDirectedParams params = { 400, 2, 1 };

    try {
        auto graph_ptr = TSnap::GenFull<PUNGraph>(n);
        auto graph = *graph_ptr;
        VertexPos pos = random_layout(graph);

        if (!pos_file.empty()) {
            CSVReader reader(pos_file);
            std::vector<CSVField> row;

            while (reader.read_row(row)) {
                pos[row[0].get_int()] = std::make_pair(
                    row[1].get_float(), row[2].get_float());
            }

            if (pos.size() < graph.GetNodes()) {
                throw std::runtime_error("Position file has " +
                    std::to_string(pos.size()) + " lines "
                    "while graph has " + std::to_string(graph.GetNodes())
                    + " nodes.");
            }
        }

        if (cube) {
            graph = hypercube();
        }
        if (tesseract) graph = hypercube_4();
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
            std::ofstream graph_out(file);
            graph_out << std::string(final_svg);
        }
        else if (still) {
            auto& final_svg = frames.back();
            final_svg.autoscale();
            std::ofstream graph_out(file);
            graph_out << std::string(final_svg);
        }
        else {
            auto final_svg = SVG::frame_animate(frames, 5);
            std::ofstream graph_out(file);
            graph_out << std::string(final_svg);
        }
    }
    catch (std::runtime_error& err) {
        std::cout << err.what() << std::endl;
    }

    return 0;
}
#include "json_reader.h"
#include "request_handler.h"
#include <fstream>
#include <iostream>

class CoutRedirect {
public:
    CoutRedirect(std::streambuf* new_buffer) 
        : old_buffer_(std::cout.rdbuf(new_buffer)) {}
    
    ~CoutRedirect() {
        std::cout.rdbuf(old_buffer_);
    }

private:
    std::streambuf* old_buffer_;
};

int main(int argc, char* argv[]) {
    TransportCatalogue catalogue;
    
    if (argc > 4) {
        std::cerr << "Usage: " << argv[0] << " [input.json] [output.json] [output.svg]\n";
        return 1;
    }

    std::ifstream input_file;
    std::ofstream json_output_file;
    std::ofstream svg_output_file;
    
    std::istream* input = &std::cin;

    // Обработка входного файла
    if (argc > 1 && std::string(argv[1]) != "-") {
        input_file.open(argv[1]);
        if (!input_file) {
            std::cerr << "Failed to open input file: " << argv[1] << "\n";
            return 2;
        }
        input = &input_file;
    }

    JsonReader json_doc(*input);
    json_doc.FillCatalogue(catalogue);

    const auto& stat_requests = json_doc.GetStatRequests();
    const auto& render_settings = json_doc.GetRenderSettings();
    const auto& renderer = json_doc.FillRenderSettings(render_settings);
    const auto& routing_settings = json_doc.FillRoutingSettings(json_doc.GetRoutingSettings());
    const TransportRouter router = {routing_settings, catalogue };

    RequestHandler rh(catalogue, renderer, router);
    
    // Обработка JSON вывода
    if (argc > 2 && std::string(argv[2]) != "-") {
        json_output_file.open(argv[2]);
        if (!json_output_file) {
            std::cerr << "Failed to open JSON output file: " << argv[2] << "\n";
            return 3;
        }
        CoutRedirect redirect(json_output_file.rdbuf());
        json_doc.ProcessRequests(stat_requests, rh);
    } else {
        // Вывод JSON в stdout
        json_doc.ProcessRequests(stat_requests, rh);
    }
    
    // Обработка SVG вывода (только если есть третий аргумент)
    if (argc > 3) {
        if (std::string(argv[3]) != "-") {
            svg_output_file.open(argv[3]);
            if (!svg_output_file) {
                std::cerr << "Failed to open SVG output file: " << argv[3] << "\n";
                return 4;
            }
            rh.RenderMap().Render(svg_output_file);
        } else {
            // Если третий аргумент "-", выводим SVG в stdout
            rh.RenderMap().Render(std::cout);
        }
    }
    // Если третий аргумент не указан, SVG не выводится

    return 0;
}
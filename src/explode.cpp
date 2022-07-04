// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <iostream>
#include <filesystem>
#include <composer/file.hpp>
#include <cmdline/cmdline.h>

int main(int argc, char *argv[]) noexcept {
    cmdline::parser options;
    options.set_program_name("composer-explode");
    options.add<std::string>("type", 't', "Shader collection type (ps, vs).", true, "", cmdline::oneof<std::string>("vs", "ps"));
    options.add<std::string>("output", 'o', "Output folder for individual shader files.", false);
    options.add("help", 'h', "Print this message.");
    options.footer("<input-file>");
    options.parse_check(argc, argv);

    auto shader_collection_type = options.get<std::string>("type");

    auto rest = options.rest();
    if(rest.empty()) {
        std::cout << "need option: input file path" << std::endl;
        std::exit(1);    
    }

    std::filesystem::path input_file = rest[0];
    std::filesystem::path output_directory = input_file.parent_path();

    if(options.exist("output")) {
        output_directory = options.get<std::string>("output");
    }

    try {
        if(shader_collection_type == "ps") {
            Composer::split_pixel_shader_collection_file(input_file, output_directory);
        }
        else {
            Composer::split_vertex_shader_collection_file(input_file, output_directory);
        }
    }
    catch(const std::runtime_error e) {
        std::cerr << e.what();
        std::exit(1);
    }

    std::cout << "shader collection exploded in folder: " << output_directory << std::endl;

    return 0;
}

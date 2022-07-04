// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <iostream>
#include <filesystem>
#include <composer/file.hpp>
#include <cmdline/cmdline.h>

int main(int argc, char *argv[]) noexcept {
    cmdline::parser options;
    options.set_program_name("composer --merge");
    options.add<std::string>("type", 't', "Shader collection type (ps, vs).", true, "", cmdline::oneof<std::string>("vs", "ps"));
    options.add<std::string>("output", 'o', "Output shader collection file.", false);
    options.add("help", 'h', "Print this message.");
    options.footer("<input-file>");
    options.parse_check(argc, argv);

    auto shader_collection_type = options.get<std::string>("type");

    auto rest = options.rest();
    if(rest.empty()) {
        std::cout << "need option: input directory path" << std::endl;
        std::exit(1);    
    }

    std::filesystem::path input_directory = rest[0];
    std::filesystem::path output_file = input_directory;
    output_file.replace_extension(".bin");

    if(options.exist("output")) {
        output_file = options.get<std::string>("output");
    }

    try {
        if(shader_collection_type == "ps") {
            Composer::create_pixel_shader_collection_file(input_directory, output_file);
        }
        else {
            Composer::create_vertex_shader_collection_file(input_directory, output_file);
        }
    }
    catch(const std::runtime_error e) {
        std::cerr << e.what();
        std::exit(1);
    }

    std::cout << "Merged shader collection file: " << output_file << std::endl;

    return 0;
}

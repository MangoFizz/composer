// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <iostream>
#include <filesystem>
#include <composer/file.hpp>
#include <cmdline/cmdline.h>

int main(int argc, char *argv[]) noexcept {
    cmdline::parser options;
    auto shader_profiles = cmdline::oneof<std::string>("ps_1_1", "ps_1_2", "ps_1_3", "ps_1_4", "ps_2_0", "ps_3_0", "vs_1_1", "vs_2_0", "vs_3_0"); 
    options.set_program_name("composer-compile");
    options.add<std::string>("entry-point", 'e', "Name of the function where execution begins (\"main\" by default).", false, "main");
    options.add<std::string>("profile", 'p', "Shader model; instruction set that will be used (ps_1_1, ps_1_2, ps_1_3, ps_1_4, ps_2_0, ps_3_0, vs_1_1, vs_2_0, vs_3_0).", true, "", shader_profiles);
    options.add<std::string>("includes-path", 'i', "Path to the directory containing the shader include files.", false, "");
    options.add<std::string>("output", 'o', "Compiled shader output file.", false);
    options.add("help", 'h', "Print this message.");
    options.footer("<input-file>");
    options.parse_check(argc, argv);

    auto entry_point = options.get<std::string>("entry-point");
    auto shader_profile = options.get<std::string>("profile");
    
    auto rest = options.rest();
    if(rest.empty()) {
        std::cout << "need option: input file path" << std::endl;
        std::exit(1);    
    }

    std::filesystem::path input_file = rest[0];
    std::filesystem::path output_file = input_file;
    output_file.replace_extension(".bin");

    if(options.exist("output")) {
        output_file = options.get<std::string>("output");
    }

    std::filesystem::path includes_path = input_file.parent_path() / "include";
    if(options.exist("includes-path")) {
        includes_path = options.get<std::string>("includes-path");
    }
    
    try {
        Composer::compile_shader_file(input_file, entry_point, shader_profile, output_file, includes_path);
    }
    catch(const std::runtime_error e) {
        std::cerr << e.what();
        std::exit(1);
    }

    std::cout << "compiled shader file: " << output_file << std::endl;

    return 0;
}

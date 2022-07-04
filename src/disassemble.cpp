// SPDX-License-Identifier: GPL-3.0-only

#include <string>
#include <iostream>
#include <filesystem>
#include <composer/file.hpp>
#include <cmdline/cmdline.h>

int main(int argc, char *argv[]) noexcept {
    cmdline::parser options;
    options.set_program_name("composer-disassemble");
    options.add<std::string>("output", 'o', "Disassembled shader output file.", false);
    options.add("help", 'h', "Print this message.");
    options.footer("<input-file>");

    if(argc == 1) {
        std::cout << options.usage() << std::endl;
        std::exit(0);
    }

    options.parse_check(argc, argv);

    auto rest = options.rest();
    if(rest.empty()) {
        std::cout << "need option: input file path" << std::endl;
        std::exit(1);    
    }

    std::filesystem::path input_file = rest[0];
    std::filesystem::path output_file = input_file;
    output_file.replace_extension(".asm");

    if(options.exist("output")) {
        output_file = options.get<std::string>("output");
    }
    
    try {
        Composer::disassemble_shader_file(input_file, output_file);
    }
    catch(const std::runtime_error e) {
        std::cerr << e.what();
        std::exit(1);
    }

    std::cout << "disassembled shader file: " << output_file << std::endl;

    return 0;
}

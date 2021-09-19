// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib::ini {

class Parser {
public:

    Parser(const std::filesystem::path& file_path) {
        
        std::ifstream ifs(file_path);

        if(!ifs.is_open()) {
            throw std::runtime_error("Error loading a file");
        }

        std::string m_buffer;

        ifs.seekg(0, std::ios::end);
        std::streampos size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        m_buffer.resize(size);
        ifs.read(reinterpret_cast<char*>(m_buffer.data()), size);

        m_lexer = std::make_unique<Lexer>(reinterpret_cast<byte*>(m_buffer.data()), m_buffer.size());
    }

    void parse() {

        m_lexer->tokenize();

        for(uint32 i = 0; i < m_buffer.size(); ++i) {
            
            
        }
    }

private:

    std::unique_ptr<Lexer> m_lexer;
    std::string m_buffer;

    std::list<std::map<std::string, std::string>> m_values;
    std::map<std::string, std::list<std::list<std::map<std::string, std::string>>>::iterator> m_sections;
};

}
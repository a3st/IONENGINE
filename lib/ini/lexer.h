// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib::ini {

struct Token {

    enum class Type {
        Commentary,
        SectionOpen,
        SectionClose,
        Equal
    };

    Type type;
    uint64 position;
};

class Lexer {
public:

    Lexer(const std::filesystem::path& file_path) {

        std::ifstream ifs(file_path, std::ios::end);
        if(!ifs.is_open()) {
            throw std::runtime_error("Error loading a file");
        }

        std::streampos position = ifs.tellg();
        ifs.seekg(std::ios::beg);
        ifs.read(reinterpret_cast<char*>(m_buffer.data()), position);
    }

    void tokenize() {

        for(uint32 i = 0; i < m_buffer.size(); ++i) {
            
            switch(m_buffer[i]) {
                case ';': m_tokens.emplace(Token { Token::Type::Commentary, i }); break;
                case '[': m_tokens.emplace(Token { Token::Type::SectionOpen, i }); break;
                case ']': m_tokens.emplace(Token { Token::Type::SectionClose, i }); break;
                case '=': m_tokens.emplace(Token { Token::Type::Equal, i }); break;
            }
        }
    }

    Token get_current() const { return m_tokens.top(); }
    std::optional<Token> get_next() {
        if(!m_tokens.empty()) {
            m_tokens.pop();
            return std::optional<Token>(m_tokens.top());
        } else {
            return std::nullopt;
        }
    }

private:

    std::string m_buffer;
    std::stack<Token> m_tokens;
};

}
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

class Node {
friend class Lexer;
public:

    Node(std::reference_wrapper<Token>& value, const std::optional<std::reference_wrapper<Node>>& next_node) 
        : m_value(value), m_next_node(next_node) {

    }

    std::optional<std::reference_wrapper<Node>> get_next() const { return m_next_node; }
    const std::reference_wrapper<Token>& get_value() const { return m_value; }

private:

    std::optional<std::reference_wrapper<Node>> m_next_node;
    std::reference_wrapper<Token> m_value;
};

class Lexer {
public:

    Lexer(const std::filesystem::path& file_path) {

        std::ifstream ifs(file_path);
        if(!ifs.is_open()) {
            throw std::runtime_error("Error loading a file");
        }

        ifs.seekg(0, std::ios::end);
        std::streampos size = ifs.tellg();
        ifs.seekg(0, std::ios::beg);
        m_buffer.resize(size);
        ifs.read(reinterpret_cast<char*>(m_buffer.data()), size);
    }

    Lexer(const byte* data, const usize size) {

        m_buffer.resize(size);
        std::memcpy(m_buffer.data(), data, size);
    }

    void tokenize() {

        for(uint32 i = 0; i < m_buffer.size(); ++i) {
            
            switch(m_buffer[i]) {
                case ';': m_tokens.emplace_back(Token { Token::Type::Commentary, i }); break;
                case '[': m_tokens.emplace_back(Token { Token::Type::SectionOpen, i }); break;
                case ']': m_tokens.emplace_back(Token { Token::Type::SectionClose, i }); break;
                case '=': m_tokens.emplace_back(Token { Token::Type::Equal, i }); break;
            }
        }

        for(auto& it = m_tokens.begin(); it != m_tokens.end(); ++it) {
            m_nodes.emplace_back(*it, std::nullopt);
        }

        for(auto& it = m_nodes.begin(); it != m_nodes.end(); ++it) {
            it->m_next_node = *std::next(it);
        }
    }

    std::optional<std::reference_wrapper<Node>> get_first() {
        return m_nodes.begin() != m_nodes.end() ? std::optional<std::reference_wrapper<Node>>(*m_nodes.begin()) : std::nullopt;
    }

private:

    std::string m_buffer;
    std::list<Token> m_tokens;
    std::list<Node> m_nodes;
};

}
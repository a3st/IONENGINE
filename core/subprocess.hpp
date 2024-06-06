// Copyright © 2020-2024 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::core
{
    inline auto subprocessExecute(std::span<std::string> const arguments) -> std::optional<std::vector<uint8_t>>
    {
        std::array<uint8_t, 256> buffer;
        std::vector<uint8_t> result;

        std::stringstream stream;

        bool isFirst = true;
        for (auto const& argument : arguments)
        {
            if (!isFirst)
            {
                stream << " ";
            }
            stream << argument;
            isFirst = false;
        }

        FILE* pipe = ::_popen(stream.str().c_str(), "rb");
        if (!pipe)
        {
            return std::nullopt;
        }

        try
        {
            while (pipe)
            {
                size_t const read_bytes = ::fread(buffer.data(), sizeof(uint8_t), 128, pipe);
                if (read_bytes > 0)
                {
                    result.insert(result.end(), buffer.begin(), std::next(buffer.begin(), read_bytes));
                }
                else
                {
                    break;
                }
            }
            _pclose(pipe);
            return result;
        }
        catch (...)
        {
            _pclose(pipe);
            return std::nullopt;
        }
    }
} // namespace ionengine::core
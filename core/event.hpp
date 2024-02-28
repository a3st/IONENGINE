#pragma once

namespace ionengine
{
    template <typename... Args>
    class Event
    {
      public:
        using handler_func_t = std::function<void(Args...)>;

        Event()
        {
        }

        auto connect(handler_func_t callback) -> void
        {
            handlers.emplace_back(callback);
        }

        auto operator()(Args... args)
        {
            for (auto const& handler : handlers)
            {
                handler(std::forward<Args>(args)...);
            }
        }

      private:
        std::list<handler_func_t> handlers;
    };
} // namespace ionengine
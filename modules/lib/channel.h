// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template<class Type>
class Sender;

template<class Type>
class Receiver;

template<class Type>
class Channel {

    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

public:

    Channel(Channel const&) = delete;

    Channel(Channel&&) noexcept = delete;

    Channel& operator=(Channel const&) = delete;

    Channel& operator=(Channel&&) noexcept = delete;

    void send(Type&& element) {

        std::unique_lock lock(_mutex);
        _queue.push(std::move(element));
    }

    void send(Type const& element) {

        std::unique_lock lock(_mutex);
        _queue.push(element);
    }

    Type receive() {

        std::unique_lock lock(_mutex);
        // Wait for elements block TODO!
        Type element = std::move(_queue.front());
        _queue.pop();
        return element;
    }

    bool try_receive(Type& element) {

        if(_mutex.try_lock()) {

            std::unique_lock lock(_mutex, std::adopt_lock);
            if(_queue.empty()) {
                return false;
            }
            element = std::move(_queue.front());
            _queue.pop();
            return true;
        }
        return false;
    }

private:

    Channel() = default;

    std::queue<Type> _queue;
    std::mutex _mutex;
};

template<class Type>
class Receiver {

    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

public:

    Receiver(Receiver const&) = default;

    Receiver(Receiver&&) noexcept = default;

    Receiver& operator=(Receiver const&) = default;

    Receiver& operator=(Receiver&&) noexcept = default;

    Type receive() {

        return _channel->receive();
    }

    bool try_receive(Type& element) {

        return _channel->try_receive(element);
    }

private:

    Receiver(std::shared_ptr<Channel<Type>> const& channel) : _channel(channel) { }

    std::shared_ptr<Channel<Type>> _channel;
};

template<class Type>
class Sender {

    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

public:

    Sender(Sender const&) = default;

    Sender(Sender&&) noexcept = default;

    Sender& operator=(Sender const&) = default;

    Sender& operator=(Sender&&) noexcept = default;

    void send(Type&& element) {

        _channel->send(element);
    }

    void send(Type const& element) {

        _channel->send(element);
    }

private:

    Sender(std::shared_ptr<Channel<Type>> const& channel) : _channel(channel) { }

    std::shared_ptr<Channel<Type>> _channel;
};

template<class Type>
inline std::tuple<Sender<Type>, Receiver<Type>> make_channel() {
    static_assert(std::is_copy_constructible_v<Type> || std::is_move_constructible_v<Type>, "Type should be copy-constructible or move-constructible");
    std::shared_ptr<Channel<Type>> channel(new Channel<Type>());
    return std::make_tuple(Sender<Type>(channel), Receiver<Type>(channel));
}

}

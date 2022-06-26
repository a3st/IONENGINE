// Copyright © 2020-2022 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::lib {

template <class Type>
class Sender;

template <class Type>
class Receiver;

///
/// Channel that allows you to transfer message between the receiver and the
/// sender (even in threads)
///
template <class Type>
class Channel {
    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

 public:
    Channel(Channel const&) = delete;

    Channel(Channel&&) noexcept = delete;

    Channel& operator=(Channel const&) = delete;

    Channel& operator=(Channel&&) noexcept = delete;

    ///
    /// Transfer of message
    /// @param element element to be transfer
    ///
    void send(Type&& element) {
        std::unique_lock lock(_mutex);
        _queue.push(std::move(element));
    }

    ///
    /// Transfer of message
    /// @param element element to be transfer
    ///
    void send(Type const& element) {
        std::unique_lock lock(_mutex);
        _queue.push(element);
    }

    ///
    /// Get the latest of message
    /// @return Last message in queue
    ///
    Type receive() {
        std::unique_lock lock(_mutex);
        assert(!_queue.empty() && "message queue is empty");
        Type element = std::move(_queue.front());
        _queue.pop();
        return element;
    }

    ///
    /// Try to get the latest of message
    /// @param element last message in queue
    /// @return The result of getting the message
    ///
    bool try_receive(Type& element) {
        if (_mutex.try_lock()) {
            std::unique_lock lock(_mutex, std::adopt_lock);
            if (_queue.empty()) {
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

///
/// Receiver belongs to the Channel object and allows you to receive messages
///
template <class Type>
class Receiver {
    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

 public:
    Receiver(Receiver const&) = default;

    Receiver(Receiver&&) noexcept = default;

    Receiver& operator=(Receiver const&) = default;

    Receiver& operator=(Receiver&&) noexcept = default;

    ///
    /// Get the latest of message
    /// @return Last message in queue
    ///
    inline Type receive() { return _channel->receive(); }

    ///
    /// Get the latest of message
    /// @return Last message in queue
    ///
    inline bool try_receive(Type& element) { return _channel->try_receive(element); }

 private:
    Receiver(std::shared_ptr<Channel<Type>> const& channel)
        : _channel(channel) {}

    std::shared_ptr<Channel<Type>> _channel;
};

///
/// Sender belongs to the channel object and allows you to send messages
///
template <class Type>
class Sender {
    friend std::tuple<Sender<Type>, Receiver<Type>> make_channel<Type>();

 public:
    Sender(Sender const&) = default;

    Sender(Sender&&) noexcept = default;

    Sender& operator=(Sender const&) = default;

    Sender& operator=(Sender&&) noexcept = default;

    ///
    /// Transfer of message
    /// @param element element to be transfer
    ///
    inline void send(Type&& element) { _channel->send(element); }

    ///
    /// Transfer of message
    /// @param element element to be transfer
    ///
    inline void send(Type const& element) { _channel->send(element); }

 private:
    Sender(std::shared_ptr<Channel<Type>> const& channel) : _channel(channel) {}

    std::shared_ptr<Channel<Type>> _channel;
};

///
/// Create a new channel
/// @return A tuple consisting of a receiver and a sender
///
template <class Type>
inline std::tuple<Sender<Type>, Receiver<Type>> make_channel() {
    static_assert(std::is_copy_constructible_v<Type> ||
                      std::is_move_constructible_v<Type>,
                  "Type should be copy-constructible or move-constructible");
    std::shared_ptr<Channel<Type>> channel(new Channel<Type>());
    return std::make_tuple(Sender<Type>(channel), Receiver<Type>(channel));
}

}  // namespace ionengine::lib

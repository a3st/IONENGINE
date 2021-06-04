// Copyright © 2020-2021 Dmitriy Lukovenko. All rights reserved.

#pragma once

namespace ionengine::platform {

enum class AddressFamily : uint16 {
    Unspecified = AF_UNSPEC,
    INet = AF_INET,
    IPX = AF_IPX,
    AppleTalk = AF_APPLETALK,
    NetBIOS = AF_NETBIOS,
    INet6 = AF_INET6,
    IrDA = AF_IRDA,
    Bluetooth = AF_BTH
};

struct SocketAddress {
    AddressFamily family;
    unsigned long ip;
    uint16 port;

    SocketAddress(const AddressFamily family_, const std::string& ip_, uint16 port_) : 
        family(family_), ip(inet_addr(ip_.c_str())), port(port_)  {
    }
};

class Socket {
public:

    Socket(const SocketAddress& address) {

    }

private:

    SocketAddress m_address;
};

}

/// \file tcp_socket.h
/// \brief This header is part of the synchrony socket system. It contains interfaces for Transmission Control Protocol sockets.
///
/// \author Raffaele D. Facendola - 2019

#pragma once

#include "syntropy/memory/memory_range.h"

#include "synchrony/network/network_endpoint.h"

namespace synchrony
{
    /************************************************************************/
    /* TCP SOCKET                                                           */
    /************************************************************************/

    /// \brief Base interface for TCP sockets.
    /// \author Raffaele D. Facendola - September 2019.
    class TCPSocket
    {
    public:

        /// \brief Virtual destructor.
        virtual ~TCPSocket() = default;

    };

    /************************************************************************/
    /* TCP SERVER                                                           */
    /************************************************************************/

    /// \brief Base interface for TCP server.
    /// This class is used to accept incoming TCP connections.
    /// \author Raffaele D. Facendola - September 2019.
    class TCPServer
    {
    public:

        /// \brief Virtual destructor.
        virtual ~TCPServer() = default;

        /// \brief Accept an incoming TCP connection.
        /// \return Returns a valid TCP socket if a connection could be established, returns nullptr otherwise.
        /// \remarks This method blocks until a connection is established or an exception occurs.
        virtual std::unique_ptr<TCPSocket> Accept() = 0;
    };

    /************************************************************************/
    /* TCP                                                                  */
    /************************************************************************/

    /// \brief Exposes TCP-related functionalities.
    /// \author Raffaele D. Facendola - September 2019.
    namespace TCP
    {
        /// \brief Connect to a remote TCP server.
        /// \param server Server interface to connect to.
        ///\ return Returns a valid TCP socket if a connection could be established with the provided server, returns false otherwise.
        std::unique_ptr<TCPSocket> Connect(const NetworkEndpoint& server);

        /// \brief Start a new TCP server.
        /// \param server Server interface to listen to.
        /// \param backlog Maximum number of simultaneous connections to the server.
        /// \return Returns a valid TCP server if the server could be created, returns nullptr otherwise.
        std::unique_ptr<TCPServer> StartServer(const NetworkEndpoint& server, std::int32_t backlog);
    }
}
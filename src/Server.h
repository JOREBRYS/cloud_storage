#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <boost/asio.hpp>

#include "DatabaseAccessor.h"
#include "FileManager.h"

class Server{
    private:
    boost::asio::ip::tcp::acceptor acceptor_;
    boost::asio::io_context& io_;
    short port_;
    std::ostream& logger_ = std::cout;
    DatabaseAccessor& db_;
    FileManager& file_manager_;

    void accept_handle();
    void terminate();

    public:
    Server(boost::asio::io_context& io, short port,
        DatabaseAccessor& db, std::ostream& logger,
        FileManager& file_manager):
    io_(io),
    port_(port),
    acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
    logger_(logger),
    db_(db),
    file_manager_(file_manager) {
        logger_ << "Сервер работает, слущает порт " << port << ";\n";
        accept_handle();
        file_manager_.start();
    }

    ~Server(){
        terminate();
    }
};

#endif
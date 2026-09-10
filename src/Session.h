#ifndef SESSION_H
#define SESSION_H

#include <boost/asio.hpp>
#include <string>
#include <array>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <vector>
#include "DatabaseAccessor.h"
#include "AccountInfo.h"
#include "FileManager.h"

class Session: public std::enable_shared_from_this<Session> {
    private:
    boost::asio::ip::tcp::socket socket_;
    boost::asio::streambuf read_buffer_{};
    std::ostream& logger_;
    DatabaseAccessor& db_;
    FileManager& file_manager_;

    AccountInfo account_;

    std::unordered_map<std::string,
        std::function<void(const std::vector<std::string>&)>> command_handlers_;

    void send_error(const std::string& message){
        write("ERR " + message);
    }

    void send_OK(){
        write("OK\n");
    }

    void send_OK(unsigned lines){
        write("OK " + std::to_string(lines) + "\n");
    }

    void init_command_handlers();
    void handle_login(const std::vector<std::string>& args);
    void handle_register(const std::vector<std::string>& args);
    void handle_list(const std::vector<std::string>& args);
    void handle_upload(const std::vector<std::string>& args);
    void handle_download(const std::vector<std::string>& args);

    void handle_open_dir(const std::vector<std::string>& args);
    void handle_back_to_parent_dir(const std::vector<std::string>& args);
    void handle_delete(const std::vector<std::string>& args);
    void handle_make_dir(const std::vector<std::string>& args);

    void read_until();
    void read_bytes(int bytes, const std::string& file_name);

    public:
    void start(){
        read_until();
    }

    void write(const std::string& query);
    void handle_command(const std::string& query);

    bool is_authenticated(){
        return account_.is_authenticated();
    }

    public:
    Session(boost::asio::ip::tcp::socket&& socket,
        std::ostream& logger,
        DatabaseAccessor& db,
        FileManager& file_manager):
        socket_(std::move(socket)), logger_(logger), db_(db), file_manager_(file_manager) {
            init_command_handlers();
        }

    ~Session(){
        boost::system::error_code err;
        socket_.close(err);
        if(err){
            logger_ << "Ошибка: " << err.message() << ";\n";
        }
    }
};

#endif
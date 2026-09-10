#include "Server.h"
#include "Session.h"
#include <memory>
#include <chrono>
#include <thread>

using namespace std;

void Server::accept_handle(){
    acceptor_.async_accept(
        [this] (const boost::system::error_code& err, boost::asio::ip::tcp::socket socket) {
            if(err == boost::asio::error::operation_aborted){
                logger_ << "Произошла критическая ошибка!\n";
                return;
            }
            else if(err){
                logger_ << "При подключении произошла ошибка.\n";
                logger_ << "Работа сервера приостановленаж\n";
                this_thread::sleep_for(chrono::milliseconds(500));
                logger_ << "Работа сервера востановлена;\n";
            }
            else{
                logger_ << "Клиент подключился.\n"; 
                make_shared<Session>(std::move(socket), logger_, db_, file_manager_)->start();
            }
            accept_handle();
        }
    );
}

void Server::terminate(){
    logger_ << "Сервер завершил свою работу.\n";
}
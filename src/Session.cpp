#include "Session.h"
#include <memory>
#include <cstddef>
#include <algorithm>
#include "Parser.h"

using namespace std;

void Session::init_command_handlers(){
    command_handlers_["LOGIN"] = [this] (const vector<string>& args) {handle_login(args); };
    command_handlers_["REGISTER"] = [this] (const vector<string>& args) {handle_register(args); };
    command_handlers_["LIST"] = [this] (const vector<string>& args) {handle_list(args); };
    command_handlers_["UPLOAD"] = [this] (const vector<string>& args) {handle_upload(args); };
    command_handlers_["DOWNLOAD"] = [this] (const vector<string>& args) {handle_download(args); };
    command_handlers_["DELETE"] = [this] (const vector<string>& args) {handle_delete(args); };
    command_handlers_["MAKE_DIR"] = [this] (const vector<string>& args) {handle_make_dir(args); };
    command_handlers_["OPEN"] = [this] (const vector<string>& args) {handle_open_dir(args); };
    command_handlers_["BACK"] = [this] (const vector<string>& args) {handle_back_to_parent_dir(args); };
}

void Session::handle_register(const vector<string>& args){
    if(args.size() != 2) { send_error("Команда принемает только два аргумента;\n"); }
    else {
         std::size_t id = db_.register_user(args[0], args[1]);
         if(id == 0){ send_error("Пользователь с таким логином уже существует.\n"); }
         else {
            send_OK();

            account_ = AccountInfo(
                id,
                args[0],
                file_manager_.create_user_dir("user_" + to_string(id))
            );
         }
    }

    read_until();
}

void Session::handle_login(const vector<string>& args){
    if(args.size() != 2) { send_error("Команда принемает только два аргумента;\n"); }
    else {
        std::size_t id = db_.login(args[0], args[1]);
        if(id == 0) { send_error("Не правильный логин или пароль.\n"); }
        else {
            send_OK();

            account_ = AccountInfo(
                id,
                args[0],
                file_manager_.get_user_dir(id)
            );
        }
    }

    read_until();
}

void Session::handle_list(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 0) {send_error("Команда не принемает аргуметков\n"); }
    else{
        vector<string> names = file_manager_.get_list(account_.current_path_);
        std::size_t size = names.size();
        
        send_OK(size);
        for(std::size_t i = 0; i < size; i++){
            write(names[i] + "\n");
        }
    }

    read_until();
}

void Session::handle_upload(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 2) {send_error("Команда принемает два агрумента\n");}
    else{
        try{
            std::size_t bytes = std::stoi(args[1]);
            read_bytes(bytes, args[0]);
        }
        catch(const std::exception& e){
            send_error("Произошла ошибка\n");
            logger_ << "Ошибка " << e.what() << '\n';
        }
    }
    
    read_until();
}

void Session::handle_open_dir(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 1) {send_error("Команда принемает один агрумента\n");}
    else{
        auto path = account_.current_path_ / args[0];
        try{
            account_.current_path_ = file_manager_.open_user_dir(path);
            send_OK();
        }
        catch(const exception& e){
            send_error(string("ошибка: ") + e.what() + '\n');
        }
    }

    read_until();
}

void Session::handle_back_to_parent_dir(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 0) {send_error("Команда не принемает параметров.\n");}
    else{
        try{
            auto back = file_manager_.back_to_parent_dir(account_.current_path_);
            account_.current_path_ = back;
            send_OK();
        }
        catch(const std::exception& e){
            send_error(string(e.what()) + "\n");
        }
    }

    read_until();
}

void Session::handle_delete(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 1) { send_error("Команда принемает один агрумента\n"); }
    else{
        try{
            auto path = account_.current_path_ / args[0];
            bool result = file_manager_.delete_entry(path);
            if(!result ) { send_error("удаление не удалось.\n"); }

            send_OK();
        }
        catch(const std::exception& e){
            send_error(string(e.what()) + "\n");
        }
    }

    read_until();
}

void Session::handle_make_dir(const vector<string>& args){
        if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 1) { send_error("Команда принемает один агрумента\n"); }
    else{
        try{
            auto path = account_.current_path_ / args[0];
            file_manager_.create_user_dir(path);
            send_OK();
        }
        catch(const std::exception& e){
            send_error(string(e.what()) + "\n");
        }
    }

    read_until();
}

void Session::handle_download(const vector<string>& args){
    if(!is_authenticated()) {
        send_error("Пользователь не зарегистрирован\n");
        read_until();
        return;
    }

    if(args.size() != 1) { send_error("Команда принемает один агрумента\n"); }
    else{
        try{
            auto path = account_.current_path_ / args[0];
            size_t file_size = file_manager_.get_file_size(path);
            string file_data = file_manager_.get_file_data<string>(path);

            send_OK(file_data.size());
            write(file_data);
        }
        catch(const exception& e){
            send_error(string(e.what()) + "\n");
        }
    }

    read_until();
}

void Session::read_until(){
    auto self = shared_from_this();

    boost::asio::async_read_until(
        socket_,
        read_buffer_,
        '\n',
        [this, self](const boost::system::error_code& err, std::size_t len){
            if(err == boost::asio::error::eof){
                logger_ << "Клиент закрыл соединение;\n";
                return;
            }
            else if(err){
                logger_ << "Ошибка: " << err.message() << ";\n";
                return;
            }
            else{
                string query(
                    boost::asio::buffers_begin(read_buffer_.data()),
                    boost::asio::buffers_begin(read_buffer_.data()) + len
                );
                read_buffer_.consume(len);
                handle_command(query);
            }
        }
    );
}

void Session::read_bytes(int bytes,const string& file_name){
    auto self = shared_from_this();

    boost::asio::async_read(
        socket_,
        read_buffer_,
        boost::asio::transfer_exactly(bytes),
        [this, self, file_name](const boost::system::error_code& err, size_t len){
            if(err == boost::asio::error::eof){
                logger_ << "Клиент закрыл соединение;\n";
            }
            else if(err) {
                logger_ << "Ошибка: " << err.message() << ";\n";
            }
            else{
                string buf(
                    boost::asio::buffers_begin(read_buffer_.data()),
                    boost::asio::buffers_begin(read_buffer_.data()) + len
                ); 
                
                auto file_path = account_.current_path_ / file_name;
                file_manager_.file_output(file_path, buf);
                   
                read_buffer_.consume(len);
            }
            read_until();
        }
    );
}

void Session::write(const string& query){
    auto self = shared_from_this();
    auto report = make_shared<string>(query);

    boost::asio::async_write(
        socket_,
        boost::asio::buffer(report->data(), report->size()),
        [this, self, report](const boost::system::error_code& err, size_t len){
            if(err == boost::asio::error::eof){
                logger_ << "Клиент закрыл соедиение;\n";
            }
            else if(err){
                logger_ << "Ошибка: " << err.message() << ";\n";
            }
        }
    );
}

void Session::handle_command(const string& query){
    if(query.empty()){ return; }

    bool success = false;
    auto tokens = parse(query, success);
    if(!success || tokens.empty()){
        send_error("Ошибка синтаксиса команды.\n");
        read_until();
        return;
    }

    auto it = command_handlers_.find(tokens[0]);
    if(it != command_handlers_.end()){
        tokens.erase(tokens.begin());
        it->second(tokens);
    }
    else{
        send_error("неизвестная команда.\n");
        read_until();
    }
}
#ifndef DATABASEACCESSOR_H
#define DATABASEACCESSOR_H

#include <sqlite3.h>
#include <iostream>
#include "DatabaseInfrastructureCreater.h"
#include "Hasher.h"
#include <cstddef>

class DatabaseAccessor{
    private:
    sqlite3* db_;
    std::ostream& logger_;
    Hasher hasher_;

    public:
    DatabaseAccessor(std::ostream& logger): logger_(logger) {
        int res = sqlite3_open("cloud_storage.db", &db_);

        if(res){
            throw std::runtime_error("Не удалось получить доступ к базе данный.");
        }

        try{
            InfrastructureCreater creater;
            creater.create(db_);
        }
        catch(const std::exception& e){
            logger_ << e.what() << '\n';
        }
    }

    ~DatabaseAccessor(){
        if(db_) { sqlite3_close(db_); }
    }

    std::size_t register_user(const std::string& login, const std::string& password);
    std::size_t login(const std::string& login, const std::string& password);
};

#endif
#ifndef DATABASEINFRASTRUCTURECREATER_H
#define DATABASEINFRASTRUCTURECREATER_H

#include <sqlite3.h>
#include <stdexcept>

class InfrastructureCreater{
    private:

    bool create_users(sqlite3*& db){
        int exec = sqlite3_exec( db,
            "create table if not exists users( "
            "id integer primary key autoincrement, "
            "login text unique not null, "
            "password text not null, "
            "salt text not null, "
            "created_at data default (datetime('now')));",
            nullptr,
            nullptr,
            nullptr
        );

        if(exec != SQLITE_OK) { throw std::runtime_error(sqlite3_errmsg(db)); }

        return true;
    }

    public:
    bool create(sqlite3*& db){
        if(!db) {return false; }

        bool success = create_users(db);

        return success;
    }
};

#endif
#include "DatabaseAccessor.h"

using namespace std;

size_t DatabaseAccessor::register_user(const string& login, const string& password){
    if(!db_){
        logger_ << "нет подключения к базе данный;" << '\n';
        return 0;
    }

    sqlite3_stmt *stmt;
    int prepared = sqlite3_prepare_v2(
        db_,
        "insert into users(login, password, salt) values "
        "(?, ?, ?);",
        -1,
        &stmt,
        nullptr
    );
    if(prepared != SQLITE_OK){
        logger_ << "Ошибка: " << sqlite3_errmsg(db_) << '\n';
        return 0;
    }

    sqlite3_bind_text(
        stmt,
        1,
        login.c_str(),
        -1,
        nullptr
    );

    string salt = hasher_.generate_salt();
    string password_hash = hasher_.generate_hash(password, salt);

    sqlite3_bind_text(
        stmt,
        2,
        password_hash.c_str(),
        -1,
        nullptr
    );

    sqlite3_bind_text(
        stmt,
        3,
        salt.c_str(),
        -1,
        nullptr
    );

    int step = sqlite3_step(stmt);
    if(step != SQLITE_DONE){
        logger_ << "Ошибка: " << sqlite3_errmsg(db_) << '\n';
        sqlite3_finalize(stmt);
        return 0;
    }
    
    sqlite3_finalize(stmt);
    return sqlite3_last_insert_rowid(db_);
}

size_t DatabaseAccessor::login(const string& login, const string& password){
    if(!db_){
        logger_ << "нет подключения к базе данный;" << '\n';
        return 0;
    }

    sqlite3_stmt *stmt;
    int prepare = sqlite3_prepare_v2(
        db_,
        "select id, password, salt from users where login = ?;",
        -1,
        &stmt,
        nullptr
    );
    if(prepare != SQLITE_OK){
        logger_ << "Ошибка " << sqlite3_errmsg(db_) << '\n';
        return 0;
    }

    sqlite3_bind_text(
        stmt,
        1,
        login.c_str(),
        -1,
        nullptr
    );

    int step = sqlite3_step(stmt);
    if(step != SQLITE_ROW){
        sqlite3_finalize(stmt);
        return 0;
    }

    int id = sqlite3_column_int(stmt, 0);
    string storage_password_hash = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    string salt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));

    string generated_password_hash = hasher_.generate_hash(password, salt);

    sqlite3_finalize(stmt);

    if(generated_password_hash != storage_password_hash){
        return 0;
    }
    
    return id;
}
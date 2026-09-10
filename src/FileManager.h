#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <cstddef>

class FileManager{
    private:
    std::filesystem::path storage_dir_;

    void create_dir(const std::filesystem::path& dir_){
        std::error_code err;
        std::filesystem::create_directories(dir_, err);
        
        if(err){
            throw std::runtime_error("При создании папки произошла ошибка. " + err.message());
        }
    }
    
    public:
    explicit FileManager(const std::filesystem::path& path): storage_dir_(path) {}

    void start(){
        if(!std::filesystem::exists(storage_dir_)){
            create_dir(storage_dir_);
        }
        else if(!std::filesystem::is_directory(storage_dir_)){
            throw std::runtime_error("Файл с таким именем уже существует.");
        }
    }

    std::filesystem::path create_user_dir(const std::string& dir_name){
        std::filesystem::path user_dir = storage_dir_ / dir_name;

        if(std::filesystem::exists(user_dir)){
            throw std::runtime_error("объект с таким именеи уже существует.");
        }

        std::error_code err;
        std::filesystem::create_directory(user_dir, err);
        if(err){
            throw std::runtime_error("при создании папки пользователя произошла ошибка. " + err.message());
        }

        return user_dir;
    }

        std::filesystem::path create_user_dir(const std::filesystem::path& dir_path){

        if(std::filesystem::exists(dir_path)){
            throw std::runtime_error("объект с таким именеи уже существует.");
        }

        std::error_code err;
        std::filesystem::create_directory(dir_path, err);
        if(err){
            throw std::runtime_error("при создании папки пользователя произошла ошибка. " + err.message());
        }

        return dir_path;
    }

    std::filesystem::path get_user_dir(std::size_t user_id){
        std::filesystem::path user_dir = storage_dir_ / ("user_" + std::to_string(user_id));

        if(!std::filesystem::exists(user_dir)){
            create_user_dir(user_dir);
        }
        else if(!std::filesystem::is_directory(user_dir)){
            throw std::runtime_error("объект с таким именем уже существует");
        }

        return user_dir;
    }

    std::vector<std::string> get_list(const std::filesystem::path& path);

    template <typename container>
    void file_output(const std::filesystem::path& file_path, const container& buf){
        std::ofstream file(file_path.string(), std::ios::binary);

        if(!file.is_open()){
            throw std::runtime_error("Не удалось создать файл: " + file_path.string());
        }

        file.write(reinterpret_cast<const char*>(buf.data()), buf.size());

        if(!file){
            throw std::runtime_error("Ошибка записи в файл.");
        }

        file.close();
    }

    std::filesystem::path open_user_dir(const std::filesystem::path& dir_path);

    std::filesystem::path back_to_parent_dir(const std::filesystem::path& dir_path);

    bool delete_entry(const std::filesystem::path& entry_path);

    std::size_t get_file_size(const std::filesystem::path& path);

    template <typename container>
    container get_file_data(const std::filesystem::path& file_path){
        if(!std::filesystem::exists(file_path) || is_directory(file_path)){
            throw std::runtime_error("не удалось найти файл");
        }

        std::ifstream file(file_path);
        if(!file.is_open()){
            throw std::runtime_error("не удалось открыть файл по пути: " + file_path.string() );
        }

        std::size_t size = get_file_size(file_path);

        container buf;
        buf.resize(size);

        file.read(reinterpret_cast<char*>(buf.data()), size);

        file.close();

        return buf;
    }
};

#endif
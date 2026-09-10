#include "FileManager.h"

using namespace std;
using namespace std::filesystem;

vector<string> FileManager::get_list(const path& path){
    if(!exists(path) || !is_directory(path)){
        throw std::runtime_error("Такой директории не существует или объект с таким именем уже существует");
    }

    vector<string> object_names{};

    string name = "";
    for(const auto& entry: directory_iterator(path)){
        name.clear();
        name += entry.is_directory() ? "[DIR] " : "[FILE] ";
        name += entry.path().filename();

        object_names.push_back(name);
    }

    return object_names;
}

path FileManager::open_user_dir(const path& dir_path){
    if(!exists(dir_path) || !is_directory(dir_path)) {
        throw std::runtime_error("Нет такой папки.");
    }

    return dir_path;
}

path FileManager::back_to_parent_dir(const path& dir_path){
    auto parent = dir_path.parent_path();
    if(parent == storage_dir_){
        return dir_path;
    }

    return parent;
}

bool FileManager::delete_entry(const path& entry_path){
    if(!exists(entry_path)) { throw std::runtime_error("Нет объкта с таким именем."); }

    if(entry_path.parent_path() == storage_dir_) { return false; }

    error_code err;
    if(is_regular_file(entry_path)){
        remove(entry_path, err);
    }
    else{
        remove_all(entry_path, err);
    }

    if(err){
        throw std::runtime_error(err.message());
    }

    return true;
}

size_t FileManager::get_file_size(const path& path){
    if(!exists(path) || is_directory(path)){
        throw runtime_error("Можно узнать только размер файла");
    }

    error_code err;
    size_t size = file_size(path, err);

    if(err){
        throw runtime_error("при попытки узнать размер файла произошла ошибка.");
    }

    return size;
}
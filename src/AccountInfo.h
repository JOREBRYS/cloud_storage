#ifndef ACCOUNTINFO_H
#define ACCOUNTINFO_H

#include <cstddef>
#include <string>
#include <filesystem>

struct AccountInfo{
    std::size_t id_;
    std::string login_;
    std::filesystem::path current_path_;

    AccountInfo(): id_(0), login_("") {}
    AccountInfo(std::size_t id,
        const std::string& login,
        std::filesystem::path& current_path) noexcept :
        id_(id), login_(login), current_path_(current_path) {}

    AccountInfo(std::size_t id,
        const std::string& login,
        std::filesystem::path&& current_path) noexcept :
        id_(id), login_(login), current_path_(std::move(current_path)) {}

    bool is_authenticated(){
        return id_ != 0
            && !login_.empty();
    }
};

#endif
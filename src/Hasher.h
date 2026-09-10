#ifndef HASHER_H
#define HASHER_H

#include <string>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/rand.h>
#include <cstddef>
#include <stdexcept>
#include <sstream>
#include <iomanip>

class Hasher{
    private:
    static constexpr std::size_t salt_len = 16;
    static constexpr const char* pepper = "jorebrys";
    static constexpr std::size_t iter_amount = 5000;

    std::string get_line_in_hex(const std::string& line){
        std::stringstream ss;
        std::size_t len = line.size();

        for(std::size_t i = 0; i < len; i++){
            ss << std::hex << std::setw(2) << std::setfill('0')
                << static_cast<int>(static_cast<unsigned char>(line[i]));
        }

        return ss.str();
    }

    public:

    std::string generate_hash(const std::string& line, const std::string& salt){
        std::string str = "";
        str += pepper;
        str += line;

        std::string hash(SHA256_DIGEST_LENGTH, ' ');

        int result = PKCS5_PBKDF2_HMAC(
            reinterpret_cast<const char*>(&str[0]), str.size(),
            reinterpret_cast<const unsigned char*>(&salt[0]), salt.size(),
            static_cast<int>(iter_amount), 
            EVP_sha256(),
            SHA256_DIGEST_LENGTH,
            reinterpret_cast<unsigned char*>(&hash[0])
        );

        if(result != 1){
            throw std::runtime_error("при генерации хэша что-то пошло не так.");
        }

        return get_line_in_hex(hash);
    }

    std::string generate_salt(){
        std::string salt(salt_len, '\0');
        bool result = RAND_bytes(reinterpret_cast<unsigned char*>(&salt[0]), salt_len);

        if(result != 1){
            throw std::runtime_error("Не удалось сгенерировать соль.");
        }

        return salt;
    }
};

#endif
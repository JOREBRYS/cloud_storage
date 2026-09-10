#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <sstream>
#include <string>
#include <cstddef>

#include <iostream>

inline std::vector<std::string> parse(const std::string& query, bool& success) noexcept{
    if(query.empty()) { success = false; return{}; }
    
    std::vector<std::string> tokens{};
    std::size_t n = query.size();

    std::size_t left = 0;
    for(; left < n && std::isspace(static_cast<unsigned char>(query[left])); left++);

    std::size_t right = query.size() - 1;
    for(;
        right > 0 &&
        (std::isspace(static_cast<unsigned char>(query[right]))
        || static_cast<unsigned char>(query[right]) == '\r'
        || static_cast<unsigned char>(query[right]) == '\n');
        right--);
    if(right == 0 && query[right] == ' '){
        success = false;
        return {};
    }

    std::size_t pos = query.find(' ', left);
    if(pos == std::string::npos){
        tokens.push_back(query.substr(left, right - left + 1));
        success = true;
        return tokens;
    }
    else{
        tokens.push_back(query.substr(left, pos - left));
        left = pos + 1;
    }

    while(left <= right){
        for(;left < n && std::isspace(static_cast<unsigned char>(query[left])); left++);

        if(query[left] == '\"'){
            left++;
            std::size_t pos = query.find('\"', left);
            if(pos != std::string::npos){
                tokens.push_back(query.substr(left, pos - left));
                left = pos + 1;
            }
            else{
                success = false;
                return tokens;
            }
        }
        else{
            std::size_t pos = query.find(' ', left);
            if(pos != std::string::npos){
                tokens.push_back(query.substr(left, pos - left));
                left = pos + 1;
            }
            else{
               tokens.push_back(query.substr(left, right - left + 1));
               break;
            }
        }
    }

    std::cout << query << '\n';
    for(auto& t : tokens) std::cout << t << '|';
    std::cout << "|\n";

    success = true;
    return tokens;
}


#endif
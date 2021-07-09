
#include <string>
#include "utils.hpp"

std::string to_string(int n)
{
    char *c_str_num;
    std::string str_num;
    
    c_str_num = _ft_itoa(n);
    str_num = std::string(c_str_num);
    free(c_str_num);
    return str_num;
}
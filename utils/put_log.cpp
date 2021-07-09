
#include <unistd.h>
#include <iostream>

float standard_delay = 0;
bool is_on = false;

/*
void put_log(std::string msg, float delay_sec)
{
    if (is_on)
    {
        usleep(1000 * 1000 * delay_sec);    
        std::cout << msg << std::endl;    
    }
}

void put_log_ip_and_port(std::string msg, std::string ip, unsigned int port, float delay_sec)
{
    if (is_on)
    {
        usleep(1000 * 1000 * delay_sec);    
        std::cout << msg << " " << ip << " port: ";
        std::cout << port << std::endl;   
    }
}


void put_log_count(std::string msg, int count, float delay_sec)
{
    if (is_on)
    {
        usleep(1000 * 1000 * delay_sec);    
        std::cout << msg << count << std::endl;
    }
}
*/

void put_log_ip_port_fd(std::string msg, std::string ip, unsigned int port, int fd, float delay_sec)
{
    if (is_on)
    {
        usleep(1000 * 1000 * delay_sec);    
        std::cout << msg << " " << ip << " port: ";
        std::cout << port << " FD: " << fd << std::endl;   
    }
}
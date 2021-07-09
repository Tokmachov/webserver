
#include <string>
#include <unistd.h>

#define BUFF_SIZE 4096

int writeFileToBuffer(int fd, std::string *buff)
{
    int r;
    char buf[BUFF_SIZE + 1];
    std::string tmp_buf;
    
    while ((r = read(fd, buf, BUFF_SIZE)) > 0)
        tmp_buf += std::string(buf, r);
    if (r < 0)
        return -1;
    *buff = tmp_buf;
    return 0;
}

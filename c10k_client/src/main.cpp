#include <iostream>

#include "epoll_cli.h"
#include "sysdata.h"

using namespace std;

//g++ main.cpp epoll_cli.cpp sysdata.cpp wrap.cpp -lpthread

int main()
{
    epoll_init();

    epoll_while();

    epoll_close();

    return 0;
}

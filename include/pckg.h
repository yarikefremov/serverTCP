#ifndef PCKG_H
#define PCKG_H

struct authpckg{
    unsigned char flag = 0u;
    bool loginflag; //0 - register, 1 - login
    char username[33];
    char password[33];
};

struct msgpckg{
    unsigned char flag = 1u;
    char srcname[33];
    char dstname[33];
    bool rsrv;
    char msg[100];
};

struct acceptpckg{
    unsigned char flag = 2u;
    bool loginflag; // 0 - register, 1 - login
    bool ans; //0 - no, 1 - yes
};

#endif // PCKG_H

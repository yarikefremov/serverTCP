#include "../include/dbtxt.h"


void printString(const char* str){
    for(int i = 0; *(str+i) != char(0); ++i)
        std::cout<<*(str+i);
    std::cout<<std::endl;
}

DBTxt::DBTxt(const std::string& filename) {
    this->filename = filename;
    std::fstream s{filename, s.in};

    if (!s.is_open())
        std::cout << "failed to open " << filename << '\n';
    else{
        char buf[33*2];
        while(!s.eof()){
            s.getline(buf, 33, ' ');
            if(buf[0] == 0) continue;
            s.getline(buf+32, 33);
            users[buf] = buf+32;
            printString(buf);
            printString(buf+32);
        }
    }
    for(const std::pair<const std::string, std::string>& el: users)
        std::cout<< "Key: " << el.first << " Value: " << el.second << std::endl;
    std::cout<< "Test function registerUser: " << "0 = " << registerUser("meepo", "123456") << " 1 = " << registerUser("NotAmeepo", "123456");
    std::cout<< "Test function loginUser: " << "0 = " << loginUser("meepo", "1223456") << " 1 = " << loginUser("NotAmeepo", "123456");
    s.close();
}

bool DBTxt::registerUser(const std::string& username, const std::string& password){
    if(users.find(username) != 0) return false;
    users[username] = password;
    return true;
}

bool DBTxt::loginUser(const std::string& username, const std::string& password){
    auto a = users.find(username);
    if(a == 0) return false;
    if(password != a->second) return false;
    return true;
}

DBTxt::~DBTxt(){
    std::fstream s{filename, s.out | s.trunc};
    if (!s.is_open())
        std::cout << "failed to open " << filename << '\n';
    else{
        for(const std::pair<const std::string, std::string>& el: users)
            s << el.first << ' ' << el.second << std::endl;
    }
    users.clear();
    s.close();
}

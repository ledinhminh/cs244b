#include <string>

struct FSException : public std::exception{
    std::string msg;
    FSException(std::string _msg): msg(_msg){}
    const char* what() const throw(){
        return msg.c_str();
    }
    virtual ~FSException() throw();
};
    

#include <string>

class FSException : public std::exception {
private:
    std::string msg;
public:
    FSException(std::string _msg): msg(_msg) {};
    const char *what() const throw() {
        return msg.c_str();
    }
    ~FSException() throw() {};
};


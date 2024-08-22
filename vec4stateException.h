#ifndef VEC4STATEEXCEPTION_H
#define VEC4STATEEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

class vec4stateException : public exception {
private:
    string message;

public:
    // Constructor to initialize the exception without a custom message
    explicit vec4stateException() {
        message = "vec4stateException occurred";
    }
    // Constructor to initialize the exception with a custom message
    explicit vec4stateException(const string& msg) : message(msg) {}

    // Override the what() method to return the custom message
    virtual const char* what() const noexcept override {
        return message.c_str();
    }
};

class vec4stateExceptionInvalidIndex : public vec4stateException {
public:
    explicit vec4stateExceptionInvalidIndex(const string& msg) : vec4stateException(msg) {}
};

class vec4stateExceptionInvalidRange : public vec4stateException {
private:
    long long end;
    long long start;
public:
    explicit vec4stateExceptionInvalidRange(long long end, long long start) : end(end), start(start) {};
    const char* what() const noexcept override {
        return ("Part select: " + to_string(start) + " to " + to_string(end) + " is out of order.").c_str();
    }
};

class vec4stateExceptionInvalidSize : public vec4stateException {
public:
    explicit vec4stateExceptionInvalidSize(const string& msg) : vec4stateException(msg) {}
};

class vec4stateExceptionUnknownVector : public vec4stateException {
public:
    explicit vec4stateExceptionUnknownVector(const string& msg) : vec4stateException(msg) {}
};

class vec4stateExceptionInvalidInput : public vec4stateException {
public:
    explicit vec4stateExceptionInvalidInput(const string& msg) : vec4stateException(msg) {}
};

class vec4stateExceptionInvalidOperation : public vec4stateException {
public:
    explicit vec4stateExceptionInvalidOperation(const string& msg) : vec4stateException(msg) {}
};

#endif // VEC4STATEEXCEPTION_H
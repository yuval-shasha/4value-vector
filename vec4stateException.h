/**
 * @file vec4stateException.h
 * @brief Declaration and implementation of the vec4stateException class.
 * 
 * This file contains the declaration and implementation of the vec4stateException class, which is the base class for all exceptions thrown by the vec4state class.
 * 
 * @author Mia Ekheizer, Yuval Shasha
 * @date 2024-08-24
 */

#ifndef VEC4STATEEXCEPTION_H
#define VEC4STATEEXCEPTION_H

#include <exception>
#include <string>

using namespace std;

/**
 * @class vec4stateException
 * @brief This class represents the base class for all exceptions thrown by the vec4state class.
 * 
 * This class is the base class for all exceptions thrown by the vec4state class. It inherits from the std::exception class.
 */
class vec4stateException : public exception {
public:
    /**
     * @brief Default constructor for the vec4stateException class.
     * 
     * Initializes the exception with a default message.
     * 
     */
    explicit vec4stateException() {
        message = "vec4stateException occurred";
    }

    /**
     * @brief Constructor for the vec4stateException class.
     * 
     * Initializes the exception with the given message.
     * 
     * @param msg The message of the exception.
     */
    explicit vec4stateException(const string& msg) : message(msg) {}

    /**
     * @brief Get the message of the exception.
     * 
     * @return The message of the exception.
     */
    virtual const char* what() const noexcept override {
        return message.c_str();
    }

private:
    /**
     * @brief The message of the exception.
     */
    string message;
};

/**
 * @class vec4stateExceptionInvalidIndex
 * @brief This class represents an exception thrown when an invalid index is given.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionInvalidIndex : public vec4stateException {
public:
    explicit vec4stateExceptionInvalidIndex(const string& msg) : vec4stateException(msg) {}
};

/**
 * @class vec4stateExceptionInvalidRange
 * @brief This class represents an exception thrown when an invalid part select is given.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionInvalidRange : public vec4stateException {
private:
    /**
     * @brief The end of the part select.
     */
    long long end;

    /**
     * @brief The start of the part select.
     */
    long long start;
public:
    /**
     * @brief Constructor for the vec4stateExceptionInvalidRange class.
     * 
     * Initializes the exception with the given start and end values.
     * 
     * @param end The end of the part select.
     * @param start The start of the part select.
     */
    explicit vec4stateExceptionInvalidRange(long long end, long long start) : end(end), start(start) {};

    /**
     * @brief Get the message of the exception.
     * 
     * @return A string containing the information about the wrong part select.
     */
    const char* what() const noexcept override {
        return ("Part select: " + to_string(start) + " to " + to_string(end) + " is out of order.").c_str();
    }
};

/**
 * @class vec4stateExceptionInvalidSize
 * @brief This class represents an exception thrown when an invalid vector size is given.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionInvalidSize : public vec4stateException {
public:
    /**
     * @brief Constructor for the vec4stateExceptionInvalidSize class.
     * 
     * Initializes the exception with the given message.
     * 
     * @param msg The message of the exception.
     */
    explicit vec4stateExceptionInvalidSize(const string& msg) : vec4stateException(msg) {}
};

/**
 * @class vec4stateExceptionUnknownVector
 * @brief This class represents an exception thrown when a vector that holds unknown bits is given.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionUnknownVector : public vec4stateException {
public:
    /**
     * @brief Constructor for the vec4stateExceptionUnknownVector class.
     * 
     * Initializes the exception with the given message.
     * 
     * @param msg The message of the exception.
     */
    explicit vec4stateExceptionUnknownVector(const string& msg) : vec4stateException(msg) {}
};

/**
 * @class vec4stateExceptionInvalidInput
 * @brief This class represents an exception thrown when an invalid input is given.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionInvalidInput : public vec4stateException {
public:
    /**
     * @brief Constructor for the vec4stateExceptionInvalidInput class.
     * 
     * Initializes the exception with the given message.
     * 
     * @param msg The message of the exception.
     */
    explicit vec4stateExceptionInvalidInput(const string& msg) : vec4stateException(msg) {}
};

/**
 * @class vec4stateExceptionInvalidOperation
 * @brief This class represents an exception thrown when an invalid operation is performed.
 * 
 * Inherits from the vec4stateException class.
 */
class vec4stateExceptionInvalidOperation : public vec4stateException {
public:
    /**
     * @brief Constructor for the vec4stateExceptionInvalidOperation class.
     * 
     * Initializes the exception with the given message.
     * 
     * @param msg The message of the exception.
     */
    explicit vec4stateExceptionInvalidOperation(const string& msg) : vec4stateException(msg) {}
};

#endif // VEC4STATEEXCEPTION_H
//
// Created by Nathan White on 12/5/22.
//

#ifndef THREADED_SECANT_METHOD_TCC
#define THREADED_SECANT_METHOD_TCC

#include <cmath>
#include <functional>
#include <algorithm>
#include <execution>
#include <vector>
#include <thread>
#include <hash_map>


// create a template that accepts a return type R, and a function template std::function<R(...)>
template<typename R, std::function<R(...)> Phi>
class SecantMethod {

public: /* Constructors */

    // default constructor
    SecantMethod() = default;

    // constructor that accepts a function template
    explicit SecantMethod(std::function<R(...)> Phi) : Phi(Phi) {}

    // constructor that accepts a function template and a tolerance
    SecantMethod(std::function<R(...)> Phi, R tolerance) : Phi(Phi), tolerance(tolerance) {}

    // constructor that accepts a function template, a tolerance, and a maximum number of iterations
    SecantMethod(std::function<R(...)> Phi, R tolerance, int maxIterations) : Phi(Phi), tolerance(tolerance),
                                                                            maxIterations(maxIterations) {}


private:


public:


};


#endif

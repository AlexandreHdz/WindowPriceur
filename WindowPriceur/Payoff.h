#pragma once


#ifndef Payoff_h
#define Payoff_h

#include <stdio.h>

#include <algorithm>

class PayOff {
public:
    PayOff();
    virtual ~PayOff();
    virtual double valeur(double S, double K) = 0;

};

class PayOffCall : public PayOff {
public:
    PayOffCall();
    virtual ~PayOffCall();
    virtual double valeur(double S, double K);

};

class PayOffPut : public PayOff {
public:
    PayOffPut();
    virtual ~PayOffPut();
    virtual double valeur(double S, double K);
};


#include "Payoff.h"


PayOff::PayOff() {
}
PayOff::~PayOff() {
}

PayOffCall::PayOffCall() {
}

PayOffCall::~PayOffCall() {
}
//Payoff d'un Call
double PayOffCall::valeur(double S, double K) {
    return std::max(S - K, 0.0); // Standard European call pay-off
}

PayOffPut::PayOffPut() {
}
PayOffPut::~PayOffPut() {
}

//Payoff d'un put
double PayOffPut::valeur(double S, double K) {
    return std::max(K - S, 0.0); // Standard European put pay-off
}

#endif /* Payoff_h */



#ifndef Option_h
#define Option_h

#include <vector>
#include "Data.h"
#include "Payoff.h"



class Option {
protected:
    PayOff* pay_off;

public:
    Option(PayOff* _pay_off);
    virtual ~Option();
    virtual double prime(Data actif) = 0;
};

//Option europenne
class Europeene : public Option {
public:
    Europeene(PayOff* _pay_off);
    virtual ~Europeene();
    virtual double prime(Data actif);
};

//Option asiatique
class Asiatique : public Option {
public:
    Asiatique(PayOff* _pay_off);
    virtual ~Asiatique();
    virtual double prime(Data actif);
};


//Option barriere
class KnockIn : public Option {
public:
    KnockIn(PayOff* _pay_off);
    virtual ~KnockIn();
    virtual double prime(Data actif);
};

class KnockOut : public Option {
public:
    KnockOut(PayOff* _pay_off);
    virtual ~KnockOut();
    virtual double prime(Data actif);
};


#include "Option.h"

#include <cmath> 
#include "utilitaire.h"
using namespace std;

#define nb_it 100000

//Constructeur de la classe option
Option::Option(PayOff* _pay_off) : pay_off(_pay_off) {
}
Option::~Option() {
}
// Constructeur de la sous classe Europeenne
Europeene::Europeene(PayOff* _pay_off) : Option(_pay_off) {
}
Europeene::~Europeene() {
}

// Calcul la prime d'une option europeenne
double Europeene::prime(Data actif) {
    double S = actif.S;
    double r = actif.r;
    double v = actif.v;
    double K = actif.K;
    double T = actif.T;
    double prime = 0;
    for (int i = 0; i < nb_it; i++) {
        vector<double> proc_prix = MBG(S, r, v, T);
        double last_value = proc_prix[proc_prix.size() - 1];
        prime = prime + pay_off->valeur(last_value, K);
    }
    return prime / nb_it;
}

//Constructeur de la sous classe Asiatique
Asiatique::Asiatique(PayOff* _pay_off) : Option(_pay_off) {
}
Asiatique::~Asiatique() {
}

// Calcul la prime d'un option asiatique
double Asiatique::prime(Data actif) {
    double S = actif.S;
    double r = actif.r;
    double v = actif.v;
    double K = actif.K;
    double T = actif.T;
    double prime = 0;
    for (int i = 0; i < nb_it; i++) {
        vector<double> proc_prix = Euler(S, r, v, T);
        double moy_proc = somme_traj(proc_prix) / 252 * T;
        prime = prime + pay_off->valeur(moy_proc, K);
    }
    return prime / nb_it;
}

//Constructeur de la sous classe Knock-in
KnockIn::KnockIn(PayOff* _pay_off) : Option(_pay_off) {
}
KnockIn::~KnockIn() {
}

//Calcul la prime d'une option Knock-in
double KnockIn::prime(Data actif) {
    double S = actif.S;
    double r = actif.r;
    double v = actif.v;
    double K = actif.K;
    double T = actif.T;
    double L = actif.L;
    double prime = 0;
    for (int i = 0; i < nb_it; i++) {
        vector<double> proc_prix = MBG(S, r, v, T);
        if (valeur_max(proc_prix) < L) {
            prime = prime + (pay_off->valeur(proc_prix[proc_prix.size() - 1], K));
        }
        else {
            prime = prime + 0;
        }
    }
    return prime / nb_it;
}

//Constructeur de la sous classe Knock-out
KnockOut::KnockOut(PayOff* _pay_off) : Option(_pay_off) {
}

KnockOut::~KnockOut() {
}

//Calcul la prime d'une option Knock-out
double KnockOut::prime(Data actif) {
    double S = actif.S;
    double r = actif.r;
    double v = actif.v;
    double K = actif.K;
    double T = actif.T;
    double L = actif.L;
    double prime = 0;
    for (int i = 0; i < nb_it; i++) {
        vector<double> proc_prix = MBG(S, r, v, T);
        if (valeur_max(proc_prix) > L) {
            prime = prime + (pay_off->valeur(proc_prix[proc_prix.size() - 1], K));
        }
        else {
            prime = prime + 0;
        }
    }
    return prime / nb_it;
}

#endif /* Option_h */
#pragma once

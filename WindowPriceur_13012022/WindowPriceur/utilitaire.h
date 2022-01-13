#pragma once


#ifndef utilitaire_h
#define utilitaire_h

#define M_PI 3.14159265358979323846
#include <vector>
#include <cmath>
#include <limits>
#include <random>
#define nb_pas 252.0




std::vector<double> vecteur_pas(double T) {
    std::vector<double> pas;
    pas.push_back(0);
    double x = T / nb_pas;
    for (int i = 1; i <= nb_pas; i++) {
        pas.push_back(pas[i - 1] + x);
    }
    return pas;
}

//Calcul la somme des elements d'un vecteur
double somme_trajLog(std::vector <double> traj) {
    double somme = 0;
    for (int i = 0; i <= traj.size() - 1; i++) {
        somme = somme + log(traj[i]);
    }
    return somme;
}

double somme_traj(std::vector <double> traj) {
    double somme = 0;
    for (int i = 0; i <= traj.size() - 1; i++) {
        somme = somme + traj[i];
    }
    return somme;
}

// Renvoie le plus grand elements d'un vecteur
double valeur_max(std::vector <double> traj) {
    double stockage = traj[0];
    for (int i = 1; i <= traj.size() - 1; i++) {
        if (traj[i] > traj[i - 1]) {
            stockage = traj[i];
        }
        else {
            stockage = stockage;
        }
    }
    return stockage;
}


//vecteur de 252 valeur alea N(0,1)
std::vector <double> NormalRnd() {
    std::vector <double> brow;
    std::random_device gen;
    std::normal_distribution<> d{ 0,1 };

    for (int i = 0; i < nb_pas; i++) {
        brow.push_back(d(gen));
    }
    return brow;
}
// Simule la trajectoire d'un mouvement brownien
std::vector <double> simu_brownien(double T) {
    std::vector <double> brow;
    brow.push_back(0);
    std::random_device gen;
    std::normal_distribution<> d{ 0,1 };    
    double pas = T / nb_pas;
    for (int i = 0; i < nb_pas; i++) {
        brow.push_back(brow[i] + sqrt(pas)*d(gen));
    }
    return brow;
}

//Pour pricer les options européennes
//std::vector<double> MBG(double S, double mu, double vol, double T) {
//    std::vector<double> traj;
//    traj.push_back(S);
//    std::vector <double> b = simu_brownien(T);
//    for (int i = 1; i < T*252; i++) {
//        traj.push_back(S * exp((mu - (pow(vol, 2) / 2)) * (i /252) + vol * b[i]));
//    }
//    return traj;
//}

//Trajectoire de prix avec GBM
std::vector<double> MBG(double S, double mu, double vol, double T) {
    std::vector<double> traj;
    traj.push_back(S);

    std::random_device gen;
    std::normal_distribution<> d{ 0,1 };

    double pas = T / nb_pas;
    double drift = 0.0;
    double diffusion = 0.0;

    for (int i = 1; i < nb_pas; i++) {
        double b = d(gen);
        drift = (mu - (double)0.5 * pow(vol, (int)2)) * pas;
        diffusion = vol * b * sqrt(pas);
        traj.push_back(traj[i - 1] * exp(drift + diffusion));
    }
    return traj;
}



//Pour pricer les options Asiatique
std::vector<double> Euler(double S, double mu, double vol, double T) {
    std::vector <double> traj;
    std::vector <double> b = simu_brownien(T);
    traj.push_back(S);
    double pas = T / nb_pas;
    for (int i = 1; i < nb_pas; i++) {
        traj.push_back(traj[i - 1] +pas * mu * traj[i - 1] + traj[i - 1] * vol * (b[i] - b[i - 1]));
    }
    return traj;
}




//Loi normal standard
double norm_pdf(const double x) {
    return (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x);
}


// Loi standard cumulée
double norm_cdf(const double x) {
    double k = 1.0 / (1.0 + 0.2316419 * x);
    double k_sum = k * (0.319381530 + k * (-0.356563782 + k * (1.781477937 + k * (-1.821255978 + 1.330274429 * k))));

    if (x >= 0.0) {
        return (1.0 - (1.0 / (pow(2 * M_PI, 0.5))) * exp(-0.5 * x * x) * k_sum);
    }
    else {
        return 1.0 - norm_cdf(-x);
    }
}


// Calcul des d1 et d2
double d_j(const int j, const double S, const double K, const double r, const double v, const double T) {
    return (log(S / K) + (r + (pow(-1, j - 1)) * 0.5 * v * v) * T) / (v * (pow(T, 0.5)));
}

// Prime Call B&S
double call_price(const double S, const double K, const double r, const double v, const double T) {
    return S * norm_cdf(d_j(1, S, K, r, v, T)) - K * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T));
}


//Delta d'un Call
double call_delta(const double S, const double K, const double r, const double v, const double T) {
    return norm_cdf(d_j(1, S, K, r, v, T));
}

//Gamma Call
double call_gamma(const double S, const double K, const double r, const double v, const double T) {
    return norm_pdf(d_j(1, S, K, r, v, T)) / (S * v * sqrt(T));
}

// Vega Call
double call_vega(const double S, const double K, const double r, const double v, const double T) {
    return S * norm_pdf(d_j(1, S, K, r, v, T)) * sqrt(T);
}

//Theta Call
double call_theta(const double S, const double K, const double r, const double v, const double T) {
    return -(S * norm_pdf(d_j(1, S, K, r, v, T)) * v) / (2 * sqrt(T))
        - r * K * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T));
}

//Rho Call
double call_rho(const double S, const double K, const double r, const double v, const double T) {
    return K * T * exp(-r * T) * norm_cdf(d_j(2, S, K, r, v, T));
}

//Prix Put
double put_price(const double S, const double K, const double r, const double v, const double T) {
    return -S * norm_cdf(-d_j(1, S, K, r, v, T)) + K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T));
}

// Calculate the European vanilla put Delta
double put_delta(const double S, const double K, const double r, const double v, const double T) {
    return norm_cdf(d_j(1, S, K, r, v, T)) - 1;
}

// Calculate the European vanilla put Gamma
double put_gamma(const double S, const double K, const double r, const double v, const double T) {
    return call_gamma(S, K, r, v, T); // Identical to call by put-call parity
}

// Calculate the European vanilla put Vega
double put_vega(const double S, const double K, const double r, const double v, const double T) {
    return call_vega(S, K, r, v, T); // Identical to call by put-call parity
}

// Calculate the European vanilla put Theta
double put_theta(const double S, const double K, const double r, const double v, const double T) {
    return -(S * norm_pdf(d_j(1, S, K, r, v, T)) * v) / (2 * sqrt(T))
        + r * K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T));
}

// Calculate the European vanilla put Rho
double put_rho(const double S, const double K, const double r, const double v, const double T) {
    return -T * K * exp(-r * T) * norm_cdf(-d_j(2, S, K, r, v, T));
}


//Pour les option asiatiques ////////////////////////////////////////////////
//Methode par réduction de variance via variable de controle////////////////
//Model de Kemma et Vorst /////////////////////////////////////////////////
double LeftTerme(double S, double mu, double vol, double T, double K) {
    return ((T / vol) * (log(S / K) - (0.5 * T) * (mu - 0.5 * pow(vol, 2))));
}

double Ez(double S, double mu, double vol, double T, double K,double w) {
    double StrikePart = -K*(1-norm_cdf(w/(sqrt(pow(T,3)/3))));
    double AssetPart = S*exp((mu-0.5 * pow(vol, 2))*(0.5*T)+pow(vol,2)*(T/6))*(1-norm_cdf((w-vol*pow(T,2)/3)/sqrt(pow(T,3)/3)));
    return AssetPart + StrikePart;
}


#endif /* utilitaire_h */




#pragma once

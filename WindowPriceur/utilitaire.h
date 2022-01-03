#pragma once


#ifndef utilitaire_h
#define utilitaire_h

#define M_PI 3.14159265358979323846
#include <vector>
#include <cmath>
#include <limits>



//Calcul la somme des elements d'un vecteur
double somme_traj(std::vector <double> traj) {
    double somme = 0;
    for (int i = 0; i <= traj.size(); i++) {
        somme = somme + traj[i];
    }
    return somme;
}

// Renvoie le plus grand elements d'un vecteur
double valeur_max(std::vector <double> traj) {
    double stockage = traj[0];
    for (int i = 1; i <= traj.size(); i++) {
        if (traj[i] > traj[i - 1]) {
            stockage = traj[i];
        }
        else {
            stockage = stockage;
        }
    }
    return stockage;
}

// Simule la trajectoire d'un mouvement brownien
std::vector <double> simu_brownien(double T) {
    std::vector <double> brow;
    brow.push_back(0);
    double unif_1, unif_2;
    for (int i = 0; i < T * 252; i++) {
        unif_1 = (double(rand() + 1)) / (double(RAND_MAX) + 2);
        unif_2 = (double(rand() + 1)) / (double(RAND_MAX) + 2);
        brow.push_back(brow[i] + sqrt(double(T / 252)) * sqrt(-2 * log(unif_1)) * cos(2 * M_PI * unif_2));
    }
    return brow;
}

//Pour pricer les options européennes
std::vector<double> MBG(double S, double mu, double vol, double T) {
    std::vector<double> traj;
    traj.push_back(S);
    std::vector <double> b = simu_brownien(T);
    for (int i = 1; i < 252 * T; i++) {
        traj.push_back(S * exp((mu - (pow(vol, 2) / 2)) * (i / 252) + vol * b[i]));
    }
    return traj;
}

//Pour pricer les options Asiatique
std::vector<double> Euler(double S, double mu, double vol, double T) {
    double nb_pas = 252;
    std::vector <double> traj;
    std::vector <double> b = simu_brownien(T);
    traj.push_back(S);
    for (int i = 1; i < nb_pas * T; i++) {
        traj.push_back(traj[i - 1] + (T / nb_pas) * mu * traj[i - 1] + traj[i - 1] * vol * (b[i] - b[i - 1]));
    }
    return traj;
}

// Calcul le coeficient de multiplication pour les options barrire
double coef_barriere(std::vector <double> traj, double L, double T, double v) {
    double produit = 1;
    for (int i = 1; i < traj.size(); i++) {
        produit = produit * (1 - exp((-2 * (L - traj[i - 1]) * (L - traj[i])) / ((T / 252.0) * v * traj[i - 1])));
    }
    return produit;
}



#endif /* utilitaire_h */

#pragma once


#ifndef Matrice_h
#define Matrice_h

class Matrice {
public:
    Matrice(); // on déclare le constructeur
    Matrice(int n, int m); //constructeur avec parametre
    int n, m;
    double** valeurs; // on reserve l'espace pour un tableau
};

// constructeur
Matrice::Matrice() {
}

Matrice::Matrice(int n, int m) {
    int i;
    this->n = n; //le param n de la matrice est égale à n
    this->m = m; // idem

    this->valeurs = new double* [this->n];

    for (i = 0; i < this->n; i++) {
        this->valeurs[i] = new double[this->m];
    }
}


#endif 
#pragma once

//On crée un fichier header qui contient la structure
//Qui est commune à nos deux programmes

#ifndef HUFFMAN_H  //Vérifie si HUFFMAN_H n'est pas déclaré
#define HUFFMAN_H  //Déclare HUFFMAN_H si ifndef génére true 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NB_POSSIBLE_CHAR 256

//Notre structure "noeud" doit contenir
// -le char associé à un noeud
// - le nombre de fois qu'il apparaît
//  -des pointeurs vers les noeuds gauche et droite

typedef struct Noeud
{
    unsigned char car;
    int freq;
    struct Noeud *gauche;
    struct Noeud *droite;
}Noeud;

#endif








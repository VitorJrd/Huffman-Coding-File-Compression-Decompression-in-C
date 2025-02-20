//Compilation: gcc -Wall huffman_decompresser.c -o huffman_decompresser
//Exemple d'execution: ./huffman_decompresser vingtmille.huff vingtmilledecomp.txt

#include "huffman.h"

Noeud* init_noeud(unsigned char car, int freq, Noeud* gauche, Noeud* droite)
{
    Noeud* n=(Noeud*) malloc (sizeof(Noeud)); 
    n->car=car;
    n->freq=freq;
    n->gauche=gauche;
    n->droite=droite;
    return n;
}

int comparer_noeuds(const void *a,const void *b)
{
    return (*(Noeud**)a)->freq - (*(Noeud**)b)->freq;  
}  

Noeud * init_arbre(int freq[])
{      
    Noeud* noeuds[NB_POSSIBLE_CHAR];
    int taille=0;
    
    for (int i=0; i<NB_POSSIBLE_CHAR; i++)
    {
        if (freq[i]>0)
        {
            noeuds[taille++]= init_noeud(i,freq[i],NULL,NULL); 
        } 

    }

    while (taille>1)
    {

        qsort(noeuds,taille,sizeof(Noeud*),comparer_noeuds);

        Noeud * gauche= noeuds[0];
        Noeud * droite= noeuds[1];
        Noeud * parent = init_noeud('\0',gauche->freq+droite->freq,gauche,droite);
        
    
        noeuds[0]=parent;   
        noeuds[1]=noeuds[taille-1];
        taille--;   

    } 
    return noeuds[0]; 
}

void decompresser(const char *fichier_compresse, const char* fichier_sortie)
{
    FILE* entree = fopen(fichier_compresse,"rb");
    FILE* sortie = fopen(fichier_sortie,"wb");

    if (!entree || !sortie)
    {
        printf("Erreur lors de l'ouverture du/des fichiers\n");
        exit(EXIT_FAILURE);
    }
    
    int freq[NB_POSSIBLE_CHAR]={0};
    
    //On commence par lire la table des fréquences depuis le fichier compressé
    fread(freq,sizeof(int),NB_POSSIBLE_CHAR,entree);

    //Construisons notre arbre à partir des fréquences
    Noeud * racine = init_arbre(freq);

    //Il nous reste déchiffrer les données en utilisant l'abre et écrire le résultat dans le fichier de sortie
    Noeud * courant = racine;
   
    unsigned char buffer;

    while (fread(&buffer, sizeof(unsigned char), 1, entree)) 
    {
        for (int i = 7; i >= 0; i--) //On lit chaque octet compressé et on extrait les bits un par un
        { 
            int bit = (buffer >> i) & 1;    
            //On isole le dernier bit en décalant vers la droite de i positions
            //on fait l'opération ET logique ("&") avec 1 afin d'isoler ce dernier bit

            courant = bit ? courant->droite : courant->gauche;  
            //Si ce bit =1 <=> on va à droite, sinon on va à gauche

            //On traverse donc l'arbre en suivant les bits

            if (!courant->gauche && !courant->droite) //Si feuille atteinte
            {      
                printf("Caractère décompressé : '%c' (%d)\n", courant->car, courant->car);
                fputc(courant->car, sortie);    //Si feuille <=> on écrit le caractère dans le fichier de sortie
                courant = racine;  //Retour à la racine pour décoder un nouveau caractère
            }
        }
    }
    
    fclose(entree);
    fclose(sortie);
}

int main(int argc, char*argv[])
{
    if (argc!=3)
    {
        printf("Nombre d'arguments incorrect\n");
        return 1;
    } 

    decompresser(argv[1],argv[2]);
    printf("La décompression du fichier est terminée\n");
    
    return 0;

} 

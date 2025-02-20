//Compilation: gcc -Wall huffman_compresser.c -o huffman_compresser
//Exemple d'execution: ./huffman_compresser vingtmille.txt vingtmillecomp.huff

#include "huffman.h"

//Il faut commencer par initialiser un noeud de notre liste/arbre

Noeud* init_noeud(unsigned char car, int freq, Noeud* gauche, Noeud* droite)
{
    //On commence par allouer de la mémoire
    Noeud* n=(Noeud*) malloc (sizeof(Noeud)); 
    //On initialise les champs de la structure
    n->car=car;
    n->freq=freq;
    n->gauche=gauche;
    n->droite=droite;
    return n;
}

//Remarque: si gauche et droite sont NULL <=> le noeud est une feuille

int comparer_noeuds(const void *a,const void *b)
{
    return (*(Noeud**)a)->freq - (*(Noeud**)b)->freq;

    //a et b sont des adresses d'éléments du tableau noeuds[]
    //On effectue donc un casting en pointeurs vers Noeud*
    //Ceci nous permettra de calculer la différence de fréquences des noeuds a et b
    //qsort utilisera ensuite cette valeur pour trier le tableau noeuds[]  
}  

Noeud * init_arbre(int freq[])
{
    //Commençons par créer un tableau de noeuds de notre arbre
    //Ainsi qu'un paramètre taille qui garde la quantité de noeuds dans la liste
       
    Noeud* noeuds[NB_POSSIBLE_CHAR];
    int taille=0;
    
    for (int i=0; i<NB_POSSIBLE_CHAR; i++)
    {
        if (freq[i]>0)
        {
            noeuds[taille++]= init_noeud(i,freq[i],NULL,NULL); 
        } 

    }

    //On parcourt le tableau freq[] qui contient la fréquence de chaque char
    //Si un char i est présent <=> freq[i]>0
    //On initialise un nouveau noeud
    //On ajoute ce noeud à notre tableau de noeuds (l'abre) et on incrémente sa taille

    //Ensuite il faut fusionner les deux noeuds avec la plus faible fréquence
    //Il faut donc extraire les derniers noeuds gauche et droite de notre tableau noeuds[]
    //Créer un nouveau noeud dont la fréq est la somme des fréq des noeuds
    //On insère ce nouveau noeud dans notre tableau
    //On répète ceci jusqu'à ce que seule la racine reste

    while (taille>1)
    {
        //Il faut commencer par trier notre tableau à chaque itération pour connaitre les deux fréquences les plus basses
        //On peut utiliser le quicksort
        //Les deux premiers indices de notre tableau contiendront donc les deux chars dont la fréquence est la plus faible

        qsort(noeuds,taille,sizeof(Noeud*),comparer_noeuds);

        Noeud * gauche= noeuds[0];
        Noeud * droite= noeuds[1];
        Noeud * parent = init_noeud('\0',gauche->freq+droite->freq,gauche,droite);
        
        //Ensuite, il faut remplacer les deux premiers éléments par le noeud parent
        noeuds[0]=parent;   
        noeuds[1]=noeuds[taille-1];
        taille--;   
    }
    return noeuds[0];
}

//Maintenant il nous faut une fonction pour générer le code binaire de l'arbre

void code_binaire(Noeud* racine,char* code_bin_actuel,char codes[NB_POSSIBLE_CHAR][NB_POSSIBLE_CHAR],int profondeur)
{
    if (!racine)
    {
        return; //car il faut commencer à générer des codes binaires à partir de la racine
    }

    //Parcourons maintenat l'abre jusqu'à ce qu'on obtienne des feuilles
    if (!racine->gauche && !racine->droite)
    {
        code_bin_actuel[profondeur]='\0';   //signale la fin de la string
        strcpy(codes[(unsigned char)racine->car], code_bin_actuel);
        return; 
    }  

    //Un 0 signifie qu'on descend à gauche dans l'abre donc on appele notre fonction récursivement
    code_bin_actuel[profondeur]='0';
    code_binaire(racine->gauche, code_bin_actuel, codes, profondeur+1);
    
    //Un 1 signifie qu'on descend à droite dans l'arbre
    code_bin_actuel[profondeur]='1';
    code_binaire(racine->droite,code_bin_actuel,codes,profondeur+1);  
}



void compresser(const char *fichier_entree,  const char *fichier_sortie)
{
    //On ouvre les deux fichiers
    FILE *entree = fopen(fichier_entree,"rb");
    FILE *sortie= fopen(fichier_sortie, "wb");

    //On vérifie si l'ouverture des fichiers a été faite correctement
    if (!entree || !sortie)
    {
        printf("Erreur lors de l'ouverture du/des fichiers\n");
        exit(EXIT_FAILURE);
    } 
    
    int freq[NB_POSSIBLE_CHAR]={0};
    unsigned char c;
    
    //On va lire le fichier et compter la fréquence de chaque caractères
    while(fread(&c, sizeof(char),1,entree))
    {
        freq[c]++; 
    } 

    //Ensuite on initialise notre arbre

    Noeud* racine = init_arbre(freq);

    //On initialise notre table des codes et on génére le code binaire respectif
    char codes[NB_POSSIBLE_CHAR][NB_POSSIBLE_CHAR] = {{0}};
    char code_bin_actuel[NB_POSSIBLE_CHAR];
    code_binaire(racine, code_bin_actuel, codes, 0);

    //On peut afficher les codes binaires générés
    printf("Table des codes binaires générés :\n");
    for (int i = 0; i < NB_POSSIBLE_CHAR; i++) 
    {
        if (freq[i] > 0) 
        {
            printf("Caractère '%c' (%d) : %s\n", i, i, codes[i]);
        }
    }

    // Écriture des fréquences dans le fichier compressé pour que la décompression puisse reconstruire l'arbre
    fwrite(freq, sizeof(int), NB_POSSIBLE_CHAR, sortie);

    //Vu qu'on va devoir parcourir le fichier encore une fois
    //Il faut donc réinitialiser la position du fichier
    rewind(entree);

    unsigned char buffer = 0;
    int compteur_bits = 0;

    while (fread(&c, sizeof(char), 1, entree))  //On va lire tous les caractères
    {
        char *code = codes[c];  //On récupère le code binaire pour ce caractère
        printf("Caractère '%c' (%d) → Code : %s\n", c, c, code);

        for (int i = 0; code[i] != '\0'; i++)   //Afin de parcourir chaque bit du code binaire
        {
            buffer = (buffer << 1) | (code[i] - '0');
            
            //On décale buffer vers la gauche ce qui équivaut à ajouter un 0 à la fin (opération <<)
            //code[i] est un caractère: soit '0' soit '1'
            //donc code[i]-'0' le convertit en un entier
            //On effectue l'opération OU ("|") afin d'ajouter cet entier au buffer

            compteur_bits++;    //Pour compter le nombre de bits stockés

            if (compteur_bits == 8)     //Si on a un octet on l'écrit
            {
                fwrite(&buffer, sizeof(unsigned char), 1, sortie);
                buffer = 0;     //On réinitialise les deux paramètres
                compteur_bits = 0;
            }
        }
    }

    //Cas où le dernier octet n'est plas complet
    //Dans ce cas on le complète avec des 0 avant d'écrire
    if (compteur_bits > 0) 
    {
        buffer <<= (8 - compteur_bits); 
        //On décale buffer vers la gauche le même nombre de fois qu'il y a de bits manquants afin de compléter l'octet

        fwrite(&buffer, sizeof(unsigned char), 1, sortie);
    }   

    fclose(entree);
    fclose(sortie);   
}

int main(int argc, char*argv[])
{
    if (argc !=3)
    {
        //Nombre de paramètres incorrect
        printf("Nombre d'arguments incorrect\n");
        return 1;
    } 
    compresser (argv[1],argv[2]);
    printf("La compression du fichier est terminée\n");

    return 0;
}


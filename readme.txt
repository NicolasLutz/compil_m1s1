//=============================
PROJET DE COMPILATION POUR M1 ILC/ISI/RISE PRODUIT PAR:
Nicolas Lutz
Lansiné Kaba
//=============================
//Utilisation:

make

//Tests valgrind:

make all_mmcheck

//Tests mips: 

make all_exec

//ou (Après avoir compilé)

make run_test

//=============================
===========FEATURES============
==========FONCTIONNE:==========
types int et float:
print, opérations (i++ et i-- seulement pour int)
if(), if() else, conditions, avec ou sans accolades
affectations, déclarations
for, while

type string:
printf

général:
pas de leak mémoire détecté
certains messages d'erreur de syntaxe/warnings

==========NE FONCTIONNE PAS OU MAL:==========
le type matrice et aucune de ses opérations ne sont pas implantées
les paranthèses des expressions simples ne fonctionnent pas
pas de fichier/ligne/colonne pour les messages d'avertissement
une seule optimisation codée: propagation des labels contigüs (mais en O(n^2))

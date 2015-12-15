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
pour types int et float:
print, opérations (i++ et i-- seulement pour int)
if(), if() else, conditions, avec ou sans accolades
affectations, déclarations

pour type string:
printf

général:
pas de leak mémoire détecté

==========NE FONCTIONNE PAS OU MAL:==========
le type matrice et toutes ses opérations ne sont pas implantées
les paranthèses des expressions simples ne fonctionnent pas
une seule optimisation codée: propagation des labels contigüs (mais en O(n^2))

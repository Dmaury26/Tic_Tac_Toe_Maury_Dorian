#include <ncurses.h>
#include <stdlib.h>
#include <limits.h>

// Prototypes des fonctions
void afficherPlateau(WINDOW *win, char plateau[3][3]);  // Affiche le plateau de jeu
void initialiserPlateau(char plateau[3][3]);           // Initialise le plateau avec des espaces vides
int verifierGagnant(char plateau[3][3]);               // Vérifie s'il y a un gagnant ou un match nul
void mouvementJoueur(WINDOW *win, char plateau[3][3], char joueur); // Gestion des mouvements du joueur
void mouvementOrdinateur(char plateau[3][3]);          // Gestion des mouvements de l'ordinateur
int afficherMenu(WINDOW *win);                         // Affiche le menu principal
int minimax(char plateau[3][3], int profondeur, int isMaximizing); // Algorithme minimax pour IA
int meilleurMouvement(char plateau[3][3]);             // Trouve le meilleur mouvement pour l'ordinateur

int main() {
    char plateau[3][3];         // Le tableau 3x3 représentant le plateau de jeu
    char joueurActuel = 'X';    // Définit le joueur actuel, commence par 'X'
    int gagnant = 0;            // Indique si un joueur a gagné ou si le match est nul
    int modeJeu;                // Stocke le mode de jeu choisi par l'utilisateur

    // Initialisation de ncurses pour l'affichage en terminal
    initscr();
    cbreak();        // Désactive le buffering des entrées, chaque touche est immédiatement capturée
    noecho();        // Empêche d'afficher les caractères tapés
    curs_set(0);     // Cache le curseur
    refresh();       // Actualise l'écran

    // Création de la fenêtre principale avec une taille et position définies
    WINDOW *win = newwin(15, 46, 1, 1); // Défninir la taille de la fenêtre
    keypad(win, TRUE);  // Permet de capturer les flèches

    // Afficher le menu et récupérer le choix du mode de jeu
    modeJeu = afficherMenu(win);
    if (modeJeu == -1) {  // Si l'utilisateur choisit de quitter
        delwin(win);      // Supprime la fenêtre principale
        endwin();         // Termine ncurses
        return 0;
    }

    // Initialise le plateau avec des espaces vides
    initialiserPlateau(plateau);

    // Boucle principale pour le jeu
    while (gagnant == 0) {
        afficherPlateau(win, plateau);  // Affiche l'état actuel du plateau

        // Gestion des tours selon le mode de jeu
        if (modeJeu == 1 || (modeJeu == 2 && joueurActuel == 'X')) {
            mouvementJoueur(win, plateau, joueurActuel); // Tour du joueur
        } else {
            mouvementOrdinateur(plateau); // Tour de l'ordinateur
        }

        // Vérifie si un joueur a gagné ou si le jeu est terminé
        gagnant = verifierGagnant(plateau);

        // Alterne entre les joueurs 'X' et 'O'
        joueurActuel = (joueurActuel == 'X') ? 'O' : 'X';
    }

    afficherPlateau(win, plateau);  // Réaffiche le plateau final

    // Affiche le résultat de la partie
    if (gagnant == 1) {
        mvwprintw(win, 9, 1, "Le joueur %c a gagné !", (joueurActuel == 'X') ? 'O' : 'X');
        mvwprintw(win, 11, 1, "Appuyer sur ENTRER pour fermer.");
    } else {
        mvwprintw(win, 9, 1, "Match nul !");
    }
    wrefresh(win); // Met à jour la fenêtre avec les messages affichés

    getch();       // Attend que l'utilisateur appuie sur une touche
    delwin(win);   // Supprime la fenêtre principale
    endwin();      // Termine ncurses proprement
    return 0;
}

// Fonction pour afficher le menu principal
int afficherMenu(WINDOW *win) {
    const char *options[] = {  // Liste des options du menu
        "1. Joueur contre Joueur",
        "2. Joueur contre Ordinateur",
        "3. Quitter"
    };
    int choix = 0, nbOptions = 3; // Initialise le choix et le nombre d'options

    while (1) {
        werase(win);          // Efface le contenu précédent de la fenêtre
        box(win, 0, 0);       // Ajoute un cadre autour de la fenêtre
        mvwprintw(win, 1, 1, "Menu principal :"); // Titre du menu
        for (int i = 0; i < nbOptions; i++) {    // Affiche les options
            if (i == choix) wattron(win, A_REVERSE);  // Met en surbrillance l'option sélectionnée
            mvwprintw(win, 3 + i, 2, options[i]);     // Affiche chaque option
            if (i == choix) wattroff(win, A_REVERSE); // Désactive la surbrillance
        }
        mvwprintw(win, 8, 2, "Utilisez les flèches, Entrée pour valider.");
        wrefresh(win);        // Actualise la fenêtre pour afficher le menu

        int touche = wgetch(win); // Capture la touche appuyée
        switch (touche) {
            case KEY_UP:      // Flèche haut
                choix = (choix - 1 + nbOptions) % nbOptions; // Passe à l'option précédente
                break;
            case KEY_DOWN:    // Flèche bas
                choix = (choix + 1) % nbOptions; // Passe à l'option suivante
                break;
            case '\n':        // Touche Entrée
                return (choix == 2) ? -1 : choix + 1; // Quitte si "3. Quitter" est sélectionné
        }
    }
}

// Fonction pour afficher le plateau de jeu
void afficherPlateau(WINDOW *win, char plateau[3][3]) {
    werase(win);    // Efface tout le contenu précédent
    box(win, 0, 0); // Ajoute un cadre autour de la fenêtre

    // Parcourt chaque cellule du plateau et affiche son contenu
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            mvwprintw(win, i * 2 + 1, j * 4 + 2, "%c", (plateau[i][j] != ' ') ? plateau[i][j] : '.');
            if (j < 2) mvwprintw(win, i * 2 + 1, j * 4 + 4, "|"); // Affiche les séparateurs verticaux
        }
        if (i < 2) mvwprintw(win, i * 2 + 2, 2, "--|---|--"); // Affiche les séparateurs horizontaux
    }

    wrefresh(win); // Actualise la fenêtre pour afficher le plateau
}

// Fonction pour initialiser le plateau
void initialiserPlateau(char plateau[3][3]) {
    for (int i = 0; i < 3; i++) {       // Parcourt les lignes
        for (int j = 0; j < 3; j++) {   // Parcourt les colonnes
            plateau[i][j] = ' ';        // Initialise chaque case avec un espace vide
        }
    }
}

// Fonction pour vérifier si un joueur a gagné ou si le jeu est terminé
int verifierGagnant(char plateau[3][3]) {
    // Vérifie les lignes pour voir si un joueur a gagné
    for (int i = 0; i < 3; i++) {
        if (plateau[i][0] == plateau[i][1] && plateau[i][1] == plateau[i][2] && plateau[i][0] != ' ')
            return 1; // Renvoie 1 si un joueur a gagné dans une ligne
    }

    // Vérifie les colonnes pour voir si un joueur a gagné
    for (int i = 0; i < 3; i++) {
        if (plateau[0][i] == plateau[1][i] && plateau[1][i] == plateau[2][i] && plateau[0][i] != ' ')
            return 1; // Renvoie 1 si un joueur a gagné dans une colonne
    }

    // Vérifie les diagonales pour voir si un joueur a gagné
    if (plateau[0][0] == plateau[1][1] && plateau[1][1] == plateau[2][2] && plateau[0][0] != ' ')
        return 1; // Diagonale principale
    if (plateau[0][2] == plateau[1][1] && plateau[1][1] == plateau[2][0] && plateau[0][2] != ' ')
        return 1; // Diagonale secondaire

    // Vérifie si le jeu continue en recherchant des cases vides
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (plateau[i][j] == ' ')
                return 0; // Renvoie 0 si le jeu n'est pas terminé
        }
    }

    return -1; // Renvoie -1 si toutes les cases sont remplies sans gagnant (match nul)
}

// Fonction pour gérer les entrées de l'utilisateur et effectuer son mouvement
void mouvementJoueur(WINDOW *win, char plateau[3][3], char joueur) {
    int x, y; // Coordonnées de la case à jouer
    do {
        echo(); // Permet de montrer ce que l'utilisateur tape
        mvwprintw(win, 7, 1, "Joueur %c, entrez ligne colonne ('1 1') : ", joueur); // Message d'invite
        wrefresh(win); // Met à jour l'affichage
        wscanw(win, "%d %d", &x, &y); // Capture les coordonnées saisies
        noecho(); // Désactive à nouveau l'affichage des entrées
    } while (x < 1 || x > 3 || y < 1 || y > 3 || plateau[x-1][y-1] != ' '); // Valide les coordonnées

    plateau[x-1][y-1] = joueur; // Place le symbole du joueur dans la case choisie
}

// Fonction pour effectuer un mouvement automatique avec l'algorithme minimax
void mouvementOrdinateur(char plateau[3][3]) {
    int mouvement = meilleurMouvement(plateau); // Appelle la fonction pour trouver le meilleur mouvement
    plateau[mouvement / 3][mouvement % 3] = 'O'; // Joue le mouvement calculé (divise et prend le reste pour trouver x et y)
}

// Fonction pour calculer le meilleur mouvement à jouer pour l'ordinateur
int meilleurMouvement(char plateau[3][3]) {
    int meilleurScore = INT_MIN; // Initialise le meilleur score à une très faible valeur
    int mouvement = -1;         // Aucun mouvement défini pour le moment

    // Parcourt toutes les cases du plateau
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (plateau[i][j] == ' ') { // Vérifie si la case est libre
                plateau[i][j] = 'O'; // Simule un coup pour l'ordinateur
                int scoreActuel = minimax(plateau, 0, 0); // Évalue le score du mouvement avec minimax
                plateau[i][j] = ' '; // Annule le coup simulé (remet la case à son état initial)
                if (scoreActuel > meilleurScore) { // Met à jour le meilleur score et mouvement si nécessaire
                    meilleurScore = scoreActuel;
                    mouvement = i * 3 + j; // Stocke l'indice du meilleur mouvement
                }
            }
        }
    }

    return mouvement; // Renvoie le meilleur mouvement trouvé
}

// Algorithme pour analyser les mouvements et prédire le résultat du jeu
int minimax(char plateau[3][3], int profondeur, int isMaximizing) {
    int score = verifierGagnant(plateau); // Vérifie si un joueur a gagné ou si le jeu est terminé

    // Retourne les scores selon les résultats du jeu
    if (score == 1)
        return (isMaximizing) ? -1 : 1; // Retourne -1 si l'adversaire gagne, 1 si l'ordinateur gagne
    if (score == -1)
        return 0; // Match nul

    // Maximisation pour l'ordinateur
    if (isMaximizing) {
        int meilleurScore = INT_MIN; // Initialise le score à une très faible valeur
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (plateau[i][j] == ' ') { // Vérifie si la case est libre
                    plateau[i][j] = 'O'; // Simule un coup pour l'ordinateur
                    int scoreActuel = minimax(plateau, profondeur + 1, 0); // Évalue le coup pour le joueur
                    plateau[i][j] = ' '; // Annule le coup simulé
                    meilleurScore = (scoreActuel > meilleurScore) ? scoreActuel : meilleurScore; // Met à jour le meilleur score
                }
            }
        }
        return meilleurScore; // Retourne le meilleur score calculé
    }
    // Minimisation pour l'adversaire
    else {
        int meilleurScore = INT_MAX; // Initialise le score à une très grande valeur
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (plateau[i][j] == ' ') { // Vérifie si la case est libre
                    plateau[i][j] = 'X'; // Simule un coup pour le joueur
                    int scoreActuel = minimax(plateau, profondeur + 1, 1); // Évalue le coup pour l'ordinateur
                    plateau[i][j] = ' '; // Annule le coup simulé
                    meilleurScore = (scoreActuel < meilleurScore) ? scoreActuel : meilleurScore; // Met à jour le meilleur score
                }
            }
        }
        return meilleurScore; // Retourne le meilleur score calculé
    }
}


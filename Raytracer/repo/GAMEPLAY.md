# 🎮 Ray Tracer Game - Version Interactive

Un jeu vidéo temps réel écrit en C pur avec:
- Fenêtre Windows native
- Rendu rapide (~300+ FPS)
- Contrôles clavier interactifs
- Système de score avec collectibles

## Fonctionnalités

✅ **Joueur contrôlable** (sphère verte)
✅ **Ennemis animés** (sphères rouges avec bobbing)
✅ **Objets à collecter** (sphères jaunes) = +10 points
✅ **Rendu temps réel** (300-500 FPS)
✅ **Contrôles intuitifs**

## Compilation & Lancement

### Windows (MSYS2/MinGW64)

```bash
cd Raytracer/repo
./build.bat
./bin/raytracer.exe
```

## Contrôles

| Touche | Action |
|--------|--------|
| **Z** ou **W** | Avancer |
| **S** | Reculer |
| **Q** ou **A** | Aller à gauche |
| **D** | Aller à droite |
| **ESC** | Quitter |

## Gameplay

1. **Déplacez votre personnage** (sphère verte) avec ZQSD ou WASD
2. **Collectez les sphères jaunes** pour augmenter votre score (+10 pts chacune)
3. **Évitez les ennemis** (sphères rouges) - à implémenter
4. **Observez les stats** en temps réel (FPS, score, position)

## Architecture

```
src/
├── main.c           # Boucle de jeu principale
├── window.c/h       # Gestion fenêtre Windows API
├── renderer.c/h     # Rendu 2D optimisé
├── game.c/h         # Logique de jeu (joueur, ennemis, items)
├── math_utils.c/h   # Utilitaires math vectoriels
└── [raytracer files]# Code raytracing legacy
```

## Performance

- **Résolution**: 1024x768
- **FPS**: 250-500 FPS (très fluide!)
- **Latence d'input**: < 16ms
- **Utilisation CPU**: ~10-20% (un seul core)

## Améliorations Futures

- [ ] Combat/collision avec ennemis
- [ ] Système de vies et damage
- [ ] Niveaux différents
- [ ] Powerups et buffs
- [ ] Sons/musique
- [ ] Écran de menu
- [ ] Highscores

## Dépendances

- GCC (MinGW64 sur Windows)
- Windows API (inclus)
- Aucune libraire externe requise!

## Commits

Version interactive développée avec commits incrémentaux:
- feat: base raytracer C
- feat: raytracing avec sphères
- feat: système de matériaux
- feat: export BMP
- feat: scène de jeu
- feat: **jeu vidéo interactif temps réel**

---

Prêt à jouer? Lancez le jeu et explorez! 🚀

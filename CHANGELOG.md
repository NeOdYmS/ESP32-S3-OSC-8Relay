# Changelog

Toutes les évolutions notables du projet sont documentées ici.

## v1.2.4 - Avril 2026

- Interface web multilingue avec selecteur de langue en haut a droite (mode discret)
- Traduction de l'interface en 5 langues: FR, EN, ES, DE, ZH
- Log unifie optimise:
- Auto-scroll activable/desactivable
- Bouton Clear visible (style corrige)
- Redimensionnement manuel du panneau log (drag)
- Historique log augmente a 200 lignes
- Preservation de la position de scroll lorsque l'auto-scroll est inactif
- Endpoint combine `/api/live` pour reduire les requetes web et conserver la priorite OSC
- Rafraichissement en polling chaine (~30 ms de pause entre cycles)
- Affichage RAM clarifie (alerte uniquement en critique + indicateur dans infos systeme)
- Section Changelog de l'onglet Systeme synchronisee avec ce fichier

### Resultats OSC (tests Avril 2026)

- UDP send latency: ~114 us (32-233 us)
- Toggle relay 1 (50 cycles): ~362 us moyenne, 18.1 ms total
- Burst 8 relais: ~0.83 ms (~101-107 us/message)
- `/relay/all` (10 cycles): ~241 us moyenne
- Throughput max: ~49 559 msg/s (1000 messages en 20.2 ms)
- Ping W5500: 0.29-0.73 ms (moyenne 0.46 ms)
- Latence estimee OSC -> relais: ~0.5-1.2 ms

## v1.2.3 - Mars 2026

- Refresh log OSC x4: 1000ms -> 250ms pour suivi temps reel
- Refresh status systeme: 3000ms -> 2000ms
- Refresh etat relais: 2000ms -> 1000ms
- UI responsive mobile: adaptation auto smartphone/tablette (media queries)
- Grilles 4->2 colonnes sur mobile, boutons pleine largeur, inputs tactiles agrandis

## v1.2.2 - Mars 2026

- LED bleue sur reception OSC: flash bleu 50ms a chaque message OSC recu
- Retour automatique au vert apres le flash

## v1.2.1 - Mars 2026

- Commande OSC `/reboot`: redemarrage a distance via OSC
- Fix LED RGB: correction ordre couleurs (NEO_GRB -> NEO_RGB) pour carte Waveshare
- Fix LED rouge permanente: suppression try/catch parasite dans la boucle status
- Tests OSC complets: 52 tests valides (relais i/f/T/F, `/relay/all`, `/ap`, `/reboot`)

## v1.2.0 - Mars 2026

- Optimisation latence OSC: boucle prioritaire sans throttle
- Drain complet des paquets UDP par cycle (multi-packet par pass)
- Sauvegarde NVS differee (500ms) pour ne pas bloquer le relais
- Web/DNS throttle a 5ms pour liberer le CPU au profit de l'OSC
- Remplacement `delay(1)` par `yield()` dans la boucle principale
- Logs PCA9554 non-bloquants (macros `LOG_RELAY`/`LOG_ERROR`)
- Portail captif DNS pour redirection auto apres connexion WiFi
- Resultats latence: ~0.5-1.2ms OSC->relais, ~70k msg/s throughput

## v1.1.0 - Mars 2026

- Log unifie temps reel: messages systeme (vert) + OSC entrants (bleu) fusionnes dans le header
- Affichage des messages OSC entrants avec adresse, type tag et valeur
- QR code WiFi (qrcode-generator v1.4.4) dans l'onglet Reseau
- Temperature CPU dans l'onglet Systeme
- Timeout AP configurable (0 = toujours actif)
- Panneau Systeme (uptime, RAM, ETH, relais actifs)
- Boutons ALL ON / ALL OFF (web + OSC `/relay/all`)
- Commande OSC `/ap` (ON/OFF du point d'acces)
- Extinction auto du WiFi AP apres timeout sans clients
- Endpoint `/api/system/status` + `/api/osc/log`
- Fix generateur QR code (remplacement par lib Kazuhiko Arase)

## v1.0.0 - Janvier 2026

- Interface Web complete
- Controle des 8 relais via OSC
- Configuration Ethernet (DHCP/statique)
- Support WiFi AP optionnel (via DI8)
- Stockage config en EEPROM
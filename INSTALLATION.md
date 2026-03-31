# 📦 Procédure d'Installation - ESP32-S3-OSC-8RELAY

## 🎯 Matériel Requis

### Carte
- **ESP32-S3-ETH-8DI-8RO** (Waveshare)
- Port USB-C pour programmation/alimentation
- Câble USB-C de qualité (données + alimentation)

### Composants Externes
- **W5500** : Module Ethernet SPI (CS=16, INT=12, SCK=15, MISO=14, MOSI=13)
- **PCA9554** : Expandeur I/O sur I2C (adresse 0x20, SDA=8, SCL=9)
- 8 relais connectés aux sorties du PCA9554

### Ordinateur
- macOS avec PlatformIO installé
- Python 3.12 (déjà configuré)
- VS Code + extension PlatformIO (optionnel mais recommandé)

---

## 🔧 Étape 1 : Vérification de l'Environnement

### Vérifier PlatformIO
```bash
export PATH="/Users/tof/Library/Python/3.12/bin:$PATH"
pio --version
```
**Attendu** : `PlatformIO Core, version 6.1.18` (ou supérieur)

### Vérifier la Compilation
```bash
cd /Users/tof/Desktop/ESP32-S3-OSC-8RELAY/ESP32-S3-OSC-8Relay
pio run
```
**Attendu** : `[SUCCESS]` avec statistiques RAM/Flash

---

## 🔌 Étape 2 : Connexion de la Carte

1. **Connecter le câble USB-C** à l'ESP32-S3
2. **Vérifier la détection** :
   ```bash
   ls /dev/cu.* | grep -i usb
   ```
   **Attendu** : `/dev/cu.usbserial-XXXX` ou `/dev/cu.usbmodem-XXXX`

3. **Si la carte n'est pas détectée** :
   - Vérifier le câble USB-C (doit supporter les données)
   - Appuyer sur le bouton **BOOT** + **RESET**, relâcher **RESET** puis **BOOT**
   - Installer le driver CH340/CP210x si nécessaire

---

## 📤 Étape 3 : Upload du Firmware

### Option A : Upload Automatique (Recommandé)
```bash
export PATH="/Users/tof/Library/Python/3.12/bin:$PATH"
cd /Users/tof/Desktop/ESP32-S3-OSC-8RELAY/ESP32-S3-OSC-8Relay
pio run --target upload
```

### Option B : Upload Manuel (si échec automatique)
```bash
# 1. Compiler
pio run

# 2. Passer en mode bootloader (maintenir BOOT + appuyer RESET)

# 3. Upload
pio run --target upload --upload-port /dev/cu.usbserial-XXXX
```

**Attendu** :
```
Writing at 0x00010000... (100 %)
Wrote 1016221 bytes at 0x00010000 in X.X seconds
Hash of data verified.
Leaving...
Hard resetting via RTS pin...
```

---

## 📡 Étape 4 : Surveillance du Démarrage

### Ouvrir le Moniteur Série
```bash
pio device monitor --baud 115200
```

### Logs Attendus au Démarrage
```
[INFO] [LOGGER] Logger initialized
[INFO] [WATCHDOG] Watchdog started (timeout: 10000 ms)
[INFO] [I2C] I2C initialized on SDA=8, SCL=9
[INFO] [CONFIG] Loading configuration from NVS
[INFO] [PCA9554] I/O expander initialized at 0x20
[INFO] [NET] Initializing Ethernet mode (WiFi STA with static IP)...
[INFO] [NET] Ethernet static IP: 192.168.1.100
[INFO] [WEB] Web server started on port 80
[INFO] [OSC] OSC server listening on UDP port 8000
[INFO] ✅ System Ready
```

**Si erreur "PCA9554 not found"** :
- Vérifier le câblage I2C (SDA=GPIO8, SCL=GPIO9)
- Vérifier l'adresse I2C avec `i2cdetect` (0x20 attendu)

---

## 🌐 Étape 5 : Configuration Initiale

### Valeurs par Défaut (NVS vide)
```json
{
  "hostname": "ESP32-OSC-Relay",
  "eth": {
    "dhcp": false,
    "ip": "192.168.1.100",
    "gw": "192.168.1.1",
    "mask": "255.255.255.0",
    "dns1": "8.8.8.8",
    "dns2": "8.8.4.4"
  },
  "oscPort": 8000,
  "wifiApAllowed": true,
  "apSsid": "ESP32-OSC-Relay",
  "apPassword": "12345678",
  "relays": [
    {"oscAddress": "/relay/0", "invert": false, "mode": 0},
    {"oscAddress": "/relay/1", "invert": false, "mode": 0},
    ...
  ]
}
```

### Accès à l'Interface Web
1. **Connecter l'ordinateur au même réseau** (192.168.1.x)
2. **Ouvrir le navigateur** : `http://192.168.1.100`
3. **Modifier la configuration** via l'API REST :

#### GET Configuration
```bash
curl http://192.168.1.100/api/config
```

#### POST Nouvelle IP Ethernet
```bash
curl -X POST http://192.168.1.100/api/config/network \
  -H "Content-Type: application/json" \
  -d '{
    "dhcp": false,
    "ip": "192.168.1.150",
    "gw": "192.168.1.1",
    "mask": "255.255.255.0"
  }'
```

#### POST Adresse OSC d'un Relais
```bash
curl -X POST http://192.168.1.100/api/config/relays \
  -H "Content-Type: application/json" \
  -d '{
    "relays": [
      {"oscAddress": "/scene/light/1", "invert": false, "mode": 0},
      {"oscAddress": "/scene/light/2", "invert": false, "mode": 0}
    ]
  }'
```

---

## 🎛️ Étape 6 : Test OSC

### Avec `oscsend` (Linux/macOS)
```bash
# Installer liblo (si absent)
brew install liblo

# Activer relais 0
oscsend 192.168.1.100 8000 /relay/0 i 1

# Désactiver relais 0
oscsend 192.168.1.100 8000 /relay/0 i 0

# Toggle relais 3
oscsend 192.168.1.100 8000 /relay/3 f 1.0
```

### Avec Python + `python-osc`
```python
from pythonosc import udp_client

client = udp_client.SimpleUDPClient("192.168.1.100", 8000)

# Activer relais 0
client.send_message("/relay/0", 1)

# Désactiver relais 0
client.send_message("/relay/0", 0)
```

### Logs OSC (Moniteur Série)
```
[OSC] Received: /relay/0 i:1 → Relay 0 = ON
[RELAY] Relay 0 set to ON (physical: ON)
[OSC] Received: /relay/0 i:0 → Relay 0 = OFF
[RELAY] Relay 0 set to OFF (physical: OFF)
```

---

## ⚙️ Fonctionnalités Avancées

### Watchdog (Protection Crash)
- **Timeout** : 10 secondes
- Si le `loop()` ne se termine pas sous 10s → **Reset automatique**
- Logs : `[WATCHDOG] Feed` toutes les 100ms

### Hot Reload Réseau
```bash
curl -X POST http://192.168.1.100/api/config/network \
  -H "Content-Type: application/json" \
  -d '{"dhcp": true}'
```
**Note** : Changement appliqué, mais **reboot recommandé** pour Ethernet

### Logging Détaillé
Activer/désactiver dans [platformio.ini](platformio.ini):
```ini
-D ENABLE_OSC_LOGGING=1      ; Logs OSC détaillés
-D ENABLE_NETWORK_LOGGING=1  ; Logs réseau
-D ENABLE_RELAY_LOGGING=1    ; Logs relais
```

### Modes Relais
- **Mode 0** : Direct (OSC contrôle immédiat)
- **Mode 1** : Toggle (chaque message inverse l'état)
- **Mode 2** : Pulse (impulsion 500ms)

---

## 🐛 Dépannage

### Problème : Pas de port série détecté
**Solution** :
1. Vérifier le câble USB-C (données + alimentation)
2. Installer driver CH340 : `brew install --cask wch-ch34x-usb-serial-driver`
3. Redémarrer macOS

### Problème : Upload échoue "Failed to connect"
**Solution** :
1. Maintenir **BOOT** + appuyer **RESET**
2. Relâcher **RESET** puis **BOOT**
3. Relancer `pio run --target upload`

### Problème : PCA9554 non détecté
**Solution** :
1. Vérifier câblage I2C : SDA=GPIO8, SCL=GPIO9
2. Vérifier alimentation 3.3V du PCA9554
3. Scanner I2C : ajouter code `Wire.beginTransmission(0x20);`

### Problème : Pas d'IP réseau
**Solution** :
1. Vérifier câble Ethernet
2. Activer mode WiFi AP : `http://192.168.4.1` (SSID: ESP32-OSC-Relay)
3. Vérifier config DHCP/statique

### Problème : Watchdog reset en boucle
**Solution** :
1. Vérifier que `loop()` ne bloque pas (pas de `delay()` > 100ms)
2. Augmenter timeout : `-D WATCHDOG_TIMEOUT_MS=30000`
3. Désactiver temporairement : `-D WATCHDOG_ENABLED=0`

---

## 📊 Statistiques Firmware

- **RAM utilisée** : 48 KB / 320 KB (14.7%)
- **Flash utilisée** : 1 016 KB / 6 554 KB (15.5%)
- **Réserve disponible** : ~5.5 MB pour OTA/logs/features

---

## 🔄 Mise à Jour du Firmware

### Upload via USB (Standard)
```bash
cd /Users/tof/Desktop/ESP32-S3-OSC-8RELAY/ESP32-S3-OSC-8Relay
pio run --target upload
```

### OTA via Réseau (Future)
```bash
# À implémenter : ArduinoOTA
pio run --target upload --upload-port 192.168.1.100
```

---

## 📝 Checklist Post-Installation

- [ ] Compilation réussie (`pio run`)
- [ ] Upload réussi (`pio run --target upload`)
- [ ] Logs série visibles (`pio device monitor`)
- [ ] IP réseau accessible (ping `192.168.1.100`)
- [ ] Interface web répond (`http://192.168.1.100`)
- [ ] API REST fonctionnelle (`curl /api/config`)
- [ ] Messages OSC reçus (logs `[OSC]`)
- [ ] Relais commutent physiquement
- [ ] Watchdog actif (logs `[WATCHDOG] Feed`)
- [ ] Configuration sauvegardée (NVS persistante)

---

## 📞 Support

**Problème persistant ?**
1. Activer logs verbeux : `pio run -v`
2. Vérifier les erreurs dans le moniteur série
3. Consulter la documentation ESP32-S3 : https://docs.espressif.com/

**GitHub** : [À compléter avec URL du repo]
**Version** : 1.0.0 (Janvier 2026)

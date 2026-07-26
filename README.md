# SQLite C API Demo (Libsqlite3)

Un'applicazione dimostrativa robusta e modulare in **C** progettata per illustrare l'utilizzo completo e avanzato delle API ufficiali di **SQLite3 (`libsqlite3`)**. 

Questo progetto ricalca la struttura e la completezza funzionale dei driver client per database relazionali, offrendo una panoramica esaustiva di tutte le funzionalità chiave messe a disposizione da SQLite.

---

## 🚀 Funzionalità Coperte (13 Step Operativi)

1. **Connessione e Aperture (`sqlite3_open`)**: Inizializzazione del database file-based con configurazione ottimizzata tramite parametri PRAGMA (`journal_mode = WAL`, `foreign_keys = ON`).
2. **Metadati di Sistema**: Estrazione della versione della libreria, ID sorgente e metriche di consumo di memoria in tempo reale (`sqlite3_status64`).
3. **DDL (`CREATE TABLE`)**: Creazione di tabelle relazionali con vincoli di integrità (`INTEGER PRIMARY KEY AUTOINCREMENT`, `TEXT`, `BLOB`).
4. **INSERT Dinamico (`sqlite3_mprintf`)**: Inserimento di dati grezzi con escaping nativo sicuro contro SQL Injection senza l'uso di prepared statements.
5. **SELECT e Scansione (`sqlite3_prepare_v2` / `sqlite3_step`)**: Esecuzione di interrogazioni e scorrimento riga per riga del cursore dei risultati.
6. **UPDATE**: Aggiornamento condizionale dei record con tracciamento delle righe modificate (`sqlite3_changes`).
7. **DELETE**: Cancellazione mirata di record dal database.
8. **Prepared Statements (INSERT Parametrizzato)**: Compilazione anticipata di statement SQL e binding sicuro dei parametri (`sqlite3_bind_*`).
9. **Prepared Statements (SELECT Parametrizzato)**: Interrogazioni con parametri segnaposto (`?`) ed estrazione tipizzata dei dati dai campi.
10. **Transazioni ACID**: Gestione esplicita di transazioni atomiche tramite comandi SQL (`BEGIN`, `COMMIT`, `ROLLBACK`).
11. **Gestione BLOB (Binary Large Objects)**: Inserimento e lettura di dati binari grezzi (es. flussi immagine o payload serializzati) tramite buffer dedicati (`sqlite3_bind_blob` / `sqlite3_column_blob`).
12. **Funzioni SQL Custom (Scalar Functions)**: Registrazione ed esecuzione di una funzione definita in C (es. `STRLEN_CUSTOM`) richiamabile direttamente all'interno delle query SQL (`sqlite3_create_function`).
13. **SQLite Backup API**: Esecuzione di un backup a caldo (hot backup) in tempo reale su un secondo database di riserva (`sqlite3_backup_init` / `sqlite3_backup_step`).
14. **Cleanup (`DROP TABLE`)**: Rimozione pulita degli oggetti di schema e chiusura corretta delle risorse.

---

## 📁 Struttura del Progetto

```text
sqlite_c_demo/
├── CMakeLists.txt         # File di configurazione Build System CMake
├── include/
│   └── db_ops.h           # Header condiviso, macro di errore e dichiarazioni
└── src/
    ├── main.c             # Entry point principale e sequenza dei 13 step
    └── db_ops.c           # Implementazione dettagliata delle operazioni SQLite
```

---

## 🛠️ Requisiti di Sistema

- **Compilatore C99** (GCC, Clang, MSVC)
- **CMake** (versione 3.14 o superiore)
- **Libreria di sviluppo SQLite3** (`libsqlite3-dev` su sistemi Debian/Ubuntu, `sqlite3` su macOS Homebrew o pacchetto nativo Windows).

---

## ⚙️ Istruzioni di Compilazione ed Esecuzione

Clona o scarica il repository, quindi esegui i seguenti comandi nel terminale:

```bash
# 1. Posizionati nella directory del progetto
cd sqlite_c_demo

# 2. Crea la cartella di build
mkdir build && cd build

# 3. Configura il progetto con CMake
cmake ..

# 4. Compila l'eseguibile
cmake --build .

# 5. Esegui la demo
./sqlite_demo
```

---

## 📦 Installazione delle Dipendenze (Riferimento Rapido)

- **Ubuntu / Debian**:
  ```bash
  sudo apt-get update && sudo apt-get install build-essential cmake libsqlite3-dev
  ```
- **macOS (Homebrew)**:
  ```bash
  brew install cmake sqlite3
  ```
- **Windows (vcpkg / MSYS2)**:
  ```bash
  pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-sqlite3
  ```

---

## 📜 Licenza

Questo progetto è distribuito sotto licenza **MIT**. Sentiti libero di utilizzarlo come base o riferimento per i tuoi progetti in C e SQLite.

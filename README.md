
```markdown
# Clinic_Tcp_Server

A minimal TCP client/server written in C. The server stores basic patient
information (age and blood group) in memory and answers simple text requests
sent by the client.

---

## English

### Project layout

```
AbomeyCli/
├── Makefile
├── client/
│   └── main.c        # TCP client
└── server/
    ├── main.c        # TCP server
    └── registre.txt  # optional persistent record file
```

### Requirements

- A C compiler (`gcc` or `clang`)
- `make`
- Linux / macOS (POSIX sockets)

### Build

From the project root:

```bash
make          # build both client and server
make client   # build only the client  -> client/main
make server   # build only the server  -> server/main
make clean    # remove both executables
make re       # clean + rebuild
```

The executables are produced inside each folder:

- `client/main`
- `server/main`

### Run

Open **two terminals** from the project root.

Terminal 1 — start the server:

```bash
./server/main
```

Terminal 2 — start the client:

```bash
./client/main
```

The server listens on a TCP port (see the constant in `server/main.c`).
The client connects to `127.0.0.1` on that port and sends requests.


```

### Limitations

- Single-process, single-threaded server.
- Data is kept in memory (and optionally in `registre.txt`).
- **Not for real clinical use** — this is a learning project.

---

## Français

### Arborescence du projet

```
AbomeyCli/
├── Makefile
├── client/
│   └── main.c        # client TCP
└── server/
    ├── main.c        # serveur TCP
    └── registre.txt  # fichier de registre optionnel
```

### Prérequis

- Un compilateur C (`gcc` ou `clang`)
- `make`
- Linux / macOS (sockets POSIX)

### Compilation

Depuis la racine du projet :

```bash
make          # compile le client et le serveur
make client   # compile seulement le client  -> client/main
make server   # compile seulement le serveur -> server/main
make clean    # supprime les deux exécutables
make re       # nettoie puis recompile tout
```

Les exécutables sont créés dans chaque dossier :

- `client/main`
- `server/main`

### Lancement

Ouvre **deux terminaux** depuis la racine du projet.

Terminal 1 — lancer le serveur :

```bash
./server/main
```

Terminal 2 — lancer le client :

```bash
./client/main
```

Le serveur écoute sur un port TCP (voir la constante dans `server/main.c`).
Le client se connecte à `127.0.0.1` sur ce port et envoie des requêtes.

```

### Limites

- Serveur mono-processus, mono-thread.
- Données gardées en mémoire (et éventuellement dans `registre.txt`).
- **Pas destiné à un usage clinique réel** — projet pédagogique.
```
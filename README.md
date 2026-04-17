# Rečko — Serbian Wordle Clone

A word-guessing game built with **Qt (C++)** and **MySQL**, developed as a Bachelor's thesis project at the University of Belgrade, Faculty of Mechanical Engineering (2024).

Rečko is a Serbian-language replica of the popular game *Wordle*. The player has 6 attempts to guess a randomly selected 5-letter Serbian word, with color-coded letter feedback after each guess.

---

## ⚠️ Important Notes Before Running

This project was developed and tested on **Ubuntu Linux** with a **local MySQL server**. It requires:

- Qt 5 with the `sql` module
- A running MySQL instance with a specific database and tables set up (see below)
- The MySQL Qt plugin (`libqt5sql5-mysql`)

The database credentials are currently hardcoded in `main.cpp`:
```cpp
db.setHostName("localhost");
db.setDatabaseName("recko");
db.setUserName("miha907");
db.setPassword("miha907miha907");
```
You will need to change these to match your own MySQL setup before building.

---

## 🎮 Gameplay

- Press **START** to begin. A random 5-letter Serbian word is selected from the database and the timer starts.
- Type a 5-letter word and press **ENTER** to submit a guess.
- After each guess, each letter is colored:
  - 🟩 **Green** — correct letter, correct position
  - 🟨 **Yellow** — correct letter, wrong position
  - ⬜ **Gray** — letter not in the word
- You have **6 attempts** to guess the word.
- On success: the timer stops and "**Pobeda!**" is displayed.
- On failure: "**Poraz!**" is shown along with the correct word.
- The game is **case-insensitive** and supports the Serbian alphabet (š, đ, ć, ž, č).
- Guesses must exist in the database's validation table — random strings are rejected.

---

## 📁 Project Structure

```
.
├── src/
│   ├── main.cpp            # App entry point, MySQL connection setup
│   ├── mainwindow.cpp      # Game logic, UI slots, feedback coloring
│   ├── mainwindow.h        # MainWindow class declaration
│   └── mainwindow.ui       # Qt Designer UI layout
├── db/
│   ├── provera.cpp         # Standalone utility: bulk-inserts words from .txt into MySQL
│   ├── reci.txt            # Word list for the secret word pool (reci table)
│   └── dodatak.txt         # Supplementary words for the validation table (provera)
└── 20202018.pro            # Qt project file
```

---

## 🛠️ Prerequisites

### 1. Install Qt 5

```bash
sudo apt update
sudo apt install qt5-default qtcreator
```

### 2. Install the Qt MySQL Plugin

```bash
sudo apt install libqt5sql5-mysql
```

### 3. Install MySQL Server

```bash
sudo apt install mysql-server
sudo mysql
```

---

## 🗄️ Database Setup

All commands are run inside the MySQL console (`sudo mysql`).

### Create the database and tables

```sql
CREATE DATABASE recko;
USE recko;

-- Table for secret words (randomly selected during gameplay)
CREATE TABLE reci (
    id INT AUTO_INCREMENT PRIMARY KEY,
    rec VARCHAR(5)
);

-- Table for word validation (checks if a guessed word is a real Serbian word)
CREATE TABLE provera (
    id INT AUTO_INCREMENT PRIMARY KEY,
    proveri VARCHAR(5)
);
```

### Create a MySQL user

```sql
CREATE USER 'miha907'@'localhost' IDENTIFIED BY 'miha907miha907';
GRANT ALL PRIVILEGES ON recko.* TO 'miha907'@'localhost';
FLUSH PRIVILEGES;
```

> Or update the credentials in `main.cpp` to match an existing user on your system.

### Populate the `reci` table

You can insert words manually:

```sql
USE recko;
INSERT INTO reci (rec) VALUES ('Staza'), ('Balon'), ('Pesma'), ('Slika'), ('Vreme');
```

Or load them from `reci.txt` using MySQL's `LOAD DATA` command:

```sql
LOAD DATA LOCAL INFILE '/path/to/reci.txt'
INTO TABLE reci (rec);
```

### Populate the `provera` table (validation word list)

The `provera.cpp` utility reads words from a `.txt` file and bulk-inserts them into the `provera` table using the MySQL C++ Connector. Compile and run it separately:

```bash
g++ provera.cpp -o provera -lmysqlcppconn
./provera
```

> Make sure `dodatak.txt` (or your word list file) is in the same directory, or update the `filePath` variable in `provera.cpp`. The `provera` table ships with ~12,000 Serbian words.

---

## 🔨 Building the Qt Application

### Option A: Qt Creator (recommended)

1. Open `20202018.pro` in Qt Creator.
2. Configure the kit for your Qt 5 installation.
3. Build and run.

### Option B: Command line

```bash
qmake 20202018.pro
make
./20202018
```

Make sure `QT += sql` is present in the `.pro` file (it already is).

---

## 🧩 How It Works

### `generateSecretWord()`
Queries all rows from the `reci` table, loads them into a `QStringList`, then uses `QRandomGenerator` seeded with the current Unix timestamp (milliseconds ÷ 1000) to pick a random word.

### `checkStringExists()`
Before processing any guess, this method runs a `SELECT COUNT(*)` query against the `provera` table to verify the input exists as a real Serbian word. Invalid guesses are rejected with a dialog box.

### `isGuessCorrect()`
Implements the full Wordle feedback algorithm with correct handling of duplicate letters:

1. A first pass marks all **exact matches** (green) and decrements the letter frequency map.
2. A second pass checks remaining letters for **positional mismatches** (yellow), using the frequency map to avoid over-counting duplicate letters.
3. Feedback is rendered as HTML color tags and appended to the history label in the UI.

---

## 🔧 Known Limitations & Possible Improvements

- **Hardcoded credentials** — database connection details are in plain text in `main.cpp`. These should be moved to a config file or environment variables.
- **Linux only** — tested exclusively on Ubuntu. Qt is cross-platform but the MySQL plugin setup differs on Windows/macOS.
- **Small secret word pool** — `reci.txt` contains around 20 words. The pool can be expanded for better replayability.
- **No score history** — games are not persisted; there is no win/loss record across sessions.
- **Serbian diacritic coverage** — characters like š, đ, ć, ž, č are accepted as input but validation table coverage for accented forms is incomplete.

Possible directions for improvement include expanding the word database, migrating to SQLite to remove the local server dependency, and adding a statistics/leaderboard screen.

---

## 📚 Built With

| Technology | Purpose |
|---|---|
| [Qt 5](https://wiki.qt.io/About_Qt) | GUI framework, signals/slots, UI design |
| [MySQL](https://dev.mysql.com/doc/) | Word storage and validation |
| [QtSql / QMYSQL](https://doc.qt.io/qt-5/qtsql-index.html) | Qt–MySQL bridge |
| C++17 | Core application language |

---

## 🎓 Academic Context

This project was submitted as a Bachelor's (B.Sc.) thesis at the **University of Belgrade, Faculty of Mechanical Engineering**, under the study program *Information Technologies in Mechanical Engineering*, for the course *Object-Oriented Programming* (2024).

**Author:** Mihajlo Katić  
**Supervisor:** Prof. dr Slobodan Radojević

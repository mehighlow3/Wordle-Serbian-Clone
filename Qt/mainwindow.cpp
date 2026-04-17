#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMainWindow>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>
#include <QFile>

#include <QCoreApplication>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QDebug>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QDateTime>

#include <QWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    timer(new QTimer(this)),
        secondsElapsed(0)
{
    ui->setupUi(this);
    setWindowTitle("Rečko");

    connect(timer, &QTimer::timeout, this, &MainWindow::updateTime);

    setupUI1();
}

MainWindow::~MainWindow()
{
        delete ui;
}

void MainWindow::setupUI1()
{
    ui->guessButton->setEnabled(false);
    ui->guessButton->setStyleSheet("background-color: #36454F");
    ui->guessLineEdit->setEnabled(false);

    connect(ui->guessButton, &QPushButton::clicked, this, &MainWindow::on_guessButton_clicked);
    connect(ui->startButton, &QPushButton::clicked, this, &MainWindow::on_startButton_clicked);
}

void MainWindow::on_startButton_clicked()
{
    generateSecretWord();

    ui->wordLabel->setText("Pogađaj reč od 5 slova : " + QString(guessedWord.length(), '_'));

    ui->startButton->setEnabled(false);
    ui->startButton->setStyleSheet("background-color: #36454F");
    ui->guessButton->setEnabled(true);
    ui->guessButton->setStyleSheet("background-color: #006400");
    ui->guessLineEdit->setEnabled(true);

    number_of_guesses = 6;

    ui->attemptsLabel->setText("Preostali pokušaji: " + QString::number(number_of_guesses));

    secondsElapsed = 0;

    if (timer->isActive())
        {
            timer->stop();
        }

        timer->start(1000);

}
void MainWindow::updateTime()
{
    secondsElapsed++;
    ui->lcdNumber->display(secondsElapsed);
}


void MainWindow::on_guessButton_clicked()
{
    QString guess = ui->guessLineEdit->text();
    if (guess.isEmpty())
    {
        return;
    }

    if (guess.length() != 5)
    {
        QMessageBox::warning(this,"Greška", "Reč mora da ima 5 slova.");
        ui->guessLineEdit->clear();
        return;
    }
       if (!checkStringExists(guess))
       {
           QMessageBox::warning(this, "Greška", "Reč nije pronađena u bazi podataka.");
           ui->guessLineEdit->clear();
           return;
       }

    if (isGuessCorrect(guess))
    {
        ui->startButton->setEnabled(true);
        ui->guessButton->setEnabled(false);
        ui->guessLineEdit->setEnabled(false);
        ui->feedbackLabel->clear();
        ui->guessLineEdit->clear();
        guessedWord.clear();
    }
    else
    {
        number_of_guesses--;

        if (number_of_guesses == 0)
        {
            ui->wordLabel->setText("<font color=\"red\">Poraz!</font> reč je: " + secretWord.toUpper());
            ui->startButton->setEnabled(true);
            ui->guessButton->setEnabled(false);
            ui->guessLineEdit->setEnabled(false);
            guessedWord.clear();
            ui->feedbackLabel->clear();
            ui->guessLineEdit->clear();
            ui->attemptsLabel->setText(" Preostali pokušaji: <font color=\"red\">0</font> ");
        }
        else
        {
            ui->guessLineEdit->clear();
            ui->attemptsLabel->setText("Preostali pokušaji: " + QString::number(number_of_guesses));
        }
    }

}

void MainWindow::generateSecretWord()
{
    QSqlQuery query;

    if (!query.exec("SELECT * FROM reci"))
    {
        QMessageBox::critical(this, "Greška", "neuspešno pristupanje bazi " + query.lastError().text());
        return;
    }

    QStringList words;
    while (query.next())
    {
        QString word = query.value(1).toString();
        words.append(word);
    }

    if (words.isEmpty())
    {
        QMessageBox::critical(this, "Greška", "baza je prazna.");
        return;
    }

    QRandomGenerator randomGenerator(QDateTime::currentMSecsSinceEpoch() / 1000);
    int index = randomGenerator.bounded(0, words.size());
    secretWord = words.at(index);
    guessedWord = QString(secretWord.length(), '_');

    qDebug() << "Tražena reč je:" << secretWord;
}

bool MainWindow::checkStringExists(const QString& inputString)
{

    QSqlQuery query;

    query.prepare("SELECT COUNT(*) FROM provera WHERE proveri = ?");

    query.addBindValue(inputString);

    if (!query.exec())
    {
        QMessageBox::critical(this, "Error", "Failed to execute query: " + query.lastError().text());
        return false;
    }

    if (query.next())
    {
        int count = query.value(0).toInt();
        return count > 0;
    }

    return false;
}

bool MainWindow::isGuessCorrect(const QString &guess)
{
    QString uppercaseGuess = guess.toUpper();
    QString lowercaseGuess = guess.toLower();
    QString lowercaseSecretWord = secretWord.toLower();

    if (guessedWord.isEmpty())
    {
        guessedWord = QString(secretWord.length(), '_');
    }

    QMap<QChar, int> secretWordFrequency;
    QMap<QChar, int> guessFrequency;
    QMap<QChar, int> correctGuessFrequency;

    for (int i = 0; i < lowercaseSecretWord.length(); ++i)
    {
        QChar charSecret = lowercaseSecretWord.at(i);
        secretWordFrequency[charSecret]++;
    }

    for (int i = 0; i < lowercaseGuess.length(); ++i)
    {
        QChar charGuess = lowercaseGuess.at(i);
        guessFrequency[charGuess]++;
    }

    QStringList feedback(secretWord.length(), QString());
    QMap<QChar, int> usedInGuess;

    for (int i = 0; i < secretWord.length(); ++i)
    {
        QChar guessChar = lowercaseGuess.at(i);
        QChar secretChar = lowercaseSecretWord.at(i);

        if (guessChar == secretChar) {
            feedback[i] = QString("<font color=\"green\">%1</font>").arg(uppercaseGuess.at(i));
            correctGuessFrequency[guessChar]++;
            usedInGuess[guessChar]++;
            secretWordFrequency[secretChar]--;

            guessedWord[i] = uppercaseGuess.at(i);
        }
    }

    for (int i = 0; i < secretWord.length(); ++i)
    {
        if (feedback[i].isEmpty())
        {
            QChar guessChar = lowercaseGuess.at(i);
            if (secretWordFrequency.contains(guessChar) && secretWordFrequency[guessChar] > 0)
            {

                int availableInstances = secretWordFrequency[guessChar] + correctGuessFrequency[guessChar] - usedInGuess.value(guessChar, 0);
                if (availableInstances > 0)
                {
                    feedback[i] = QString("<font color=\"yellow\">%1</font>").arg(uppercaseGuess.at(i));
                    usedInGuess[guessChar]++;
                } else {
                    feedback[i] = QString("%1").arg(uppercaseGuess.at(i));
                }
            } else {
                feedback[i] = QString("%1").arg(uppercaseGuess.at(i));
            }
        }
    }

    QString feedbackHtml = feedback.join("");

    ui->feedbackLabel->setText(ui->feedbackLabel->text() + QString(" ") + feedbackHtml + QString(", "));

    QString spacedGuessedWord = guessedWord.split("").join(" ").trimmed();

    ui->wordLabel->setText("Pogađaj reč: " + spacedGuessedWord);

    if (lowercaseGuess == lowercaseSecretWord)
    {
        ui->wordLabel->setText("<font color=\"lime\">Pobeda!</font> reč je: " + secretWord.toUpper());
        timer->stop();

        return true;
    } else {
        ui->wordLabel->setText("Pogađaj reč od 5 slova: " + spacedGuessedWord.toUpper());
        return false;
    }
}





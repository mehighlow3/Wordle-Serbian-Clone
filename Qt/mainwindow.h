#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMessageBox>
#include <QTimer>
#include <QElapsedTimer>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

private slots:

    void on_startButton_clicked();

    void on_guessButton_clicked();

private:
    Ui::MainWindow *ui;

    QStringList wordList;

    QString secretWord;
    QString guessedWord;

    QTimer *timer;
    int secondsElapsed;

    int number_of_guesses;

    void setupUI1();


    void generateSecretWord();
    bool isGuessCorrect(const QString &guess);
    bool checkStringExists(const QString& inputString);
    void updateTime();
};
#endif // MAINWINDOW_H


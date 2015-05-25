#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class QGridLayout;


class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString currentfile;
    QString starttag;
    void initialize(void);

private slots:
    void on_pushButton_clicked();

private:

    void setButtons(QString file, QString tag="Menu");
    void rmButtons(void);
    void SetFile(QString file, QString tag);


    Ui::MainWindow *ui;
    QStringList cmds;
    QWidgetList widgets;
    int nMax,maxi;
    int rowhi, colwidth;
    bool launchexit;
    QString defstyle;
    bool shrink;
};

#endif // MAINWINDOW_H

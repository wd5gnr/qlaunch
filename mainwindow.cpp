#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QSettings>
#include <QPushButton>
#include <QProcess>
#include <QMessageBox>

#include "expando.h"

#include <QDebug>

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */



/*
 *
 *
 *
 *  Possible TODO
 * 1: Could do an "edit mode" (command line option or menu item). Have an "add row", "del row", "add col", "del col" item. Pushing on button would bring up a dialog to edit/add/clear that button.
 * Then another menu brings up the generic option dialog that lets you set up the [Menu] or [<tag>] sections
 * 2: Make Expando respect a $QLPROMPT that pops up a box (done)
 * */

void MainWindow::SetFile(QString file,QString tag)
{

    QSettings ini(file,QSettings::IniFormat);
    if (nMax!=0) rmButtons();  // remove existing, if applicable
    currentfile=file;
    // Read main options
    ini.beginGroup("Main");
    int opts=this->windowFlags()&~(Qt::FramelessWindowHint|Qt::WindowStaysOnTopHint);
    maxi=ini.value("max",1000).toInt();
//    rowhi=ini.value("height",-1).toInt();
//    colwidth=ini.value("width",-1).toInt();

    bool frameless=ini.value("frameless",false).toBool();
    bool ontop=ini.value("ontop",false).toBool();
    QString title=ini.value("title","QLaunch").toString();
    launchexit=ini.value("launchexit",false).toBool();

    QString wstyle=ini.value("style","").toString();
    defstyle=ini.value("buttonstyle","").toString();
    shrink=ini.value("shrink",true).toBool();
    ini.endGroup();

    ini.beginGroup(tag);
    if (ini.value("frameless",frameless).toBool()) opts|=Qt::FramelessWindowHint;
    if (ini.value("ontop",ontop).toBool())  opts|=Qt::WindowStaysOnTopHint;
    wstyle=ini.value("style",wstyle).toString();
    defstyle=defstyle + " " + ini.value("buttonstyle","").toString();
    launchexit=ini.value("launchexit",launchexit).toBool();
    shrink=ini.value("shrink",shrink).toBool();
    this->setWindowTitle(ini.value("title",title).toString());
    ini.endGroup();



    this->setWindowFlags((Qt::WindowType)opts);
    this->setStyleSheet(wstyle);
    this->update();



}


void MainWindow::setButtons(QString file, QString tag)
{
    QSettings ini(file,QSettings::IniFormat);
    QGridLayout *grid=new QGridLayout();
    int i;
    SetFile(file,tag);
// scan for buttons
    for (i=0;i<=maxi;i++)
    {
        QString key=tag + QString::number(i+1);
        QPushButton *btn;
        ini.beginGroup(key);
        int row=ini.value("row",-1).toInt();
        int col=ini.value("col").toInt();
        if (row==-1)
        {
            ini.endGroup();
            continue;                     // don't break in case author skips
        }
        QString name=ini.value("name").toString();
        QString icon=ini.value("icon").toString();
        QString cmd=ini.value("cmd").toString();
        QString style=ini.value("style","").toString();
        QString tip=ini.value("tip","").toString();
        QString objectname=ini.value("buttonclass","MenuButton").toString();
        style=defstyle+" "+style;
        ini.endGroup();
        cmds.insert(nMax,cmd);
        btn=new QPushButton(name, this);
        if (!(icon.isEmpty()||icon.isNull()))
        {
            QIcon qicon(icon);
//            btn->setIconSize(QSize(btn->width()/2,btn->height()-10));
            btn->setIcon(qicon);
        }
        if (!style.isEmpty()) btn->setStyleSheet(style);
//        btn->setSizePolicy(colwidth==-1?QSizePolicy::Preferred:QSizePolicy::Expanding,rowhi==-1?QSizePolicy::Preferred:QSizePolicy::Expanding);
        btn->setToolTip(tip);
        btn->setEnabled(true);
        btn->setVisible(true);
        btn->setProperty("N",nMax);  // set N property so we can find it later in slot
        btn->setObjectName("MenuButton");
        widgets.append(btn);         // add to list
        QObject::connect(btn,SIGNAL(clicked()),this,SLOT(on_pushButton_clicked()));
        grid->addWidget(btn,row-1,col-1);  // add to grid
        nMax++;

    }

    // if column/row size in force, set that
   //  if (rowhi!=-1) /* rowhi=0; */ for (i=0;i<grid->rowCount();i++) grid->setRowMinimumHeight(i,rowhi);
   //  if (colwidth!=-1) /* colwidth=0; */ for (i=0;i<grid->columnCount();i++) grid->setColumnMinimumWidth(i,colwidth);

     ui->centralWidget->setSizePolicy(QSizePolicy::Minimum,QSizePolicy::Minimum);

     grid->setSizeConstraint(QLayout::SetFixedSize);

     // Try to set layout
     delete ui->centralWidget->layout();
     ui->centralWidget->setLayout(grid);
     this->setVisible(true);  // setWindowFlags hides window!
     if (shrink)
     {
        ui->centralWidget->adjustSize();
        this->adjustSize();
     }
}

void MainWindow::rmButtons()
{
    cmds.clear();
    while (!widgets.isEmpty())
    {
       QWidget *w=widgets.takeFirst();
       this->centralWidget()->layout()->removeWidget(w);
       w->deleteLater();
    }
     nMax=0;

}

void MainWindow::initialize()
{
    QString file=currentfile;
    QSettings ini(file,QSettings::IniFormat);
    ini.beginGroup("Main");
    int startx=ini.value("startx",-1).toInt();
    int starty=ini.value("starty",-1).toInt();
    QString tag=starttag;
    if (tag.isEmpty()) tag=ini.value("starttag","Menu").toString();
     ini.endGroup();
    if (startx!=-1 && starty!=-1) this->move(startx,starty);
    nMax=0;
    setButtons(file,tag);

}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}





void MainWindow::on_pushButton_clicked()
{
    int N=QObject::sender()->property("N").toInt();
    QProcess *process=new QProcess(this);
    QString cmd=expando(cmds[N]);
    if (cmd[0]=='*')
    {
        // special command
        if (cmd=="**EXIT")
        {
            this->close();
        }
        else if (cmd=="**TOP")
        {
            int opts=this->windowFlags()|(Qt::WindowStaysOnTopHint);
            this->setWindowFlags((Qt::WindowFlags)opts);
            this->setVisible(true);

        }
        else if (cmd=="**NOTOP")
        {
             int opts=this->windowFlags()&~(Qt::WindowStaysOnTopHint);
             this->setWindowFlags((Qt::WindowFlags)opts);
             this->setVisible(true);
        }
        else if (cmd[1]!='*')
        {
            if (cmd.mid(1,1)=="!")
            {
                QMessageBox dlg;
                dlg.setText(cmd.mid(2));
                dlg.exec();
            }
            else if (cmd.mid(1,1)=="#")
            {
                // new tag in current file
                setButtons(currentfile,cmd.mid(2));
            } else
        // load new menu
               setButtons(cmd.mid(1),"Menu");
        }
        else
            qDebug()<<"Unknown special command\n";
    }
    else
    {
        process->startDetached(cmds[N]);
        process->waitForStarted();
        if (launchexit) this->close();
    }



}

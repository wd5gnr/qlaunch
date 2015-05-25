#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QDesktopServices>
#include <QCommandLineParser>
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


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setApplicationName("qlaunch");
    a.setApplicationVersion("1.0");
    QCommandLineParser cmdline;
    cmdline.setApplicationDescription("Quick Laucnher");
    cmdline.addHelpOption();
    cmdline.addVersionOption();
    cmdline.addPositionalArgument("file","File that contains the menu definition");

    QCommandLineOption tagopt(QStringList()<<"t"<<"tag","Sets initial tag (default=Menu)","tag");
    cmdline.addOption(tagopt);
    QCommandLineOption styleopt(QStringList()<<"s"<<"style","Sets base stylesheet","style");
    cmdline.addOption(styleopt);
    cmdline.process(a);

    qDebug()<<cmdline.value(styleopt)<<"\n";

// Load default built-in style sheet
    QFile file(":/app/css/qlaunch.css");
    file.open(QFile::ReadOnly);
    qApp->setStyleSheet(QLatin1String(file.readAll()));
// However, if user provides file, use that
    QString cfg=QStandardPaths::locate(QStandardPaths::ConfigLocation,"qlaunch.css");
    if (QFile::exists(cfg))
    {
        QFile efile(cfg);
        efile.open(QFile::ReadOnly);
        qApp->setStyleSheet(QLatin1String(efile.readAll()));
    }

// and pick up command line if present
    QString ccfg=cmdline.value(styleopt);
    if (!ccfg.isEmpty())
    {
        if (QFile::exists(ccfg))
        {
            QFile efile(ccfg);
            efile.open(QFile::ReadOnly);
            qApp->setStyleSheet(QLatin1String(efile.readAll()));
        }
    }
    MainWindow w;
    w.starttag=cmdline.value(tagopt);
    if (w.starttag.isEmpty()) w.starttag="";
    QString defmenu;   // priority: command line, .config /etc/xdg    .local/share/qlaunch   /usr/local/share/qlaunch   /usr/share/qlaunch   ~
    if (!cmdline.positionalArguments().isEmpty()) defmenu=cmdline.positionalArguments()[0];
    if (defmenu.isEmpty()) defmenu=QStandardPaths::locate(QStandardPaths::ConfigLocation,"qlaunch.menu");
    if (defmenu.isEmpty()) defmenu=QStandardPaths::locate(QStandardPaths::AppDataLocation,"qlaunch.menu");
    if (defmenu.isEmpty()) defmenu=QStandardPaths::locate(QStandardPaths::HomeLocation,"qlaunch.menu");
    w.currentfile=defmenu;

    w.setWindowIcon(QIcon(":/app/images/rocket.jpg"));
    w.initialize();
    w.show();

    return a.exec();
}

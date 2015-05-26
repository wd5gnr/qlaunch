#include <Qt>
#include <QString>
#include <QProcessEnvironment>
#include <QInputDialog>
#include "expando.h"

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
    */

/* Simple function to expand environment variables
 * Can handle $FOO or $(FOO)
 * Respects escape characters (like \$FOO or even $\(FOO
 */



QString expando(const QString cmd)
{
    QString rv;
    QString word, c, endc;
    QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
    rv.reserve(cmd.length()+1024);
    int state=0;
    int escape=0;
    for (int i=0;i<cmd.length();i++)
    {
        c=cmd.mid(i,1);
        if (escape!=0)
        {
            if (escape==1) rv+=c; else word+=c;
            escape=0;
            continue;
        }
        switch (state)
        {
        case 2:   // looking for word sep
            if (c=="\\")
            {
                escape=2;
                continue;
            }
            if (c[0].isLetterOrNumber()||c[0]=='-'||c[0]=='_')   // is this list complete?
            {
                word+=c;
                continue;
            }
            rv+=env.value(word,"");
            state=0;
// fall through
        case 0:   // no env
            if (c=="\\")
            {
                escape=1;
                continue;
            }
            if (c!="$")
            {
                rv+=c;
                continue;
            }
            word="";
            state=1;
            continue;

        case 1:   // found $
            if (c=="("||c=="{"||c=="[")
            {
                endc=")";
                if (c=="{") endc="}";
                if (c=="[") endc="]";
                state=3;
                continue;
            }
            if (c!="\\") word+=c;
            state=2;
            continue;


        case 3:   // looking for )
            if (c=="\\")
            {
                escape=2;
                continue;
            }
            if (c==endc)
            {
                if (word.mid(0,9)=="QLPROMPT:")
                {
                    bool ok;
                    QString title="qlaunch prompt", prompt="", deftext="";

                    int n;
                    prompt=word.mid(9);  // put $QLPROMPT:prompt:default:title (all but prompt optional)
                    n=prompt.indexOf(':');
                    if (n!=-1)
                    {
                        deftext=prompt.mid(n+1);
                        prompt=prompt.mid(0,n);
                        n=deftext.indexOf(':');
                        if (n!=-1)
                        {
                            title=deftext.mid(n+1);
                            deftext=deftext.mid(0,n);
                        }

                    }
                    QString itext=QInputDialog::getText(NULL,title,prompt,QLineEdit::Normal,deftext,&ok);
                    if (ok) rv+=itext;
                }
                else rv+=env.value(word,"");
                state=0;
                continue;
            }
            word+=c;
            continue;

        }
    }
    if (state==2||state==3) rv+=env.value(word,""); // pick up any left over word at the end
    return xtilde(rv);

}

QString xtilde(QString src)
{
    QString rv,c,word;
    int escape=0, state=0;
    QProcessEnvironment env=QProcessEnvironment::systemEnvironment();
    for (int i=0;i<src.length();i++)
    {
        c=src.mid(i,1);
        if (escape!=0)
        {
            if (escape==1) rv+=c; else word+=c;
            escape=0;
            continue;
        }
        switch (state)
        {
        case 1:
            if (c[0].isLetterOrNumber()||c[0]=='+'||c[0]=='-')
            {
                word+=c;
                continue;
            }
            if (word=="+") word="PWD";
            if (word=="-") word="OLDPWD";
            if (word=="") word="HOME";
            rv+=env.value(word,"");
            state=0;
            // fall through

        case 0:
            if (c=="\\")
            {
                escape=1;
                continue;
            }
            if (c=="~")
            {
                state=1;
                word="";
                continue;
            }
            rv+=c;
            continue;
        }

    }
    if (state==1)
    {
        if (word=="+") word="PWD";
        if (word=="-") word="OLDPWD";
        if (word=="") word="HOME";
        rv+=env.value(word,"");
    }
    return rv;
}

qlaunch v1.0

By Al Williams al.williams@awce.com

Purpose: Provide an easy way to make a launch panel that is easily configurable with text files.

Platform: Linux

License: GPL

Usage:
To use qlaunch, you need at least one configuration file. The format of the configuration file is similar to a common INI file.


```
; You may have a [Main] section to set the  basic window settingsn
; style is the stylesheet for the entire window
; buttonstyle is the stylesheet for all buttons
; title is the window title
; frameless is true if you don't want a window title
; ontop is true if you want the launch window to always be on top
; launchexit is true if you want the launch window to close after starting something

; Buttons start with this style sheet:
;     font: 'Times New Roman'; font-size: 24pt; width: 250px; height: 100px; icon-size: 55px;
; Although you can totally override this in ~/.config/qlaunch.css
; The main window has no style sheet unless you put it in the qlaunch.css file, or in style below

; Quotes around strings are always a good idea, especially stylesheets. You should not need to quote just numeric entries

; Commands can have the following special keywords
; $ENV - environment variable ENV (e.g., $HOME)
; $[ENV] - Same as $ENV
; ${ENV} - Same as $ENV
; $(ENV) - Same as $ENV
; ${QLPROMPT:Prompt:Default:Title} - Pops up a dialog box with the given prompt, default value, and title (you can omit the title, or both the title and the default, if you wish)
;                                    Note you must use {}, [], or () with this $QLPROMPT: will not work
; ~ - Home directory
; -+ - Current working directory (when qlaunch started; same as $PWD)
; ~- - Previous working directory (same as $OLDPWD)
; Expansion of ~ occurs after variables, so it is ok to have ~ in
; the variables
[Main]
style="background: cadetblue; border-radius: 10;"        ; style for main window
buttonstyle="background: cornsilk; border-radius: 50;  color: green;"   ; default button style (can override)
title="3...2...1..."                  ; window title
frameless=true                        ; true to hide window frame
ontop=true                            ; true to start out always on top
launchexit=false                      ; true to exit after launching something
maxi=900                              ; Don't scan file for more MenuX items (default=1000)
shrink=true                           ; shrink main window to fit contents (window will always get bigger if needed)
; starttag=Office                     ; Start at tag (default=Menu; override with -t)

; Initial position if you want to force it
;startx=2000
;starty=200

; In addition to the [Main] tag, you can also include a [Menu] tag that will override
; some things in [Main] if you proivde them. This is important because
; you can use other tags as submenus (e.g., [SubMenuA]) and this allows you to set
; global parameters that specific menus can override

; For each menu item you need a [MenuX] section where X is 1, 2, 3, 4... and can be up to 1000
; The order they are read will be their tab order (e.g., 1 is the first on tab order even if
; you put it last in the file

; cmd can be **EXIT to close the program
; cmd can be */path to load a new menu file at /path
; cmd can be *#tag to change to another tag instead of [Menu]
; cmd can be *!Message to pop up a dialog message
; If you need to quote any of the commands, do it for the entire string, for example:
; cmd="*!This menu item is not yet working"


; Note that there is no stack so you can build
; arbitrarily complex graphs of menus

; Note that default style sheets get added together along with the button's individual style sheet
[Menu1]
row=1        ; row
col=1        ; column
name=Emacs    ; Label on button
icon=/usr/local/share/icons/rocket.jpg  ; icon on button (optional)
cmd=emacs    ; command line (quote unless single word)
tip="Run the emacs editor"   ; Tool tip
[Menu2]
row=1
col=2
name=Konsole
icon=/usr/share/icons/Humanity/apps/128/konsole.svg
cmd=konsole
[Menu3]
row=1
col=5
name=Eyes
cmd=xeyes
[Menu4]
row=2
col=1
name=Office Submenu    ; Link to menu in this file
cmd=*#Office
[Menu5]                ; Link to a submenu in another file
row=1
col=4
name=SubMenu
cmd=*/home/alw/projects/qlaunch/menu2.ini
[Menu300]             ; Menus don't have to be in sequence or in order
row=2
col=3
name=envtest
cmd="*!Hey $HOME$HOME should be twice $HOME and the same as ~"
[Menu900]
row=1
col=3
name=Exit
cmd=**EXIT
style="color: rgb(255,0,0); background:black;"  ; Black/red exit button
tip="Press me to exit"
; Here's another menu built into this file
[Office]
title="HEY OFFICE GUY"
style="background: yellow;"     ; ugly styles for this menu only
buttonstyle="background: orange;"
frameless=false
shrink=true
[Office1]
name=Show home
row=1
col=2
cmd=*!$HOME
[Office2]     ; go back to main menu
name=Back!
row=1
col=1
cmd=*#Menu


```

If you don't name a file, the launcher looks for qlaunch.menu in the following places (in this order):
1. command line
2. ~/.config
3. /etc/xdg
4. ~/.local/share/qlaunch
5. /usr/local/share/qlaunch
6. /usr/share/qlaunch
7. ~

You can also override the default style sheet at several levels:
1. command line (-s options)
2. ~/.config/qlaunch.css
3. /etc/xdg/qlaunch.css

Note that whichever of these "wins" overrides the others.

In addition, the menu file can hold style entries in a main section or for a menu item. These add to the styles already present. There is also a buttonstyle that sets the default buttons style.
So a button takes style from:
1. The default CSS
2. qlaunch.css (if found)
3. The buttonstyle entry for the file (in the [Main] tag
4. The buttonstyle entry for the menu (in the [Menu] or other top level tag
5. The style entry for this item

You can see examples of style entries above. Here's an example qlaunch.css file:

```

QPushButton#MenuButton {
    font: 'Times New Roman'; font-size: 24pt;
    width: 250px; height: 100px; icon-size: 55px;
}
```

[![Code Triagers Badge](https://www.codetriage.com/pankajb64/choqok-facebook/badges/users.svg)](https://www.codetriage.com/pankajb64/choqok-facebook)

Choqok-Facebook
===============

**Choqok-Facebook** is a plugin for Choqok to access the social media site **[Facebook] (http://facebook.com)**.

**Choqok** is a **[KDE] (http://kde.org) Micro-Blogging client** - More Information at http://choqok.gnufolks.org,

###Author:
[Pankaj Bhambhani] (http://github.com/pankajb64) <<pankajb64@gmail.com>>

###License:
GNU GPL v2 or v3 or Later

###Requirements to build(both these must be built from source):


* **Choqok** ( https://projects.kde.org/projects/extragear/network/choqok/ )

* **KFb API** ( https://projects.kde.org/projects/extragear/libs/libkfbapi )

How To Build The Project
------------------------
```Shell
$ cd choqok-facebook-src-root-dir [It's choqok-facebook]
$ mkdir build
$ cd build
$ cmake -DCMAKE_INSTALL_PREFIX=`kde4-config --prefix` ..
$ make
$ sudo make install OR su -c 'make install'
```
to uninstall the project:
```Shell
$ make uninstall or su -c 'make uninstall'
```

Please check the [Wiki] (https://github.com/pankajb64/choqok-facebook/wiki) for screenshots and more help.

In case of any issues, feel free to email me.

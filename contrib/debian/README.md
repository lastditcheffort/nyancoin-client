
Debian
====================
This directory contains files used to package nyancoind/nyancoin-qt
for Debian-based Linux systems. If you compile nyancoind/nyancoin-qt yourself, there are some useful files here.

## nyancoin: URI support ##


nyancoin-qt.desktop  (Gnome / Open Desktop)
To install:

	sudo desktop-file-install nyancoin-qt.desktop
	sudo update-desktop-database

If you build yourself, you will either need to modify the paths in
the .desktop file or copy or symlink your nyancoin-qt binary to `/usr/bin`
and the `../../share/pixmaps/bitcoin128.png` to `/usr/share/pixmaps`

nyancoin-qt.protocol (KDE)


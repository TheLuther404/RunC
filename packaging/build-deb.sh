#!/bin/sh
# build-deb.sh - Crée un paquet .deb pour runc
# À exécuter sur une machine Linux (Ubuntu/Debian)
set -e

VERSION="1.0.0"
PKG="runc-cli"
ARCH="amd64"

echo "==> Construction du paquet ${PKG}_${VERSION}_${ARCH}.deb"

# 1. Compile le binaire
make clean
make

# 2. Crée la structure du .deb
#    Un .deb c'est juste des dossiers qui imitent le système de fichiers
#    + un dossier DEBIAN/ avec les métadonnées
BUILDDIR="${PKG}_${VERSION}_${ARCH}"
rm -rf "$BUILDDIR"

# Les fichiers qui seront installés sur le système
mkdir -p "$BUILDDIR/usr/local/bin"
cp runc "$BUILDDIR/usr/local/bin/runc"
chmod 755 "$BUILDDIR/usr/local/bin/runc"
ln -sf runc "$BUILDDIR/usr/local/bin/rc"

# Les métadonnées du paquet
mkdir -p "$BUILDDIR/DEBIAN"
cat > "$BUILDDIR/DEBIAN/control" << EOF
Package: ${PKG}
Version: ${VERSION}
Section: devel
Priority: optional
Architecture: ${ARCH}
Maintainer: TheLuther404 <noreply@github.com>
Description: Compile and run C files instantly
 runc compiles and runs .c files in a single command.
 No build system or Makefile needed.
Homepage: https://github.com/TheLuther404/runc
EOF

# 3. Construit le .deb
dpkg-deb --build "$BUILDDIR"

# 4. Nettoyage
rm -rf "$BUILDDIR"
make clean

echo "==> Créé : ${BUILDDIR}.deb"
echo "    Pour installer : sudo dpkg -i ${BUILDDIR}.deb"
echo "    Pour désinstaller : sudo apt remove ${PKG}"

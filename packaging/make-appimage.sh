#!/bin/sh

# Build the Linux release as an AppImage, in one script, run from the Arch
# container of the appimage job in .github/workflows/ci.yml (tag pushes and
# manual dispatch only -- the plain Linux leg of that workflow is the compile
# gate; this file is the release). Same shape as dethrace's
# packaging/make-appimage.sh: install the distro SDL3, build, stage a flat
# AppDir, then let quick-sharun bundle the libraries and appimagetool turn it
# into an image. The tooling (quick-sharun, appimagetool) comes from
# pkgforge-dev/anylinux-setup-action@v2 in the workflow, not from this repo.
#
# No game data is bundled, and none may be: the repository ships no original
# game code or assets (see README.md, Legal). The AppRun hook stages a
# per-user data directory instead and tells the player where to copy DATA/.

set -eu

ARCH=$(uname -m)
export ARCH
export OUTPATH=./dist
export APPNAME=carpocalypse2
export STARTUPWMCLASS=carpocalypse2
export DEPLOY_OPENGL=1
export DEPLOY_VULKAN=1
export ADD_HOOKS="self-updater.hook"
# gh-releases-zsync|owner|repo|channel|pattern. The channel field is a
# reserved keyword, not a tag name: "latest" resolves to the newest
# non-prerelease release, which is exactly what a version tag push creates via the
# release job. Guarded so the script can also be run outside Actions, where
# there is no repository to update from and UPINFO would only make the
# tooling emit a .zsync pointing at nothing.
if [ -n "${GITHUB_REPOSITORY:-}" ]; then
    export UPINFO="gh-releases-zsync|${GITHUB_REPOSITORY%/*}|${GITHUB_REPOSITORY#*/}|latest|*${ARCH}.AppImage.zsync"
fi

echo "Installing package dependencies..."
echo "---------------------------------------------------------------"
pacman -Syu --noconfirm \
    cmake     \
    libdecor  \
    patchelf  \
    sdl3

echo "Installing debloated packages..."
echo "---------------------------------------------------------------"
# The display/audio stack the bundled libSDL3 dlopens, in smaller variants.
# SDL3 itself comes from Arch's sdl3 package above, which pulls its build
# dependencies; find_package(SDL3) then resolves through /usr/lib/cmake/SDL3.
get-debloated-pkgs --add-common --prefer-nano

echo "Building carpocalypse2..."
echo "---------------------------------------------------------------"
# Version from the checked-out tree. A short commit hash keeps VERSION.txt
# simple; the workflow names the release after the tag it was pushed with,
# not after this.
VERSION="$(git rev-parse --short HEAD)"
export VERSION

mkdir -p ./dist
echo "${VERSION}" > ./dist/VERSION.txt

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    -DCARPOCALYPSE2_PLATFORM=SDL3
cmake --build build -j"$(nproc)"

mkdir -p ./AppDir/bin
# CMAKE_RUNTIME_OUTPUT_DIRECTORY puts every runtime at the build root, so the
# executable is exactly here -- no install step involved (dethrace's recipe;
# this tree has no install() rules to go through either way).
mv -v ./build/carpocalypse2 ./AppDir/bin/

# Desktop entry + icon. The icon must be the entry's Icon= value with no
# extension, next to the .desktop file; quick-sharun surfaces that as the
# AppImage icon.
cp -v ./packaging/carpocalypse2.desktop ./AppDir/carpocalypse2.desktop
cp -v ./packaging/icon_source.png ./AppDir/carpocalypse2.png

# First-run hook: creates the per-user data directory, explains what to copy
# into it, and cd's there so the game's cwd-relative DATA/ lookups land in
# writable storage rather than inside the read-only mount.
cp -v ./packaging/setup-data.hook ./AppDir/bin/

# GPL keeps the licence with the binary it covers.
mkdir -p ./AppDir/usr/share/licenses/carpocalypse2
cp -v ./LICENSE ./AppDir/usr/share/licenses/carpocalypse2/LICENSE

# The engine parses decimals out of text files with strtod (c2_strtof in
# c2_stdlib.h, BRender's br_strtod, atof in interface.c), which reads the
# decimal separator from the ambient locale -- a comma where a dot was
# expected silently corrupts the parsed value. The original ran under the C
# locale; pin it for the same reason dethrace does.
echo 'LC_ALL=C.UTF-8' >> ./AppDir/.env

# Deploy the shared libraries the binary needs (libSDL3 and its backends)
# into the AppDir, then turn it into an AppImage (+ .zsync for UPINFO above).
quick-sharun ./AppDir/bin/carpocalypse2
quick-sharun --make-appimage

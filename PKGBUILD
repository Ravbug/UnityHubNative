# Maintainer: madratzz
# Contributor: Ravbug

pkgname=unity-hub-native
pkgver=1.60.0
pkgrel=1
pkgdesc="A lightweight C++/wxWidgets alternative to the official Electron Unity Hub - with preset and env-var support"
arch=('x86_64')
url="https://github.com/madratzz/unity-hub-native"
license=('GPL3')
depends=(
  'gtk3'
  'libnotify'
  'libsdl2'
  'webkit2gtk-4.1'
  'libcurl-gnutls'
  'libsecret'
)
makedepends=(
  'cmake'
  'ninja'
  'gcc'
  'imagemagick'
)
source=("${pkgname}-${pkgver}.tar.gz::${url}/archive/refs/tags/v${pkgver}.tar.gz")
sha256sums=('SKIP')
provides=('unityhubnative')

build() {
  cd "${srcdir}/${pkgname}-${pkgver}"
  cmake \
    -B build \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Release
  cmake --build build -j "$(nproc)"
}

package() {
  cd "${srcdir}/${pkgname}-${pkgver}"

  # Binary
  install -Dm755 "build/Release/UnityHubNative" "${pkgdir}/usr/bin/unity-hub-native"

  # Desktop entry
  install -Dm644 "resources/unity-hub-native.desktop" \
    "${pkgdir}/usr/share/applications/unity-hub-native.desktop"

  # Icon (convert xpm to png)
  install -Dm644 "source/wxlin.xpm" \
    "${pkgdir}/usr/share/icons/hicolor/scalable/apps/unity-hub-native.xpm"
}

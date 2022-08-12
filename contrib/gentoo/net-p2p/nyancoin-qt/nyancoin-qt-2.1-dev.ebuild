# Copyright 2010-2019 Gentoo Authors
# Distributed under the terms of the GNU General Public License v2

EAPI=7

DESCRIPTION="An end-user Qt GUI for the Nyancoin crypto-currency"
LICENSE="DWTFYW"
HOMEPAGE="https://nyanchain.com/"
SLOT="0"
SRC_URI="https://codeload.github.com/nyancoin-official/nyancoin-client/tar.gz/refs/tags/v2.0 -> nyancoin-v2.0.tar.gz"

DB_VER="4.8"
inherit autotools bash-completion-r1 db-use gnome2-utils xdg-utils

SLOT="0"
KEYWORDS="~amd64 ~arm ~arm64 ~x86 ~amd64-linux ~x86-linux"

IUSE="+asm +berkdb dbus nat-pmp +qrcode sqlite systemtap test +wallet zeromq"
RESTRICT="!test? ( test )"

REQUIRED_USE="
	berkdb? ( wallet )
	wallet? ( sqlite ) 
"
RDEPEND="
	>=dev-libs/boost-1.63.0:=[threads(+)]
	>dev-libs/libsecp256k1-0.1_pre20200911:=[recovery]
	>=dev-libs/univalue-1.0.4:=
	dev-qt/qtcore:5
	dev-qt/qtgui:5
	dev-qt/qtnetwork:5
	dev-qt/qtwidgets:5
	virtual/bitcoin-leveldb
	dbus? ( dev-qt/qtdbus:5 )
	dev-libs/libevent:=
	nat-pmp? ( net-libs/libnatpmp )
	qrcode? (
		media-gfx/qrencode:=
	)
	berkdb? ( sys-libs/db:$(db_ver_to_slot "${DB_VER}")=[cxx] )
	zeromq? ( net-libs/zeromq:= )
"
BDEPEND="
	>=sys-devel/automake-1.13
	|| ( >=sys-devel/gcc-7[cxx] >=sys-devel/clang-5 )
	dev-qt/linguist-tools:5
	knots? (
		gnome-base/librsvg
		media-gfx/imagemagick[png]
	)
"

DOCS=( doc/files.md doc/release-notes.md doc/REST-interface.md doc/tor.md )

S="${WORKDIR}/Nyancoin"

pkg_pretend() {
		elog "You are building ${PN} from Nyancoin Core."
		elog "For more information, see:"
		elog "https://github.com/nyancoin-official/nyancoin-client/releases/tag/${PV}/"
}

src_prepare() {

	# Save the generic icon for later
	cp src/qt/res/src/bitcoin.svg bitcoin128.svg || die

	echo '#!/bin/true' >share/genbuild.sh || die
	mkdir -p src/obj || die
	echo "#define BUILD_SUFFIX gentoo${PVR#${PV}}" >src/obj/build.h || die

	eapply_user

	eautoreconf
	rm -r src/leveldb src/secp256k1 || die
}

src_configure() {
	local my_econf=(
		$(use_enable asm)
		$(use_with dbus qtdbus)
		$(use_enable systemtap ebpf)
		$(use_enable external-signer)
		$(use_with nat-pmp natpmp)
		$(use_with nat-pmp natpmp-default)
		$(use_with qrcode qrencode)
		$(use_enable test tests)
		$(use_enable wallet)
		$(use_enable zeromq zmq)
		--with-gui=qt5
		--disable-util-cli
		--disable-util-tx
		--disable-util-util
		--disable-util-wallet
		--disable-bench
		--without-libs
		--without-daemon
		--disable-fuzz
		--disable-fuzz-binary
		--disable-ccache
		--disable-static
		$(use_with berkdb bdb)
		--with-system-leveldb
		--with-system-libsecp256k1
		--with-system-univalue
	)
	econf "${my_econf[@]}"
}

src_install() {
	default

	insinto /usr/share/icons/hicolor/scalable/apps/
	doins bitcoin128.svg
	if use evo; then
		newins src/qt/res/src/bitcoin.svg nyancoin.svg
	fi
}

update_caches() {
	gnome2_icon_cache_update
	xdg_desktop_database_update
}

pkg_postinst() {
	update_caches

	elog "To have ${PN} automatically use Tor when it's running, be sure your"
	elog "'torrc' config file has 'ControlPort' and 'CookieAuthentication' setup"
	elog "correctly, and add your user to the 'tor' user group."
}

pkg_postrm() {
	update_caches
}

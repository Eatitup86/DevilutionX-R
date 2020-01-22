#!/usr/bin/env bash

set -euo pipefail

usage() {
	echo "Usage: build.sh [target]"
	echo "	target: target architecture. Either rg350 or retrofw"
}

if [[ $# -ne 1 ]]; then
	echo "Error: target is missing"
	usage
	exit 1
fi

if [[ "$1" != "rg350" ]] && [[ "$1" != "retrofw" ]]; then
	echo "Error: invalid target"
	usage
	exit 1
fi

cd "$(dirname "${BASH_SOURCE[0]}")/../.."

declare -r TARGET="${1}"
declare -r BUILD_DIR="build-${TARGET}"

declare -rA BUILDROOT_REPOS=(
	[retrofw]=https://github.com/retrofw/buildroot.git
	[rg350]=https://github.com/tonyjih/RG350_buildroot.git
)
BUILDROOT="${BUILDROOT:-$HOME/buildroot-${TARGET}-devilutionx}"

main() {
	>&2 echo "Building for target ${TARGET} in ${BUILD_DIR}"
	set -x
	prepare_buildroot
	make_buildroot
	build
	package
}

prepare_buildroot() {
	if [[ -d $BUILDROOT ]]; then
		return
	fi
	git clone --depth=1 "${BUILDROOT_REPOS[$TARGET]}" "$BUILDROOT"
}

make_buildroot() {
	cp Packaging/OpenDingux/buildroot_${TARGET}_defconfig \
		"$BUILDROOT/configs/${TARGET}_devilutionx_defconfig"
	cd "$BUILDROOT"
	make "${TARGET}_devilutionx_defconfig"
	BR2_JLEVEL=0 make toolchain libzip sdl sdl_mixer sdl_ttf
	cd -
}

build() {
	mkdir -p "$BUILD_DIR"
	cd "$BUILD_DIR"
	rm -f CMakeCache.txt
	cmake .. -DBINARY_RELEASE=ON "-D${TARGET^^}=ON" \
		-DCMAKE_TOOLCHAIN_FILE="$BUILDROOT/output/host/usr/share/buildroot/toolchainfile.cmake"
	make -j $(getconf _NPROCESSORS_ONLN)
	cd -
}

package() {
	if [[ "$TARGET" == "retrofw" ]]; then
		Packaging/OpenDingux/package-ipk.sh "${PWD}/${BUILD_DIR}/devilutionx-${TARGET}.ipk"
	else
		Packaging/OpenDingux/package-opk.sh "${PWD}/${BUILD_DIR}/devilutionx-${TARGET}.opk"
	fi
}

main

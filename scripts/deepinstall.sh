#!/bin/env bash
# VaLib - Vast Library
# Licensed under GNU GPL v2 License. See LICENSE file.
# (C) 2025 VaLibTeam

cd "$(dirname "$0")" || exit 1
source "scripts/utils.sh" || exit 1
set -e || exit 1

InvalidDistroExit=1
SuccessExit=0

DetectDistro() {
	if [ -f /etc/os-release ]; then
		. /etc/os-release
		echo $ID
	else
		ShowError $InvalidDistroExit "Cannot detect distribution"
	fi
}

InstallDependencies() {
	local distro=$1
	local missingDeps=()

	for dep in "${dependencies[@]}"; do
		if ! dpkg -s $dep &>/dev/null && ! rpm -q $dep &>/dev/null && ! pacman -Q $dep &>/dev/null; then
			missingDeps+=($dep)
		fi
	done

	if [ ${#missingDeps[@]} -ne 0 ]; then
		ShowInfo "Missing dependencies: ${missingDeps[*]}"
        read -p "Do you want to install them? [Y/n] " -n 1 -r answer
        if [[ ! ("$answer" =~ ^[Yy]$ || -z "$answer") ]]; then
            exit $SuccessExit
        fi
		case $distro in
		"debian" | "zorinos" | "ubuntu")
			sudo apt-get update
			sudo apt-get install -y "${missingDeps[@]}"
			;;
		"fedora" | "nobara")
			sudo dnf install -y "${missingDeps[@]}"
			;;
		"centos" | "rhel")
			sudo yum install -y "${missingDeps[@]}"
			;;
		"arch" | "manjaro")
			sudo pacman -Syu --noconfirm "${missingDeps[@]}"
			;;
		"opensuse")
			sudo zypper install -y "${missingDeps[@]}"
			;;
		*)
			ShowError $InvalidDistroExit "Unknown distribution: $distro"
			;;
		esac
	else
		exit $SuccessExit
	fi
}

declare -A dependenciesMap
dependenciesMap["debian"]="qt5-default qtbase5-dev qtdeclarative5-dev libkf5config-dev libqt5x11extras5-dev"
dependenciesMap["zorinos"]="qt5-default qtbase5-dev qtdeclarative5-dev libkf5config-dev libqt5x11extras5-dev"
dependenciesMap["ubuntu"]="qt5-default qtbase5-dev qtdeclarative5-dev libkf5config-dev libqt5x11extras5-dev"
dependenciesMap["fedora"]="qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative kf5-kconfig-devel qt5-qtx11extras-devel qt5-qtbase-devel qt5-qtsvg-devel kf5-kwidgetsaddons-devel kf5-kcoreaddons-devel"
dependenciesMap["nobara"]="qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative kf5-kconfig-devel qt5-qtx11extras-devel qt5-qtbase-devel qt5-qtsvg-devel kf5-kwidgetsaddons-devel kf5-kcoreaddons-devel"
dependenciesMap["centos"]="qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative kf5-kconfig-devel qt5-qtx11extras-devel qt5-qtbase-devel qt5-qtsvg-devel kf5-kwidgetsaddons-devel kf5-kcoreaddons-devel"
dependenciesMap["rhel"]="qt5-qtbase qt5-qtbase-devel qt5-qtdeclarative kf5-kconfig-devel qt5-qtx11extras-devel qt5-qtbase-devel qt5-qtsvg-devel kf5-kwidgetsaddons-devel kf5-kcoreaddons-devel"
dependenciesMap["arch"]="qt5-base qt5-declarative kconfig kconfig-devel qt5-x11extras"
dependenciesMap["manjaro"]="qt5-base qt5-declarative kconfig kconfig-devel qt5-x11extras"
dependenciesMap["opensuse"]="libqt5-qtbase libqt5-qtbase-devel libqt5-qtdeclarative kf5-kconfig-devel libqt5-qtx11extras-devel"

dependencies="${dependenciesMap[$(DetectDistro)]}"
[ -z "$dependencies" ] && ShowError $InvalidDistroExit "Unknown distribution"

# ShowInfo "Detected distribution: $(DetectDistro)"
InstallDependencies "$(DetectDistro)"
exit "$SuccessExit"

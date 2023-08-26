#!/bin/bash

python ./scripts/generate_includes.py
python ./scripts/create_combos.py

# west_flags='-p'
# config_folder="/home/huy/repositories/kyria"
config_folder=$(pwd)
zmk_path="/home/huy/repositories/zmk/app"

# use flag below with west to not have to cd do the folder
# -s /path/to/zmk/app

SILENT=0
BUILD=0
while getopts :sb option; do
	case $option in
		s) SILENT=1;;
		b) BUILD=1;;
		*) echo "Error: Option '$option' does not exist." ;;
	esac
done
shift $((OPTIND -1))

# echoerr() { printf "%s\n" "$*" >&2; }
echoerr() { echo "$@" 1>&2; }

notify() {
	if [[ $SILENT -eq 0 ]]; then
		if [ -n "$notification_id" ]; then
			replace="--replace-id=$notification_id"
		fi

		notification_id=$(notify-send --app-name='ZMK Build' -p $replace -t $1 "$2" "$3")
	fi
}

notify_short() {
	echo "$2"
	notify 2000 "$1" "$2"
}

notify_long() {
	echoerr "$2"
	notify 20000 "$1" "$2"
}

wait_copy() {
	path_to_file=$1
	path_to_target=/run/media/huy/NICENANO$2
	notify_long 'Waiting' 'Waiting for board...'
	until [ -e $path_to_target ]; do
		sleep 1
	done
	notify_long 'Found' 'Board found. Copying...'

	cp $path_to_file $path_to_target/zmk.uf2

	notify_short 'Done' 'Keyboard should boot any moment.'
}

build() {
	cd $zmk_path

	board_info="nice_nano_v2 -- -DSHIELD=kyria_rev3_$1 -DZMK_CONFIG=$config_folder/config"
	west build $west_flags -d build/$1 -b $board_info

	if [ $? -eq 1 ]; then
		notify_long 'Build failed.'
		exit 1
	fi

	notify_short 'Completed' 'Build has completed.'

	if [[ $BUILD -eq 0 ]]; then
		cp ./build/$1/zephyr/zmk.uf2  "$config_folder/backups/last_builds/$1_$(date +'%FT%X').uf2"
		wait_copy ./build/$1/zephyr/zmk.uf2 $2
	fi
}

left() {
	build left $@
}

right() {
	build right $@
}

restore() {
	path_to_file="./backups/restore/right_restore"
	if [ -n "$1" ]; then
		path_to_file="./backups/restore/$1.uf2"
	fi
	wait_copy $path_to_file
}

if [ $# -eq 0 ]; then
	left &
	right 1 &
	exit 0
elif declare -f "$1" > /dev/null; then
	"$@"
	exit 0
else
	echo "'$1' is not a known function name" >&2
	exit 1
fi

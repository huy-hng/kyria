#!/bin/bash

python ./generate_includes.py
config_folder="/home/huy/repositories/kyria"

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

side() {
	cd /home/huy/repositories/zmk/app

	board_info="nice_nano_v2 -- -DSHIELD=kyria_rev3_$1 -DZMK_CONFIG=$config_folder/config"
	west build -d build/$1 -b $board_info

	if [ $? -eq 1 ]; then
		notify_long 'Build failed.'
		exit 1
	fi

	notify_short 'Completed' 'Build has completed.'

	if [[ $BUILD -eq 0 ]]; then
		cp ./build/$1/zephyr/zmk.uf2  "$config_folder/backups/$1_$(date +%s).uf2"
		wait_copy ./build/$1/zephyr/zmk.uf2 $2
	fi
}

left() {
	side left $@
}

right() {
	side right $@
}

restore() {
	path_to_file="./backups/right_restore.uf2"
	if [ -n "$1" ]; then
		path_to_file="./backups/$1.uf2"
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

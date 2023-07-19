python ./generate_includes.py

cd /home/huy/repositories/zmk/app
config_folder="/home/huy/repositories/kyria"

side() {
	board_info="nice_nano_v2 -- -DSHIELD=kyria_rev3_$1 -DZMK_CONFIG=$config_folder/config"
	west build -d build/$1 -b $board_info
	if [ $? -eq 1 ]; then
		notify-send 'Build failed.'
		exit 1
	fi

	cp ./build/$1/zephyr/zmk.uf2  "$config_folder/backups/$1_$(date +%s).uf2"

	notify-send 'Build Completed.'
	echo 'Build Completed.'
	echo 'Waiting for board...'

	until [ -e /run/media/huy/NICENANO$2 ]; do
		sleep 1
	done

	echo 'Board found. Copying...'

	cp ./build/$1/zephyr/zmk.uf2  /run/media/huy/NICENANO$2/zmk.uf2
	# cp ./build/$1/zephyr/zmk.uf2  /mnt/chromeos/removable/NICENANO$1/zmk.uf2
	echo 'Done.'
}

left() {
	side left $@
}

right() {
	side right $@
}

if [ $# -eq 0 ]; then
	left &
	right 1 &
	exit 0
elif declare -f "$1" > /dev/null; then
	"$@"
	exit 0
else
	# Show a helpful error
	echo "'$1' is not a known function name" >&2
	exit 1
fi

cd /home/huy/repositories/zmk/app


left() {
	west build -p -d build/left -b nice_nano_v2 -- -DSHIELD=kyria_rev3_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
	# west build -p -d build/left -b nice_nano_v2 -- -DSHIELD=kyria_rev3_left
	# west build -d build/left
	cp ./build/left/zephyr/zmk.uf2  /run/media/huy/NICENANO$1/zmk.uf2
}

right() {
	west build -p -d build/right -b nice_nano_v2 -- -DSHIELD=kyria_rev3_right -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
	# west build -p -d build/right -b nice_nano_v2 -- -DSHIELD=kyria_rev3_right
	# west build -d build/right
	cp ./build/right/zephyr/zmk.uf2  /run/media/huy/NICENANO$1/zmk.uf2
}


if [ $# -eq 0 ]; then
	left &
	right 1 &
	exit 0
elif declare -f "$1" > /dev/null; then
	# call arguments verbatim
	"$@"
	exit 0
else
	# Show a helpful error
	echo "'$1' is not a known function name" >&2
	exit 1
fi


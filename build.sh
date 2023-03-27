cd /home/huy/repositories/zmk/app

west build -d build/left -b nice_nano -- -DSHIELD=kyria_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
west build -d build/right -b nice_nano -- -DSHIELD=kyria_right -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
# west build -b nice_nano -- -DSHIELD=kyria_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
# west build -b nice_nano -- -DSHIELD=kyria_right -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
# west build -s /home/huy/repositories/zmk/app -b nice_nano -- -DSHIELD=kyria_left -DZMK_CONFIG="/home/huy/repositories/zmk-config/config"
# west build -s /home/huy/repositories/zmk/app -b nice_nano -- -DSHIELD=kyria_right -DZMK_CONFIG=""
# west build -d build/left -b nice_nano -- -DSHIELD=kyria_left
# west build -d build/right -b nice_nano -- -DSHIELD=kyria_right

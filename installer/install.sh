#!/bin/sh

err_report() {
  echo "errexit on line $(caller)" >&2
}

trap err_report ERR

PREFIX=/usr/local
EXTRAS_PATH=interact_box_extras/src

install_item() {
  local item_name=$1
  local filepath=$2
  local destination_path=$3
  echo "Installing $item_name"
  install -Dm755 $filepath $destination_path
}

mkdir -p /etc/interact_box

install_item "Interact Box" "interact_box" "$PREFIX/bin/interact_box"
install_item "Interact Box Settings" "$EXTRAS_PATH/interact_box_settings/interact_box_settings" "$PREFIX/bin/interact_box_settings"
install_item "Malware Date Settings" "$EXTRAS_PATH/malware_date_settings/malware_date_settings" "$PREFIX/bin/malware_date_settings"
install_item "Message Box" "$EXTRAS_PATH/message_box_process/message_box_process" "$PREFIX/bin/message_box_process"
install_item "Trivia Game" "$EXTRAS_PATH/trivia_game/trivia_game" "$PREFIX/bin/trivia_game"

install -Dm644 ./interact_box.desktop \
  "$PREFIX/share/applications/interact_box.desktop"
install -Dm644 ./interact_box_settings.desktop \
  "$PREFIX/share/applications/interact_box_settings.desktop"
install -Dm644 ./malware_date_settings.desktop \
  "$PREFIX/share/applications/malware_date_settings.desktop"

install -Dm644 icon/config_icon-linux.ico \
  "$PREFIX/share/icons/hicolor/256x256/apps/config_icon-linux.ico"
install -Dm644 icon/interact_box.png \
  "$PREFIX/share/icons/hicolor/256x256/apps/interact_box.png"
install -Dm644 icon/malware_date_settings.ico \
  "$PREFIX/share/icons/hicolor/256x256/apps/malware_date_settings.ico"

install -Dm644 ./installer/interact_box_config.json /etc/interact_box/interact_box_config.json
install -Dm644 ./installer/malware_with_date_specific_deployment.json /etc/interact_box/malware_with_date_specific_deployment.json

echo "Done."

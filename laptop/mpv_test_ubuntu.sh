#!/usr/bin/env bash
# Quick CLI mpv matching jip_show.py (Ubuntu / typical Linux). Same flags as start_mpv().
#
# Usage:
#   ./mpv_test_ubuntu.sh                 # fs-screen 1, socket /tmp/jip-mpv-test.sock
#   ./mpv_test_ubuntu.sh 0               # try other indices if fullscreen hits wrong monitor
#   JIP_MPV_SOCK=/tmp/foo.sock ./mpv_test_ubuntu.sh 1
#   DEBUG=1 ./mpv_test_ubuntu.sh        # verbose mpv (drops --really-quiet)
#
# IPC from another terminal (pick one):
#
#   # socat (recommended): sudo apt install socat
#   printf '%s\n' '{"command":["loadfile","/path/to/clip.mp4","replace"]}' | socat - UNIX-CONNECT:/tmp/jip-mpv-test.sock
#   printf '%s\n' '{"command":["stop"]}' | socat - UNIX-CONNECT:/tmp/jip-mpv-test.sock
#
#   # OpenBSD nc with -U: sudo apt install netcat-openbsd
#   printf '%s\n' '{"command":["stop"]}' | nc -U /tmp/jip-mpv-test.sock
#
# Monitor index: xrandr --listmonitors / try 0,1,2 if fullscreen is wrong.
#
set -euo pipefail

FS_SCREEN="${1:-1}"
SOCK="${JIP_MPV_SOCK:-/tmp/jip-mpv-test.sock}"

MPV_ARGS=(
  --idle=yes
  --script-opts-append=osc-idlescreen=no
  --force-window=immediate
  --loop-file=inf
  --fullscreen
  --native-fs=yes
  --fs-screen="${FS_SCREEN}"
  "--input-ipc-server=${SOCK}"
  --no-config
  --no-terminal
)

if [[ "${DEBUG:-}" == "1" ]]; then
  MPV_ARGS+=(--msg-level=cplayer=v,ipc=v)
else
  MPV_ARGS+=(--really-quiet)
fi

rm -f "${SOCK}"

echo "IPC socket: ${SOCK}  (fs-screen=${FS_SCREEN})" >&2
echo "Send JSON (e.g.): socat - UNIX-CONNECT:${SOCK}  # see script header" >&2

exec mpv "${MPV_ARGS[@]}"

#!/usr/bin/env bash
# Quick CLI mpv matching jip_show.py (macOS). Same flags as start_mpv() in jip_show.py.
#
# Usage:
#   ./mpv_test_macos.sh                  # fs-screen 1, socket /tmp/jip-mpv-test.sock
#   ./mpv_test_macos.sh 0                # built-in display as fullscreen target
#   JIP_MPV_SOCK=/tmp/foo.sock ./mpv_test_macos.sh 1
#   DEBUG=1 ./mpv_test_macos.sh          # verbose mpv (drops --really-quiet)
#
# In another terminal after mpv is running:
#   printf '%s\n' '{"command":["loadfile","'"$HOME"'/Movies/clip.mp4","replace"]}' | nc -U /tmp/jip-mpv-test.sock
#   printf '%s\n' '{"command":["stop"]}' | nc -U /tmp/jip-mpv-test.sock
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
echo "Send JSON lines with: nc -U ${SOCK}" >&2

exec mpv "${MPV_ARGS[@]}"

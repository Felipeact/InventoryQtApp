#!/usr/bin/env bash
# Ensures the toolchain needed to build and run the unit tests is present.
# Used by the SessionStart hook (Claude Code on the web) and usable manually.
#
# Idempotent: skips the (slow) package install when Qt6 is already available.
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"

have_qt6() {
    # Qt6 CMake package marker on Debian/Ubuntu.
    [ -d /usr/lib/x86_64-linux-gnu/cmake/Qt6 ] || command -v qmake6 >/dev/null 2>&1
}

if have_qt6; then
    echo "[setup-tests] Qt6 already installed; skipping apt install."
else
    echo "[setup-tests] Installing Qt6 + nlohmann/json + build tools..."
    sudo apt-get update -y || true
    sudo apt-get install -y --no-install-recommends \
        qt6-base-dev nlohmann-json3-dev cmake ninja-build g++
fi

echo "[setup-tests] Configuring CMake build..."
cmake -S "$repo_root" -B "$repo_root/build" -G Ninja >/dev/null

echo "[setup-tests] Ready. Build with: cmake --build build && ctest --test-dir build"

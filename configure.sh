#!/usr/bin/env bash
set -euo pipefail

BUILD_DIR="build"
mkdir -p "${BUILD_DIR}"

# 透传其它自定义参数，例如 -G "Ninja" 或 -DCMAKE_BUILD_TYPE=Debug
cmake -S . -B "${BUILD_DIR}" "$@"

echo "Done."
#!/usr/bin/env bash
set -euo pipefail

# 用法: ./build_run.sh <target>
if [ $# -lt 1 ]; then
  echo "Usage: ./build_run.sh <target>"
  echo "Example: ./build_run.sh DX11Texture"
  exit 1
fi

TARGET="$1"
CONFIG="Debug"
BUILD_DIR="build"

# 生成构建系统（如未生成）
if [ ! -f "${BUILD_DIR}/CMakeCache.txt" ]; then
  cmake -S . -B "${BUILD_DIR}"
fi

# 构建指定 target
cmake --build "${BUILD_DIR}" --config "${CONFIG}" --target "${TARGET}"

# 运行可执行（按 Examples 目录结构）
EXE_PATH="${BUILD_DIR}/${CONFIG}/Examples/${TARGET}"
if [ -f "${EXE_PATH}" ]; then
  echo "Running: ${EXE_PATH}"
  "${EXE_PATH}"
elif [ -f "${EXE_PATH}.exe" ]; then
  echo "Running: ${EXE_PATH}.exe"
  "${EXE_PATH}.exe"
else
  echo "Executable not found: ${EXE_PATH}(.exe)"
  echo "If your output path differs, adjust EXE_PATH in this script."
  exit 1
fi
#!/usr/bin/env bash
set -euo pipefail

cd "$(dirname "$0")"

CC="${CC:-gcc}"
CFLAGS="${CFLAGS:--Wall -Wextra -O2 -Iinclude}"

SRCS=(
  src/wish.c
  src/error.c
  src/args.c
  src/path.c
  src/execute.c
)

echo "Building wish..."
"$CC" $CFLAGS -o wish "${SRCS[@]}"
echo "Built ./wish"

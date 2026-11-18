#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT

c++ -std=c++20 -Wall -Wextra -Werror -I"$repo_root" \
  "$repo_root/secant/browser/tab_state.cc" \
  "$repo_root/secant/browser/tab_state_unittest.cc" \
  -o "$out/tab_state_unittest"

"$out/tab_state_unittest"

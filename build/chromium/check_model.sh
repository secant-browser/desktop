#!/usr/bin/env bash
set -euo pipefail

repo_root="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
out="$(mktemp -d)"
trap 'rm -rf "$out"' EXIT

c++ -std=c++20 -Wall -Wextra -Werror -I"$repo_root" \
  "$repo_root/secant/browser/tab_model.cc" \
  "$repo_root/secant/browser/tab_model_unittest.cc" \
  -o "$out/tab_model_unittest"

"$out/tab_model_unittest"

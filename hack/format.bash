#!/bin/bash

set -e -u -o pipefail

cmakefmt() {
	cmake-format --in-place CMakeLists.txt
}

shfmt() {
	command shfmt -w -d hack/format.bash
	command shfmt -w -d hack/test.bash
}

clang_format() {
	clang-format-8 -i pg_find_signal_handlers.cc
}

_main() {
	shfmt
	cmakefmt
	clang_format
}

_main "$@"

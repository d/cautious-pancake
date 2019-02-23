#!/bin/bash

set -e -u -o pipefail

shellcheck() {
	command shellcheck -s bash hack/format.bash
	command shellcheck -s bash hack/test.bash
}

build() {
	env LDFLAGS=-fuse-ld=lld-8 cmake -GNinja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -H. -Bbuild
	cmake --build build
}

clang_tidy() {
	clang-tidy-8 -p build pg_find_signal_handlers.cc
}

_main() {
	shellcheck
	build
	clang_tidy
}

_main "$@"

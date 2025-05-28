#!/bin/bash

pushd ..
Vendor/Binaries/Premake/Linux/premake-core/bin/release/premake5 --cc=clang --file=Build.lua gmake2
popd

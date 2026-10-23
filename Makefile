# Build SLUS_007.57 from src/ with the original PSY-Q toolchain and check its SHA-1.
#
#   make          compile, link, verify (tools/build.py)
#   make check    compare every object's code and data with baserom/SLUS_007.57
#   make clean
#
# Needs wine and the PSY-Q SDKs: see README.md.

.PHONY: all check clean
all:
	python3 tools/build.py

check: all
	python3 tools/objcmp.py baserom/SLUS_007.57 build/obj/*.obj
	-python3 tools/tudata.py baserom/SLUS_007.57 build/obj/*.obj   # a diagnostic: a few known false DIFFs
	python3 tools/exediff.py

clean:
	rm -rf build

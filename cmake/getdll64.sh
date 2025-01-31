#!/bin/bash
DLLS=`wine ../cmake/dumpbin.exe /dependents $1 | grep '.*.dll'  | sed '/msvcrt.dll/d' | sed '/KERNEL32.dll/d' | sed 's/ //g' | sed 's/\r$//'`

for fn in $DLLS ; do
	cp /usr/x86_64-w64-mingw32/bin/${fn} .
done
cp /usr/x86_64-w64-mingw32/bin/libwinpthread-1.dll .
cp /usr/x86_64-w64-mingw32/bin/libssp-0.dll .
cp /usr/x86_64-w64-mingw32/bin/SDL3.dll .
cp "/usr/x86_64-w64-mingw32/bin/libstdc++-6.dll" .
cp /usr/x86_64-w64-mingw32/bin/libgcc_s_seh-1.dll .

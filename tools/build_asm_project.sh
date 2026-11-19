#!/bin/bash
# Command-line rebuild of a LoongIDE bare-metal assembly project (same flags as the IDE-generated makefiles).
# Usage: build_asm_project.sh <project_dir> <out_dir> <name>
# Used only for independent verification; the IDE build remains the reference build.
set -e
PROJ="$1"; OUT="$2"; NAME="$3"
TC=/d/LoongIDE/mips-2015.05/bin/mips-sde-elf
CFLAGS="-mips32 -G0 -EL -msoft-float -DLS1B -O0 -g -Wall -c -fmessage-length=0 -pipe"
LDFLAGS="-mips32 -G0 -EL -msoft-float -nostartfiles -nodefaultlibs -nostdlib -static"
rm -rf "$OUT"; mkdir -p "$OUT"
cd "$PROJ"
objs=""
for f in bsp_start.S core/*.S; do
  [ -f "$f" ] || continue
  o="$OUT/$(echo "$f" | tr '/.' '__').o"
  $TC-gcc.exe $CFLAGS -I"./" -I"./include" -I"./core/include" -I"./core/mips" "$f" -o "$o" 2>>"$OUT/compile.log"
  case "$f" in core/start.S) starto="$o";; *) objs="$objs $o";; esac
done
$TC-gcc.exe $LDFLAGS -T ld.script -Wl,-Map,"$OUT/$NAME.map" -o "$OUT/$NAME.exe" -e start $starto $objs
$TC-objcopy.exe -O binary "$OUT/$NAME.exe" "$OUT/$NAME.bin"
$TC-size.exe "$OUT/$NAME.exe" | tee "$OUT/size.txt"
$TC-objdump.exe -d "$OUT/$NAME.exe" > "$OUT/$NAME.disasm.txt"
$TC-nm.exe -n "$OUT/$NAME.exe" > "$OUT/$NAME.symbols.txt"
sha256sum "$OUT/$NAME.exe" "$OUT/$NAME.bin" | tee "$OUT/sha256.txt"
echo "warnings in compile.log: $(grep -c warning "$OUT/compile.log" || true)"

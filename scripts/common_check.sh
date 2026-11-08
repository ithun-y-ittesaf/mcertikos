# Shared checks, sourced by every host's setup.sh after the tools are installed.
# Expects CC_CHECK (the compiler to test, e.g. gcc or i686-elf-gcc).

ok()   { printf '  [ ok ] %s\n' "$1"; }
bad()  { printf '  [FAIL] %s\n' "$1"; FAILED=1; }
note() { printf '  [note] %s\n' "$1"; }

check_tools() {
	FAILED=0
	echo
	echo "Checking the environment..."

	if command -v "$CC_CHECK" >/dev/null 2>&1; then
		ok "$CC_CHECK: $("$CC_CHECK" --version | head -n 1)"
	else
		bad "$CC_CHECK not found"
	fi

	# Link a 32-bit 64-bit division against libgcc, as the kernel does.
	LIBGCC=$("$CC_CHECK" -m32 -print-libgcc-file-name 2>/dev/null || true)
	TMP=$(mktemp -d)
	printf 'unsigned long long d(unsigned long long a, unsigned long long b) { return a / b; }\nvoid _start(void) {}\n' > "$TMP/t.c"
	if [ -f "$LIBGCC" ] && "$CC_CHECK" -m32 -ffreestanding -nostdlib -static -o "$TMP/t" "$TMP/t.c" "$LIBGCC" >/dev/null 2>&1; then
		ok "32-bit libgcc: $LIBGCC"
	else
		bad "32-bit libgcc missing (linking would fail with 'undefined reference to __udivdi3')"
	fi
	rm -rf "$TMP"

	for t in python3 perl qemu-system-x86_64; do
		if command -v "$t" >/dev/null 2>&1; then ok "$t"; else bad "$t not found"; fi
	done

	MAKE_VER=$(${MAKE_CHECK:-make} --version 2>/dev/null | head -n 1)
	case "$MAKE_VER" in
		"GNU Make 3."*) bad "$MAKE_VER is too old; GNU Make 4 or newer is needed" ;;
		"GNU Make"*)    ok "$MAKE_VER" ;;
		*)              bad "GNU Make not found" ;;
	esac

	if [ -e /dev/kvm ]; then
		if [ -r /dev/kvm ] && [ -w /dev/kvm ]; then
			ok "KVM available: QEMU runs at full speed"
		else
			note "/dev/kvm exists but you cannot use it yet: log out and log in again (kvm group)"
		fi
	else
		note "no KVM: QEMU emulates the CPU. It works, but booting and tests are slower"
	fi

	echo
	if [ "$FAILED" = 0 ]; then
		echo "Environment ready. Next steps:"
		echo "    cd mcertikos"
		echo "    ${MAKE_CHECK:-make} LAB=1 TEST=1      # exit QEMU with Ctrl-a x"
	else
		echo "Some checks failed. See SETUP.md and Appendix A (Troubleshooting) of the lab handout."
		exit 1
	fi
}

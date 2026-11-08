# -*-Makefile-*-

OBJDIRS		+= $(USER_OBJDIR)/fork_test

USER_FORK_TEST_SRC	+= $(USER_DIR)/fork_test/fork_test.c
USER_FORK_TEST_OBJ	:= $(patsubst %.c, $(OBJDIR)/%.o, $(USER_FORK_TEST_SRC))
USER_FORK_TEST_OBJ	:= $(patsubst %.S, $(OBJDIR)/%.o, $(USER_FORK_TEST_OBJ))
KERN_BINFILES	+= $(USER_OBJDIR)/fork_test/fork_test

fork_test: $(USER_OBJDIR)/fork_test/fork_test

$(USER_OBJDIR)/fork_test/fork_test: $(USER_LIB_OBJ) $(USER_FORK_TEST_OBJ)
	@echo + ld[USER/fork_test] $@
	$(V)$(LD) -o $@ $(USER_LDFLAGS) $(USER_LIB_OBJ) $(USER_FORK_TEST_OBJ) $(GCC_LIBS)
	$(V)$(OBJDUMP) -S $@ > $@.asm
	$(V)$(NM) -n $@ > $@.sym

$(USER_OBJDIR)/fork_test/%.o: $(USER_DIR)/fork_test/%.c
	@echo + cc[USER/fork_test] $<
	@mkdir -p $(@D)
	$(V)$(CC) $(USER_CFLAGS) -c -o $@ $<

$(USER_OBJDIR)/fork_test/%.o: $(USER_DIR)/fork_test/%.S
	@echo + as[USER/fork_test] $<
	@mkdir -p $(@D)
	$(V)$(CC) $(USER_CFLAGS) -c -o $@ $<

# formatting

COM_COLOR   := \033[0;34m
OBJ_COLOR   := \033[0;36m
OK_COLOR    := \033[0;32m
ERROR_COLOR := \033[0;31m
WARN_COLOR  := \033[0;33m
NO_COLOR    := \033[m
TST_COLOR   := \033[0;33m

OK_STRING    := "[OK]"
ERROR_STRING := "[ERROR]"
WARN_STRING  := "[WARNING]"
PASSED_STRING:= "[PASSED]"
FAILED_STRING:= "[FAILED]"
COM_STRING   := "Compiling"
LNK_STRING   := "Linking"
TST_STRING   := "Testing"

define compile_and_test
printf "%b" "$(COM_COLOR)$(2) $(OBJ_COLOR)$(@F)$(NO_COLOR)\r"; \
$(1) 2> $@.log; RESULT=$$?; \
if [ $$RESULT -ne 0 ]; then printf "%-76b%b" "$(COM_COLOR)$(2)$(OBJ_COLOR) $@"    "$(ERROR_COLOR)$(ERROR_STRING)$(NO_COLOR)\n"   ; \
elif [ -s $@.log ];    then printf "%-76b%b" "$(COM_COLOR)$(2)$(OBJ_COLOR) $@"    "$(WARN_COLOR)$(WARN_STRING)$(NO_COLOR)\n"   ; \
else                        printf "%-76b%b" "$(COM_COLOR)$(2)$(OBJ_COLOR) $(@F)" "$(OK_COLOR)$(OK_STRING)$(NO_COLOR)\n"   ; \
fi; cat $@.log; rm -f $@.log; exit $$RESULT
endef

define run_test
printf "%b" "$(COM_COLOR) $(TST_STRING) $(TST_COLOR)$$(basename $(1) .test)$(NO_COLOR)\r"; \
$(1) >$(1).log 2>&1 $(1).log; RESULT=$$?; \
if [ $$RESULT -ne 0 ]; then printf "%-76b%b" "$(COM_COLOR) $(TST_STRING)$(TST_COLOR) $$(basename $(1) .test)" "$(ERROR_COLOR)$(FAILED_STRING)$(NO_COLOR)\n"   ; \
else printf "%-76b%b" "$(COM_COLOR) $(TST_STRING)$(TST_COLOR) $$(basename $(1) .test)" "$(OK_COLOR)$(PASSED_STRING)$(NO_COLOR)\n"   ; \
fi; \echo "\n\033[0;37m~~~~~~~~~~~~~~~~~~~~~~~~~~~ :Test Results: ~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m\n"; \
cat $(1).log; echo "\n\033[0;37m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m\n"; \
rm -f $(1).log;
endef

define run_test_profile
printf "%b" "$(COM_COLOR) Profiling-Test $(TST_COLOR)$$(basename $(1) .test)$(NO_COLOR)\r"; \
valgrind --leak-check=full $(1) >$(1).log 2>&1 $(1).log; RESULT=$$?; \
if [ $$RESULT -ne 0 ]; then printf "%-76b%b" "$(COM_COLOR) Profiling-Test $(TST_COLOR) $$(basename $(1) .test)" "$(ERROR_COLOR)$(FAILED_STRING)$(NO_COLOR)\n"   ; \
else printf "%-76b%b" "$(COM_COLOR) Profiling-Test $(TST_COLOR) $$(basename $(1) .test)" "$(OK_COLOR)$(PASSED_STRING)$(NO_COLOR)\n"   ; \
fi; \echo "\n\033[0;37m~~~~~~~~~~~~~~~~~~~~~~~~~~~ :Test Results: ~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m\n"; \
cat $(1).log; echo "\n\033[0;37m~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\033[0m\n"; \
rm -f $(1).log;
endef

# compiler
CC :=g++
CFLAGS :=-std=c++17 -fdiagnostics-color=always -pthread

# binary subdirectories
BIN := bin

# selector for compilation mode
ifeq ($(MODE),debug)
	OUT := $(BIN)/debug
	EXTF := -g -Wall -Wextra -O0 -Wconversion -pedantic
else
	OUT := $(BIN)/release
	EXTF := -Wall
endif


SRC := src
INC := include
TST := $(SRC)/tests
APP := $(SRC)/app
OBJ := $(OUT)/.objs

O   := $(shell find src/* -path $(TST) -prune -o -path $(APP) -prune -o -name '*.cpp' -exec basename {} \;)
O   := $(O:.cpp=.o)
I   := $(addprefix -I, $(shell find $(INC) -type d))
T   := $(shell find $(TST)/* -type f -name "*.cpp" -exec basename {} \;)
T   := $(T:.cpp=.test)
A   := $(shell find $(APP)/* -type f -name "*.cpp" -exec basename {} \;)
A   := $(T:.cpp=.app)

clean:
	rm -rfv $(BIN)

# all tests
test: $(T)
	@$(eval t := $(patsubst %, $(OUT)/.tests/%, $(T)))
	@$(foreach tf, $(t), $(call run_test, ./$(tf)))

# all tests mem profile
test-profile: $(T)
	@$(eval t := $(patsubst %, $(OUT)/.tests/%, $(T)))
	@$(foreach tf, $(t), $(call run_test_profile, $(tf)))

# specific test
Test.%: %.test
	@$(call run_test, ./$(OUT)/.tests/$<)

# specific test profile
ProfileTest.%: %.test
	@$(call run_test_profile, $(OUT)/.tests/$<)

%.o:
	@$(eval src  := $(shell find $(SRC) -name "$*.cpp"))
	@$(eval base := $(subst $(SRC)/,,$(dir $(src))))
	@mkdir -p $(OBJ)/$(base)
	@$(call compile_and_test, $(CC) $(EXTF) $(CFLAGS) $(I) -c $(src) -o $(OBJ)/$(base)$@, $(COM_STRING))

%.test: %.o $(O)
	@test -s $(TST)/$*.cpp || { echo "Test source $(TST)/$*.cpp does not exist!"; exit 1; }
	@$(eval o := $(foreach fn,$(O) $*.o,$(shell find $(OBJ)/* -type f -name "$(fn)")))
	@mkdir -p $(OUT)/.tests
	@$(call compile_and_test, $(CC) $(EXTF) $(CFLAGS) $(I) -o $(OUT)/.tests/$*.test $(o), $(LNK_STRING))
	@chmod +x $(OUT)/.tests/$*.test

%.app: %.o $(O)
	@test -s $(APP)/$*.cpp || { echo "App source '$(APP)/$*.cpp' does not exist!"; exit 1; }
	@$(eval o := $(foreach fn,$(O) $*.o,$(shell find $(OBJ)/* -type f -name "$(fn)")))
	@$(call compile_and_test, $(CC) $(EXTF) $(CFLAGS) $(I) -o $(OUT)/$*.app $(o), $(LNK_STRING))
	@chmod +x $(OUT)/$*.app
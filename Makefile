CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -std=c++20 -msse4.1 -mpclmul
LDFLAGS  := -L/usr/lib -lstdc++ -lm -lbotan-2
BUILD    := ./out
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := kauma
INCLUDE  := -Iinclude/ -Iexternal/ -I/usr/include/botan-2/
SRC      :=                      \
   $(wildcard src/*.cpp)         \
   $(wildcard src/actions/*.cpp)	\
   $(wildcard src/tcp/*.cpp)	 \
   $(wildcard src/padding_oracle/*.cpp) \
   $(wildcard src/gcm/*.cpp)	\
   $(wildcard src/gcm/cantor_zassenhaus/*.cpp)	\

HEADERS	 :=                      \
   $(wildcard include/*.hpp)	 \
   $(wildcard include/tcp/*.hpp) \
   $(wildcard include/padding_oracle/*.hpp) \
   $(wildcard include/gcm/*.hpp)	\
   $(wildcard include/gcm/cantor_zassenhaus/*.hpp)	\

OBJECTS  := $(SRC:%.cpp=$(OBJ_DIR)/%.o)
DEPENDENCIES \
         := $(OBJECTS:.o=.d)

all: build $(APP_DIR)/$(TARGET)

$(OBJ_DIR)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -MMD -o $@

$(APP_DIR)/$(TARGET): $(OBJECTS)
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -o $(APP_DIR)/$(TARGET) $^ $(LDFLAGS)

-include $(DEPENDENCIES)

.PHONY: all build clean debug release info docs

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

unittest: clean
unittest: CXXFLAGS += -DTEST -g
unittest: all
unittest:
	@$(APP_DIR)/$(TARGET)

systemtest:
	@./test.py

test: unittest systemtest

format:
	-@clang-format -i $(SRC) $(HEADERS)

docs: build
	-@doxygen Doxyfile

clean:
	-@rm -rvf $(OBJ_DIR)/*
	-@rm -rvf $(APP_DIR)/*
	-@rm -rvf $(BUILD)

info:
	@echo "[*] Application dir: ${APP_DIR}     "
	@echo "[*] Object dir:      ${OBJ_DIR}     "
	@echo "[*] Sources:         ${SRC}         "
	@echo "[*] Objects:         ${OBJECTS}     "
	@echo "[*] Dependencies:    ${DEPENDENCIES}"
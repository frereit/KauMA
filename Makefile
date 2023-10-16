CXX      := -c++
CXXFLAGS := -pedantic-errors -Wall -Wextra -std=c++20
LDFLAGS  := -L/usr/lib -lstdc++ -lm
BUILD    := ./out
OBJ_DIR  := $(BUILD)/objects
APP_DIR  := $(BUILD)/apps
TARGET   := kauma
INCLUDE  := -Iinclude/ -Iexternal/
SRC      :=                      \
   $(wildcard src/*.cpp)         \

HEADERS	 :=                      \
   $(wildcard include/*.hpp)	 \


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

.PHONY: all build clean debug release info

build:
	@mkdir -p $(APP_DIR)
	@mkdir -p $(OBJ_DIR)

debug: CXXFLAGS += -DDEBUG -g
debug: all

release: CXXFLAGS += -O2
release: all

test: clean
test: CXXFLAGS += -DTEST -g
test: all
test:
	@echo Executing unit tests
	@$(APP_DIR)/$(TARGET)
	@echo Executing system tests
	@./test.py

format:
	-@clang-format -i $(SRC) $(HEADERS)

docs:
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
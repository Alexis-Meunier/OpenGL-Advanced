CC = g++

# SRCS = src/main.cpp src/matrix.cpp
SRCS = src/app.cpp src/main.cpp src/material.cpp src/object.cpp src/program.cpp src/scene.cpp src/shader.cpp src/matrix.cpp src/camera.cpp src/light.cpp
OBJS = $(SRCS:.cpp=.o)

CFLAGS = -Wall -Wextra -O3 -g -std=c++20
CFLAGS += -m64 -march=native
CFLAGS += $(shell pkg-config --cflags glew glut)

CPPFLAGS = -Iincludes

LDFLAGS = -lGL  -lGLEW -lglut -lpthread
LDFLAGS += $(shell pkg-config --libs glew glut)

TARGET = main

export GCC_COLORS=1

define color
    if test -n "${TERM}" ; then\
	if test `tput colors` -gt 0 ; then \
	    tput setaf $(1); \
        fi;\
    fi
endef

define default_color
    if test -n "${TERM}" ; then\
	if test `tput colors` -gt 0 ; then  tput sgr0 ; fi; \
    fi
endef


all: post-build

pre-build:
	@$(call color,4)
	@echo "******** Starting Compilation ************"
	@$(call default_color)

post-build:
	@make --no-print-directory main-build ; \
	sta=$$?;	  \
	$(call color,4); \
	echo "*********** End Compilation **************"; \
	$(call default_color); \
	exit $$sta;

main-build: pre-build build

build: $(OBJS)
	$(CC) $(MAIN_FILE) -o $(TARGET) $(OBJS) $(CFLAGS) $(LDFLAGS)


%.o: %.cpp
	@$(call color,2)
	@echo "[$@] $(CFLAGS)"
	@$(call default_color)
	@$(CC) -c -o $@ $< $(CFLAGS) $(CPPFLAGS); \
	sta=$$?;	  \
	if [ $$sta -eq 0 ]; then  \
	  $(call color,2) ; \
	  echo "[$@ succes]" ; \
	  $(call default_color) ; \
	else  \
	  $(call color,1) ; \
	  echo "[$@ failure]" ; \
	  $(call default_color) ; \
	fi ;\
	exit $$sta

.PHONY: all clean pre-build post-build main-build build

clean:
	$(RM) $(OBJS) $(TARGET)


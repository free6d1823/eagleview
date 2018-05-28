PROJECT_ROOT = $(dir $(abspath $(lastword $(MAKEFILE_LIST))))
TARGET_NAME = play
OBJS =   main.o basicobject.o controls.o sphere.o car.o
COMMON_OBJS = common/texture.o common/shader.o common/objloader.o common/vboindexer.o
SOIL_OBJS = SOIL/SOIL.o SOIL/image_DXT.o SOIL/image_helper.o SOIL/stb_image_aug.o

ifeq ($(BUILD_MODE),debug)
	CFLAGS += -g
else 
	CFLAGS += -O2
endif

INCLUDE_PATH = -I./
INCLUDE_PATH += -I$(PROJECT_ROOT)external/glfw-3.1.2/include/ 
INCLUDE_PATH += -I$(PROJECT_ROOT)external/glm-0.9.7.1/ 
INCLUDE_PATH += -I$(PROJECT_ROOT)external/glew-1.13.0/include/
CFLAGS += $(INCLUDE_PATH)

LIB_PATH = -L$(PROJECT_ROOT)external/lib
LIB_LINK = -lGLEW_1130 -lglfw3  -lX11 -lm -lpthread -lXrandr -lXinerama -lXi -lXxf86vm -lXcursor -lGL -ldl
LFLAGS += $(LIB_PATH) $(LIB_LINK)  
 
all:	$(TARGET_NAME)

$(TARGET_NAME):	$(OBJS) $(COMMON_OBJS) $(SOIL_OBJS)
	$(CXX) $^ -o $@ $(LFLAGS)	
	mkdir -p $(PROJECT_ROOT)bin
	cp $@ $(PROJECT_ROOT)bin/.

SOIL/%.o:	SOIL/%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

common/%.o:	common/%.cpp
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

	
%.o:	$(PROJECT_ROOT)%.cpp calibration.h 
	$(CXX) -c $(CFLAGS) $(CXXFLAGS) $(CPPFLAGS) -o $@ $<

%.o:	$(PROJECT_ROOT)%.c
	$(CC) -c $(CFLAGS) $(CPPFLAGS) -o $@ $<

clean:
	rm -fr $(TARGET_NAME) $(OBJS) $(COMMON_OBJS)

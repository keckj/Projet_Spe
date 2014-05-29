
#fichier de config pour definir des variables makefiles
CONFIG = $(shell find config.mk 2> /dev/null)
ifeq ($(CONFIG), config.mk)
$(info Chargement du fichier de configuration 'config.mk' !)
include config.mk
else
$(info Pas de fichier de configuration présent (config.mk) !)
endif

ifndef N_MAIN
N_MAIN = 0
endif
$(info Compilation du main numéro $(N_MAIN),)

####################
### LIB EXTERNES ###
####################

OS=$(shell uname -s)
$(info pour la plateforme $(OS))

# Linux ########################################################
ifeq ($(OS), Linux)

VIEWER_LIBPATH = -L/usr/X11R6/lib64 -L/usr/lib/x86_64-linux-gnu
VIEWER_INCLUDEPATH = -I/usr/include/Qt -I/usr/include/QtCore -I/usr/include/QtGui -I/usr/share/qt4/mkspecs/linux-g++-64 -I/usr/include/QtOpenGL -I/usr/include/QtXml -I/usr/X11R6/include -I/usr/include/qt4/ $(foreach dir, $(shell ls /usr/include/qt4 | xargs), -I/usr/include/qt4/$(dir))
VIEWER_LIBS = -lGLU -lglut -lGL -lQtXml -lQtOpenGL -lQtGui -lQtCore -lpthread -lGLEW
VIEWER_DEFINES = -D_REENTRANT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

CUDA_INCLUDEPATH = -I/usr/local/cuda/include
CUDA_LIBPATH = -L/usr/lib/nvidia-331 -L/usr/local/cuda/lib64
CUDA_LIBS = -lcuda -lcudart
NVCC=nvcc

OPENCL_INCLUDEPATH = -I/usr/include/boost
OPENCL_LIBPATH =
OPENCL_LIBS = -lOpenCL

DISTRIB=$(filter-out Distributor ID:, $(shell lsb_release -i))
$(info et la distrib $(DISTRIB))

ifeq ($(DISTRIB), Ubuntu)
###### Ubuntu
else 
###### Centos
endif

endif
# Fin Linux ####################################################

# Pomme ###############################################
ifeq ($(OS), Darwin)
VIEWER_LIBPATH = -L/usr/local/Cellar/qt/4.8.5/lib -L/usr/local/Cellar/qt/4.8.5/lib  -L/opt/X11/lib -L/usr/local/Cellar/qt/4.8.5/lib -L/usr/local/Cellar/qt/4.8.5/lib
VIEWER_INCLUDEPATH       = -I/usr/local/Cellar/qt/4.8.5/mkspecs/macx-g++ -I. -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtCore.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtGui.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtOpenGL.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtOpenGL.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtXml.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/lib/QtXml.framework/Versions/4/Headers -I/usr/local/Cellar/qt/4.8.5/include -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/System/Library/Frameworks/AGL.framework/Headers -I. -L/usr/local/Cellar/qt/4.8.5/lib
VIEWER_LIBS = -framework Glut -framework OpenGL -framework AGL -framework QtXml -framework QtCore -framework QtOpenGL -framework QtGui -lGLEW
VIEWER_DEFINES = -D_REENTRANT -DQT_NO_DEBUG -DQT_XML_LIB -DQT_OPENGL_LIB -DQT_GUI_LIB -DQT_CORE_LIB -DQT_SHARED

CUDA_INCLUDEPATH = -I/Developer/NVIDIA/CUDA/include
CUDA_LIBPATH = -L/Developer/NVIDIA/CUDA/lib 
CUDA_LIBS = -lcudart
NVCC=/Developer/NVIDIA/CUDA-5.5/bin/nvcc -ccbin /usr/bin/clang

OPENCL_INCLUDEPATH =
OPENCL_LIBPATH =
OPENCL_LIBS =
endif
##Fin Pomme ####################################################

# Macros
containing = $(foreach v,$2,$(if $(findstring $1,$v),$v))
not_containing = $(foreach v,$2,$(if $(findstring $1,$v),,$v))
subdirs = $(shell find $1 -type d)

# Source et destination des fichiers
SRCDIR = $(realpath .)/src
OBJDIR = $(realpath .)/obj
EXCL=deprecated/ #excluded dirs 
EXCLUDED_SUBDIRS = $(foreach DIR, $(EXCL), $(call subdirs, $(SRCDIR)/$(DIR)))

SUBDIRS =  $(filter-out $(EXCLUDED_SUBDIRS), $(call subdirs, $(SRCDIR)))
TARGET = main
SRC_EXTENSIONS = c C cc cpp s S asm cu #cl
WEXT = $(addprefix *., $(SRC_EXTENSIONS))

MOCSRC = $(shell grep -rlw $(SRCDIR) -e 'Q_OBJECT' --include=*.h --include=*.hpp | xargs) #need QT preprocessor
MOCOUTPUT = $(addsuffix .moc, $(basename $(MOCSRC)))

SRC = $(foreach DIR, $(SUBDIRS), $(foreach EXT, $(WEXT), $(wildcard $(DIR)/$(EXT))))
OBJ = $(subst $(SRCDIR), $(OBJDIR), $(addsuffix .o, $(basename $(SRC))))


#Compilateurs
LINK= g++
LINKFLAGS= -W -Wall -Wextra -pedantic -std=c++0x
LDFLAGS= $(VIEWER_LIBS) $(CUDA_LIBS) $(OPENCL_LIBS) -llog4cpp
INCLUDE = -I$(SRCDIR) $(foreach dir, $(SUBDIRS), -I$(dir)) $(VIEWER_INCLUDEPATH) $(CUDA_INCLUDEPATH) $(OPENCL_INCLUDEPATH)
LIBS = $(VIEWER_LIBPATH) $(CUDA_LIBPATH) $(OPENCL_LIBPATH)
DEFINES= $(VIEWER_DEFINES) $(OPT) -D_N_MAIN=$(N_MAIN) 

CC=gcc
CFLAGS= -W -Wall -Wextra -pedantic -std=c99 -m64

CXX=g++
CXXFLAGS= -W -Wall -Wextra -Wshadow -Wstrict-aliasing -Werror -pedantic -std=c++0x -m64  -Wno-unused-parameter -Wno-comment -Wno-unused-variable #-Weffc++

#preprocesseur QT
MOC=moc
MOCFLAGS=

NVCCFLAGS= -Xcompiler -Wall -m64 -arch sm_$(NARCH) -O3

AS = nasm
ASFLAGS= -f elf64

# Autres flags
DEBUGFLAGS= -g -O0
CUDADEBUGFLAGS= -Xcompiler -Wall -m64 -G -g -arch sm_$(NARCH) -Xptxas="-v"
PROFILINGFLAGS= -pg
RELEASEFLAGS= -O3

include rules.mk

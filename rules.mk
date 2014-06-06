
# Règles
all: create_dirs moc $(TARGET)

ifeq ($(LINK), NVCC)
debug: LINKFLAGS = $(CUDADEBUGFLAGS) 
else
debug: LINKFLAGS = $(DEBUGFLAGS)
endif
debug: CFLAGS += $(DEBUGFLAGS) -D__DEBUG 
debug: CXXFLAGS += $(DEBUGFLAGS) -D__DEBUG -D__CONSOLE_LOG_LEVEL="DEBUG" -D__FILE_LOG_LEVEL="DEBUG"
debug : NVCCFLAGS = $(CUDADEBUGFLAGS) -D__DEBUG -D__DEBUG -D__CONSOLE_LOG_LEVEL="DEBUG" -D__FILE_LOG_LEVEL="DEBUG"
debug: all

profile: LINKFLAGS += $(PROFILINGFLAGS)
profile: CFLAGS += $(PROFILINGFLAGS)
profile: CXXFLAGS += $(PROFILINGFLAGS)
profile: all

release: LINKFLAGS += $(RELEASEFLAGS)
release: CFLAGS += $(RELEASEFLAGS)
release: CXXFLAGS += $(RELEASEFLAGS)
release: all

$(TARGET): $(OBJ)
	@echo
	@echo
	$(LINK) $(LIBS) $(OBJ) -o $@ $(LDFLAGS) $(LINKFLAGS) $(DEFINES)
	@echo

#QT macro preprocessor
$(SRCDIR)%.moc : $(SRCDIR)%.hpp
	@echo
	$(MOC) $(INCLUDE) $(DEFINES) -o $@ -i $^ $(MOCFLAGS)
################


$(OBJDIR)%.o : $(SRCDIR)%.c
	$(CC) $(INCLUDE) -o $@ -c $^ $(CFLAGS) $(DEFINES)
	@echo
$(OBJDIR)%.o : $(SRCDIR)%.C 
	$(CXX) $(INCLUDE) -o $@ -c $^ $(CXXFLAGS) $(DEFINES)
	@echo
$(OBJDIR)%.o : $(SRCDIR)%.cc 
	$(CXX) $(INCLUDE) -o $@ -c $^ $(CXXFLAGS) $(DEFINES)
	@echo
$(OBJDIR)%.o : $(SRCDIR)%.cpp
	$(CXX) $(INCLUDE) -o $@ -c $^ $(CXXFLAGS) $(DEFINES)
	@echo

$(OBJDIR)%.o : $(SRCDIR)%.s
	@echo
	$(AS) $(INCLUDE) -o $@ $^ $(ASFLAGS) 
$(OBJDIR)%.o : $(SRCDIR)%.S
	@echo
	$(AS) $(INCLUDE) -o $@ $^ $(ASFLAGS)
$(OBJDIR)%.o : $(SRCDIR)%.asm
	@echo
	$(AS) $(INCLUDE) -o $@ $^ $(ASFLAGS)

$(OBJDIR)%.o: $(SRCDIR)%.cu 
	@echo
	$(NVCC) $(INCLUDE) -o $@ -c $^ $(NVCCFLAGS) $(DEFINES)


# "-" pour enlever les messages d'erreurs
# "@" pour silent
.PHONY: clean cleanall create_dirs all distrib moc

moc: $(MOCOUTPUT)

clean:
	-@rm -f $(OBJ) 

cleanall:
	-@rm -rf $(TARGET) $(TARGET).out $(OBJDIR) $(MOCOUTPUT) tags types_c.taghl program.log

create_dirs:
	@mkdir -p $(subst $(SRCDIR), $(OBJDIR), $(SUBDIRS))

distrib:
	echo $(DISTRIB)

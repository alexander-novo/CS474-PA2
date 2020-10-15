CXXFLAGS     = -std=c++17 -fopenmp
OBJDIR       = obj
DEPDIR       = $(OBJDIR)/.deps
# Flags which, when added to gcc/g++, will auto-generate dependency files
DEPFLAGS     = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.d

# Source files - add more to auto-compile into .o files
SOURCES      = Common/image.cpp Q1-Correlation/main.cpp Q2-Smoothing/main.cpp Q3-Median/main.cpp Q4-Unsharp/main.cpp Q5-Gradient/main.cpp
# Executable targets - add more to auto-make in default 'all' target
EXEC         = Q1-Correlation/correlate Q2-Smoothing/smooth Q3-Median/median Q4-Unsharp/unsharp Q5-Gradient/gradient
# Targets required for the homework, spearated by question
REQUIRED_1   = 
REQUIRED_2   = 
REQUIRED_3   = 
REQUIRED_4   = 
REQUIRED_5   =
REQUIRED_OUT = $(REQUIRED_1) $(REQUIRED_2) $(REQUIRED_3) $(REQUIRED_4) $(REQUIRED_5)

OBJDIRS      = $(addprefix $(OBJDIR)/, $(dir $(SOURCES)))
DEPDIRS      = $(addprefix $(DEPDIR)/, $(dir $(SOURCES)))
DEPFILES     = $(SOURCES:%.cpp=$(DEPDIR)/%.d)

.PHONY: all clean report

# By default, make all executable targets and the images required for the homework
all: $(EXEC) $(REQUIRED_OUT)

# Executable Targets
Q1-Correlation/correlate: $(OBJDIR)/Q1-Correlation/main.o $(OBJDIR)/Common/image.o
	$(CXX) $(CXXFLAGS) $^ -o $@

Q2-Smoothing/smooth: $(OBJDIR)/Q2-Smoothing/main.o $(OBJDIR)/Common/image.o
	$(CXX) $(CXXFLAGS) $^ -o $@

Q3-Median/median: $(OBJDIR)/Q3-Median/main.o $(OBJDIR)/Common/image.o
	$(CXX) $(CXXFLAGS) $^ -o $@

Q4-Unsharp/unsharp: $(OBJDIR)/Q4-Unsharp/main.o $(OBJDIR)/Common/image.o
	$(CXX) $(CXXFLAGS) $^ -o $@

Q5-Gradient/gradient: $(OBJDIR)/Q5-Gradient/main.o $(OBJDIR)/Common/image.o
	$(CXX) $(CXXFLAGS) $^ -o $@

### Question 1 Outputs ###


### Question 2 Outputs ###


### Question 3 Outputs ###


### Question 4 Outputs ###


### Question 5 Outputs ###


# Figures needed for the report
report: 

clean:
	rm -rf $(OBJDIR)
	rm -f $(EXEC)
	rm -rf out
	rm -f Images/*.png

# Generate .png images from .pgm images. Needed for report, since pdfLaTeX doesn't support .pgm images
%.png: %.pgm
	pnmtopng $< > $@

# Auto-Build .cpp files into .o
$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d | $(DEPDIRS) $(OBJDIRS)
	$(CXX) $(DEPFLAGS) $(CXXFLAGS) -c $< -o $@

# Make generated directories
$(DEPDIRS) $(OBJDIRS) out: ; @mkdir -p $@
$(DEPFILES):
include $(wildcard $(DEPFILES))
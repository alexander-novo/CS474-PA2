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
REQUIRED_1   = out/patterns-pattern-correlated.pgm
REQUIRED_2   = out/lenna-7-average-smoothed.pgm out/lenna-15-average-smoothed.pgm out/lenna-7-gaussian-smoothed.pgm out/lenna-15-gaussian-smoothed.pgm out/sf-7-average-smoothed.pgm out/sf-15-average-smoothed.pgm out/sf-7-gaussian-smoothed.pgm out/sf-15-gaussian-smoothed.pgm
REQUIRED_3   = out/lenna-7-30-median.pgm out/lenna-15-30-median.pgm out/lenna-7-50-median.pgm out/lenna-15-50-median.pgm out/boat-7-30-median.pgm out/boat-15-30-median.pgm out/boat-7-50-median.pgm out/boat-15-50-median.pgm
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
.SECONDEXPANSION:
out/%-correlated.pgm: Q1-Correlation/correlate Images/$$(word 1,$$(subst -, ,$$*)).pgm | out
	Q1-Correlation/correlate Images/$(word 1,$(subst -, ,$*)).pgm $@ Images/$(word 2,$(subst -, ,$*)).pgm 83 55

### Question 2 Outputs ###
.SECONDEXPANSION:
out/%-smoothed.pgm: Q2-Smoothing/smooth Images/$$(word 1,$$(subst -, ,$$*)).pgm | out
	Q2-Smoothing/smooth Images/$(word 1,$(subst -, ,$*)).pgm $@ $(word 2,$(subst -, ,$*)) $(word 3,$(subst -, ,$*))

### Question 3 Outputs ###
.SECONDEXPANSION:
out/%-median.pgm: Q3-Median/median Images/$$(word 1,$$(subst -, ,$$*)).pgm | out
	Q3-Median/median Images/$(word 1,$(subst -, ,$*)).pgm $@ $(word 2,$(subst -, ,$*)) $(word 3,$(subst -, ,$*))

### Question 4 Outputs ###


### Question 5 Outputs ###


# Figures needed for the report
report: Images/patterns.png Images/pattern.png out/patterns-pattern-correlated.png
report: Images/lenna.png Images/sf.png out/lenna-7-average-smoothed.png out/lenna-15-average-smoothed.png out/lenna-7-gaussian-smoothed.png out/lenna-15-gaussian-smoothed.png out/sf-7-average-smoothed.png out/sf-15-average-smoothed.png out/sf-7-gaussian-smoothed.png out/sf-15-gaussian-smoothed.png
report: Images/boat.png out/lenna-7-30-median.png out/lenna-15-30-median.png out/lenna-7-50-median.png out/lenna-15-50-median.png out/boat-7-30-median.png out/boat-15-30-median.png out/boat-7-50-median.png out/boat-15-50-median.png

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

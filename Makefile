# Copyright (c) 2019 Tom Hancocks
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

# Locate all KAS Source Files and Intermediates
kas-sources := $(shell find kas -type f \( -name "*.cpp" \))
kas-objects := $(kas-sources:%.cpp=%.o)

# Top Level Rules
.PHONY: all
all: build/kas

.PHONY: clean
clean:
	-@rm -v $(kas-objects)
	-@rm -rf build
	
# Directory
build:
	-@mkdir -p build

# Targets
build/kas: build $(kas-objects)
	$(CXX) -I./kas -o $@ $(kas-objects)

# Intermediates
%.o: %.cpp
	$(CXX) -c -I./kas -std=gnu++14 -o $@ $^

/*
* Copyright (c) 2019 Tom Hancocks
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all
* copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/

#include <string>
#include <vector>
#include "structures/resource.hpp"

#if !defined(KDK_TARGET)
#define KDK_TARGET

namespace kdk
{

/**
 * Represents the output target that is the result of the assembling process.
 * The target contains information about how it should be saved, what
 * resources it contains, etc.
 *
 * The target is the ultimate location of all resources, and the reference point
 * for looking up resources.
 */
class target
{
public:
    /**
     * Construct a new target with the specified output path.
     */
    target(std::string path);
    
    /**
     * Add resources to the target.
     */
    void add_resources(const std::vector<kdk::resource> resources);
    
    /**
     * Build the kestrel data file.
     *
     * This method will begin the process of validating all of the resources added
     * to the target, converting them into data objects and assembling the resource
     * file.
     */
    void build();
    
private:
    std::string m_path;
    std::vector<kdk::resource> m_resources;
};

};

#endif

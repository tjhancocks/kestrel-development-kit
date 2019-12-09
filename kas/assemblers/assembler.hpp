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

#include "rsrc/data.hpp"
#include "structures/resource.hpp"

#if !defined(KDK_ASSEMBLER)
#define KDK_ASSEMBLER

namespace kdk
{

/**
 * The kdk::assembler class houses the core implementation for converting a
 * kdk::resource into a rsrc::data object. The class itself should be subclassed
 * in order to set up the appropriate layout and definition for the resource
 * type.
 */
class assembler
{
public:
    /**
     * Construct a new assembler using the specified resource. This assembler
     * does not specifically care about type (this information should be provided
     * by the subclass.)
     */
    assembler(const kdk::resource& resource);
    
    /**
     * Performs assembly of the resource.
     *
     * This method should be implemented by the subclass.
     */
    void assemble();
    
private:
    kdk::resource m_resource;
    rsrc::data m_blob;
};

};

#endif


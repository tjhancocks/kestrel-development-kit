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

#include <initializer_list>

#include "rsrc/file.hpp"
#include "rsrc/data.hpp"
#include "structures/resource.hpp"

#if !defined(KDK_BASE_RESOURCE_TYPE)
#define KDK_BASE_RESOURCE_TYPE

namespace kdk
{

/**
 * Base resource type for all kestrel types.
 *
 * This base resource type supplies a variety of functions and methods that
 * are used for reading, parsing and interpreting fields.
 */
struct base_type
{
public:
    
    /**
     * Construct a base type with the specified resource object.
     */
    base_type(kdk::resource res, std::shared_ptr<rsrc::file> rf);
    
    /**
     * Assemble the resource into a binary data blob.
     */
    rsrc::data assemble();
    
    /**
     * Check if the supplied field exists, and one of the specified types.
     */
    bool field_exists(const std::string name, std::initializer_list<kdk::resource::field::value_type> allowed_types = {}) const;
    
    /**
     * Returns the value of the specified field as a resource id.
     */
    int64_t resource_id_for(const std::string name, const int value = 0) const;
    
    /**
     * Returns the value of the specified field as an integer.
     */
    int64_t integer_for(const std::string name, const int value = 0) const;
    
    /**
     * Returns the value of the specified field as a string.
     */
    std::string string_for(const std::string name, const int value_index) const;
    
    /**
     * Import and resolve the specified file reference, returning a Resource ID.
     */
    std::shared_ptr<rsrc::file::resource> import_and_resolve_file(const std::string name, int64_t resource_id = 0, const int value = 0) const;
    
    
private:
    kdk::resource m_source;
    std::shared_ptr<rsrc::file> m_rf;
};

};

#endif

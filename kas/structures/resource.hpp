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
#include <tuple>

#if !defined(KDK_RESOURCE)
#define KDK_RESOURCE

namespace kdk
{

/**
 * Represents a generic resource. It contains type information, resource id,
 * name, and specified fields & values. Resource objects can then be used as
 * a source of information when constructing and validating actual resource
 * data blobs.
 */
class resource
{
public:
    
    /**
     * The resource field denotes a key-value pair of sorts. Fields can represent
     * a few different types of value, which are stored as textual values along with
     * a value type.
     */
    struct field
    {
    public:
        
        /**
         * The type of a value encoded in the field.
         */
        enum value_type
        {
            identifier,
            resource_id,
            integer,
            string,
            percentage,
            file_reference,
        };
        
    public:
        /**
         * Construct a new resource field with the specified name, values and encodings.
         */
        field(const std::string name, std::vector<std::tuple<std::string, value_type>> values);
        
        /**
         * Returns the name of the field
         */
        std::string name() const;
        
        /**
         * Returns the vector containing the values.
         */
        std::vector<std::tuple<std::string, resource::field::value_type>> values() const;
        
    private:
        std::string m_name;
        std::vector<std::tuple<std::string, value_type>> m_values;
    };
    
    
public:
    /**
     * Construct a new target with the specified output path.
     */
    resource(const std::string type, const int64_t id, const std::string name);
    
    /**
     * Add a new field to the end of the resource.
     */
    void add_field(const resource::field& field);
    
private:
    int64_t m_id { 0 };
    std::string m_type { "" };
    std::string m_name { "" };
    std::vector<resource::field> m_fields;
};

};

#endif

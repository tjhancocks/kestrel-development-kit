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
#include <memory>
#include "rsrc/data.hpp"


#if !defined(RSRC_FILE)
#define RSRC_FILE

namespace rsrc
{

/**
 * Represents a Resource File on disk.
 *
 * A Resource File contains the ResourceFork format and is the format that is
 * used and understood by Kestrel.
 */
class file
{
public:
    
    /**
     * A resource object in the resource file.
     *
     * Resource objects represent actual resource data in the resource file. They
     * contain the actual binary blob data, name, ID and attributes of the
     * resource.
     */
    struct resource
    {
    public:
        /**
         * Construct a new resource object.
         */
        resource(int64_t id, const std::string name, rsrc::data blob);
        
        /**
         * Create a new shared resource object.
         */
        static std::shared_ptr<rsrc::file::resource> create(int64_t id, const std::string name, rsrc::data blob);
        
    private:
        int64_t m_id;
        std::string m_name;
        rsrc::data m_blob;
    };
    
    /**
     * A type container in the resource file.
     *
     * Type containers are characterised by their 4 character code, such as 'mïsn',
     * 'PICT', 'vers', etc. They contain a list of resources that are of that
     * type.
     */
    struct type_container
    {
    public:
        /**
         * Construct a new type container.
         */
        type_container(const std::string type_code);
        
        /**
         * Create a new shared type container.
         */
        static std::shared_ptr<rsrc::file::type_container> create(const std::string type_code);
        
        /**
         * Add a new resource object to the type.
         */
        void add_resource(std::shared_ptr<rsrc::file::resource> resource);
        
        /**
         * Return the type code of the container.
         */
        std::string type_code() const;
        
    private:
        std::string m_code;
        std::vector<std::shared_ptr<rsrc::file::resource>> m_resources;
    };
    
public:
    /**
     * Construct a new ResourceFile instance at the specified path.
     *
     * Resource File's saved currently will be saved in the bespoke "extended"
     * format. This should be updated to allow the legacy format as well in order
     * for plugin developers to target EV Nova. However for the standard format
     * to also work for EV Nova, certain checks and tests may need to be performed,
     * along with correct conversions of graphics.
     */
    file(const std::string path);
    
    /**
     * Create a new shared ResourceFile.
     */
    static std::shared_ptr<rsrc::file> create(const std::string path);
    
    /**
     * Add a new resource to the file.
     */
    void add_resource(const std::string type_code, int64_t id, const std::string name, rsrc::data data);
    
    /**
     * Find or create the specified type container.
     */
    std::shared_ptr<rsrc::file::type_container> get_type_container(const std::string type_code);
    
private:
    std::string m_path;
    std::vector<std::shared_ptr<rsrc::file::type_container>> m_containers;
};

};

#endif

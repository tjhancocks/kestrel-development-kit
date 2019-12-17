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
     * The ResourceFork format.
     */
    enum format
    {
        standard,
        extended,
        rez
    };
    
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
        
        /**
         * Returns the resource id.
         */
        int64_t id() const;
        
        /**
         * Returns the resource name.
         */
        std::string name() const;
        
        /**
         * Returns the binary data contents of the resource.
         */
        rsrc::data blob() const;
        
        /**
         * Set the offset of the resource data within the file it is being written to.
         */
        void set_data_offset(uint64_t offset);
        
        /**
         * Get the offset of the resource data with in the file.
         */
        uint64_t data_offset() const;
        
    private:
        int64_t m_id;
        std::string m_name;
        rsrc::data m_blob;
        uint64_t m_data_offset;
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
        
        /**
         * Returns a vector containing shared pointers to each of its resources.
         */
        std::vector<std::shared_ptr<rsrc::file::resource>> resources() const;
        
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
    std::shared_ptr<rsrc::file::resource> add_resource(const std::string type_code, int64_t id, const std::string name, rsrc::data data);
    
    /**
     * Find or create the specified type container.
     */
    std::shared_ptr<rsrc::file::type_container> get_type_container(const std::string type_code);
    
    /**
     * Write the Resource File to disk.
     *
     * This will write the entire contents of the resource file to disk, replacing
     * the file currently present.
     */
    void write();
    
private:
    file::format m_format { file::format::standard };
    std::string m_path;
    std::vector<std::shared_ptr<rsrc::file::type_container>> m_containers;
    
    rsrc::data write_extended();
    rsrc::data write_standard();
    rsrc::data write_rez();
};

};

#endif

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

#include <type_traits>
#include <memory>
#include <tuple>
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
     * The Field structure represents a field that will be parsed from a `kdk::resource`
     * and converted into raw binary data.
     */
    struct field
    {
    public:
        
        /**
         * The Value structure represents a particular value component of a field. This
         * tells the assembler what type of information it is reading and how to convert
         * it to a raw binary format.
         *
         * Additionally rules can be bound to a value to help validate the value.
         */
        struct value
        {
        public:
            
            /**
             * Denotes the type of data that the value is expected to house.
             */
            enum type
            {
                integer = (1 << 0),
                resource_reference = (1 << 1),
                bitmask = (1 << 2),
                string = (1 << 3),
                color = (1 << 4),
                c_string = (1 << 3) | (1 << 10),
                p_string = (1 << 3) | (1 << 11),
            };
            
        public:
            
            /**
             * Construct a new value expectation
             */
            value(std::string name, kdk::assembler::field::value::type type, uint64_t offset, uint64_t size);
            
            /**
             * Create a new named value expectation.
             */
            static kdk::assembler::field::value expect(const std::string& name, kdk::assembler::field::value::type type, uint64_t offset, uint64_t size);
            
            /**
             * Specify the symbols that can be provided as a value substitution
             */
            kdk::assembler::field::value set_symbols(const std::vector<std::tuple<std::string, int64_t>> symbols);
            
            /**
             * Specify a lambda that can be called so a default value can be written into the
             * data object.
             */
            kdk::assembler::field::value set_default_value(const std::function<void(rsrc::data&)> default_value);
            
            /**
             * Returns the size of the value when encoded
             */
            uint64_t size() const;
            
            /**
             * Returns the offset of the value when encoded.
             */
            uint64_t offset() const;
            
            /**
             * Test the type of the value
             */
            bool type_allowed(kdk::resource::field::value_type type) const;
            
            /**
             * Returns the type mask of the value.
             */
            kdk::assembler::field::value::type type_mask() const;
            
            /**
             * Write the default value into the data.
             */
            void write_default_value(rsrc::data& data) const;
            
            /**
             * Returns a vector of symbol tuples for the value.
             */
            std::vector<std::tuple<std::string, int64_t>>& symbols();
            
        private:
            std::string m_name;
            kdk::assembler::field::value::type m_type_mask;
            std::vector<std::tuple<std::string, int64_t>> m_symbols;
            uint64_t m_size;
            uint64_t m_offset;
            std::function<void(rsrc::data&)> m_default_value;
        };
        
    public:
        
        /**
         * Construct a basic field for the assembler.
         */
        field(const std::string& name);
        
        /**
         * Create a new named field for the assembler.
         */
        static kdk::assembler::field named(const std::string& name);
        
        /**
         * Indicate if the field is deprecated or not
         */
        kdk::assembler::field set_deprecated(bool deprecated);
        
        /**
         * Indicate if the field is required or not
         */
        kdk::assembler::field set_required(bool required);
        
        /**
         * Indicate the expected values for the field.
         */
        kdk::assembler::field set_values(const std::vector<kdk::assembler::field::value>& values);
        
        /**
         * Returns the overall size of the field (If the values are not contiguous, then size is the sum
         * of the sizes of the values)
         */
        uint64_t size() const;
        
        /**
         * Returns the minimum size of the data object required for the field to be able to be encoded.
         */
        uint64_t required_data_size() const;
        
        /**
         * Returns the offset of the _first_ value of the field.
         */
        uint64_t offset() const;
        
        /**
         * Returns the required status of the field.
         */
        bool is_required() const;
        
        /**
         * Returns the deprecated status of the field.
         */
        bool is_deprecated() const;
        
        /**
         * Returns the name of the field.
         */
        std::string& name();
        
        /**
         * Returns the expected values vector.
         */
        std::vector<kdk::assembler::field::value>& expected_values();
        
    private:
        bool m_required { false };
        bool m_deprecated { false };
        std::string m_name;
        std::vector<kdk::assembler::field::value> m_expected_values;
    };
    
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
    rsrc::data assemble();
    
    /**
     * Assemble the specified field.
     */
    void assemble(const kdk::assembler::field field);
    
    /**
     * Find the specified field in the source resource.
     */
    std::shared_ptr<kdk::resource::field> find_field(std::string& name, bool required = false) const;
    
private:
    kdk::resource m_resource;
    rsrc::data m_blob;
    
    /**
     * Write the specified value as an integer to the data at the current
     * offset.
     */
    void encode(const std::string value, uint64_t width, bool is_signed = true);
};

};

#endif


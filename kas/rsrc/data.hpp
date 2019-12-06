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

#include <cstdint>
#include <sys/types.h>
#include <vector>
#include <type_traits>

#if !defined(RSRC_DATA)
#define RSRC_DATA

namespace rsrc
{
  
/**
 * A class intended for writing binary data objects.
 *
 * This is a variant of the `rsrc::data` class found in Kestrel itself,
 * but the inverse. It is intended for writing resource data, not reading.
 */
class data
{
public:
    enum endian
    {
        little,
        big
    };
    
public:
    /**
     * Construct a new data object with the specified endianess.
     *
     * The endianess of the object denotes the byte order of bytes being written to
     * the file.
     */
    data(data::endian e = big);
    
    /**
     * Create a data object using a raw data blob.
     */
    data(std::shared_ptr<std::vector<uint8_t>> blob);
    
    /**
     * Save the contents of the data object to the specified file.
     */
    void save(const std::string& path) const;
    
    /**
     * Set the endianess of the data object.
     */
    void set_endian(data::endian e);
    
    void write_byte(uint8_t v);
    void write_signed_byte(int8_t v);
    
    void write_word(uint16_t v);
    void write_signed_word(int16_t v);
    
    void write_long(uint32_t v);
    void write_signed_long(int32_t v);
    
    void write_quad(uint64_t v);
    void write_signed_quad(int64_t v);
    
    void write_pstr(const std::string& str);
    void write_cstr(const std::string& str, size_t size = 0);
    
    /**
     * Export the contents of the data to disk.
     */
    void export_file(const std::string path) const;
    
private:
    data::endian m_endian;
    std::vector<uint8_t> m_data;
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    T swap(T value, data::endian mode) const;
    
    template<typename T, typename std::enable_if<std::is_arithmetic<T>::value>::type* = nullptr>
    void write_integer(const T& value);
};

};

#endif

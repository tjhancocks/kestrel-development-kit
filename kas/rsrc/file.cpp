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

#include <fstream>
#include <iostream>
#include <algorithm>
#include "rsrc/file.hpp"
#include "rsrc/macroman.hpp"

// MARK: - Constructor

rsrc::file::file(const std::string path)
    : m_path(path) 
{
    
}

// MARK: - Convenience

std::shared_ptr<rsrc::file> rsrc::file::create(const std::string path)
{
    return std::make_shared<rsrc::file>(path);
}

// MARK: - Resource Management

std::shared_ptr<rsrc::file::resource> rsrc::file::add_resource(const std::string type, int64_t id, const std::string name, rsrc::data data)
{
    // Get the container
    auto container = get_type_container(type);
    if (!container) {
        throw std::runtime_error("Failed to find or create resource type container for " + type);
    }
    
    // Add the resource...
    auto resource = rsrc::file::resource::create(id, name, data);
    container->add_resource(resource);
    return resource;
}

std::shared_ptr<rsrc::file::type_container> rsrc::file::get_type_container(const std::string type_code)
{
    // Try and find the container first...
    for (auto container : m_containers) {
        if (container->type_code() == type_code) {
            return container;
        }
    }
    
    // Failed to find, so create it...
    auto container = rsrc::file::type_container::create(type_code);
    m_containers.push_back(container);
    return container;
}


// MARK: - Resource

rsrc::file::resource::resource(int64_t id, const std::string name, rsrc::data blob)
    : m_id(id), m_name(name), m_blob(blob)
{
    
}

std::shared_ptr<rsrc::file::resource> rsrc::file::resource::create(int64_t id, const std::string name, rsrc::data blob)
{
    return std::make_shared<rsrc::file::resource>(id, name, blob);
}

int64_t rsrc::file::resource::id() const
{
    return m_id;
}

std::string rsrc::file::resource::name() const
{
    return m_name;
}

rsrc::data rsrc::file::resource::blob() const
{
    return m_blob;
}

void rsrc::file::resource::set_data_offset(uint64_t offset)
{
    m_data_offset = offset;
}

uint64_t rsrc::file::resource::data_offset() const
{
    return m_data_offset;
}

// MARK: - Type Container

rsrc::file::type_container::type_container(const std::string type_code)
    : m_code(type_code)
{
    
}

std::shared_ptr<rsrc::file::type_container> rsrc::file::type_container::create(const std::string type_code)
{
    return std::make_shared<rsrc::file::type_container>(type_code);
}

void rsrc::file::type_container::add_resource(std::shared_ptr<rsrc::file::resource> resource)
{
    m_resources.push_back(resource);
}

std::string rsrc::file::type_container::type_code() const
{
    return m_code;
}

std::vector<std::shared_ptr<rsrc::file::resource>> rsrc::file::type_container::resources() const
{
    return m_resources;
}

// MARK: - File Writing

void rsrc::file::write()
{
    rsrc::data fork_data;
    
    switch (m_format) {
        case file::format::standard: {
            fork_data = write_standard();
            break;
        }
        case file::format::extended: {
            fork_data = write_extended();
            break;
        }
        case file::format::rez: {
            fork_data = write_rez();
            break;
        }
    }
    
    fork_data.save(m_path);
}

// MARK: - Private Write Format Specific Methods

rsrc::data rsrc::file::write_extended()
{
    rsrc::data fork_data;
    
    // Write the initial preamble and header to the data.
    // Certain aspects of this are not currently known and thus need to be written later.
    uint64_t data_offset = 256;
    uint64_t data_length = 0;
    uint64_t map_offset = 0;
    uint64_t map_length = 0;
    
    fork_data.write_quad(1);
    fork_data.write_quad(data_offset);
    fork_data.write_quad(map_offset);
    fork_data.write_quad(data_length);
    fork_data.write_quad(map_length);
    fork_data.pad_to_size(256);
    
    // Iterate through all of the resources and write their data blobs to the file data.
    // When doing this we need to record the starting points of each resources data, as
    // they will be needing later.
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            // Get the data for the resource and determine its size.
            auto data = resource->blob();
            auto size = data.size();
            resource->set_data_offset(fork_data.size());
            fork_data.write_quad(size);
            fork_data.write_data(data);
        }
    }
    
    // Start writing the ResourceMap. This consists of several characteristics,
    // The first of which is a secondary preamble. We can now calculate the map_offset and
    // the data_length, but we're still waiting on the map_length. For now, write these values
    // as zeros.
    map_offset = fork_data.size();
    data_length = map_offset - data_offset;
    
    // TODO: Complete the extended resource file save method
    
    return fork_data;
}

rsrc::data rsrc::file::write_standard()
{
    rsrc::data fork_data;
    
    // Write the initial preamble and header to the data.
    // Certain aspects of this are not currently known and thus need to be written later.
    uint32_t data_offset = 256;
    uint32_t data_length = 0;
    uint32_t map_offset = 0;
    uint32_t map_length = 0;
    
    fork_data.write_long(data_offset);
    fork_data.write_long(map_offset);
    fork_data.write_long(data_length);
    fork_data.write_long(map_length);
    fork_data.pad_to_size(256);
    
    // Iterate through all of the resources and write their data blobs to the file data.
    // When doing this we need to record the starting points of each resources data, as
    
    uint16_t resource_count = 0;
    for (auto container : m_containers) {
        resource_count += container->resources().size();
        
        for (auto resource : container->resources()) {
            // Get the data for the resource and determine its size.
            auto data = resource->blob();
            auto size = data.size();
            resource->set_data_offset(fork_data.size() - data_offset);
            fork_data.write_long(static_cast<uint32_t>(size));
            fork_data.write_data(data);
        }
    }
    
    // Start writing the ResourceMap. This consists of several characteristics,
    // The first of which is a secondary preamble. We can now calculate the map_offset and
    // the data_length, but we're still waiting on the map_length. For now, write these values
    // as zeros.
    map_offset = static_cast<uint32_t>(fork_data.size());
    data_length = map_offset - data_offset;
    
    fork_data.write_long(data_offset);
    fork_data.write_long(map_offset);
    fork_data.write_long(data_length);
    fork_data.write_long(map_length);
    
    // The next six bytes are used by the MacOS ResourceManager and thus not important to
    // us.
    fork_data.write_byte(0x00, 6);
    
    // We're now writing the primary map information, which includes flags, and offsets for
    // the type list and the name list. We can calculate where each of these will be.
    const uint16_t resource_type_length = 8;
    const uint16_t resource_length = 20;
    uint16_t type_list_offset = 28; // The type list is 28 bytes from the start of the resource map.
    uint16_t name_list_offset = (m_containers.size() * resource_type_length) + (resource_count * resource_length) + sizeof(uint16_t);
    
    fork_data.write_word(0x0000);
    fork_data.write_word(type_list_offset);
    fork_data.write_word(name_list_offset);
    
    // Now moving on to actually writing each of the type descriptors into the data.
    uint16_t resource_offset = sizeof(uint16_t) + (m_containers.size() * resource_type_length);
    fork_data.write_word(m_containers.size() - 1);
    for (auto container : m_containers) {
        // We need to ensure that the type code is 4 characters -- otherwise this file will be
        // massively corrupt when produced.
        auto mac_roman = rsrc::mac_roman::from_str(container->type_code());
        if (mac_roman.bytes().size() != 4) {
            throw std::runtime_error("Attempted to write invalid type code to Resource File '" + container->type_code() + "'");
        }
        fork_data.write_cstr(container->type_code(), 4);
        fork_data.write_word(container->resources().size() - 1);
        fork_data.write_word(resource_offset);
        
        resource_offset += container->resources().size() * resource_length;
    }
    
    // Now we're writing the actual resource headers.
    uint16_t name_offset = 0;
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            
            fork_data.write_signed_word(static_cast<int16_t>(resource->id()));
            
            // The name is actually stored in the name list, and the resource stores an offset
            // to that name. If no name is assigned to the resource then the offset is encoded as
            // 0xFFFF.
            if (resource->name().empty()) {
                fork_data.write_word(0xFFFF);
            }
            else {
                // Convert the name to MacRoman so that we can get the length of it when encoded.
                auto mac_roman = rsrc::mac_roman::from_str(resource->name());
                std::vector<uint8_t> bytes = mac_roman.bytes();
                uint16_t len = bytes.size();
                
                fork_data.write_word(name_offset);
                name_offset += (len >= 0x100 ? 0xFF : len) + 1;
            }
            
            // Write the resource attributes - these are currently hard coded as nothing.
            fork_data.write_byte(0x00);
            
            // The data offset is a 3 byte (24-bit) value. This means the hi-byte needs discarding
            // and then a swap performing.
            auto offset = static_cast<uint32_t>(resource->data_offset());
            fork_data.write_byte((offset >> 16) & 0xFF);
            fork_data.write_byte((offset >>  8) & 0xFF);
            fork_data.write_byte((offset >>  0) & 0xFF);
            
            // Finally this is a reserved field for use by the ResourceManager.
            fork_data.write_long(0x00000000);
            
        }
    }
    
    // Finally we write out each of the resource names, and calculate the map length.
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            if (resource->name().empty()) {
                continue;
            }
            
            auto mac_roman = rsrc::mac_roman::from_str(resource->name());
            std::vector<uint8_t> bytes = mac_roman.bytes();
            auto len = bytes.size();
            len = len >= 0x100 ? 0xFF : len;
            
            fork_data.write_byte(static_cast<uint8_t>(len));
            fork_data.write_data(bytes);
        }
    }
    map_length = static_cast<uint16_t>(fork_data.size() - map_offset);

    // Fix the preamble values.
    fork_data.set_insertion_point(0);
    fork_data.write_long(data_offset);
    fork_data.write_long(map_offset);
    fork_data.write_long(data_length);
    fork_data.write_long(map_length);
    
    fork_data.set_insertion_point(map_offset);
    fork_data.write_long(data_offset);
    fork_data.write_long(map_offset);
    fork_data.write_long(data_length);
    fork_data.write_long(map_length);
    
    return fork_data;
}

rsrc::data rsrc::file::write_rez()
{
    rsrc::data fork_data = data(data::endian::little);
    
    const std::string rez_signature = "BRGR";
    const std::string map_name = "resource.map";
    const uint32_t rez_version = 1;
    const uint32_t resource_offset_length = 12;
    const uint32_t map_header_length = 8;
    const uint32_t type_info_length = 12;
    const uint32_t resource_info_length = 266;
    
    // Count up the total number of resources
    uint32_t resource_count = 0;
    for (auto container : m_containers) {
        resource_count += container->resources().size();
    }
    
    // The resource map itself is considered an entry for the offsets in the header
    uint32_t entry_count = resource_count + 1;
    
    // Calculate header length - this is from the end of the preamble to the start of the resource data
    uint32_t header_length = 12 + (entry_count * resource_offset_length) + map_name.size()+1;
    
    // Write the preamble
    fork_data.write_cstr(rez_signature, 4);
    fork_data.write_long(rez_version);
    fork_data.write_long(header_length);
    
    // Calculate the offset to the first resource data
    uint32_t resource_offset = fork_data.size() + header_length;
    
    // Write the header
    uint32_t index = 1; // Index of first resource, starting at 1
    fork_data.write_long(1); // Unknown value
    fork_data.write_long(index);
    fork_data.write_long(entry_count);
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            // Get the data for the resource and determine its size.
            auto data = resource->blob();
            auto size = data.size();
            fork_data.write_long(resource_offset);
            fork_data.write_long(static_cast<uint32_t>(size));
            fork_data.write_long(0); // Unknown value
            resource_offset += size;
        }
    }
    
    uint32_t type_count = m_containers.size();
    // Calculate offset within map to start of resource info
    uint32_t type_offset = map_header_length + (type_count * type_info_length);
    uint32_t map_length = type_offset + (resource_count * resource_info_length);
    // Write the offset and size of the resource map
    fork_data.write_long(resource_offset);
    fork_data.write_long(map_length);
    fork_data.write_long(12 + (entry_count * resource_offset_length)); // Unknown value

    // Write the name of the resource map
    fork_data.write_cstr(map_name);
    
    // Write each resource
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            fork_data.write_data(resource->blob());
        }
    }
    
    // Write the resource map as big endian
    // Map header
    fork_data.set_endian(data::endian::big);
    fork_data.write_long(8); // Unknown value
    fork_data.write_long(type_count);
    
    // Type counts and offsets
    for (auto container : m_containers) {
        auto count = container->resources().size();
        fork_data.write_cstr(container->type_code(), 4);
        fork_data.write_long(type_offset);
        fork_data.write_long(count);
        type_offset += resource_info_length * count;
    }
    
    // Info for each resource
    for (auto container : m_containers) {
        for (auto resource : container->resources()) {
            fork_data.write_long(index++);
            fork_data.write_cstr(container->type_code(), 4);
            fork_data.write_signed_word(static_cast<int16_t>(resource->id()));
            fork_data.write_cstr(resource->name(), 256);
        }
    }
    
    return fork_data;
}

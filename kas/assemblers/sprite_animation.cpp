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

#include <iostream>
#include "assemblers/sprite_animation.hpp"

// MARK: - Assembly

rsrc::data kdk::sprite_animation::assemble()
{
    rsrc::data data;
    
    // FIELD 1: Sprites ID
    int64_t sprites_id = 0;
    if (field_exists("sprites", { kdk::resource::field::value_type::resource_id })) {
        sprites_id = resource_id_for("sprites");
    }
    else if (field_exists("sprites", { kdk::resource::field::value_type::file_reference })) {
        auto resource = import_and_resolve_file("sprites");
        sprites_id = resource->id();
    }
    else {
        throw std::runtime_error("Missing 'sprite' field on 'SpriteAnimation'. It must be either a Resource ID or File Reference.");
    }
    
    // FIELD 2: Masks ID - This is largely deprecated by Kestrel and is used for legacy purposes.
    // Warn about it, if it exists.
    if (field_exists("masks")) {
        std::cout << "Use of the 'masks' field in 'SpriteAnimation' is deprecated and is present for legacy data files." << std::endl;
        std::cout << "Please use alpha channels on TGA or PNG files instead." << std::endl;
    }
    
    // FIELD 3: Sizes
    int16_t x_size = 0;
    int16_t y_size = 0;
    if (field_exists("size", { kdk::resource::field::value_type::integer, kdk::resource::field::value_type::integer })) {
        x_size = static_cast<int16_t>(integer_for("size", 0));
        y_size = static_cast<int16_t>(integer_for("size", 1));
    }
    else {
        throw std::runtime_error("The 'size' field is required for 'SpriteAnimation'");
    }
    
    // FIELD 4: Tile Counts - This involves knowing the size of the sprites image, and calculating
    // based on the size of sprites. For this to work we need to partially load the sprites image.
    // If the counts are explicitly supplied, then loading the sprites is not required.
    int16_t x_tiles = 0;
    int16_t y_tiles = 0;
    if (field_exists("tiles", { kdk::resource::field::value_type::integer, kdk::resource::field::value_type::integer })) {
        x_tiles = static_cast<int16_t>(integer_for("tiles", 0));
        y_tiles = static_cast<int16_t>(integer_for("tiles", 1));
    }
    else {
        // TODO: Determine the tile counts based on the size of the sprites image.
        throw std::runtime_error("The 'tiles' field is currently required for 'SpriteAnimation', as it is not yet deduced.");
    }
    
    // Build the binary data blob for the SpriteAnimation resource.
    data.write_signed_word(sprites_id);
    data.write_signed_word(-1);
    data.write_signed_word(x_size);
    data.write_signed_word(y_size);
    data.write_signed_word(x_tiles);
    data.write_signed_word(y_tiles);
    
    return data;
}

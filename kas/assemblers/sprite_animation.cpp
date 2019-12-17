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

#include "assemblers/sprite_animation.hpp"

// MARK: - Assembly

std::shared_ptr<kdk::assembler> kdk::sprite_animation_assembler()
{
    auto assembler = std::make_shared<kdk::assembler>();
    
    assembler->add_field(
        kdk::assembler::field::named("sprites").set_required(true).set_values({
            kdk::assembler::field::value::expect("sprite_id", kdk::assembler::field::value::type::resource_reference, 0, 2)
        })
    );

    assembler->add_field(
        // The 'masks' field is intended to be deprecated by Kestrel and thus should be warned about.
        kdk::assembler::field::named("masks").set_required(false).set_deprecated(true).set_values({
            kdk::assembler::field::value::expect("mask_id", kdk::assembler::field::value::type::resource_reference, 2, 2)
        })
    );
    
    assembler->add_field(
        kdk::assembler::field::named("size").set_required(true).set_values({
            kdk::assembler::field::value::expect("width", kdk::assembler::field::value::type::integer, 4, 2),
            kdk::assembler::field::value::expect("height", kdk::assembler::field::value::type::integer, 6, 2),
        })
    );
    
    assembler->add_field(
        kdk::assembler::field::named("tiles").set_required(true).set_values({
            kdk::assembler::field::value::expect("x_count", kdk::assembler::field::value::type::integer, 8, 2),
            kdk::assembler::field::value::expect("y_count", kdk::assembler::field::value::type::integer, 10, 2),
        })
    );
    
    return assembler;
}

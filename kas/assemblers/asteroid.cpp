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

#include "assemblers/asteroid.hpp"

// MARK: - Assembly

rsrc::data kdk::asteroid::assemble()
{
    // Handle each of the fields in the resource.
    assembler::assemble(
        kdk::assembler::field::named("strength").set_required(true).set_values({
            kdk::assembler::field::value::expect("value", kdk::assembler::field::value::type::integer, 0, 2),
        })
    );
    
    assembler::assemble(
        kdk::assembler::field::named("spin_rate").set_values({
            kdk::assembler::field::value::expect("value", kdk::assembler::field::value::type::integer, 2, 2)
                .set_symbols({
                    std::make_tuple("very_slow", 10),
                    std::make_tuple("slow", 30),
                    std::make_tuple("medium", 50),
                    std::make_tuple("fast", 70),
                    std::make_tuple("very_fast", 100)
                })
                .set_default_value([] (rsrc::data& data) {
                    data.write_signed_word(50);
                }),
        })
    );
    
    assembler::assemble(
        kdk::assembler::field::named("yield").set_required(true).set_values({
            kdk::assembler::field::value::expect("type", kdk::assembler::field::value::type::resource_reference, 4, 2)
                .set_symbols({
                    std::make_tuple("food", 0),
                    std::make_tuple("industrial", 1),
                    std::make_tuple("medical", 2),
                    std::make_tuple("luxury", 3),
                    std::make_tuple("metal", 4),
                    std::make_tuple("equipment", 5),
                }),
            kdk::assembler::field::value::expect("quantity", kdk::assembler::field::value::type::integer, 6, 2)
        })
    );
    
    assembler::assemble(
		kdk::assembler::field::named("particles").set_required(false).set_values({
            kdk::assembler::field::value::expect("count", kdk::assembler::field::value::type::integer, 8, 2)
                .set_default_value([] (rsrc::data& data) {
                    data.write_signed_word(5);
                }),
            kdk::assembler::field::value::expect("color", kdk::assembler::field::value::type::color, 10, 4)
                .set_default_value([] (rsrc::data& data) {
                    data.write_long(0x00FFFFFF);
                }),
        })
	);
    
    // Finish assembly and return the result to the caller.
    return assembler::assemble();
}

// MARK: - Fields



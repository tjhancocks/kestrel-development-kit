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

#if !defined(KDL_LEXER)
#define KDL_LEXER

namespace kdl
{
  
/**
 * The KDL Lexical Analyser (Lexer) is responsible for taking raw textual content
 * and splitting it up into a stream of tokens (Lexemes) that have atomicity and
 * meaning.
 */
class lexer
{
public:
    
    /**
     * Represents an individual token extracted from the raw textual content.
     */
    struct token
    {
    public:
        
        /**
         * Denotes the type of information that a token is representing.
         */
        enum type
        {
            unknown, identifier, resource_id, string, integer, percentage,
            lbrace, rbrace, lparen, rparen, langle, rangle, lbracket, rbracket,
            plus, minus, star, slash, pipe, ampersand, equals, colon, semicolon,
            dot, comma, caret
        };
        
    public:
        /**
         * Construct a NULL token.
         */
        token();
        
        /**
         * Construct a new token.
         */
        token(const int line, const int offset, const std::string text, token::type type);
        
        /**
         * Returns the line number of the original source file in which the token
         * originated from.
         */
        int line() const;
        
        /**
         * Returns the offset in the line where the token started from.
         */
        int offset() const;
        
        /**
         * Returns the textual representation of the token.
         */
        std::string text() const;
        
        /**
         * Test if the type of the token matches the specified type.
         */
        bool is_a(token::type type) const;
        
    private:
        int m_line;
        int m_offset;
        std::string m_text;
        token::type m_type;
    };
    
public:
    /**
     * Construct a new lexical analyser using the source code provided.
     */
    lexer(const std::string& source);
    
private:
    std::string::size_type m_pos;
    std::string::size_type m_length;
    std::string m_source;
    std::vector<token> m_tokens;
};

};

#endif

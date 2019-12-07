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
#include <type_traits>
#include <memory>

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
            plus, minus, star, slash, pipe, ampersand, equals, colon, dot, comma,
            caret, directive, semi_colon
        };
        
    public:
        /**
         * Construct a NULL token.
         */
        token();
        
        /**
         * Construct a new token.
         */
        token(const std::string file, const int line, const int offset, const std::string text, token::type type);
        
        /**
         * Returns the name of the file where the token was located.
         */
        std::string file() const;
        
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
        std::string m_file;
    };
    
public:
    /**
     * Construct a new lexical analyser using the source code provided.
     */
    lexer(const std::string path, const std::string& source);
    
    /**
     * Create a new lexer, using the contents of the specified file as the source.
     */
    static kdl::lexer open_file(const std::string path);
    
    /**
     * Perform the lexical analysis.
     *
     * This method is responsible for kicking off the main lexical analysis task
     * and running it. It progressively steps through the source, extracting tokens
     * as its finds them.
     *
     * It does no direct semantic checking, but does throw exceptions when it is
     * unable to infer types, or doesn't recognise symbols.
     *
     * \return A token stream of all tokens found.
     */
    std::vector<kdl::lexer::token> analyze();
    
    /**
     * Test if there is any more characters in source to parse.
     */
    bool available(long offset = 0, std::string::size_type size = 1) const;
    
    /**
     * Advance the lexer forward by the specified number of characters.
     *
     * \note It is safe to advance forward beyond the end of the source with this method,
     * as no content needs to be accessed.
     */
    void advance(long offset = 1);
    
    /**
     * Peek a string/character from the source.
     *
     * This peek happens at the specified offset for the specified number of characters.
     * A check is performed to see if the peek is valid. If it is not an exception
     * will be raised.
     *
     * This operation does not advance the internal pointer to the source.
     */
    std::string peek(long offset = 0, std::string::size_type size = 1) const;
    
    /**
     * Read a string/character from the source.
     *
     * This read happens at the specified offset for the specified number of characters.
     * A check is performed to see if the read is valid. If it is not an exception
     * will be raised.
     *
     * This operation does advance the internal pointer to the source.
     */
    std::string read(long offset = 0, std::string::size_type size = 1);
    
    /**
     * Test if the content pointed to can be validated by the provided function.
     */
    bool test_if(std::function<bool(const std::string)> testFn, long offset = 0, std::string::size_type size = 1) const;
    
    /**
     * Keep consuming characters whilst the test function evaluates true.
     *
     * Upon completion the `m_slice` member will have been updated. If the function failed
     * then m_slice will be empty, otherwise it will contain the matched content.
     */
    bool consume_while(std::function<bool(const std::string)> testFn);
    
private:
    int m_line;
    std::string::size_type m_pos;
    std::string::size_type m_length;
    std::string m_source;
    std::vector<token> m_tokens;
    std::string m_slice;
    std::string m_path;
};

// MARK: - Test Functions

/**
 * Template function for testing if a chunk matches the specified character.
 *
 * The `test_if` function of the kdl::lexer will pass a std::string reference to the template
 * function to be tested against the value given. This is only capable of testing single
 * characters, not entire strings.
 *
 * \example
 *  test_if(match<'='>::yes)
 */
template <char c>
struct match {
    static bool yes(const std::string);
    static bool no(const std::string);
};

template <char lC, char uC>
struct range {
    static bool contains(const std::string);
    static bool not_contains(const std::string);
};


/**
 * Template function for testing if all characters of a chunk exist within a specified set
 * of characters.
 *
 * The `test_if` function of the kdl::lexer will pass a std::string reference to the template
 * function to be tested against the value given.
 *
 * \example
 *  test_if(set<' ', '\t'>::contains)
 */
template<char tC, char... ttC>
struct set {
    static bool contains(const std::string);
    static bool not_contains(const std::string);
};

/**
 * Specialised test function for testing identifier characters.
 */
struct identifier_set {
    static bool contains(const std::string);
};

/**
 * Specialised test function for testing number characters.
 */
struct number_set {
    static bool contains(const std::string);
};


};

#endif

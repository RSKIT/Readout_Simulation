#ifndef TINYXML2_ADDON_H
#define TINYXML2_ADDON_H

/**
 * @brief returns the first node with the provided name
 * @details searches the child elements of `parent` for a node with name/value `value` starting 
 *             from `start` where `start` is a child element of parent. If it is not, the search
 *             starts with the first child element of `parent`
 * 
 * @param parent             - node of which the child elements are to be searched
 * @param value              - name/value of the child element to find
 * @param start              - first element to consider in the search (a child element of `parent`
 * 									or it will be treated as not passed - null pointer)
 * @return                   - the first child element found with the name `value`
 */
tinyxml2::XMLNode* getNode(tinyxml2::XMLNode* parent, std::string value, tinyxml2::XMLNode* start = 0);

/**
 * @brief replaces the characters for putting it into an XML file
 * @details the escaped characters (\",\',&,<,>) are replaced by sequences allowed in XML strings
 * 
 * @param text               - the text to convert
 * @return                   - the text without characters to be escaped
 */
std::string escapeString(std::string text);

/**
 * @brief replaced the escape sequences by the characters themselves
 * @details &quot; &apos; &amp; &gt; &lt; are replaced by \" \' & > <
 * 
 * @param escapedtext        - the text containing the escape sequences
 * @return                   - a string with the escape sequences replaced by the characters
 */
std::string unescapeString(std::string escapedtext);

/**
 * @brief replaced the escape sequences by the characters themselves
 * @details &quot; &apos; &amp; &gt; &lt; are replaced by \" \' & > <
 * 
 * @param escapedtext        - the text containing the escape sequences
 * @return                   - a string with the escape sequences replaced by the characters
 */
std::string unescapeStringCheck(const char* escapedtext);

/**
 * @brief converts the char* into a std::string
 * @details it also prevents errors due to empty strings (a null pointer is converted to "")
 * 
 * @param text               - the char* text to convert
 * @return                   - a std::string representing the char*'s contents
 */
std::string checkString(const char* text);

#endif //TINYXML2_ADDON_H
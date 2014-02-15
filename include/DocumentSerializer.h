/*
 * DocumentSerializer.h
 *
 *  Created on: Feb 14, 2014
 *      Author: trananh
 */

#ifndef DOCUMENT_SERIALIZER_H_
#define DOCUMENT_SERIALIZER_H_

#include <iostream>
#include <string>
#include <vector>

#include "Document.h"

using namespace std;

namespace processors {

/**
 * Saves/loads a Document to/from a stream
 * An important focus here is to minimize the size of the serialized Document.
 * For this reason, we use a custom (compact) text format, rather than XML.
 *
 * This class mirrors the Scala implementation from sistanlp's processors.
 */
class DocumentSerializer {

public:

    /**
     * Load the NLP annotation from an input stream (file or string).
     * 	@param stream The input stream.
     */
    static Document load(istream &stream);

    /**
     * Save the NLP annotation to an output stream (file or string).
     * 	@param doc The annotated document.
     * 	@param out The output stream.
     */
    static void save(Document doc, ostream &out);

    // Useful string constants
    static const char NIL;
    static const char SEP;
    static const string START_SENTENCES;
    static const string START_TOKENS;
    static const string START_COREF;
    static const string START_DEPENDENCIES;
    static const string START_CONSTITUENTS;
    static const string END_OF_SENTENCE;
    static const string END_OF_DOCUMENT;
    static const string END_OF_DEPENDENCIES;

private:

    /**
     * Tokenize a line.
     * 	@param line The string to tokenize.
     * 	@return A vector of tokens.
     */
    static vector<string> tokenize(const string &line);

    /**
     * Loads the annotation for the next sentence in the stream.
     * 	@param stream The input stream.
     * 	@return A sentence containing the annotations.
     */
    static Sentence loadSentence(istream &stream);

    /**
     * Print the NLP annotation for the sentence to an output stream.
     * 	@param doc The annotated sentence.
     * 	@param out The output stream.
     */
    static void saveSentence(Sentence sentence, ostream &out);

    /**
     * Print the NLP annotation for the token to an output stream.
     * 	@param doc The annotated sentence.
     * 	@param offset The token offset in the sentence.
     * 	@param out The output stream.
     */
    static void saveToken(Sentence sentence, int offset, ostream &out);

};

}

#endif /* DOCUMENT_SERIALIZER_H_ */

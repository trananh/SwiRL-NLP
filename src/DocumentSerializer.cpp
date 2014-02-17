/*
 * DocumentSerializer.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: trananh
 */

#include <sstream>
#include <assert.h>

#include "DocumentSerializer.h"

using namespace std;
using namespace processors;

/* Useful string constants */
const char DocumentSerializer::NIL = '_';
const char DocumentSerializer::SEP = '\t';
const string DocumentSerializer::START_SENTENCES = "S";
const string DocumentSerializer::START_TOKENS = "T";
const string DocumentSerializer::START_COREF = "C";
const string DocumentSerializer::START_DEPENDENCIES = "D";
const string DocumentSerializer::START_CONSTITUENTS = "Y";
const string DocumentSerializer::END_OF_SENTENCE = "EOS";
const string DocumentSerializer::END_OF_DOCUMENT = "EOD";
const string DocumentSerializer::END_OF_DEPENDENCIES = "EOX";

/**
 * Load the NLP annotation from an input stream (file or stringstream).
 * 	@param stream The input stream.
 */
Document DocumentSerializer::load(istream &stream) {
    // First line from the stream should be start of sentences
    string line;
    getline(stream, line);
    vector<string> tokens = tokenize(line);
    assert(tokens.at(0).compare(START_SENTENCES) == 0);
    int sentCount = atoi(tokens.at(1).c_str());
    vector<Sentence> sentences;
    int offset = 0;
    while (offset < sentCount) {
        sentences.push_back(loadSentence(stream));
        offset += 1;
    }
    Document doc(sentences);
    return doc;
}

/**
 * Print the NLP annotation to an output stream (file or string).
 * 	@param doc The annotated document.
 * 	@param out The output stream.
 * 	@return The output stream.
 */
void DocumentSerializer::save(Document doc, ostream &out) {
    out << START_SENTENCES << SEP << doc.sentences.size() << endl;
    for (int i = 0; i < (int) doc.sentences.size(); i++) {
        saveSentence(doc.sentences.at(i), out);
    }
    out << END_OF_DOCUMENT << endl;
}

/**
 * Tokenize a line.
 * 	@param line The string to tokenize.
 * 	@return A vector of tokens.
 */
vector<string> DocumentSerializer::tokenize(const string &line) {
    vector<string> tokens;
    string token;
    stringstream stream(line);
    while (getline(stream, token, SEP)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * Loads the annotation for the next sentence in the stream.
 * 	@param stream The input stream.
 * 	@return A sentence containing the annotations.
 */
Sentence DocumentSerializer::loadSentence(istream &stream) {
    // First line should be the start of tokens
    string line;
    getline(stream, line);
    vector<string> tokens = tokenize(line);
    assert(tokens.at(0).compare(START_TOKENS) == 0);
    int tokenCount = atoi(tokens.at(1).c_str());

    // Initialize the vectors to hold the annotations
    vector<string> wordBuffer;
    vector<int> startOffsetBuffer;
    vector<int> endOffsetBuffer;
    vector<string> tagBuffer;
    bool nilTags = true;
    vector<string> lemmaBuffer;
    bool nilLemmas = true;
    vector<string> entityBuffer;
    bool nilEntities = true;
    vector<string> normBuffer;
    bool nilNorms = true;
    vector<string> chunkBuffer;
    bool nilChunks = true;

    // Each line = a token in the sentence
    int offset = 0;
    while (offset < tokenCount) {
        getline(stream, line);
        tokens = tokenize(line);

        // We expect 8 different annotations for each token
        assert(tokens.size() == 8);

        wordBuffer.push_back(tokens.at(0));
        startOffsetBuffer.push_back(atoi(tokens.at(1).c_str()));
        endOffsetBuffer.push_back(atoi(tokens.at(2).c_str()));

        tagBuffer.push_back(tokens.at(3));
        if (tokens.at(3).compare(&NIL) != 0)
            nilTags = false;
        lemmaBuffer.push_back(tokens.at(4));
        if (tokens.at(4).compare(&NIL) != 0)
            nilLemmas = false;
        entityBuffer.push_back(tokens.at(5));
        if (tokens.at(5).compare(&NIL) != 0)
            nilEntities = false;
        normBuffer.push_back(tokens.at(6));
        if (tokens.at(6).compare(&NIL) != 0)
            nilNorms = false;
        chunkBuffer.push_back(tokens.at(7));
        if (tokens.at(7).compare(&NIL) != 0)
            nilChunks = false;

        offset += 1;
    }
    assert((int) wordBuffer.size() == tokenCount);
    assert((int) startOffsetBuffer.size() == tokenCount);
    assert((int) endOffsetBuffer.size() == tokenCount);
    assert((int) tagBuffer.size() == 0 || (int) tagBuffer.size() == tokenCount);
    assert((int) lemmaBuffer.size() == 0
                    || (int) lemmaBuffer.size() == tokenCount);
    assert((int) entityBuffer.size() == 0
                    || (int) entityBuffer.size() == tokenCount);
    assert((int) normBuffer.size() == 0
                    || (int) normBuffer.size() == tokenCount);
    assert((int) chunkBuffer.size() == 0
                    || (int) chunkBuffer.size() == tokenCount);

    // Skip the rest of the information that we currently aren't processing
    // (such as constituent trees and coref chains).
    do {
        getline(stream, line);
        tokens = tokenize(line);
        if (tokens.at(0).compare(START_DEPENDENCIES) == 0) {
            do { // Eat up dependencies
                getline(stream, line);
                tokens = tokenize(line);
            } while (tokens.at(0).compare(END_OF_DEPENDENCIES) != 0);
        } else if (tokens.at(0).compare(START_CONSTITUENTS) == 0) {
            do { // Eat up constituents
                getline(stream, line);
                tokens = tokenize(line);
            } while (tokens.at(0).compare(END_OF_SENTENCE) != 0);
        }
    } while (tokens.at(0).compare(END_OF_SENTENCE) != 0);

    // Now let's create a sentence and return it
    vector<string> empty;
    Sentence sent(wordBuffer, startOffsetBuffer, endOffsetBuffer,
            (nilTags) ? empty : tagBuffer,
            (nilLemmas) ? empty : lemmaBuffer,
            (nilEntities) ? empty : entityBuffer,
            (nilNorms) ? empty : normBuffer, (nilChunks) ? empty : chunkBuffer);

    return sent;
}

/**
 * Print the NLP annotation for the sentence to an output stream.
 * 	@param doc The annotated sentence.
 * 	@param out The output stream.
 */
void DocumentSerializer::saveSentence(Sentence sentence, ostream &out) {
    out << START_TOKENS + SEP << sentence.size() << endl;
    int offset = 0;
    while (offset < sentence.size()) {
        saveToken(sentence, offset, out);
        offset += 1;
    }
    out << END_OF_SENTENCE << endl;
}

/**
 * Print the NLP annotation for the token to an output stream.
 * 	@param doc The annotated sentence.
 * 	@param offset The token offset in the sentence.
 * 	@param out The output stream.
 */
void DocumentSerializer::saveToken(Sentence sentence, int offset,
        ostream &out) {
    out << sentence.words.at(offset) + SEP << sentence.startOffsets.at(offset)
            << SEP << sentence.endOffsets.at(offset);

    out << SEP;
    if (sentence.tags.size() > 0)
        out << sentence.tags.at(offset);
    else
        out << NIL;

    out << SEP;
    if (sentence.lemmas.size() > 0)
        out << sentence.lemmas.at(offset);
    else
        out << NIL;

    out << SEP;
    if (sentence.entities.size() > 0)
        out << sentence.entities.at(offset);
    else
        out << NIL;

    out << SEP;
    if (sentence.norms.size() > 0)
        out << sentence.norms.at(offset);
    else
        out << NIL;

    out << SEP;
    if (sentence.chunks.size() > 0)
        out << sentence.chunks.at(offset);
    else
        out << NIL;

    out << endl;
}

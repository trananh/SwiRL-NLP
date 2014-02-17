/*
 * Document.h
 *
 *  Created on: Feb 14, 2014
 *      Author: trananh
 */

#ifndef PROCESSORS_DOCUMENT_H_
#define PROCESSORS_DOCUMENT_H_

#include <string>
#include <vector>

#include <assert.h>

using namespace std;

namespace processors {

/**
 * Stores the annotations for a single sentence.
 * This class mirrors the Scala implementation from sistanlp's processors.
 */
class Sentence {

public:

    /**
     * Constructor.
     * 	@param tokens Actual tokens in this sentence
     * 	@param startOff Start character offsets for the words; start at 0
     * 	@param endOff End character offsets for the words; start at 0
     * 	@param pos POS tags for words
     * 	@param lem Lemmas
     * 	@param en Named entity labels
     * 	@param normVals Normalized values of named/numeric entities, such as dates
     * 	@param chunkLabels Shallow parsing labels
     */
    Sentence(const vector<string> & tokens, const vector<int> & startOff,
            const vector<int> & endOff, const vector<string> & pos,
            const vector<string> & lem, const vector<string> & en,
            const vector<string> & normVals,
            const vector<string> & chunkLabels) :
            words(tokens), startOffsets(startOff), endOffsets(endOff),
            tags(pos), lemmas(lem), entities(en), norms(normVals),
            chunks(chunkLabels) {
    }

    // Public variables.
    vector<string> words;
    vector<int> startOffsets;
    vector<int> endOffsets;
    vector<string> tags;
    vector<string> lemmas;
    vector<string> entities;
    vector<string> norms;
    vector<string> chunks;

    // TODO: Future impelementation
    // Tree syntacticTree;  /* Constituent tree of this sent; incl head words */
    // DAG dependencies;    /* DAG of syntactic dependencies; offsets at 0 */

    /** Size of the sentence */
    int size() {
        return words.size();
    }

    /** Concatenated string tokens. */
    string getTokenizedText() {
        string text = "";
        for (unsigned int i = 0; i < words.size(); i++) {
            text = text + words.at(i);
            if (i < words.size() - 1) {
                text = text + string(" ");
            }
        }
        return text;
    }

    /** Concatenated string tokens with POS tags. */
    string getTokenizedTextWithTags() {
        assert(words.size() == tags.size());
        string text = "";
        for (unsigned int i = 0; i < words.size(); i++) {
            text = text + words.at(i) + string(" ") + tags.at(i);
            if (i < words.size() - 1) {
                text = text + string(" ");
            }
        }
        return text;
    }

    /** Concatenated string tokens with named entities. */
    string getTokenizedTextWithEntities() {
        assert(words.size() == entities.size());
        string text = "";
        for (unsigned int i = 0; i < words.size(); i++) {
            text = text + words.at(i) + string(" ") + entities.at(i);
            if (i < words.size() - 1) {
                text = text + string(" ");
            }
        }
        return text;
    }

    /** Concatenated string tokens with POS tags & entities. */
    string getTokenizedTextWithTagsEntities() {
        assert(words.size() == tags.size() && words.size() == entities.size());
        string text = "";
        for (unsigned int i = 0; i < words.size(); i++) {
            text = text + words.at(i) + string(" ") + tags.at(i)
                    + string(" ") + entities.at(i);
            if (i < words.size() - 1) {
                text = text + string(" ");
            }
        }
        return text;
    }

};

/**
 * Stores all annotations for one document
 * This class mirrors the Scala implementation from sistanlp's processors.
 */
class Document {

public:

    /**
     * Constructor.
     * 	@param sentences Sentences from the document.
     */
    Document(const vector<Sentence> & s) : sentences(s) { }

    // Public variables
    vector<Sentence> sentences;

    // TODO: Future implementation
    // CorefChains coreferenceChains;	/* Chains of coref mentions */
};

}

#endif /* PROCESSORS_DOCUMENT_H_ */

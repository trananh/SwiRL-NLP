/*
 * demo.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: trananh
 */

#include <iostream>
#include <fstream>
#include <string>
#include <dirent.h>
#include <sys/stat.h>

#include <Swirl.h>

#include "DocumentSerializer.h"

using namespace std;
using namespace processors;
using namespace srl;

/**
 * Check if a file exists.
 *  @param filename Name of the file.
 *  @return True if the file exists.
 */
bool exists(const string& filename) {
    struct stat buffer;
    return (stat(filename.c_str(), &buffer) == 0);
}

/**
 * Remove the trailing slash from a file path.
 *  @param path Path of a file or directory.
 *  @return The same path with any trailing slash removed.
 */
string removeTrailingSlash(const string& path) {
    char last = path[path.size() - 1];
    string newPath = path;
    if (last == '/') {
        newPath = path.substr(0, path.size() - 1);
    }
    return newPath;
}

/**
 * Escape any quotes in the string.
 *  @param before The string to escape.
 *  @return The same string with all " replaced with \".
 */
string escapeQuotes(const string &before) {
    string after;
    after.reserve(before.length() + 4);

    for (string::size_type i = 0; i < before.length(); ++i) {
        switch (before[i]) {
        case '"':
        case '\\':
            after += '\\';
            // Fall through.
        default:
            after += before[i];
        }
    }

    return after;
}

/**
 * Use Swirl to parse the NLP annotation from a single file.
 * Precondition: SwiRL must have been initialized.
 *  @param filename Path to file containing the NLP annotation.
 *  @param out The output stream to print role labels.
 */
void processFile(const string& filename, ostream& out = std::cout) {
    ifstream stream(filename.c_str());
    // Always check to see if file opening succeeded
    if (stream.is_open()) {

        // Load annotation
        Document doc = DocumentSerializer::load(stream);

        // For each sentence, parse with swirl
        TreePtr tree;
        string text, swirlCode;
        const char * line;
        out << (int) doc.sentences.size() << " sentences." << endl << endl;
        for (int i = 0; i < (int) doc.sentences.size(); i++) {
            // Form the sentence text with the necessary tags
            Sentence sentence = doc.sentences.at(i);
            if (sentence.entities.size() == sentence.words.size()) {
                if (sentence.tags.size() == sentence.words.size()) {
                    text = sentence.getTokenizedTextWithTagsEntities();
                    swirlCode = "1";
                } else {
                    text = sentence.getTokenizedTextWithEntities();
                    swirlCode = "2";
                }
            } else {
                text = sentence.getTokenizedText();
                swirlCode = "3";
            }

            // Escape quotes and add swirl parse code
            text = escapeQuotes(text);
            out << (int) sentence.words.size() << " tokens." << endl;
            out << text << endl << endl;
            text = swirlCode + string(" ") + text;
            line = text.c_str();

            // Classify all predicates in this sentence
            tree = Swirl::parse(line);

            // Dump extended Treebank format
            if (tree != (const Tree *) NULL) {
                tree->serialize(out);
                out << endl << endl;
            }

            // Dump extended CoNLL format
            Swirl::serialize(tree, line, out);
        }

        // Close the stream
		stream.close();

    } else {

        cerr << "Failed to find annotation file!\n";
        exit(1);

    }
}

/**
 * Swirl parse all NLP annotations contained in the given directory.
 * All NLP annotations are expected to be stored in txt files.
 * Precondition: SwiRL must have been initialized.
 *  @param directory The directory containing NLP annotation files.
 *  @param outdir The output directory.
 *  @param overwrite Overwrite all files in the output directory if True.
 *      Defaults to false.
 */
void processBatch(const string& directory, const string& outdir,
        bool overwrite = false) {

    if (!exists(directory) || !exists(outdir)) {
        cerr << "Failed to find directory!\n";
        exit(1);
    }

    // Remove trailing slash from dirs
    string inPath = removeTrailingSlash(directory);
    string outPath = removeTrailingSlash(outdir);

    int count = 0;
    dirent* pdir;
    string filename, infile, outfile;
    DIR* dir = opendir(inPath.c_str());

    // Iterate over all files in the input directory
    while ((pdir = readdir(dir)) != 0) {
        filename = string(pdir->d_name);
        infile = inPath + string("/") + filename;
        if (filename.substr(filename.find_last_of(".")) == ".txt") {
            // We only work with txt files
            outfile = outPath + string("/") + filename;
            if (overwrite || !exists(outfile)) {
                // Process the file if it doesn't already exist
                // in the output directory (or if overwrite is specified).
                cout << "Procesing new file: " << infile << endl;

                // Process nlp annotation and save swirl's output to file
                ofstream outstream;
                outstream.open(outfile.c_str());
                processFile(infile, outstream);
                outstream.close();

                // Keep acount of files processed
                count++;
            }
        }
    }

    cout << "Files processed: " << count << endl;
}

/**
 * Main entry point. A demo of how to use SwiRL to parse information
 * from an NLP annotation.
 *  @param argc Number of arguments.
 *  @param argv Command line arguments.
 */
int main(int argc, char ** argv) {

    /**
     * TODO: Expose command line arguments & remove hardcoded values!
     * For now, make sure you update the hardcoded paths below appropriately
     * before running the demo.
     */

    /* IMPORTANT: UPDATE THESE PATHS!
     * Paths to models.
     */
    string swirl = "./model_swirl";
    string charniak = "./model_charniak";

    /* IMPORTANT: UPDATE THESE PATHS!
     * Path to the directory containing NLP annotation (in batch mode) files,
     * and path to the output directory. One output file will be generated
     * for each input annotation file.
     */
    string path = "/path/to/NLP-annotations";
    string out = "/path/to/Swirl-output";

    // Initialize SwiRL
    bool caseSensitive = true;
    if (!Swirl::initialize(swirl.c_str(), charniak.c_str(), caseSensitive)) {
        cerr << "Failed to initialize SRL system!\n";
        exit(1);
    }

	// Process an entire directory of NLP annotations
    processBatch(path, out);

    return 0;
}

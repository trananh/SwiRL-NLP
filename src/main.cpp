/*
 * main.cpp
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
 * Swirl parse the NLP annotation from a single file.
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
		string text;
		string swirlCode;
		const char * line;
		out << (int)doc.sentences.size() << " sentences."<< endl << endl;
		for (int i = 0; i < (int)doc.sentences.size(); i++) {
            // form the sentence text with the necessary POS and entities
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

		    // escape quotes and add swirl parse code
		    text = escapeQuotes(text);
            out << (int)sentence.words.size() << " tokens." << endl;
            out << text << endl << endl;
            text = swirlCode + string(" ") + text;
            line = text.c_str();

		    // classify all predicates in this sentence
		    tree = Swirl::parse(line);

		    // dump extended Treebank format
		    if (tree != (const Tree *) NULL){
		      tree->serialize(out);
		      out << endl << endl;
		    }

		    // dump extended CoNLL format
		    Swirl::serialize(tree, line, out);
		}

		stream.close();

	} else {

		cerr << "Failed to find annotation file!\n";
		exit(1);

	}
}

/**
 * Swirl parse all NLP annotations contained in the given directory.
 * All NLP annotations are expected to be stored in txt files.
 *  @param directory The directory containing NLP annotation files.
 *  @param outdir The output directory.
 *  @param overwrite Overwrite all files in the output directory if True.
 *      Defaults to false.
 */
void processBatch(const string& directory,
        const string& outdir,
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
 * Main entry point.
 *  @param argc Number of arguments.
 *  @param argv Command line arguments.
 */
int main(int argc, char ** argv) {

    /** TODO: Expose command line arguments & remove hardcoded values. **/

	string swirl = "./model_swirl";
	string charniak = "./model_charniak";

	// Initialize Swirl
	bool caseSensitive = true;
	if (!Swirl::initialize(swirl.c_str(), charniak.c_str(), caseSensitive)) {
		cerr << "Failed to initialize SRL system!\n";
		exit(1);
	}

	/* Process a single NLP annotation */
//	string path = "/Users/trananh/Workspace/Data/text/corpora/Gigaword-chase-nlp/AFP_ENG_19940516.0024.txt";
//	string path = "/Users/trananh/Workspace/Data/text/corpora/Gigaword-chase-nlp/AFP_ENG_19940530.0183.txt";
//	processFile(path, cout);

	/* Process an entire directory of NLP annotations */
    string path = "/Users/trananh/Workspace/Data/text/corpora/Gigaword-chase-nlp";
    string out = "/Users/trananh/Workspace/Data/text/corpora/Gigaword-chase-swirl/";
    processBatch(path, out);

	return 0;
}

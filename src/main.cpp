/*
 * main.cpp
 *
 *  Created on: Feb 14, 2014
 *      Author: trananh
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "DocumentSerializer.h"

using namespace std;
using namespace processors;

int main(int argc, char ** argv) {

	if (argc != 2) // argc should be 2 for correct execution
		// We print argv[0] assuming it is the program name
		cout << "Usage: " << argv[0] << " <filename>\n";
	else {
		// We assume argv[1] is a filename to open
		ifstream stream(argv[1]);
		// Always check to see if file opening succeeded
		if (!stream.is_open())
			cout << "Could not open file\n";
		else {

			// Load the annotation
			Document doc = DocumentSerializer::load(stream);

			// Do something with the annotation
			stringstream ss;
			DocumentSerializer::save(doc, ss);
			string s1 = ss.str();

			Document doc2 = DocumentSerializer::load(ss);
			DocumentSerializer::save(doc, std::cout);
			stringstream ss2;
			DocumentSerializer::save(doc, ss2);
			string s2 = ss2.str();

			cout << "Comparison (should equal 0): " << s1.compare(s2) << endl;

			stream.close();
		}
	}

	return 0;
}

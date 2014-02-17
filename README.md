SwiRL-NLP
=========

This project provides C++ integration between Mihai Surdeanu's SISTA CoreNLP
[processors](https://github.com/sistanlp/processors) (written in Scala)
and his [SwiRL](http://www.surdeanu.info/mihai/swirl/index.php) system
(written in C++).

The code provides seamless serialization & deserialization of SISTA's CoreNLP
documents in C++. This allows us to use SwiRL to add semantic role labels to a
document previously annotated with CoreNLP, which has the advantage of using
CoreNLP's POS tagger and named-entity engine to bootstrap SwiRL.



## Dependencies

**WordNet**

Make sure you have WordNet installed and the environment variable $WNHOME is
set to the root of the installation directory.  You can download and install
WordNet from [here](http://wordnet.princeton.edu/), then set $WNHOME
accordingly. See example below.
```
$ # If you installed wordnet to /usr/local/WordNet-3.0/
$ export WNHOME='/usr/local/WordNet-3.0/'
$ echo $WNHOME
/usr/local/WordNet-3.0/
```

**SwiRL**

Make sure SwiRL is installed.  Download, build, and install SwiRL from
[here](http://www.surdeanu.info/mihai/swirl/).  Alternatively if you are
developing in OSX, check out [SwiRL-OSX](https://github.com/trananh/SwiRL-OSX).

For common build errors, please refer to this
[FAQ](http://www.surdeanu.info/mihai/swirl/faq.php).



## Compilation

Before compiling, you may need to update the installation paths of SwiRL in the
Makefile. By default, the include path for SwiRL is set at
```/usr/local/include/swirl``` and the lib path is set to ```/usr/local/lib```.
If you installed SwiRL to a different location, please update the variables at
the top of the Makefile accordingly.
```
# SWIRL's installation
SWIRLINC = /usr/local/include/swirl
SWIRLLIB = /usr/local/lib
```

Once all the paths are resolved. The usual make will compile the project.
```
$ cd /path/to/SwiRL-NLP
$ make
```



## Code Example

The following snippet demonstrates how to read CoreNLP annotation from file, 
and then using SwiRL to further parse the document for role labels. It shows
how one can integrate the output of CoreNLP's POS & named entity taggers
into the input for SwiRL.

Alternatively, you can take a look at ```src/main.cpp```.

```C++
#include <iostream>
#include <fstream>
#include <string>

#include <Swirl.h>
#include "DocumentSerializer.h"

using namespace std;
using namespace processors;
using namespace srl;

int main(int argc, char ** argv) {

    // Set these paths accordingly
    string swirl = "./model_swirl";
    string charniak = "./model_charniak";
    string filename = "/path/to/nlp-annotation.txt";

    // Initialize Swirl
    bool caseSensitive = true;
    if (!Swirl::initialize(swirl.c_str(), charniak.c_str(), caseSensitive)) {
        cerr << "Failed to initialize SRL system!\n";
        exit(1);
    }

    // Open NLP annotation
    ifstream stream(filename.c_str());
    if (stream.is_open()) {

        // Load annotation
        Document doc = DocumentSerializer::load(stream);

        // For each sentence, parse with swirl
        TreePtr tree;
        string text, swirlCode;
        const char * line;
        for (int i = 0; i < (int) doc.sentences.size(); i++) {
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
            cout << text << endl << endl;
            text = swirlCode + string(" ") + text;
            line = text.c_str();

            // classify all predicates in this sentence
            tree = Swirl::parse(line);

            // dump extended Treebank format
            if (tree != (const Tree *) NULL) {
                tree->serialize(cout);
                cout << endl << endl;
            }

            // dump extended CoNLL format
            Swirl::serialize(tree, line, cout);
        }

        stream.close();

    } else {
        cerr << "Failed to find annotation file!\n";
        exit(1);
    }

    return 0;
} 

```



## Compatibility

- **[02/2014]** The code was tested to work with SwiRL-OSX (based on
swirl-1.1.0), WordNet 3.0, and edu.arizona.sista.processors v2.0.

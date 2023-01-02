#!/usr/bin/env python3
import nltk
nltk.download('punkt')
nltk.download('propbank')

# Load the propbank corpus
corpus = nltk.corpus.propbank

def tokenize(text):
    tokens = nltk.word_tokenize(text)
    print(tokens)
    return tokens

def PropbankCorpusReaderPrint():
    # Load the propbank corpus
    corpus = nltk.corpus.propbank

    # Access the list of files in the corpus
    print(corpus.fileids())

    # Access a specific file in the corpus
    file = corpus.fileids()[0]
    print(corpus.raw(file))

    # Access the annotated propositions in the file
    print(corpus.prop_sents(file))

    # Access the part-of-speech tags for the words in the file
    print(corpus.tagged_sents(file))


# Define a function to extract features from the data
def extract_features(sentence, index):
    return {
        'word': sentence[index],
        'is_first': index == 0,
        'is_last': index == len(sentence) - 1,
        'is_capitalized': sentence[index][0].upper() == sentence[index][0],
        'is_all_caps': sentence[index].upper() == sentence[index],
        'is_all_lower': sentence[index].lower() == sentence[index],
        'prefix-1': sentence[index][0],
        'prefix-2': sentence[index][:2],
        'prefix-3': sentence[index][:3],
        'suffix-1': sentence[index][-1],
        'suffix-2': sentence[index][-2:],
        'suffix-3': sentence[index][-3:],
        'prev_word': '' if index == 0 else sentence[index - 1],
        'next_word': '' if index == len(sentence) - 1 else sentence[index + 1],
        'has_hyphen': '-' in sentence[index],
        'is_numeric': sentence[index].isdigit(),
        'capitals_inside': sentence[index][1:].lower() != sentence[index][1:]
    }

# Define a function to extract the features for a whole sentence
def sentence_features(sentence):
    return [extract_features(sentence, index) for index in range(len(sentence))]

# Define a function to extract the features for a whole proposition
def prop_features(prop):
    sentence = [word for word, pos in prop]
    return [extract_features(sentence, index) for index in range(len(sentence))]

# Define a function to extract the features for a whole sentence
def sentence_features(sentence):
    return [extract_features(word, index) for index, word in enumerate(sentence)]




def PropbankCorpusReaderTrain():
#    # Extract the features for all propositions in the corpus
#     featuresets = [(prop_features(prop), pos) for prop in corpus.pos_sents() for word, pos in prop]

#     # Split the data into train and test sets
#     train_set, test_set = featuresets[100:], featuresets[:100]

#     # Train a classifier
#     classifier = nltk.NaiveBayesClassifier.train(train_set)

#     # Test the classifier
#     print(nltk.classify.accuracy(classifier, test_set))

    # Extract the features for all words in the corpus
    featuresets = [(extract_features(word, index), tag) for index, (word, tag) in enumerate(corpus.words())]

    # Split the data into train and test sets
    train_set, test_set = featuresets[100:], featuresets[:100]

    # Train a classifier
    classifier = nltk.NaiveBayesClassifier.train(train_set)

    # Test the classifier
    print(nltk.classify.accuracy(classifier, test_set))



PropbankCorpusReaderTrain()



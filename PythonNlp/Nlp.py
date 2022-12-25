#!/usr/bin/env python3
import nltk
nltk.download('punkt')

def tokenize(text):
    tokens = nltk.word_tokenize(text)
    print(tokens)
    return tokens

#tokenize("I love you")

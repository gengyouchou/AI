#!/usr/bin/env python3
import nltk

def tokenize(text):
    tokens = nltk.word_tokenize(text)
    return tokens

tokenize("I love you")

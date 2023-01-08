#!/usr/bin/env python3
#coding=utf-8
import random
import nltk
nltk.download('names')
from nltk.corpus import names

def gender_features(word): 
    # '''''提取每个单词的最后一个字母作为特征'''  
	return {'last_letter': word[-1]}

def test():
    # 先为原始数据打好标签  
    labeled_names = ([(name, 'male') for name in names.words('male.txt')] + [(name, 'female') for name in names.words('female.txt')])  
    # 随机打乱打好标签的数据集的顺序，  
    random.shuffle(labeled_names)  
    # 从原始数据中提取特征（名字的最后一个字母， 参见gender_features的实现）  
    featuresets = [(gender_features(name), gender) for (name, gender) in labeled_names]  
    # 将特征集划分成训练集和测试集  
    train_set, test_set = featuresets[500:], featuresets[:500]  
    # 使用训练集训练模型（核心就是求出各种后验概率）  
    classifier = nltk.NaiveBayesClassifier.train(train_set)  
    # 通过测试集来估计分类器的准确性  
    print(nltk.classify.accuracy(classifier, test_set))  
    # 如果一个人的名字的最后一个字母是‘a’，那么这个人是男还是女  
    print(classifier.classify({'last_letter': 'a'}))  
    # 找出最能够区分分类的特征值  
    classifier.show_most_informative_features(5)  

test()
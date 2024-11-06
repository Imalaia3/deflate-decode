/*
 * Part of the Imalaia3 Utility Classes.
 * Copyright (c): Imalaia3 2024-
 */

#pragma once
#include <iostream>
#include <assert.h>

template <typename T>
class BinaryTree {
public:
    struct Node {
        T data; // leave this unitialized so it's clear that no value has been given
        Node* left = nullptr;
        Node* right = nullptr;
        Node(T value) : data(value) {}
        Node* addLeft(T data) { left = new Node(data); return left; }
        Node* addRight(T data) { right = new Node(data); return right; }
        bool isLeaf() {
            // if (left != nullptr && right != nullptr && data != 0) {
            //     std::cout << "Warning: isLeaf() determined node " << this << " to be non-leaf, but it contains a non-zero value!\n";
            // } 
            return (left == nullptr && right == nullptr); 
        }
    };

    BinaryTree() {std::cout << "Using Default constructor\n"; }
    BinaryTree(T parentNodeData) {
        m_treeRoot = new Node(parentNodeData);
    }

    ~BinaryTree() {
        deleteTree(m_treeRoot);
    }

    Node* addLeftChild(Node* parent, T data) {
        assert(parent != nullptr);
        return parent->addLeft(data);
    }
    Node* addRightChild(Node* parent, T data) {
        assert(parent != nullptr);
        return parent->addRight(data);
    }

    // delete children of node and then delete node. Recursive function.
    void deleteTree(Node* begin) {
        if (begin == nullptr)
            return;
        
        deleteTree(begin->left);
        deleteTree(begin->right);
        delete begin;
    }

    // search for T in begin and in children. Recursive function.
    Node* findInTree(Node* begin, T dataToFind) {
        if (begin == nullptr) {
            return nullptr;
        }
        if (begin->data == dataToFind) {
            return begin;
        }
        auto leftSearch = findInTree(begin->left, dataToFind);
        auto rightSearch = findInTree(begin->right, dataToFind);
        if (leftSearch)
            return leftSearch;
        return rightSearch;
    }

    // Print out whole tree with begin acting as a root. Recursive function.
    void printTree(Node* begin) {
        std::cout << "Node " << begin << ":\n";
        std::cout << "\t" << "Value = " << begin->data << "\n";
        std::cout << "\t" << "Left = " << begin->left << "\n";
        std::cout << "\t" << "Right = " << begin->right << "\n";
        std::cout << "\t" << "IsLeafNode = " << begin->isLeaf() << "\n";

        if (begin->left)
            printTree(begin->left);
        if (begin->right)
            printTree(begin->right);
    }

    Node* getRoot() { return m_treeRoot; }
private:
    Node* m_treeRoot = nullptr;
};

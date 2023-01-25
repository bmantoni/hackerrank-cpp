#include <cmath>
#include <cstdio>
#include <vector>
#include <map>
#include <iostream>
#include <algorithm>
using namespace std;

/*
Author:  Bobby Mantoni
Problem: https://www.hackerrank.com/challenges/attribute-parser/problem

Sample Input:
    4 3
    <tag1 value = "HelloWorld">
    <tag2 name = "Name1">
    </tag2>
    </tag1>
    tag1.tag2~name
    tag1~name
    tag1~value
Output:
    Name1
    Not Found!
    HelloWorld
*/

void removeChars(string *str, char c)
{
    str->erase(std::remove(str->begin(), str->end(), c), str->end());
}

class Node
{
public:
    string name;
    vector<Node *> children;
    Node *parent;
    map<string, string> attributes;
};

class Parser
{
private:
    int numMarkupLines;
    int numQueries;
    Node *tagsRoot;

public:
    void parseHeaderLine()
    {
        string line;
        std::getline(std::cin, line);

        string numMarkupStr = line.substr(0, line.find(' '));
        numMarkupLines = stoi(numMarkupStr);

        string numQueriesStr = line.substr(line.find(' ') + 1);
        numQueries = stoi(numQueriesStr);
    }
    void parseMarkupLines()
    {
        string line;
        Node *curr = new Node();
        curr->name = "ROOT";
        tagsRoot = curr;

        for (int i = 0; i < numMarkupLines; ++i)
        {
            std::getline(std::cin, line);
            parseMarkupLine(line, &curr);
        }
    }

    void parseQueries()
    {
        string line;

        for (int i = 0; i < numQueries; ++i)
        {
            std::getline(std::cin, line);
            parseQuery(line, tagsRoot);
        }
    }

    void parseQuery(string q, Node *n)
    {
        int nextDot = q.find('.');
        string tagName = q.substr(0, q.find(nextDot == std::string::npos ? '~' : '.'));

        vector<Node *>::iterator result = find_if(n->children.begin(), n->children.end(), [tagName](const Node *node)
                                                  { return node->name == tagName; });
        if (result != n->children.end())
        {
            if (nextDot == std::string::npos)
            {
                // we've reached the leaf
                printAttribute(q, *result);
            }
            else
            {
                // go deeper
                parseQuery(q.substr(q.find('.') + 1), *result);
            }
        }
        else
        {
            // the node is not found at this level
            printf("%s\n", "Not Found!");
        }
    }

    void printAttribute(string q, Node *n)
    {
        const int firstTilda = q.find('~');
        string nodeName = q.substr(0, firstTilda);
        string attr = q.substr(firstTilda + 1);

        if (n->attributes.find(attr) == n->attributes.end())
        {
            printf("%s\n", "Not Found!");
            return;
        }

        string attrValue = n->attributes.at(attr);
        printf("%s\n", attrValue.c_str());
    }

    void parseMarkupLine(string line, Node **curr)
    {
        if (line.substr(0, 2) == "</")
        {
            // could validate that the closing tag matches the opening tag here
            *curr = (*curr)->parent;
            return;
        }
        else if (line.substr(0, 1) == "<")
        {
            Node *newNode = new Node();
            int firstSpace = line.find(' ');
            if (firstSpace != std::string::npos)
            {
                newNode->name = line.substr(1, firstSpace - 1);
                parseAttributes(line.substr(line.find(' ')), newNode);
            }
            else
            {
                int endPos = line.find('>');
                newNode->name = line.substr(1, endPos - 1);
            }

            newNode->parent = *curr;
            (*curr)->children.push_back(newNode);
            *curr = newNode;
        }
        else
        {
            std::cout << "Invalid markup input";
        }
    }

    void parseAttributes(string line, Node *node)
    {
        int nextEquals = line.find('=');
        if (nextEquals == std::string::npos)
        {
            return;
        }
        string attrName = line.substr(0, nextEquals);
        removeChars(&attrName, ' ');
        string remainder = line.substr(nextEquals + 1);
        remainder = remainder.substr(remainder.find('"') + 1);
        string attrValue = remainder.substr(0, remainder.find('"'));
        remainder = remainder.substr(attrValue.length() + 1);
        remainder = remainder.substr(remainder.find_first_not_of(' '));

        node->attributes.insert({attrName, attrValue});
        parseAttributes(remainder, node);
    }
};

int main()
{
    Parser p;

    p.parseHeaderLine();
    p.parseMarkupLines();
    p.parseQueries();

    return 0;
}

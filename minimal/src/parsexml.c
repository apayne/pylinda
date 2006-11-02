/*
*    Copyright 2006 Andrew Wilkinson <aw@cs.york.ac.uk>.
*
*    This file is part of Minimal (http://www-users.cs.york.ac.uk/~aw/pylinda)
*
*    Minimal is free software; you can redistribute it and/or modify
*    it under the terms of the GNU Lesser General Public License as published by
*    the Free Software Foundation; either version 2.1 of the License, or
*    (at your option) any later version.
*
*    Minimal is distributed in the hope that it will be useful,
*    but WITHOUT ANY WARRANTY; without even the implied warranty of
*    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*    GNU Lesser General Public License for more details.
*
*    You should have received a copy of the GNU Lesser General Public License
*    along with Minimal; if not, write to the Free Software
*    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#include <string.h>
#include <stdlib.h>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "minimal_internal.h"

Minimal_SyntaxTree* Minimal_xmlToSyntaxTree(xmlNodePtr node);

Minimal_SyntaxTree* Minimal_parseXMLCode(const char* code) {
    xmlDocPtr doc = xmlReadMemory(code, strlen(code), NULL, NULL, 0);

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if(strcmp((char*)(node->name), "minimal") != 0) {
        fprintf(stderr, "Error: Not a Minimal XML file.\n");
        return NULL;
    }

    Minimal_SyntaxTree* tree = Minimal_xmlToSyntaxTree(node);

    return tree;
}

Minimal_SyntaxTree* Minimal_xmlToSyntaxTree(xmlNodePtr node) {
    if(strcmp((char*)(node->name), "minimal") == 0) {
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        Minimal_SyntaxTree* tree2 = tree;
        tree->type = ST_SEQENTIAL_DEFS;

        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                tree2->branch1 = Minimal_xmlToSyntaxTree(cur_node);
                tree2->branch2 = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
                tree2 = tree2->branch2;
                tree2->type = ST_SEQENTIAL_DEFS;
            }
            cur_node = cur_node->next;
        }
        tree2->type = ST_BLANK;
        return tree;
    } else if(strcmp((char*)(node->name), "function") == 0) {
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        Minimal_SyntaxTree* tree2 = NULL;
        tree->type = ST_FUNCTION_DEF;
        xmlChar* fn = xmlGetProp(node, (xmlChar*)"name");
        tree->func_name = (char*)malloc(strlen((char*)fn)+1);
        strcpy(tree->func_name, (char*)fn);
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                tree2 = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "type_spec") == 0) {
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        Minimal_SyntaxTree* tree2 = NULL;
        tree->type = ST_TYPE_SPEC;

        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                tree2 = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else {
        fprintf(stderr, "Error: Not a Minimal XML tag (%s).\n", node->name);
        return NULL;
    }
}

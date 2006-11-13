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
MinimalValue Minimal_xmlToValue2(xmlNodePtr node, ValueMemo* memo);

Minimal_SyntaxTree* Minimal_parseXMLCode(const char* code) {
    xmlDocPtr doc = xmlReadMemory(code, strlen(code), NULL, NULL, 0);

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if(strcmp((char*)(node->name), "minimal") != 0) {
        fprintf(stderr, "Error: Not a Minimal XML file.\n");
        return NULL;
    }

    Minimal_SyntaxTree* tree = Minimal_xmlToSyntaxTree(node);
    xmlFreeDoc(doc);

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
    } else if(strcmp((char*)(node->name), "product_type") == 0) {
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        Minimal_SyntaxTree* tree2 = tree;
        tree->type = ST_PRODUCT_TYPE;
        tree->branch1 = NULL;
        tree->branch2 = NULL;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(tree2->branch1 == NULL) {
                    tree2->branch1 = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    tree2->branch2 = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
                    tree2 = tree2->branch2;
                    tree2->type = ST_PRODUCT_TYPE;
                    tree2->branch1 = Minimal_xmlToSyntaxTree(cur_node);
                    tree2->branch2 = NULL;
                }
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "sum_type") == 0) {
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        Minimal_SyntaxTree* tree2 = tree;
        tree->type = ST_SUM_TYPE;
        tree->branch1 = NULL;
        tree->branch2 = NULL;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(tree2->branch1 == NULL) {
                    tree2->branch1 = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    tree2->branch2 = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
                    tree2 = tree2->branch2;
                    tree2->type = ST_PRODUCT_TYPE;
                    tree2->branch1 = Minimal_xmlToSyntaxTree(cur_node);
                    tree2->branch2 = NULL;
                }
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "id") == 0) {
        xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        tree->type = ST_IDENTIFIER;
        tree->string = (char*)malloc(strlen((char*)name)+1);
        strcpy(tree->string, (char*)name);
        free(name);
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
    } else if(strcmp((char*)(node->name), "ptr") == 0) {
        xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
        Minimal_SyntaxTree* tree = (Minimal_SyntaxTree*)malloc(sizeof(Minimal_SyntaxTree));
        tree->type = ST_POINTER;
        tree->ptr = (char*)malloc(strlen((char*)name)+1);
        strcpy(tree->ptr, (char*)name);
        free(name);
        return tree;
    } else {
        fprintf(stderr, "Error: Not a Minimal XML tag for Syntax Trees (%s).\n", node->name);
        return NULL;
    }
}

MinimalValue Minimal_parseXMLValue(const char* code) {
    MinimalValue value;
    xmlDocPtr doc = xmlReadMemory(code, strlen(code), NULL, NULL, 0);

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if(strcmp((char*)(node->name), "minimal") != 0) {
        fprintf(stderr, "Error: Not a Minimal XML file.\n");
        return NULL;
    }

    value = Minimal_xmlToValue(node);

    xmlFreeDoc(doc);

    return value;
}

MinimalValue Minimal_xmlToValue(xmlNodePtr node) {
    MinimalValue value;
    ValueMemo memo;
    memo.found = NULL;
    memo.waiting = NULL;

    value = Minimal_xmlToValue2(node, &memo);

    if(memo.found != NULL) {
        free(memo.found);
    }
    if(memo.waiting != NULL) {
        free(memo.waiting);
    }

    return value;
}

MinimalValue Minimal_xmlToValue2(xmlNodePtr node, ValueMemo* memo) {
    MinimalValue value = NULL;
    if(strcmp((char*)(node->name), "minimal") == 0) {
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && value == NULL) {
                value = Minimal_xmlToValue2(cur_node, memo);
            } else if(cur_node->type == XML_ELEMENT_NODE && value != NULL) {
                fprintf(stderr, "Error: Minimal_xmlToValue can only parse only value.\n");
                Minimal_delReference(value);
                return NULL;
            }
            cur_node = cur_node->next;
        }
    } else if(strcmp((char*)(node->name), "nil") == 0) {
        Minimal_addReference(Minimal_Nil)
        value = Minimal_Nil;
    } else if(strcmp((char*)(node->name), "string") == 0) {
        xmlNodePtr cur_node = node->children;
        char* text = NULL;
        while(cur_node) {
            if(cur_node->type == XML_TEXT_NODE) {
                text = (char*)cur_node->content;
            }
            cur_node = cur_node->next;
        }
        value = Minimal_string(text);
    } else if(strcmp((char*)(node->name), "tuple") == 0) {
        value = Minimal_tuple(0);
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(strcmp((char*)(cur_node->name), "type") == 0) {
                    value->typeobj = Minimal_xmlToValue2(cur_node, memo);
                } else {
                    Minimal_tupleAdd(value, Minimal_xmlToValue2(cur_node, memo));
                }
            }
            cur_node = cur_node->next;
        }
    } else if(strcmp((char*)(node->name), "type") == 0) {
        Minimal_SyntaxTree* tree = NULL;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && tree == NULL) {
                tree = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
        value = Minimal_typeSpec((char*)name, tree);
        Minimal_SyntaxTree_free(tree);
        free(name);
    } else if(strcmp((char*)(node->name), "integer") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_int(i);
    } else if(strcmp((char*)(node->name), "ptr") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        unsigned long ptr;
        sscanf((char*)val, "%li", &ptr);
        free(val);

        if(memo->found_size > 0) {
            int i;
            for(i=0; i<memo->found_size; i++) {
                if(memo->found[i].ptrid == ptr) {
                    Minimal_addReference(memo->found[i].realptr);
                    value = Minimal_ptr(NULL, (void*)(memo->found[i].realptr));
                    break;
                }
            }
        }
        if(value == NULL) {
            value = Minimal_ptr(NULL, NULL);
            if(memo->waiting == NULL) {
                memo->waiting_size = 1;
                memo->waiting = malloc(sizeof(struct PointerReplacementArray));
                memo->waiting[0].ptrid = ptr;
                memo->waiting[0].realptr = (void**)&(value->ptr);
            } else {
                int i;
                unsigned char matched = 0;
                for(i=0; i<memo->waiting_size; i++) {
                    if(memo->waiting[i].ptrid == 0) {
                        memo->waiting[i].ptrid = ptr;
                        memo->waiting[i].realptr = (void**)&(value->ptr);
                        matched = 1;
                    }
                }
                if(!matched) {
                    struct PointerReplacementArray* array = malloc(sizeof(struct PointerReplacementArray)*(memo->waiting_size+1));
                    memcpy(array, memo->waiting, sizeof(struct PointerReplacementArray)*memo->waiting_size);
                    array[memo->waiting_size].ptrid = ptr;
                    array[memo->waiting_size].realptr = (void**)&(value->ptr);
                    free(memo->waiting);
                    memo->waiting = array;
                    memo->waiting_size++;
                }
            }
        }

    } else {
        fprintf(stderr, "Error: Not a Minimal XML tag for Values (%s).\n", node->name);
        return NULL;
    }
    xmlChar* cid = xmlGetProp(node, (xmlChar*)"id");
    if(cid != NULL) {
        unsigned long id;
        sscanf((char*)cid, "%li", &id);
        free(cid);
        if(memo->waiting != NULL) {
            int i;
            for(i=0; i<memo->waiting_size; i++) {
                if(memo->waiting[i].ptrid == id) {
                    Minimal_addReference(value);
                    *(memo->waiting[i].realptr) = value;
                    memo->waiting[i].ptrid = 0;
                }
            }
        }
        if(memo->found == NULL) {
            memo->found_size = 1;
            memo->found = malloc(sizeof(struct PointerReplacementArray));
            memo->found[0].ptrid = id;
            memo->found[0].realptr = (void**)value;
        } else {
            struct PointerReplacementArray* array = malloc(sizeof(struct PointerReplacementArray)*(memo->found_size+1));
            memcpy(array, memo->found, sizeof(struct PointerReplacementArray)*memo->found_size);
            array[memo->found_size].ptrid = id;
            array[memo->found_size].realptr = (void**)value;
            free(memo->found);
            memo->found = array;
            memo->found_size++;
        }
    }
    return value;
}

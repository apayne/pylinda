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

Minimal_SyntaxTree Minimal_xmlToSyntaxTree(xmlNodePtr node);
MinimalValue Minimal_xmlToValue2(xmlNodePtr node, ValueMemo* memo, MinimalLayer types);

Minimal_SyntaxTree Minimal_parseXMLCode(const char* code) {
    Minimal_SyntaxTree tree;
    xmlDocPtr doc = xmlReadMemory(code, strlen(code), NULL, NULL, 0);

    xmlNodePtr node = xmlDocGetRootElement(doc);

    if(strcmp((char*)(node->name), "minimal") != 0) {
        fprintf(stderr, "Error: Not a Minimal XML file.\n");
        return NULL;
    }

    tree = Minimal_xmlToSyntaxTree(node);
    xmlFreeDoc(doc);

    return tree;
}

Minimal_SyntaxTree Minimal_xmlToSyntaxTree(xmlNodePtr node) {
    if(strcmp((char*)(node->name), "minimal") == 0) {
        int i = 0;
        xmlNode* cur_node;
        Minimal_SyntaxTree tree = Minimal_SyntaxTree_createSequence();
        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                Minimal_SyntaxTree_addToSequence(tree, Minimal_xmlToSyntaxTree(cur_node));
                i++;
            }
            cur_node = cur_node->next;
        }
        tree->length = i;
        return tree;
    } else if(strcmp((char*)(node->name), "function") == 0) {
        Minimal_SyntaxTree params = NULL;
        Minimal_SyntaxTree code = NULL;
        xmlNode* cur_node;
        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(strcmp((char*)(cur_node->name), "parameter_list") == 0) {
                    params = Minimal_xmlToSyntaxTree(cur_node);
                } else if(strcmp((char*)(cur_node->name), "code") == 0) {
                    code = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        return Minimal_SyntaxTree_createFunctionDef(Minimal_SyntaxTree_createID((char*)xmlGetProp(node, (xmlChar*)"name")), params, code);
    } else if(strcmp((char*)(node->name), "product_type") == 0) {
        int i = 0;
        xmlNode* cur_node;
        Minimal_SyntaxTree tree = Minimal_SyntaxTree_createProductType();
        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                Minimal_SyntaxTree_addToProductType(tree, Minimal_xmlToSyntaxTree(cur_node));
                i++;
            }
            cur_node = cur_node->next;
        }
        tree->length = i;
        return tree;
    } else if(strcmp((char*)(node->name), "sum_type") == 0) {
        int i = 0;
        xmlNode* cur_node;
        Minimal_SyntaxTree tree = Minimal_SyntaxTree_createSumType();
        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                Minimal_SyntaxTree_addToSumType(tree, Minimal_xmlToSyntaxTree(cur_node));
                i++;
            }
            cur_node = cur_node->next;
        }
        tree->length = i;
        return tree;
    } else if(strcmp((char*)(node->name), "type_function") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree arg = NULL;
        Minimal_SyntaxTree value = NULL;
        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && arg == NULL) {
                arg = Minimal_xmlToSyntaxTree(cur_node);
            } else if(cur_node->type == XML_ELEMENT_NODE && value == NULL) {
                value = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        return Minimal_SyntaxTree_createFunction(arg, value);
    } else if(strcmp((char*)(node->name), "nil") == 0) {
        return Minimal_SyntaxTree_createNil();
    } else if(strcmp((char*)(node->name), "id") == 0) {
        xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
        xmlChar* typeid = xmlGetProp(node, (xmlChar*)"typeid");
        Minimal_SyntaxTree tree = Minimal_SyntaxTree_createID((char*)name);
        free(name);
        if(typeid != NULL) {
            tree->type_id = (char*)typeid;
        } else {
            tree->type_id = NULL;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "integer") == 0) {
        int i;
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        i = atoi((char*)val);
        free(val);
        return Minimal_SyntaxTree_createInteger(i);
    } else if(strcmp((char*)(node->name), "string") == 0) {
        Minimal_SyntaxTree v;
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        v = Minimal_SyntaxTree_createString((char*)val);
        free(val);
        return v;
    } else if(strcmp((char*)(node->name), "type") == 0) {
        xmlChar* name;
        xmlNode* cur_node;
        Minimal_SyntaxTree type_spec = NULL;
        Minimal_SyntaxTree tree;

        name = xmlGetProp(node, (xmlChar*)"name");

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                type_spec = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        tree = Minimal_SyntaxTree_createTypeSpec(Minimal_SyntaxTree_createID((char*)name), type_spec);
        free(name);
        return tree;
    } else if(strcmp((char*)(node->name), "type_spec") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree type_spec = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                type_spec = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        return type_spec;
    } else if(strcmp((char*)(node->name), "ptr") == 0) {
        xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
        Minimal_SyntaxTree tree = Minimal_SyntaxTree_createPointer(Minimal_SyntaxTree_createID((char*)name));
        free(name);
        return tree;
    } else if(strcmp((char*)(node->name), "parameter_list") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "param") == 0) {
                xmlChar* name = xmlGetProp(cur_node, (xmlChar*)"name");
                if(name == NULL) {
                    fprintf(stderr, "Warning: Got a parameter without a name...\n");
                    name = (xmlChar*)malloc(1);
                } else if(tree == NULL) {
                    tree = Minimal_SyntaxTree_createParameterList1(Minimal_SyntaxTree_createID((char*)name));
                } else {
                    tree = Minimal_SyntaxTree_createParameterList2(tree, Minimal_SyntaxTree_createID((char*)name));
                }
                free(name);
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "code") == 0) {
        Minimal_SyntaxTree tree = NULL;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && tree == NULL) {
                tree = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        return tree;
    } else if(strcmp((char*)(node->name), "operator") == 0) {
        xmlNode* cur_node;
        xmlChar* op = xmlGetProp(node, (xmlChar*)"op");
        Minimal_SyntaxTree tree;
        Minimal_SyntaxTree op1 = NULL;
        Minimal_SyntaxTree op2 = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(op1 == NULL) {
                    op1 = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    op2 = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        tree = Minimal_SyntaxTree_createOperator(Minimal_SyntaxTree_createID((char*)op), op1, op2);
        free(op);
        return tree;
    } else if(strcmp((char*)(node->name), "index") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree;
        Minimal_SyntaxTree op = NULL;
        Minimal_SyntaxTree index = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(op == NULL) {
                    op = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    index = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        tree = Minimal_SyntaxTree_createIndex(op, index);
        return tree;
    } else if(strcmp((char*)(node->name), "let") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree;
        Minimal_SyntaxTree var = NULL;
        Minimal_SyntaxTree letexpr = NULL;
        Minimal_SyntaxTree code = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(var == NULL) {
                    var = Minimal_xmlToSyntaxTree(cur_node);
                } else if(letexpr == NULL) {
                    letexpr = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    code = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        tree = Minimal_SyntaxTree_createLet(var, letexpr, code);
        return tree;
    } else if(strcmp((char*)(node->name), "funccall") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree;
        Minimal_SyntaxTree func = NULL;
        Minimal_SyntaxTree args = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(func == NULL) {
                    func = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    args = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        tree = Minimal_SyntaxTree_createFunctionCall(func, args);
        return tree;
    } else if(strcmp((char*)(node->name), "argument_list") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree;
        Minimal_SyntaxTree arg = NULL;
        Minimal_SyntaxTree next_arg = NULL;

        cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                if(arg == NULL) {
                    arg = Minimal_xmlToSyntaxTree(cur_node);
                } else {
                    next_arg = Minimal_xmlToSyntaxTree(cur_node);
                }
            }
            cur_node = cur_node->next;
        }
        if(next_arg == NULL) {
             tree = Minimal_SyntaxTree_createArgumentList1(arg);
        } else {
             tree = Minimal_SyntaxTree_createArgumentList2(arg, next_arg);
        }
        return tree;
    } else if(strcmp((char*)(node->name), "tuple") == 0) {
        xmlNode* cur_node;
        Minimal_SyntaxTree tree;

        xmlChar* size_str = xmlGetProp(node, (xmlChar*)"size");
        int size = atoi((char*)size_str);
        free(size_str);

        tree = Minimal_SyntaxTree_createProductType();

        cur_node = node->children;
        while(tree->size < size && cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE) {
                Minimal_SyntaxTree_addToProductType(tree, Minimal_xmlToSyntaxTree(cur_node));
            }
            cur_node = cur_node->next;
        }
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
    MinimalLayer types;
    MinimalLayer old_layer;
    MinimalValue value;
    ValueMemo memo;
    memo.found = NULL;
    memo.waiting = NULL;

    old_layer = Minimal_getCurrentLayer();
    types = Minimal_createLayer();
    Minimal_setCurrentLayer(types);

    value = Minimal_xmlToValue2(node, &memo, types);

    Minimal_delReference(types);

    if(memo.found != NULL) {
        free(memo.found);
    }
    if(memo.waiting != NULL) {
        free(memo.waiting);
    }

    Minimal_setCurrentLayer(old_layer);
    Minimal_delReference(old_layer);

    return value;
}

MinimalValue Minimal_xmlSeriesToValue(xmlNodePtr node, ValueMemo* memo, MinimalLayer types) {
    MinimalValue root = NULL;
    while(node) {
        if(node->type == XML_ELEMENT_NODE) {
            if(strcmp((char*)node->name, "type") == 0) {
                MinimalValue type = Minimal_xmlToValue2(node, memo, types);
                Minimal_addName(types->map, type->type_name, type);
            } else if(root == NULL) {
                char* type;
                char* typeid;
                root = Minimal_xmlToValue2(node, memo, types);
                type = (char*)xmlGetProp(node, (xmlChar*)"typename");
                if(type != NULL) {
                    MinimalValue t = Minimal_getName(types, type);
                    if(t != NULL) {
                        Minimal_setType(root, t);
                        Minimal_delReference(t);
                    }
                    free(type);
                }
                typeid = (char*)xmlGetProp(node, (xmlChar*)"typeid");
                if(typeid != NULL) {
                    MinimalValue t = Minimal_typeFromId(typeid);
                    Minimal_setType(root, t);
                    Minimal_delReference(t);
                    free(typeid);
                }
                /*if(Minimal_isType(root) && root->type_name != NULL) {
                    Minimal_addReference(root);
                    Minimal_addName(&(types->map), root->type_name, root);
                }
                Minimal_setTypeMap(root, types);*/
            } else {
                char* type;
                char* typeid;
                MinimalValue v = Minimal_xmlToValue2(node, memo, types);
                type = (char*)xmlGetProp(node, (xmlChar*)"typename");
                if(type != NULL) {
                    MinimalValue t = Minimal_getName(types, type);
                    Minimal_setType(root, t);
                    Minimal_delReference(t);
                    free(type);
                }
                typeid = (char*)xmlGetProp(node, (xmlChar*)"typeid");
                if(typeid != NULL) {
                    Minimal_setType(root, Minimal_typeFromId((char*)typeid));
                    free(typeid);
                }
                /*if(Minimal_isType(v) && v->type_name != NULL) {
                    Minimal_addReference(v);
                    Minimal_addName(&(types->map), v->type_name, v);
                }
                Minimal_setTypeMap(v, types);*/
                Minimal_delReference(v);
            }
        }
        node = node->next;
    }
    return root;
}

MinimalValue Minimal_xmlToValue2(xmlNodePtr node, ValueMemo* memo, MinimalLayer types) {
    MinimalValue value = NULL;
    xmlChar* tid;
    xmlChar* cid;
    if(strcmp((char*)(node->name), "minimal") == 0) {
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && value == NULL && strcmp((char*)cur_node->name, "type") != 0) {
                value = Minimal_xmlToValue2(cur_node, memo, types);
            } else if(cur_node->type == XML_ELEMENT_NODE && value != NULL) {
                fprintf(stderr, "Error: Minimal_xmlToValue can only parse one value.\n");
                Minimal_delReference(value);
                return NULL;
            }
            cur_node = cur_node->next;
        }
    } else if(strcmp((char*)(node->name), "nil") == 0) {
        value = Minimal_nil();
    } else if(strcmp((char*)(node->name), "string") == 0) {
        xmlNodePtr cur_node = node->children;
        char* text = NULL;
        while(cur_node) {
            if(cur_node->type == XML_TEXT_NODE) {
                text = (char*)cur_node->content;
            }
            cur_node = cur_node->next;
        }
        if(text != NULL) {
            value = Minimal_string(text);
        } else {
            value = Minimal_string("");
        }
    } else if(strcmp((char*)(node->name), "tuple") == 0) {
        xmlNode* cur_node = node->children;
        value = Minimal_tuple(0);
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "type") == 0) {
                Minimal_delReference(Minimal_xmlToValue2(cur_node, memo, types));
            } else if(cur_node->type == XML_ELEMENT_NODE && strcmp((char*)cur_node->name, "element") == 0) {
                Minimal_tupleAdd(value, Minimal_xmlToValue2(cur_node, memo, types));
            }
            cur_node = cur_node->next;
        }
    } else if(strcmp((char*)(node->name), "sum") == 0) {
        MinimalValue v = NULL;
        char* sum_pos;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(v == NULL) {
                v = Minimal_xmlToValue2(cur_node, memo, types);
            }
            cur_node = cur_node->next;
        }
        sum_pos = (char*)xmlGetProp(node, (xmlChar*)"sum_pos");
        value = Minimal_sum(v, atoi(sum_pos));
        free(sum_pos);
    } else if(strcmp((char*)(node->name), "element") == 0) {
        value = Minimal_xmlSeriesToValue(node->children, memo, types);
    } else if(strcmp((char*)(node->name), "type") == 0) {
        Minimal_SyntaxTree tree = NULL;
        xmlChar* name;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && tree == NULL) {
                tree = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        name = xmlGetProp(node, (xmlChar*)"name");
        value = Minimal_typeSpec((char*)name, tree);
        Minimal_addReference(value);
        Minimal_addName(value->typemap->map, (char*)name, value);
        free(name);
    } else if(strcmp((char*)(node->name), "typeobj") == 0) {
        Minimal_SyntaxTree tree = NULL;
        xmlChar* tid;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && tree == NULL) {
                tree = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        tid = xmlGetProp(node, (xmlChar*)"typeid");
        if(tid != NULL) {
            value = Minimal_typeFromId((char*)tid);
            Minimal_setTypeMap(value, types);
            free(tid);
            if(tree != NULL) { Minimal_delReference(tree); }
        } else {
            xmlChar* name = xmlGetProp(node, (xmlChar*)"name");
            value = Minimal_typeSpec((char*)name, tree);
            Minimal_addReference(value);
            Minimal_addName(value->typemap->map, (char*)name, value);
            free(name);
        }
    } else if(strcmp((char*)(node->name), "false") == 0) {
        value = Minimal_bool(0);
    } else if(strcmp((char*)(node->name), "true") == 0) {
        value = Minimal_bool(1);
    } else if(strcmp((char*)(node->name), "byte") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_byte(i);
    } else if(strcmp((char*)(node->name), "short") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_short(i);
    } else if(strcmp((char*)(node->name), "integer") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_int(i);
    } else if(strcmp((char*)(node->name), "long") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_long(i);
    } else if(strcmp((char*)(node->name), "ubyte") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_ubyte(i);
    } else if(strcmp((char*)(node->name), "ushort") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_ushort(i);
    } else if(strcmp((char*)(node->name), "uinteger") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_uint(i);
    } else if(strcmp((char*)(node->name), "ulong") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        int i;
        sscanf((char*)val, "%i", &i);
        free(val);
        value = Minimal_ulong(i);
    } else if(strcmp((char*)(node->name), "float") == 0) {
        xmlChar* val = xmlGetProp(node, (xmlChar*)"val");
        float f;
        sscanf((char*)val, "%f", &f);
        free(val);
        value = Minimal_float(f);
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
                    value = Minimal_ptr((void*)(memo->found[i].realptr));
                    break;
                }
            }
        }
        if(value == NULL) {
            value = Minimal_ptr(NULL);
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
    } else if(strcmp((char*)(node->name), "function") == 0) {
        Minimal_SyntaxTree params = NULL;
        Minimal_SyntaxTree code = NULL;
        xmlChar* name;
        xmlNode* cur_node = node->children;
        while(cur_node) {
            if(cur_node->type == XML_ELEMENT_NODE && params == NULL && strcmp((char*)cur_node->name, "parameter_list") == 0) {
                params = Minimal_xmlToSyntaxTree(cur_node);
            } else if(cur_node->type == XML_ELEMENT_NODE && code == NULL && strcmp((char*)cur_node->name, "code") == 0) {
                code = Minimal_xmlToSyntaxTree(cur_node);
            }
            cur_node = cur_node->next;
        }
        if(params == NULL) {
            fprintf(stderr, "Error: Didn't get parameters in function.\n"); return NULL;
        } else if(code == NULL) {
            fprintf(stderr, "Error: Didn't get code in function.\n"); return NULL;
        }
        name = xmlGetProp(node, (xmlChar*)"name");
        value = Minimal_function2((char*)name, NULL, params, code);
        free(name);
    } else if(strcmp((char*)(node->name), "tsid") == 0) {
        xmlNodePtr cur_node = node->children;
        char* text = NULL;
        while(cur_node) {
            if(cur_node->type == XML_TEXT_NODE) {
                text = (char*)cur_node->content;
            }
            cur_node = cur_node->next;
        }
        if(text != NULL) {
            value = Minimal_tupleSpace(text);
        } else {
            value = Minimal_nil();
        }
    } else {
        fprintf(stderr, "Error: Not a Minimal XML tag for Values (%s).\n", node->name);
        return NULL;
    }

    tid = xmlGetProp(node, (xmlChar*)"typeid");
    if(tid == NULL) {
        char* typename = (char*)xmlGetProp(node, (xmlChar*)"typename");
        if(typename != NULL) {
            MinimalValue type = Minimal_getName(types, typename);
            if(type != NULL) { Minimal_setType(value, type); Minimal_delReference(type); }
            free(typename);
        }
    } else {
        MinimalValue type = Minimal_typeFromId((char*)tid);
        Minimal_setType(value, type);
        Minimal_delReference(type);

        free(tid);
    }

    cid = xmlGetProp(node, (xmlChar*)"id");
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

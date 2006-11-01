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

#include <libxml/xmlmemory.h>
#include <libxml/xmlsave.h>

#include "minimal_internal.h"

void Minimal_serializeTypeSpec(xmlDocPtr doc, xmlNodePtr parent, struct Minimal_SyntaxTree_t* type_spec);

xmlDocPtr Minimal_serializeXML(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f) {
    if(doc == NULL) {
        doc = xmlNewDoc(NULL);
    }
    if(parent == NULL) {
        parent = xmlNewDocNode(doc, NULL, (xmlChar*)"minimal", NULL);
        xmlDocSetRootElement(doc, parent);
    }

    Minimal_serializeValue(doc, parent, f);

    return doc;
}

char* Minimal_serialize(MinimalValue f) {
    xmlChar* buf;
    int size;

    xmlDocPtr doc = Minimal_serializeXML(NULL, NULL, f);

    xmlDocDumpFormatMemory(doc, &buf, &size, 1);

    char* outbuf = (char*)malloc(size+1);
    memcpy(outbuf, buf, size);
    outbuf[size] = '\0';

    xmlFreeDoc(doc);
    xmlFree(buf);

    return outbuf;
}

#define AddSumPos(x) if(f->sum_pos != -1) { \
                        char* pos = (char*)malloc(snprintf(NULL, 0, "%i", f->sum_pos)+1); \
                        sprintf(pos, "%i", f->sum_pos); \
                        xmlNewProp(x, (xmlChar*)"sum_pos", (xmlChar*)pos); \
                        free(pos); \
                     }
void Minimal_serializeValue(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f) {
    if(f->typeobj != NULL) {
        xmlNodePtr typenode = xmlNewDocNode(doc, NULL, (xmlChar*)"type", NULL);
        xmlAddChild(parent, typenode);
        xmlNewProp(typenode, (xmlChar*)"name", (xmlChar*)f->typeobj->type_name);
        Minimal_serializeTypeSpec(doc, typenode, f->typeobj->type_spec);
    }

    switch(f->type) {
    case NIL:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"nil", NULL);
        xmlAddChild(parent, node);
        AddSumPos(node);
        return;
        }
    case INTEGER:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"integer", NULL);
        xmlAddChild(parent, node);
        AddSumPos(node);
        char* integer = (char*)malloc(snprintf(NULL, 0, "%li", f->integer)+1);
        sprintf(integer, "%li", f->integer);
        xmlNewProp(node, (xmlChar*)"val", (xmlChar*)integer);
        free(integer);
        return;
        }
    case TUPLE:
        {
        int i;
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"tuple", NULL);
        xmlAddChild(parent, node);
        AddSumPos(node);
        for(i=0; i<f->size; i++) {
            Minimal_serializeValue(doc, node, f->values[i]);
        }
        return;
        }
    case FUNCTION:
        {
        Minimal_serializeFunction(doc, parent, f);
        return;
        }
    default:
        fprintf(stderr, "Error: Unknown value id in Minimal_serializeValue (%i).\n", f->type);
        return;
    }
}

void Minimal_serializeTypeSpec(xmlDocPtr doc, xmlNodePtr parent, struct Minimal_SyntaxTree_t* type_spec) {
    switch(type_spec->type) {
    case ST_IDENTIFIER:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"id", NULL);
        xmlAddChild(parent, node);
        xmlNewProp(node, (xmlChar*)"name", (xmlChar*)type_spec->string);
        return;
        }
    case ST_TYPE_SPEC:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"type_spec", NULL);
        xmlAddChild(parent, node);
        xmlNewProp(node, (xmlChar*)"name", (xmlChar*)type_spec->type_name);
        Minimal_serializeTypeSpec(doc, node, type_spec->type_def);
        return;
        }
    case ST_TYPE_FUNCTION:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"type_function", NULL);
        xmlAddChild(parent, node);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch1);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch2);
        return;
        }
    case ST_PRODUCT_TYPE:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"product_type", NULL);
        xmlAddChild(parent, node);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch1);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch2);
        return;
        }
    case ST_SUM_TYPE:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"sum_type", NULL);
        xmlAddChild(parent, node);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch1);
        Minimal_serializeTypeSpec(doc, node, type_spec->branch2);
        return;
        }
    default:
        fprintf(stderr, "Error: Unknown syntax tree id in Minimal_serializeTypeSpec (%i).\n", type_spec->type);
        return;
    }
}

void Minimal_serializeParameterList(xmlDocPtr doc, xmlNodePtr parent, struct Minimal_SyntaxTree_t* parameter_list) {
    xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"parameter_list", NULL);
    xmlAddChild(parent, node);

    while(parameter_list != NULL) {
        xmlNodePtr p = xmlNewDocNode(doc, NULL, (xmlChar*)"param", NULL);
        xmlAddChild(node, p);
        xmlNewProp(p, (xmlChar*)"name", (xmlChar*)parameter_list->var_name);

        parameter_list = parameter_list->next_var;
    }
}

void Minimal_serializeCode(xmlDocPtr doc, xmlNodePtr parent, struct Minimal_SyntaxTree_t* tree) {
    switch(tree->type) {
    case ST_IDENTIFIER:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"id", NULL);
        xmlAddChild(parent, node);
        xmlNewProp(node, (xmlChar*)"name", (xmlChar*)tree->string);
        return;
        }
    case ST_INTEGER:
        {
        xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"integer", NULL);
        xmlAddChild(parent, node);
        char* integer = (char*)malloc(snprintf(NULL, 0, "%i", tree->integer)+1);
        sprintf(integer, "%i", tree->integer);
        xmlNewProp(node, (xmlChar*)"val", (xmlChar*)integer);
        return;
        }
    case ST_OPERATOR:
        {
        xmlNodePtr p = xmlNewDocNode(doc, NULL, (xmlChar*)"operator", NULL);
        xmlAddChild(parent, p);
        xmlNewProp(p, (xmlChar*)"op", (xmlChar*)tree->_operator);
        Minimal_serializeCode(doc, p, tree->op1);
        Minimal_serializeCode(doc, p, tree->op2);
        return;
        }
    default:
        fprintf(stderr, "Error: Unknown syntax tree id in Minimal_serializeCode (%i).\n", tree->type);
        return;
    }
}

void Minimal_serializeFunction(xmlDocPtr doc, xmlNodePtr parent, MinimalValue f) {
    xmlNodePtr node = xmlNewDocNode(doc, NULL, (xmlChar*)"function", NULL);
    xmlNewProp(node, (xmlChar*)"name", (xmlChar*)f->func_name);
    xmlAddChild(parent, node);
    AddSumPos(node);

    xmlNodePtr type_spec = xmlNewDocNode(doc, NULL, (xmlChar*)"type_spec", NULL);
    xmlAddChild(node, type_spec);
    Minimal_serializeTypeSpec(doc, type_spec, f->type_spec);

    Minimal_serializeParameterList(doc, node, f->parameter_list);

    xmlNodePtr code = xmlNewDocNode(doc, NULL, (xmlChar*)"type_spec", NULL);
    xmlAddChild(node, code);
    Minimal_serializeCode(doc, code, f->code);
}


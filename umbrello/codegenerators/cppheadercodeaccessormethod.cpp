
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

/*  This code generated by: 
 *      Author : thomas
 *      Date   : Fri Aug 31 2003
 */

#include <kdebug.h>

#include "cppheadercodeaccessormethod.h"

#include "../attribute.h"
#include "../classifiercodedocument.h"
#include "../umlobject.h"
#include "../umlrole.h"

#include "cppsourcecodedocument.h"
#include "cppcodeclassfield.h"
#include "cppcodedocumentation.h"

// Constructors/Destructors
//  

CPPHeaderCodeAccessorMethod::CPPHeaderCodeAccessorMethod ( CPPCodeClassField * field, CodeAccessorMethod::AccessorType type)
   : CodeAccessorMethod ( (CodeClassField*) field ) 
{
	setType(type);
        updateMethodDeclaration();
	updateContent();
}

CPPHeaderCodeAccessorMethod::~CPPHeaderCodeAccessorMethod ( ) { }

// Other
//

/** Save the XMI representation of this object
 * @return      bool    status of save
 */
bool CPPHeaderCodeAccessorMethod::saveToXMI ( QDomDocument & doc, QDomElement & root ) {
        bool status = true;

        QDomElement blockElement = doc.createElement( "codeaccessormethod" );

        // set attributes
        setAttributesOnNode(doc, blockElement);

        root.appendChild( blockElement );

        return status;
}

void CPPHeaderCodeAccessorMethod::setAttributesOnNode ( QDomDocument & doc, QDomElement & blockElement)
{

        // set super-class attributes
        CodeAccessorMethod::setAttributesOnNode(doc, blockElement);

        // set local attributes now

}

/**
 * load params from the appropriate XMI element node.
 */
void CPPHeaderCodeAccessorMethod::loadFromXMI ( QDomElement & root )
{
        setAttributesFromNode(root);
}

void CPPHeaderCodeAccessorMethod::setAttributesFromNode( QDomElement & root)
{

        // set attributes from superclass method the XMI
        CodeAccessorMethod::setAttributesFromNode(root);

        // load local stuff

}

// we basically want to update the body of this method
void CPPHeaderCodeAccessorMethod::updateContent( )
{
	CodeClassField * parentField = getParentClassField();
	CPPCodeClassField * cppfield = (CPPCodeClassField*)parentField;
        QString fieldName = cppfield->getFieldName();
	QString text = "";

	switch(getType()) {
		case CodeAccessorMethod::GET:
		default:
			text = "return "+fieldName+";";
			break;
	}

        setText(text);
}

// we basically want to update the start text of this method
void CPPHeaderCodeAccessorMethod::updateMethodDeclaration() 
{

	CodeClassField * parentField = getParentClassField();
	ClassifierCodeDocument * doc = parentField->getParentDocument();

	//CPPClassifierCodeDocument * cppdoc = dynamic_cast<CPPClassifierCodeDocument*>(doc);
//	CPPClassifierCodeDocument * cppdoc = (CPPClassifierCodeDocument*) doc;
	CPPCodeClassField * cppfield = (CPPCodeClassField*) parentField;

        QString fieldName = cppfield->getFieldName();
        QString fieldType = cppfield->getTypeName();
        QString methodReturnType = "void";
        QString methodName = ""; // "get"+cppdoc->capitalizeFirstLetter(fieldName);
        QString methodParams = ""; // "get"+cppdoc->capitalizeFirstLetter(fieldName);
        QString headerText = ""; 
	QString endLine = doc->getParentGenerator()->getNewLineEndingChars();
	
	switch(getType()) {
		case CodeAccessorMethod::ADD:
        		methodName = "add_"+fieldName;
        		methodReturnType = "void";
			headerText = "Add a "+fieldName+" object to the "+fieldName+"List"+endLine+getParentObject()->getDoc()+endLine+"@return void";
			break;
		case CodeAccessorMethod::REMOVE:
        		methodName = "remove_"+fieldName;
        		methodReturnType = "void";
			headerText = "Remove a "+fieldName+" object from the "+fieldName+"List"+endLine+getParentObject()->getDoc()+endLine+"@return void";
			break;
		case CodeAccessorMethod::LIST:
        		methodName = "get_"+fieldName+"List";
        		methodReturnType = "vector";
			headerText = "Get the "+fieldName+"List"+endLine+getParentObject()->getDoc()+endLine+"@return vector with list of objects";
			break;
		case CodeAccessorMethod::SET:
        		methodName = "set_"+fieldName;
        		methodReturnType = "void";
			headerText = "Set the value of "+fieldName+endLine+getParentObject()->getDoc()+endLine+"@return the value of "+fieldName;
			break;
		case CodeAccessorMethod::GET:
		default:
        		methodName = "get_"+fieldName;
        		methodReturnType = fieldType;
        		methodParams = "";
			headerText = "Get the value of "+fieldName+endLine+getParentObject()->getDoc()+endLine+"@return the value of "+fieldName;
			break;
	}

	// set header
        CPPCodeDocumentation * header = new CPPCodeDocumentation(doc);
	if(!getParentObject()->getDoc().isEmpty())
		header->setText(headerText);
	setComment(header);

	// set start/end method text
        setStartMethodText(methodReturnType+" "+methodName+" ( "+methodParams+" ) ;");
        setEndMethodText("");

	setOverallIndentationLevel(1);
}



#include "cppheadercodeaccessormethod.moc"

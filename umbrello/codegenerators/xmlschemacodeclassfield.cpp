
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
 *      Date   : Mon Jun 30 2003
 */


#include <iostream.h>
#include <qregexp.h>

#include "xmlschemacodeclassfield.h"
#include "xmlschemaclassifiercodedocument.h"

#include "../attribute.h"
#include "../umlobject.h"
#include "../umlrole.h"


// Constructors/Destructors
//  

XMLSchemaCodeClassField::XMLSchemaCodeClassField (XMLSchemaClassifierCodeDocument * parentDoc, UMLRole * role) 
    : CodeClassField((ClassifierCodeDocument*)parentDoc, role) 
{

};

XMLSchemaCodeClassField::XMLSchemaCodeClassField (XMLSchemaClassifierCodeDocument * parentDoc, UMLAttribute * attrib) 
    : CodeClassField((ClassifierCodeDocument*)parentDoc, attrib) 
{

};

XMLSchemaCodeClassField::~XMLSchemaCodeClassField ( ) { };

//  
// Methods
//  

// Other methods
//  

// IF the type is "string" we need to declare it as
// the XMLSchema Object "String" (there is no string primative in XMLSchema).
// Same thing again for "bool" to "boolean"
QString XMLSchemaCodeClassField::fixTypeName(QString string)
{
        string.replace(QRegExp("^string$"),"xsd:string");
        string.replace(QRegExp("^bool$"),"xsd:boolean");
        return cleanName(string);
}

/*
QString XMLSchemaCodeClassField::getElementName() {
        if (parentIsAttribute()) 
	{
		UMLAttribute * at = (UMLAttribute*) getParentObject();
		return cleanName(at->getName());
	} 
	else
	{
		UMLRole * role = (UMLRole*) getParentObject();
		QString roleName = role->getName();
		if(fieldIsSingleValue()) {
			return roleName.replace(0, 1, roleName.left(1).lower());
		} else { 
			return roleName.lower() + "Vector";
		}
	}
}
*/



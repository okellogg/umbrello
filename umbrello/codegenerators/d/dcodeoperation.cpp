/*
    SPDX-License-Identifier: GPL-2.0-or-later

    SPDX-FileCopyrightText: 2007 Jari-Matti Mäkelä <jmjm@iki.fi>
    copyright (C) 2008-2020
    Umbrello UML Modeller Authors <umbrello-devel@kde.org>
*/

#include "dcodeoperation.h"

#include "dcodegenerator.h"
#include "dcodegenerationpolicy.h"
#include "dclassifiercodedocument.h"
#include "dcodedocumentation.h"
#include "uml.h"

// Constructors/Destructors
//

DCodeOperation::DCodeOperation(DClassifierCodeDocument * doc, UMLOperation *parent,
const QString & body, const QString & comment) : CodeOperation (doc, parent, body, comment)
{
    // lets not go with the default comment and instead use
    // full-blown d documentation object instead
    setComment(new DCodeDocumentation(doc));

    // these things never change..
    setOverallIndentationLevel(1);
}

DCodeOperation::~DCodeOperation()
{
}

// we basically want to update the doc and start text of this method
void DCodeOperation::updateMethodDeclaration()
{
    CodeDocument * doc = getParentDocument();
    DClassifierCodeDocument * ddoc = dynamic_cast<DClassifierCodeDocument*>(doc);
    Q_ASSERT(ddoc);
    UMLOperation * o = getParentOperation();
    bool isInterface = ddoc->getParentClassifier()->isInterface();
    QString endLine = getNewLineEndingChars();

    /*
     * Member function declaration
     *
     * (visibility) (static | abstract | override) retType name (param1, ..., paramN) (; | {)
     *      a          b         c          d         e      f     g                     h
     */

    QString startText;

    // (a) visibility modifier
    //FIXME: startText += o->getVisibility().toString() + " ";

    // (b) static
    if (o->isStatic()) startText += QLatin1String("static ");

    // (c) abstract
    //TODO

    // (d) override
    //TODO

    // (e) return type
    if (!o->isConstructorOperation()) {
    //FIXME:     startText += DCodeGenerator::fixTypeName(o->getTypeName()) + QLatin1String(" ");
    }

    // (f) name
    startText += o->name();

    // (g) params
    startText += QLatin1Char('(');

    // assemble parameters
    QString paramStr;
    UMLAttributeList list = getParentOperation()->getParmList();
    int paramNum = list.count();

    foreach (UMLAttribute* parm, list) {
        QString rType = parm->getTypeName();
        QString paramName = parm->name();
        paramStr += rType + QLatin1Char(' ') + paramName;
        paramNum--;

        if (paramNum > 0) paramStr += QLatin1String(", ");
    }

    startText += paramStr;

    startText += QLatin1Char(')');

    // (h) function body
    if(isInterface) {
        startText += QLatin1Char(';');
        setEndMethodText(QString());
    } else {
        startText += QLatin1String(" {");
        setEndMethodText(QLatin1String("}"));
    }

    setStartMethodText(startText);

    // Lastly, for text content generation, we fix the comment on the
    // operation, IF the codeop is autogenerated & currently empty
    QString comment = o->doc();
    if(comment.isEmpty() && contentType() == CodeBlock::AutoGenerated)
    {
        UMLAttributeList parameters = o->getParmList();
        foreach (UMLAttribute* currentAtt, parameters) {
            comment += endLine + QLatin1String("@param ") + currentAtt->name() + QLatin1Char(' ');
            comment += currentAtt->doc();
        }
        // add a returns statement too
        // TODO proper return type comments
        //if(!returnType.isEmpty())
        //    comment += endLine + QLatin1String("@return ") + returnType + QLatin1Char(' ');

        getComment()->setText(comment);
    }
}

int DCodeOperation::lastEditableLine()
{
    ClassifierCodeDocument * doc = dynamic_cast<ClassifierCodeDocument*>(getParentDocument());

    // very last line is NOT editable as its a one-line declaration
    // w/ no body in an interface.
    if (doc->parentIsInterface()) return -1;

    return 0;
}


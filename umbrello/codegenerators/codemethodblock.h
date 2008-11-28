/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2003      Brian Thomas <thomas@mail630.gsfc.nasa.gov>   *
 *   copyright (C) 2004-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

#ifndef CODEMETHODBLOCK_H
#define CODEMETHODBLOCK_H

#include "ownedcodeblock.h"
#include "codeblockwithcomments.h"
#include "umlnamespace.h"

#include <QtCore/QString>

class ClassifierCodeDocument;

/**
 * class CodeMethodBlock
 * A  common type of "code block" that occurs in OO code.
 * Note: keep the inheritance sequence: QObject needs to be first in inheritance list.
 */
class CodeMethodBlock : public OwnedCodeBlock, public CodeBlockWithComments
{
    friend class CodeGenObjectWithTextBlocks;
    Q_OBJECT
public:

    /**
     * Constructors
     */
    CodeMethodBlock ( ClassifierCodeDocument * doc, UMLObject * parentObj,
                      const QString & body = "", const QString & comment = "");

    /**
     * Empty Destructor
     */
    virtual ~CodeMethodBlock ( );

    /**
     * @return  QString
     */
    virtual QString toString ( ) const;

    /**
     * Get the starting text that begins this method before the body is printed.
     */
    QString getStartMethodText () const;

    /**
     * Get the ending text that finishes this method after the body is printed.
     */
    QString getEndMethodText () const;

    /**
     * Get the parent code document.
     */
    CodeDocument * getParentDocument();

    /**
     * This is the method called from within syncToparent()
     * to update the *body* of the method.
     * It is only called if the method is Auto-generated.
     */
    virtual void updateContent ( ) = 0;

protected:

    /**
     * Causes the text block to release all of its connections
     * and any other text blocks that it 'owns'.
     * needed to be called prior to deletion of the textblock.
     */
    virtual void release ();

    /**
     * Set the starting text that begins this method before the body is printed.
     */
    void setStartMethodText (const QString &value);

    /**
     * Set the ending text that finishes this method after the body is printed.
     */
    void setEndMethodText (const QString &value);

    /**
     * This is the method called from within syncToparent().
     * To update the start and end Method text. It is called
     * whether or not the method is Auto or User generated.
     */
    virtual void updateMethodDeclaration ( ) = 0;

    /**
     * Set attributes of the node that represents this class
     * in the XMI document.
     */
    virtual void setAttributesOnNode ( QDomDocument & doc, QDomElement & blockElement);

    /**
     * Set the class attributes of this object from
     * the passed element node.
     */
    virtual void setAttributesFromNode ( QDomElement & element);

    /**
     * Set the class attributes from a passed object
     */
    virtual void setAttributesFromObject (TextBlock * obj);

private:

    QString m_startMethod;
    QString m_endMethod;

public slots:

    virtual void syncToParent();
};

#endif // CODEMETHODBLOCK_H
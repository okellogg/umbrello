
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
 *      Date   : Thu Jun 19 2003
 */

#include <cstdlib> //to get the user name
#include <iostream.h>

#include <qdatetime.h>
#include <qregexp.h>
#include <qdir.h>

#include <klocale.h>
#include <kdebug.h>
#include <kmessagebox.h>
#include <kdialogbase.h>
#include <kapplication.h>

#include "dialogs/overwritedialogue.h"

#include "codegenerator.h"
#include "codegenerators/simplecodegenerator.h"

#include "attribute.h"
#include "associationwidget.h"
#include "class.h" 
#include "classifier.h" 
#include "classifiercodedocument.h" 
#include "codedocument.h"
#include "operation.h"
#include "umldoc.h" 
#include "umlobject.h"
#include "umlview.h"
#include "umlattributelist.h"
#include "umloperationlist.h"

// Constructors/Destructors
//  

CodeGenerator::CodeGenerator ( UMLDoc * doc , const char *name ) 
    : QObject ( (QObject*) doc, name ) 
{
	initFields( doc);
}

// FIX
// hmm. this should be pure virtual so that implemented in sub-class
CodeGenerator::CodeGenerator ( UMLDoc * doc,  const char *name, QDomElement & element ) 
    : QObject ( (QObject *)doc, name) {
	initFields( doc);
	// loadFromXMI(element); // hmm. cant call this here.. its 'pure' virtual
}

CodeGenerator::~CodeGenerator ( ) { 
	// destroy all owned codedocuments
 	for (CodeDocument *doc = m_codedocumentVector.first(); doc; doc=m_codedocumentVector.next())
                delete doc;
}

//  
// Methods
//  


// Accessor methods
//  

QString CodeGenerator::getListFieldClassName () {
	return QString ("");
}

QString CodeGenerator::getUniqueID(CodeDocument * codeDoc)
{

        QString id = codeDoc->getID();

	// does this document already exist? then just return its present id
	if (!id.isEmpty() && findCodeDocumentByID(id))
		return id;

	// approach now differs by whether or not its a classifier code document
	ClassifierCodeDocument * classDoc = dynamic_cast<ClassifierCodeDocument*>(codeDoc);
	if(classDoc) {
		UMLClassifier *c = classDoc->getParentClassifier();
        	id = QString::number(c->getID()); // this is supposed to be unique already.. 
	} else {

		QString prefix = "doc";
		QString id = prefix + "_0";
		int number = lastIDIndex;
        	for ( ; findCodeDocumentByID(id); number++) {
                	id = prefix + "_" + QString::number(number);
        	}
        	lastIDIndex = number;
	}

        return id;
}

CodeDocument * CodeGenerator::findCodeDocumentByID(QString tag) {
        //if we already know to which file this class was written/should be written, just return it.
	CodeDocument * doc = (CodeDocument*)NULL;
        if((doc = m_codeDocumentDictionary.find(tag)))
{
	kdWarning()<<" * findCodeDocumentByID finds a match to ["<<tag.latin1()<<"]"<<":"<<doc<<endl;
                return doc;
}

	kdWarning()<<" * findCodeDocumentByID finds NO match to ["<<tag.latin1()<<"]"<<endl;
        return doc;
}

bool CodeGenerator::addCodeDocument ( CodeDocument * doc ) 
{

        QString tag = doc->getID();

    // assign a tag if one doesnt already exist
        if(tag.isEmpty())
        {
                tag = getUniqueID(doc);
                doc->setID(tag);
        }

        if(m_codeDocumentDictionary.find(tag))
		return false; // return false, we already have some object with this tag in the list
	else 
		m_codeDocumentDictionary.insert(tag, doc);

	m_codedocumentVector.append(doc);
	return true;
}

/**
 * Remove a CodeDocument object from m_codedocumentVector List
 */
bool CodeGenerator::removeCodeDocument ( CodeDocument * remove_object ) {
        QString tag = remove_object->getID();
        if(!(tag.isEmpty()))
		m_codeDocumentDictionary.remove(tag);
	else 
		return false;
 
	m_codedocumentVector.remove(remove_object); 
	return true;
}

/**
 * Get the list of CodeDocument objects held by m_codedocumentVector
 * @return QPtrList<CodeDocument *> list of CodeDocument objects held by
 * m_codedocumentVector
 */
QPtrList<CodeDocument> * CodeGenerator::getCodeDocumentList ( ) {
	return &m_codedocumentVector;
}

/**
 * Set the Policy object 
 */
void CodeGenerator::setPolicy ( CodeGenerationPolicy * policy ) {

	// should always have a policy
	if (!policy)
		return;

	m_codegeneratorpolicy = policy;
	 
	connect(m_codegeneratorpolicy,SIGNAL(modifiedCodeContent()),this,SLOT(syncCodeToDocument()));

}

/**
 * Get the Policy object 
 */
CodeGenerationPolicy * CodeGenerator::getPolicy ( ) {
	return m_codegeneratorpolicy;
}

/**
 * Get the value of m_document
 * @return the value of m_document
 */
UMLDoc * CodeGenerator::getDocument ( ) {
	return m_document;
}

// Other methods
//  

void CodeGenerator::loadFromXMI (QDomElement & qElement ) {

	// dont do anything for simple (compatability) code generators 
	if(dynamic_cast<SimpleCodeGenerator*>(this))
		return;

        //now look for our particular child element
        QDomNode node = qElement.firstChild();
        QDomElement element = node.toElement();
        QString langType = getLanguage();

cerr<<" **** LOAD FROM XMI CALLED for "<<langType.latin1()<<" CODE GENERATOR **** "<<endl;
        while( !element.isNull() ) {
                QString tag = element.tagName();
                if( tag == "codegenerator" && element.attribute( "language", "UNKNOWN" ) == langType ) {
                        // got our code generator element, now load
                        // codedocuments
                        QDomNode codeDocNode = element.firstChild();
                        QDomElement codeDocElement = codeDocNode.toElement();
                        while( !codeDocElement.isNull() ) {

                                QString docTag = codeDocElement.tagName();
cerr<<" XMI LOAD: GOT CODE DOCUMENT W/ tag:"<<docTag.latin1()<<endl;
                                if( docTag == "codedocument" ||
                                    docTag == "classifiercodedocument"
                                  ) {
                                        QString id = codeDocElement.attribute( "id", "-1" );
cerr<<" XMI LOAD: GOT CODE DOCUMENT W/ ID:"<<id.latin1()<<endl;
                                        CodeDocument * codeDoc = findCodeDocumentByID(id);
                                        if(codeDoc)
                                                codeDoc->loadFromXMI(codeDocElement);
                                        else {
                                                kdError()<<" LOAD FROM XMI: MISSING CODE DOCUMENT, create a new one or ignore and throw a warning?"<<endl;
                                        }
                                } else 
/*
				// load Policy??
                                if( docTag == "codegenpolicy" ) {
					getPolicy()->loadFromXMI(codeDocElement);
                                } else 
*/
{
cerr<<" XMI WARNING: got strange codegenerator child node:"<<docTag.latin1()<<", ignoring."<<endl;
                                        kdWarning()<<" XMI WARNING: got strange codegenerator child node:"<<docTag<<", ignoring."<<endl;
}

                                codeDocNode = codeDocElement.nextSibling();
                                codeDocElement = codeDocNode.toElement();
                        }
                        break; // no more to do
                }
                node = element.nextSibling();
                element = node.toElement();
        }
}

bool CodeGenerator::saveToXMI ( QDomDocument & doc, QDomElement & root ) {
        QString langType = getLanguage();
        QDomElement docElement = doc.createElement( "codegenerator" );
        docElement.setAttribute("language",langType);
        bool status = true;

cerr<<"Code GENERATOR saveToXMI Called:"<<langType.latin1()<<endl;

        QPtrList<CodeDocument> * docList = getCodeDocumentList();
        for (CodeDocument * codeDoc = docList->first(); codeDoc; codeDoc= docList->next())
                status = codeDoc->saveToXMI(doc, docElement) ? status : false;

        root.appendChild( docElement );

        return status;
}

/**
 * Initialize this code generator from its parent UMLDoc. When this is called, it will
 * (re-)generate the list of code documents for this project (generator) by checking
 * for new objects/attributes which have been added or changed in the documnet. One or more
 * CodeDocuments will be created/overwritten/amended as is appropriate for the given language.
 *
 * In this 'generic' version a ClassifierCodeDocument will exist for each and 
 * every classifier that exists in our UMLDoc. IF when this is called, a code document 
 * doesnt exist for the given classifier, then we will created and add a new code
 * document to our generator. 
 *
 * IF you want to add non-classifier related code documents at this step, 
 * you will need to overload this method in the appropriate
 * code generatator (see JavaCodeGenerator for an example of this).
 */
void CodeGenerator::initFromParentDocument( ) {

kdWarning()<<"INITTOPARENT called"<<endl;

        // Walk through the document converting classifiers into
        // classifier code documents as needed (e.g only if doesnt exist)
        UMLClassifierList concepts = getDocument()->getConcepts();
        for (UMLClassifier *c = concepts.first(); c; c = concepts.next())
        {

		// Doesnt exist? Then build one.
                CodeDocument * codeDoc = findCodeDocumentByClassifier(c);
		if (!codeDoc)
		{
                	// codeDoc = (CodeDocument*) newClassifierCodeDocument(c);
                	codeDoc = newClassifierCodeDocument(c);
			addCodeDocument(codeDoc); // this will also add a unique tag to the code document 
		}
        }

kdWarning()<<"INITTOPARENT END"<<endl;

}

/**
 * Force a syncronize of this code generator, and its present contents, to that of the parent UMLDocument.
 * "UserGenerated" code will be preserved, but Autogenerated contents will be updated/replaced
 * or removed as is apppropriate.
 */
void CodeGenerator::syncCodeToDocument ( ) {
kdWarning()<<"============ sync CODE TO DOCUMENT CALLED =========="<<endl;
	for (CodeDocument * doc = m_codedocumentVector.first(); doc; doc=m_codedocumentVector.next())
		doc->syncronize();
}

// in this 'vanilla' version, we only worry about adding classifier
// documents
void CodeGenerator::checkAddUMLObject (UMLObject * obj) {

	if (!obj)
		return;

	UMLClassifier * c = dynamic_cast<UMLClassifier*>(obj);
	if(c) {
		// CodeDocument * cDoc = (CodeDocument*) newClassifierCodeDocument(c);
		CodeDocument * cDoc = newClassifierCodeDocument(c);
		addCodeDocument(cDoc);
	}
}

void CodeGenerator::checkRemoveUMLObject (UMLObject * obj) 
{

        if (!obj)
                return;

        UMLClassifier * c = dynamic_cast<UMLClassifier*>(obj);
        if(c) {
                ClassifierCodeDocument * cDoc = (ClassifierCodeDocument*) findCodeDocumentByClassifier(c);
		if (cDoc)
                	removeCodeDocument(cDoc);
        }

}

/**
 * @return	CodeDocument
 * @param	classifier 
 */
CodeDocument * CodeGenerator::findCodeDocumentByClassifier ( UMLClassifier * classifier ) {
	return findCodeDocumentByID(QString::number(classifier->getID()));
}


/**
 * Write out all code documents to file as appropriate.
 */
void CodeGenerator::writeCodeToFile ( ) 
{
	writeCodeToFile(m_codedocumentVector);
}

void CodeGenerator::writeCodeToFile ( UMLClassifierList & concepts) {
	QPtrList<CodeDocument> docs;
	docs.setAutoDelete(false);

        for (UMLClassifier *concept= concepts.first(); concept; concept= concepts.next())
	{
		CodeDocument * doc = findCodeDocumentByClassifier(concept);
		if(doc)
			docs.append(doc);
	}
	
	writeCodeToFile(docs);
}

// Main method. Will write out passed code documents to file as appropriate.
void CodeGenerator::writeCodeToFile ( QPtrList<CodeDocument> & docs ) {

	// iterate thru all code documents
        for (CodeDocument *doc = docs.first(); doc; doc = docs.next())
	{

		// we need this so we know when to emit a 'codeGenerated' signal
		ClassifierCodeDocument * cdoc = dynamic_cast<ClassifierCodeDocument *>(doc);
		bool codeGenSuccess = false; 

		// we only write the document, if so requested
		if(doc->getWriteOutCode())
		{
			QString filename = findFileName(doc);
kdWarning()<<"CODE GEN writes code for:"<<filename.latin1()<<endl;
	   		// check that we may open that file for writing
	        	QFile file;
	        	if ( openFile(file,filename) ) {
	        		QTextStream stream(&file);
				stream<<doc->toString()<<endl;
				file.close();
				codeGenSuccess = true; // we wrote the code OK 
	        	} else {
	                	kdWarning() << "Cannot open file :"<<filename<<" for writing " << endl;
			}
/*
	//operations
	UMLOperationList *opl = c->getFilteredOperationsList();
	for(UMLOperation *op = opl->first(); op ; op = opl->next()) {
		temp =0;
		//check return value
		// temp =(UMLClassifier*) m_doc->findUMLObject(Uml::ot_Concept,op->getReturnType());
		temp =(UMLClassifier*) m_doc->findUMLClassifier(op->getReturnType());
		if(temp && !cList.containsRef(temp))
			cList.append(temp);
		//check parameters
		atl = op->getParmList();
		for(at = atl->first(); at; at = atl->next()) {
			// temp = (UMLClassifier*)m_doc->findUMLObject(Uml::ot_Concept,at->getTypeName());
			temp = (UMLClassifier*)m_doc->findUMLClassifier(at->getTypeName());
			if(temp && !cList.containsRef(temp))
				cList.append(temp);
*/
		}

		if(cdoc)
			emit codeGenerated(cdoc->getParentClassifier(), codeGenSuccess);

/*
	//attributes
	UMLClass * myClass = dynamic_cast<UMLClass*>(c);
	if(myClass) {
		atl = myClass->getFilteredAttributeList();
		for(at = atl->first(); at; at = atl->next()) {
			temp=0;
			// temp =(UMLClassifier*) m_doc->findUMLObject(Uml::ot_Concept,at->getTypeName());
			temp =(UMLClassifier*) m_doc->findUMLClassifier(at->getTypeName());
			if(temp && !cList.containsRef(temp))
				cList.append(temp);
		}
*/
	}

}

/**
 * Create a new Code document belonging to this package.
 * @return	CodeDocument
 */
CodeDocument * CodeGenerator::newCodeDocument ( ) {
   CodeDocument * newCodeDoc = new CodeDocument (this);
   return newCodeDoc;
}

CodeGenerationPolicy * CodeGenerator::newCodeGenerationPolicy ( KConfig * config) {
	return new CodeGenerationPolicy(this, config);
}

/**
 * @return	QString
 * @param	file 
 */

QString CodeGenerator::getHeadingFile(QString file) {
        return m_codegeneratorpolicy->getHeadingFile(file);
}

/**
 * @return	QString
 * @param	codeDoc
 * @param	name 
 */
QString CodeGenerator::overwritableName ( QString name ) {

	QDir outputDirectory = m_codegeneratorpolicy->getOutputDirectory();

        int suffix;
        OverwriteDialogue overwriteDialogue( name, outputDirectory.absPath(),
                                             m_applyToAllRemaining, kapp -> mainWidget() );
        switch(overwritePolicy()) {  //if it exists, check the OverwritePolicy we should use
                case CodeGenerationPolicy::Ok:                //ok to overwrite file
                        break;
                case CodeGenerationPolicy::Ask:               //ask if we can overwrite
                        switch(overwriteDialogue.exec()) {
                                case KDialogBase::Yes:  //overwrite file
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                setOverwritePolicy(CodeGenerationPolicy::Ok);
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        break;
                                case KDialogBase::No: //generate similar name
                                        suffix = 1;
                                        while( outputDirectory.exists(name + QString::number(suffix)) ) {
                                                suffix++;
                                        }
                                        name += QString::number(suffix);
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                setOverwritePolicy(CodeGenerationPolicy::Never);
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        break;
                                case KDialogBase::Cancel: //don't output anything
                                        if ( overwriteDialogue.applyToAllRemaining() ) {
                                                setOverwritePolicy(CodeGenerationPolicy::Cancel);
                                        } else {
                                                m_applyToAllRemaining = false;
                                        }
                                        return NULL;
                                        break;
                        }

                        break;
                case CodeGenerationPolicy::Never: //generate similar name
                        suffix = 1;
                        while( outputDirectory.exists(name + QString::number(suffix) ) ) {
                                suffix++;
                        }
                        name += QString::number(suffix);
                        break;
                case CodeGenerationPolicy::Cancel: //don't output anything
                        return NULL;
                        break;
        }

        return name;
}


/**
 * @return	bool
 * @param	file 
 * @param	name 
 */
bool CodeGenerator::openFile (QFile & file, QString fileName ) {
        //open files for writing.
        if(fileName.isEmpty()) {
                kdWarning() << "cannot find a file name" << endl;
                return false;
        } else {
		QDir outputDirectory = getPolicy()->getOutputDirectory();
                file.setName(outputDirectory.absFilePath(fileName));
                if(!file.open(IO_WriteOnly)) {
                        KMessageBox::sorry(0,i18n("Cannot open file %1 for writing. Please make sure the directory exists and you have permisions to write to it.").arg(file.name()),i18n("Cannot Open File"));
                        return false;
                }
                return true;
        }

}


/**
 * @return	QString
 * @param	name 
 */
QString CodeGenerator::cleanName (QString name ) {
	return name;
}

QString CodeGenerator::findFileName ( CodeDocument * codeDocument ) {

        //else, determine the "natural" file name
        QString name;

        // Get the path name
        QString path = codeDocument->getPath();

        // if path is given add this as a directory to the file name
        if (!path.isEmpty()) {
                name = path + "/" + codeDocument->getFileName() + codeDocument->getFileExtension();
                path = "/" + path;  
        } else {
                name = codeDocument->getFileName() + codeDocument->getFileExtension();
        }

        // Convert all "." to "/" : Platform-specific path separator
	// What UNIX platform has '.' for path separator?? -b.t.
        // name.replace(QRegExp("\\."),"/"); // Simple hack!

        // if a path name exists check the existence of the path directory
        if (!path.isEmpty()) {
		QDir outputDirectory = getPolicy()->getOutputDirectory();
                QDir pathDir(outputDirectory.absPath() + path);
                if (! (pathDir.exists() || pathDir.mkdir(pathDir.absPath()) ) ) {
                        KMessageBox::error(0, i18n("Cannot create the directory:\n") +
                                           pathDir.absPath() + i18n("\nPlease check the access rigths"),
                                           i18n("Cannot Create Directory"));
                        return NULL;
                }
        }

        name.simplifyWhiteSpace();
        name.replace(QRegExp(" "),"_");

        return overwritableName( name);
}
/*
bool CodeGenerator::hasDefaultValueAttr(UMLClass *c) {
	UMLAttributeList *atl = c->getFilteredAttributeList();
	for(UMLAttribute *at = atl->first(); at; at = atl->next())
		if(!at->getInitialValue().isEmpty())
			return true;
	return false;
}

bool CodeGenerator::hasAbstractOps(UMLClassifier *c) {
	UMLOperationList *opl = c->getFilteredOperationsList();
	for(UMLOperation *op = opl->first(); op ; op = opl->next())
		if(op->getAbstract())
			return true;
	return false;
}

void CodeGenerator::generateAllClasses() {
	if(!m_doc) {
		kdWarning() << "generateAllClasses::Error: doc is NULL!" << endl;
		return;
	}
	m_fileMap->clear();
	UMLClassifierList cList = m_doc->getConcepts();
	generateCode(cList);
}
*/

void CodeGenerator::findObjectsRelated(UMLClassifier *c, UMLClassifierList &cList) {
        UMLClassifier *temp;
        UMLView *view;

        view = m_document->getCurrentView();

	if(!view)
		return;

        AssociationWidgetList associations;
        associations.setAutoDelete(false);
        view->getWidgetAssocs(c,associations);

        UMLAttributeList *atl;
        UMLAttribute *at;


        //associations
        for(AssociationWidget *a = associations.first(); a ; a = associations.next()) {
                temp = 0;
                switch(a->getAssocType()) {
                        case Uml::at_Generalization:
                                if(a->getWidgetAID()==c->getID())
                                        temp =(UMLClassifier*) m_document->findUMLObject(a->getWidgetBID());
                                if(temp  && !cList.containsRef(temp))
                                        cList.append(temp);
                                break;
                        case Uml::at_Aggregation:
                        case Uml::at_Composition:
                                if(a->getWidgetBID()==c->getID())
                                        temp = (UMLClassifier*)m_document->findUMLObject(a->getWidgetAID());
                                if(temp  && !cList.containsRef(temp))
                                        cList.append(temp);
                                break;
                        default:
                                break;
                }
        }

        //operations
	UMLOperationList *opl = c->getFilteredOperationsList();
        for(UMLOperation *op = opl->first(); op ; op = opl->next()) {
                temp =0;
                //check return value
                // temp =(UMLClassifier*) m_document->findUMLObject(Uml::ot_Concept,op->getReturnType());
                temp =(UMLClassifier*) m_document->findUMLClassifier(op->getReturnType());
                if(temp && !cList.containsRef(temp))
                        cList.append(temp);
                //check parameters
                atl = op->getParmList();
                for(at = atl->first(); at; at = atl->next()) {
                        // temp = (UMLClassifier*)m_document->findUMLObject(Uml::ot_Concept,at->getTypeName());
                        temp = (UMLClassifier*)m_document->findUMLClassifier(at->getTypeName());
                        if(temp && !cList.containsRef(temp))
                                cList.append(temp);
                }

        }

        //attributes
        UMLClass * myClass = dynamic_cast<UMLClass*>(c);
        if(myClass) {
		atl = myClass->getFilteredAttributeList();
                for(at = atl->first(); at; at = atl->next()) {
                        temp=0;
                        // temp =(UMLClassifier*) m_document->findUMLObject(Uml::ot_Concept,at->getTypeName());
                        temp =(UMLClassifier*) m_document->findUMLClassifier(at->getTypeName());
                        if(temp && !cList.containsRef(temp))
                                cList.append(temp);
                }
        }


}

/**
 * Format an output document.
 * @return	QString
 * @param	text 
 * @param	lineprefix 
 * @param	linewidth 
 */
QString CodeGenerator::formatDoc(const QString &text, const QString &linePrefix, int lineWidth) {
        QString output,
        comment(text);

	QString endLine = getNewLineEndingChars();
        comment.replace(QRegExp(endLine)," ");
        comment.simplifyWhiteSpace();

        int index;
        do {
                index = comment.findRev(" ",lineWidth + 1);
                output += linePrefix + comment.left(index) + endLine; // add line
                comment.remove(0, index + 1);                      //and remove processed string, including
                // white space
        } while(index > 0 );

        return output;
}

void CodeGenerator::initFields ( UMLDoc * doc ) {

	m_document = doc;
	setPolicy(new CodeGenerationPolicy(this)); // use just vannila a policy

        m_codeDocumentDictionary.setAutoDelete(false);
	m_codedocumentVector.setAutoDelete(false);
        m_applyToAllRemaining = true;
	lastIDIndex = 0;

	// initial population of our project generator
	// CANT Be done here because we would call pure virtual method
	// of newClassifierDocument (bad!).
	// We should only call from the child 
	// initFromParentDocument();

	connect(doc,SIGNAL(sigObjectCreated(UMLObject*)),this,SLOT(checkAddUMLObject(UMLObject*)));
	connect(doc,SIGNAL(sigObjectRemoved(UMLObject*)),this,SLOT(checkRemoveUMLObject(UMLObject*)));

}

// these are utility methods for accessing the default 
// code gen policy object and should go away when we 
// finally implement the CodeGenDialog class -b.t. 
QString CodeGenerator::getNewLineEndingChars ( ) {
	return getPolicy()->getNewLineEndingChars( );
}

void CodeGenerator::setOutputDirectory(QString d) {
	getPolicy()->setOutputDirectory(d);
}

QString CodeGenerator::outputDirectory() const {
	QDir dir = m_codegeneratorpolicy->getOutputDirectory();
	return dir.absPath(); // dir.dirName();
}

void  CodeGenerator::setOverwritePolicy(CodeGenerationPolicy::OverwritePolicy p) {
	getPolicy()->setOverwritePolicy(p);
}

CodeGenerationPolicy::OverwritePolicy CodeGenerator::overwritePolicy() const {
	return m_codegeneratorpolicy->getOverwritePolicy();
}

void  CodeGenerator::setModifyNamePolicy(CodeGenerationPolicy::ModifyNamePolicy p) {
	getPolicy()->setModifyPolicy(p);
}

CodeGenerationPolicy::ModifyNamePolicy  CodeGenerator::modifyNamePolicy()const {
	return m_codegeneratorpolicy->getModifyPolicy();
}

void CodeGenerator::setIncludeHeadings(bool i) {
	getPolicy()->setIncludeHeadings(i);
}

bool CodeGenerator::includeHeadings() const {
	return m_codegeneratorpolicy->getIncludeHeadings();
}

void CodeGenerator::setHeadingFileDir ( const QString & path) {
	getPolicy()->setHeadingFileDir(path);
}

QString CodeGenerator::headingFileDir() const {
     return m_codegeneratorpolicy->getHeadingFileDir();
}

void CodeGenerator::setForceDoc(bool f) {
	getPolicy()->setCodeVerboseDocumentComments(f);
}

bool CodeGenerator::forceDoc() const {
     return m_codegeneratorpolicy->getCodeVerboseDocumentComments();
}

void CodeGenerator::setForceSections(bool f) {
	getPolicy()->setCodeVerboseSectionComments(f);
}

bool CodeGenerator::forceSections() const {
     return m_codegeneratorpolicy->getCodeVerboseSectionComments();
}


#include "codegenerator.moc"

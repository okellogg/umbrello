/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002                                                    *
 *   Paul Hensgen <phensgen@users.sourceforge.net>                         *
 *   copyright (C) 2003-2008                                               *
 *   Umbrello UML Modeller Authors <uml-devel@uml.sf.net>                  *
 ***************************************************************************/

// own header
#include "codegenerationwizard.h"

// qt includes
#include <QtGui/QWizardPage>

// kde includes
#include <kdebug.h>
#include <klocale.h>
#include <kmessagebox.h>

// local includes
#include "codegenselectpage.h"
#include "codegenoptionspage.h"
#include "codegenstatuspage.h"
#include "classifier.h"
#include "uml.h"


const QSize CodeGenerationWizard::c_pageSize = QSize(520, 530);

/**
 * Constructor. Sets up the wizard and loads the wizard pages.
 * Each wizard page has its own class.
 * @param classList   the list of classes, which have to be generated
 */
CodeGenerationWizard::CodeGenerationWizard(UMLClassifierList *classList)
  : QWizard((QWidget*)UMLApp::app())
  {
    setWizardStyle(QWizard::ModernStyle);  //:TODO: does not work for MAC
    setWindowTitle(i18n("Code Generation Wizard"));
    setPixmap(QWizard::LogoPixmap, QPixmap("../hi48-app-umbrello.png"));  //:TODO: does not work ?

    setPage(SelectionPage, createSelectionPage(classList));
    setPage(OptionsPage, createOptionsPage());
    setPage(StatusPage, createStatusPage());
}

/**
 * Destructor.
 */
CodeGenerationWizard::~CodeGenerationWizard()
{
}

/**
 * Creates the class selection page.
 * @param classList   the list of classes, which have to be generated
 * @return            the wizard page
 */
QWizardPage* CodeGenerationWizard::createSelectionPage(UMLClassifierList *classList)
{
    m_CodeGenSelectPage = new CodeGenSelectPage(this);
    m_CodeGenSelectPage->setClassifierList(classList);
    m_CodeGenSelectPage->setTitle(i18n("Select Classes"));
    m_CodeGenSelectPage->setFixedSize(c_pageSize);
    return m_CodeGenSelectPage;
}

/**
 * Creates the code generation options page, which allows to tune 
 * the code generation by setting some parameters.
 * @return   the wizard page
 */
QWizardPage* CodeGenerationWizard::createOptionsPage()
{
    m_CodeGenOptionsPage = new CodeGenOptionsPage(this);
    m_CodeGenOptionsPage->setTitle(i18n("Code Generation Options"));
    m_CodeGenOptionsPage->setFixedSize(c_pageSize);
    return m_CodeGenOptionsPage;
}

/**
 * Creates the code generation status page, which shows the progress
 * of the generation.
 * @return   the wizard page
 */
QWizardPage* CodeGenerationWizard::createStatusPage()
{
    m_CodeGenStatusPage = new CodeGenStatusPage(this);
    m_CodeGenStatusPage->setTitle(i18n("Status of Code Generation Progress"));
    m_CodeGenStatusPage->setFixedSize(c_pageSize);
    return m_CodeGenStatusPage;
}

/**
 * Returns the list widget, which holds the classes for generation.
 * With this function the list of classes to generate can be transferred
 * from the select page to the status page.
 * @return   the list widget
 */
QListWidget* CodeGenerationWizard::getSelectionListWidget()
{
    return m_CodeGenSelectPage->getSelectionListWidget();
}
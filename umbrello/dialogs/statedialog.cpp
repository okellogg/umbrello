/***************************************************************************
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   copyright (C) 2002-2014                                               *
 *   Umbrello UML Modeller Authors <umbrello-devel@kde.org>                *
 ***************************************************************************/

// own header
#include "statedialog.h"

// local includes
#include "activitypage.h"
#include "documentationwidget.h"
#include "umlview.h"
#include "umlscene.h"
#include "umlviewlist.h"
#include "umldoc.h"
#include "uml.h"
#include "statewidget.h"
#include "dialog_utils.h"
#include "icon_utils.h"

// kde includes
#include <KComboBox>
#include <klineedit.h>
#include <KLocalizedString>

// qt includes
#include <QFrame>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>

/**
 * Constructor.
 */
StateDialog::StateDialog(QWidget * parent, StateWidget * pWidget)
  : MultiPageDialogBase(parent),
    m_pActivityPage(0),
    m_pStateWidget(pWidget),
    m_bChangesMade(false),
    pageActivity(0)
{
    setCaption(i18n("Properties"));
    setupPages();
    connect(this, SIGNAL(okClicked()), this, SLOT(slotOk()));
    connect(this, SIGNAL(applyClicked()), this, SLOT(slotApply()));
}

/**
 * Entered when OK button pressed.
 */
void StateDialog::slotOk()
{
    applyPage(pageGeneral);
    applyPage(pageFont);
    applyPage(pageActivity);
    applyPage(pageStyle);
    accept();
}

/**
 * Entered when Apply button pressed.
 */
void StateDialog::slotApply()
{
    applyPage(currentPage());
}

/**
 * Sets up the pages of the dialog.
 */
void StateDialog::setupPages()
{
    setupGeneralPage();
    if (m_pStateWidget->stateType() == StateWidget::Normal) {
        setupActivityPage();
    }
    pageStyle = setupStylePage(m_pStateWidget);
    pageFont = setupFontPage(m_pStateWidget);
}

/**
 * Applies changes to the given page.
 */
void StateDialog::applyPage(KPageWidgetItem*item)
{
    m_bChangesMade = true;
    if (item == pageGeneral) {
        if (m_pStateWidget->stateType() == StateWidget::Combined) {
            QString name = m_GenPageWidgets.diagramLinkCB->currentText();
            Uml::ID::Type id;
            foreach (UMLView *view, UMLApp::app()->document()->viewIterator()) {
                if (name == view->umlScene()->name())
                    id = view->umlScene()->ID();
            }
            m_pStateWidget->setDiagramLink(id);
        }
        m_pStateWidget->setName(m_GenPageWidgets.nameLE->text());
        m_GenPageWidgets.docWidget->apply();
    }
    else if (item == pageActivity) {
        if (m_pActivityPage) {
            m_pActivityPage->updateActivities();
        }
    }
    else if (item == pageStyle) {
        applyStylePage();
    }
    else if (item == pageFont) {
        applyFontPage(m_pStateWidget);
    }
}

/**
 * Sets up the general page of the dialog.
 */
void StateDialog::setupGeneralPage()
{
    StateWidget::StateType type = m_pStateWidget->stateType();

    QWidget* page = new QWidget();
    QVBoxLayout* topLayout = new QVBoxLayout();
    page->setLayout(topLayout);

    pageGeneral = createPage(i18nc("general page", "General"), i18n("General Properties"),
                             Icon_Utils::it_Properties_General, page);

    m_GenPageWidgets.generalGB = new QGroupBox(i18n("Properties"));
    topLayout->addWidget(m_GenPageWidgets.generalGB);

    QGridLayout * generalLayout = new QGridLayout(m_GenPageWidgets.generalGB);
    generalLayout->setSpacing(spacingHint());
    generalLayout->setMargin(fontMetrics().height());

    QString typeStr;
    switch (type) {
    case StateWidget::Initial:
        typeStr = i18nc("initial state in statechart", "Initial state");
        break;
    case StateWidget::Normal:
        typeStr = i18nc("state in statechart", "State");
        break;
    case StateWidget::End:
        typeStr = i18nc("end state in statechart", "End state");
        break;
    case StateWidget::Fork:
        typeStr = i18nc("fork state in statechart", "Fork");
        break;
    case StateWidget::Join:
        typeStr = i18nc("join state in statechart", "Join");
        break;
    case StateWidget::Junction:
        typeStr = i18nc("junction state in statechart", "Junction");
        break;
    case StateWidget::DeepHistory:
        typeStr = i18nc("deep history state in statechart", "DeepHistory");
        break;
    case StateWidget::ShallowHistory:
        typeStr = i18nc("shallow history state in statechart", "ShallowHistory");
        break;
    case StateWidget::Choice:
        typeStr = i18nc("choice state in statechart", "Choice");
        break;
    case StateWidget::Combined:
        typeStr = i18nc("combined state in statechart", "Combined");
        break;
    default:
        typeStr = QString::fromLatin1("???");
        break;
    }
    int row = 0;
    Dialog_Utils::makeLabeledEditField(generalLayout, row++,
                                       m_GenPageWidgets.typeL, i18n("State type:"),
                                       m_GenPageWidgets.typeLE, typeStr);
    m_GenPageWidgets.typeLE->setEnabled(false);

    Dialog_Utils::makeLabeledEditField(generalLayout, row++,
                                       m_GenPageWidgets.nameL, i18n("State name:"),
                                       m_GenPageWidgets.nameLE);

    if (type != StateWidget::Normal && type != StateWidget::Combined) {
        m_GenPageWidgets.nameLE->setEnabled(false);
        m_GenPageWidgets.nameLE->setText(QString());
    } else
        m_GenPageWidgets.nameLE->setText(m_pStateWidget->name());

    if (type == StateWidget::Combined) {
        m_GenPageWidgets.diagramLinkL = new QLabel(i18n("Linked diagram:"));
        generalLayout->addWidget(m_GenPageWidgets.diagramLinkL, row, 0);

        m_GenPageWidgets.diagramLinkCB = new KComboBox;
        generalLayout->addWidget(m_GenPageWidgets.diagramLinkCB, row++, 1);

        QStringList diagrams;
        QString name;
        foreach (UMLView *view, UMLApp::app()->document()->viewIterator()) {
            if (view->umlScene()->ID() == m_pStateWidget->diagramLink())
                name = view->umlScene()->name();
            if (view->umlScene()->ID() == m_pStateWidget->umlScene()->ID())
                continue;
            diagrams << view->umlScene()->name();
        }
        diagrams.sort();
        m_GenPageWidgets.diagramLinkCB->insertItems(-1, diagrams);
        int currentIndex = m_GenPageWidgets.diagramLinkCB->findText(name);
        if (currentIndex > -1) {
            m_GenPageWidgets.diagramLinkCB->setCurrentIndex(currentIndex);
        }
        m_GenPageWidgets.diagramLinkCB->completionObject()->addItem(name);
    }

    m_GenPageWidgets.docWidget = new DocumentationWidget(m_pStateWidget);
    generalLayout->addWidget(m_GenPageWidgets.docWidget, row, 0, 1, 2);
}

/**
 * Sets up the activity page.
 */
void StateDialog::setupActivityPage()
{
    m_pActivityPage = new ActivityPage(0, m_pStateWidget);
    pageActivity = createPage(i18n("Activities"), i18n("Activities"),
                              Icon_Utils::it_Properties_Activities, m_pActivityPage);
}


/*
    SPDX-License-Identifier: GPL-2.0-or-later

    SPDX-FileCopyrightText: 2006 Gael de Chalendar (aka Kleag) kleag@free.fr
    copyright (C) 2006-2020
    Umbrello UML Modeller Authors <umbrello-devel@kde.org>
*/

#ifndef XHTMLGENERATOR_H
#define XHTMLGENERATOR_H

#include "basictypes.h"

#if QT_VERSION < 0x050000
#include <kurl.h>
#endif

#include <QObject>
#if QT_VERSION >= 0x050000
#include <QUrl>
#endif

class UMLDoc;

class Docbook2XhtmlGeneratorJob;

/**
 * class XhtmlGenerator is a documentation generator for UML documents.
 * It uses first @ref DocbookGenerator to convert the XMI generated by
 * UMLDoc::saveToXMI1 to docbook and next libxslt through
 * the XSLT file stored in resources to convert the docbook file to XHTML.
 * The latter uses the XSLT available on the Web at
 * http://docbook.sourceforge.net/release/xsl/snapshot/html/docbook.xsl
 *
 * @todo allow to specify the destination and ensure that it works with distant
 * ones
 */
class XhtmlGenerator : public QObject
{
    Q_OBJECT
public:

    XhtmlGenerator();
    virtual ~XhtmlGenerator();

    bool generateXhtmlForProject();
#if QT_VERSION >= 0x050000
    bool generateXhtmlForProjectInto(const QUrl& destDir);
#else
    bool generateXhtmlForProjectInto(const KUrl& destDir);
#endif
    static QString customXslFile();

signals:
    void finished(bool status);

protected slots:

    void slotDocbookToXhtml(bool status);
    void slotHtmlGenerated(const QString& tmpFileName);

    void threadFinished();

private:

    Docbook2XhtmlGeneratorJob* m_d2xg;

    bool m_pStatus;
    bool m_pThreadFinished;

#if QT_VERSION >= 0x050000
    QUrl m_destDir;  ///< Destination directory where the final documentation will be written.
#else
    KUrl m_destDir;  ///< Destination directory where the final documentation will be written.
#endif
    UMLDoc* m_umlDoc;
};

#endif // XHTMLGENERATOR_H

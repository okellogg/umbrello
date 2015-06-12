/*
    Copyright 2015  Ralf Habacker  <ralf.habacker@freenet.de>

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License or (at your option) version 3 or any later version
    accepted by the membership of KDE e.V. (or its successor approved
    by the membership of KDE e.V.), which shall act as a proxy
    defined in Section 14 of version 3 of the license.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "testbase.h"

#include "uml.h"

// qt includes
#include <QApplication>

static QApplication *app;
static UMLApp *umlApp;

TestBase::TestBase(QObject *parent)
  : QObject(parent)
{
}

void TestBase::initTestCase()
{
    // FIXME  we need to force QTEST_MAIN to use the GUI variant
    char **argv = { 0 };
    int argc = 0;
    app = new QApplication(argc , argv);
    QWidget *w = new QWidget;
    umlApp = new UMLApp(w);
}

void TestBase::cleanupTestCase()
{
    delete umlApp;
    delete app;
}

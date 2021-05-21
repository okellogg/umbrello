/*
    SPDX-License-Identifier: GPL-2.0-or-later

    SPDX-FileCopyrightText: 2003 Brian Thomas <thomas@mail630.gsfc.nasa.gov>
    copyright (C) 2003-2020
    Umbrello UML Modeller Authors <umbrello-devel@kde.org>
*/

#ifndef JAVACODEGENERATIONPOLICY_H
#define JAVACODEGENERATIONPOLICY_H

#include "codegenpolicyext.h"
#include "codegenerationpolicy.h"

#include <QString>

class CodeGenerationPolicyPage;

class JavaCodeGenerationPolicy : public CodeGenPolicyExt
{
    Q_OBJECT
public:

    //JavaCodeGenerationPolicy(CodeGenerationPolicy * defaults = 0);
    JavaCodeGenerationPolicy();

    virtual ~JavaCodeGenerationPolicy();

    void setAutoGenerateAttribAccessors(bool var);
    bool getAutoGenerateAttribAccessors();

    void setAutoGenerateAssocAccessors(bool var);
    bool getAutoGenerateAssocAccessors();

    virtual void setDefaults(CodeGenPolicyExt * defaults, bool emitUpdateSignal = true);
    virtual void setDefaults(bool emitUpdateSignal = true);

    CodeGenerationPolicyPage * createPage (QWidget *parent = 0, const char * name = 0);

private:

    CodeGenerationPolicy *m_commonPolicy;

    void init();
};

#endif // JAVACODEGENERATIONPOLICY_H

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "class.h"
#include "umldoc.h"
#include "umllistview.h"
#include "umllistviewitem.h"
#include "umlview.h"
#include <kapplication.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <kdebug.h>

UMLListViewItem::UMLListViewItem( UMLListView * parent, QString name,
                                  Uml::ListView_Type t, UMLObject* o)
  : QListViewItem(parent, name) {
	m_bCreating = false;
	s_pListView = parent;
	m_Data.setType( t );
	m_Data.setUMLObject( o );
	if( !o )
		m_Data.setID( -1 );
	else
		m_Data.setID( o -> getID() );
	setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Home ) );
	m_Data.setListViewItem( this );
	setText( name );
	setRenameEnabled( 0, false );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
UMLListViewItem::UMLListViewItem(UMLListViewItem * parent, QString name, Uml::ListView_Type t,UMLObject*o) : QListViewItem(parent, name) {
	m_bCreating = false;
	m_Data.setType( t );
	m_Data.setUMLObject( o );
	if( !o ) {
		m_Data.setID( -1 );
		updateFolder();
	} else {
		updateObject();
		m_Data.setID( o -> getID() );
	}
	if( t == Uml::lvt_Logical_View || t == Uml::lvt_UseCase_View ||
	    t == Uml::lvt_Component_View || t == Uml::lvt_Deployment_View )
		setRenameEnabled( 0, false );
	else
		setRenameEnabled( 0, true );
	m_Data.setListViewItem(this);
	setText( name );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
UMLListViewItem::UMLListViewItem(UMLListViewItem * parent, QString name, Uml::ListView_Type t,int id) : QListViewItem(parent, name) {
	m_bCreating = false;
	m_Data.setType( t );
	m_Data.setUMLObject( 0 );
	m_Data.setID( id );
	setPixmap(0, s_pListView -> getPixmap( UMLListView::it_Diagram ) );
	m_Data.setListViewItem( this );
	/*
		Constructor also used by folder so just make sure we don't need to
		to set pixmap to folder.  doesn't hurt diagrams.
	*/
	updateFolder();
	setText( name );
	setRenameEnabled( 0, true );
}
////////////////////////////////////////////////////////////////////////////////////////////////////
UMLListViewItem::~UMLListViewItem() {}
////////////////////////////////////////////////////////////////////////////////////////////////////
Uml::ListView_Type UMLListViewItem::getType() const {
	return m_Data.getType();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
int UMLListViewItem::getID() {
	return m_Data.getID();
}
////////////////////////////////////////////////////////////////////////////////////////////////////
void UMLListViewItem::updateObject() {
	if( !m_Data.getUMLObject() )
		return;

	Uml::Scope scope = m_Data.getUMLObject() -> getScope();
	setText( m_Data.getUMLObject() -> getName());

	switch(m_Data.getUMLObject() -> getBaseType()) {
		case Uml::ot_Actor:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Actor ) );
			break;

		case Uml::ot_UseCase:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_UseCase ) );
			break;

		case Uml::ot_Class:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Class ) );
			break;

		case Uml::ot_Template:
			setPixmap( 0, s_pListView->getPixmap(UMLListView::it_Template) );
			break;

		case Uml::ot_Package:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Package ) );
			break;

		case Uml::ot_Component:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Component ) );
			break;

		case Uml::ot_Node:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Node ) );
			break;

		case Uml::ot_Artifact:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Artifact ) );
			break;

		case Uml::ot_Interface:
			setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Interface ) );
			break;

		case Uml::ot_Operation:
			if( scope == Uml::Public )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Public_Method ) );
			else if( scope == Uml::Private )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Private_Method ) );
			else
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Protected_Method ) );
			break;

		case Uml::ot_Attribute:
			if( scope == Uml::Public )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Public_Attribute ) );
			else if( scope == Uml::Private )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Private_Attribute ) );
			else
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Protected_Attribute ) );
			break;
		default:
			break;
	}//end switch
}

UMLListViewItemData* UMLListViewItem::getdata() {
	return &m_Data;

}

void UMLListViewItem::setdata(UMLListViewItemData& NewData) {
	m_Data = NewData;
}

void UMLListViewItem::updateFolder() {
	switch( m_Data.getType() ) {
		case Uml::lvt_UseCase_View:
		case Uml::lvt_UseCase_Folder:
			if( isOpen() )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Folder_Grey_Open ) );
			else
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Folder_Grey ) );
			break;

		case Uml::lvt_Logical_View:
		case Uml::lvt_Logical_Folder:
			if( isOpen() )
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Folder_Green_Open ) );
			else
				setPixmap( 0, s_pListView -> getPixmap( UMLListView::it_Folder_Green ) );
			break;

		case Uml::lvt_Component_View:
		case Uml::lvt_Component_Folder:
			if ( isOpen() ) {
				setPixmap(0, s_pListView->getPixmap(UMLListView::it_Folder_Red_Open) );
			} else {
				setPixmap(0, s_pListView->getPixmap(UMLListView::it_Folder_Red) );
			}
			break;

		case Uml::lvt_Deployment_View:
		case Uml::lvt_Deployment_Folder:
			if ( isOpen() ) {
				setPixmap(0, s_pListView->getPixmap(UMLListView::it_Folder_Violet_Open) );
			} else {
				setPixmap(0, s_pListView->getPixmap(UMLListView::it_Folder_Violet) );
			}
			break;

		default:
			break;
	}
}

void UMLListViewItem::setOpen( bool open ) {
	QListViewItem::setOpen( open );
	updateFolder();
}

void UMLListViewItem::setText(QString newText) {
	m_Data.setLabel(newText);
	QListViewItem::setText(0, newText);
}

void UMLListViewItem::okRename( int col ) {

	if (m_bCreating) {
		m_bCreating = false;
		QListViewItem::okRename( col );
		if ( s_pListView -> slotItemRenamed( this, 0 ) ) {
			m_Data.setLabel( text(col) );
		} else {
			startRename(0);
		}
		return;
	}
	QListViewItem::okRename( col );
	QString newText = text( col );
	UMLObject * object = 0;
	UMLClassifier * parent = 0;
	UMLView * view = 0, * anotherView;
	if( newText.length() == 0 ) {
		KMessageBox::error( kapp->mainWidget() ,
				    i18n("The name you entered was invalid!\nRenaming process has been cancelled."),
		                    i18n("Name Not Valid") );
		setText( m_Data.getLabel() );
		return;
	}
	switch( m_Data.getType( ) ) {
		case Uml::lvt_UseCase:
		case Uml::lvt_Actor:
		case Uml::lvt_Class:
		case Uml::lvt_Package:
		case Uml::lvt_Interface:
			object = m_Data.getUMLObject();
			if( object ) {
				object = s_pListView -> getDocument() -> findUMLObject( object -> getBaseType(), newText );
				if( object && object == m_Data.getUMLObject() )
					object = 0;
				if( !object ) {
					m_Data.getUMLObject() -> setName( newText );
					m_Data.setLabel( newText );
					return;
				}
			}
			break;

		case Uml::lvt_Operation:
			object = m_Data.getUMLObject();
			if( object ) {
				parent = static_cast<UMLClassifier *>( object -> parent() );
				//see if op already has that name and not the op/att we are renaming
				//then give a warning about the name being the same
				QPtrList<UMLObject> list = parent -> findChildObject( object -> getBaseType(), newText );
				if(list.isEmpty() || (!list.isEmpty() && KMessageBox::warningYesNo( kapp -> mainWidget() ,
				                      i18n( "The name you entered was not unique!\nIs this what you wanted?" ),
				                      i18n( "Name Not Unique" ) ) == KMessageBox::Yes )) {
					object -> setName( newText );
					m_Data.setLabel( newText );
					return;
				}
				setText( m_Data.getLabel() );
				return;
			}
			break;

		case Uml::lvt_Attribute:
			object = m_Data.getUMLObject();
			if( object ) {
				parent = static_cast<UMLClass*>( object -> parent() );
				QPtrList<UMLObject> list = parent -> findChildObject( object -> getBaseType(), newText );
				if (list.isEmpty()) {
					object -> setName( newText );
					m_Data.setLabel( newText );
					return;
				}
			}
			break;

		case Uml::lvt_UseCase_Diagram:
		case Uml::lvt_Class_Diagram:
		case Uml::lvt_Sequence_Diagram:
		case Uml::lvt_Collaboration_Diagram:
		case Uml::lvt_State_Diagram:
		case Uml::lvt_Activity_Diagram:
		case Uml::lvt_Component_Diagram:
		case Uml::lvt_Deployment_Diagram:
			view = s_pListView -> getDocument() -> findView( m_Data.getID() );
			if( view ) {
				anotherView = s_pListView -> getDocument() -> findView( view -> getType(), newText );
				if( anotherView && anotherView -> getID() == m_Data.getID() )
					anotherView = 0;
				if( !anotherView ) {
					view->UMLViewData::setName( newText );
					m_Data.setLabel( newText );
					s_pListView->getDocument()->signalDiagramRenamed(view);
					return;
				}
			}
			break;
		case Uml::lvt_UseCase_Folder:
		case Uml::lvt_Logical_Folder:
		case Uml::lvt_Component_Folder:
		case Uml::lvt_Deployment_Folder:
			m_Data.setLabel( newText );
			return;
			break;
		default:
			break;
	}
	KMessageBox::error( kapp->mainWidget() ,
			    i18n("The name you entered was invalid!\nRenaming process has been cancelled."),
	                    i18n("Name Not Valid") );
	setText( m_Data.getLabel() );
}

void UMLListViewItem::cancelRename(int col) {
	QListViewItem::cancelRename(col);
	if (m_bCreating) {
		s_pListView->cancelRename(this);
	}
}

// sort the listview items by type and alphabetically
int UMLListViewItem::compare(QListViewItem *other, int col, bool ascending) const
{
	Uml::ListView_Type ourType = getType();
	Uml::ListView_Type otherType = static_cast<UMLListViewItem*>( other )->getType();

	if ( ourType == otherType )
		return key( col, ascending ).compare( other->key( col, ascending) );

	if ( ourType < otherType )
		return -1;
	if ( ourType > otherType )
		return 1;

	return 0;
}
UMLListView* UMLListViewItem::s_pListView = 0;

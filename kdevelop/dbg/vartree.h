/***************************************************************************
                          vartree.h  -  description                              
                             -------------------                                         
    begin                : Sun Aug 8 1999                                           
    copyright            : (C) 1999 by John Birch
    email                : jb.nz@writeme.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#ifndef VARTREE_H
#define VARTREE_H

#include <qlistview.h>
#include <qwidget.h>

enum {VarNameCol = 0, ValueCol = 1};
enum DataType { typeUnknown, typeValue, typePointer, typeReference,
                typeStruct, typeArray, typeQString, typeWhitespace,
                typeName };

class VarFrameRoot;
class WatchRoot;
class VarItem;
class VarTree;

class KLineEdit;

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarViewer : public QWidget
{
  Q_OBJECT

public:
  VarViewer( QWidget *parent=0, const char *name=0 );
  VarTree* varTree()      { return varTree_; }
  void clear();

public slots:
  virtual void setEnabled(bool bEnabled);

private slots:
  void slotAddWatchVariable();

private:
  VarTree* varTree_;
  KLineEdit* watchVarEntry_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarTree : public QListView
{
  Q_OBJECT

public:
  VarTree( QWidget *parent=0, const char *name=0 );
  virtual ~VarTree();

  QListViewItem* lastChild() const;

  int activeFlag() const                { return activeFlag_; }
  void setActiveFlag()                  { activeFlag_++; }

  QListViewItem* findRoot(QListViewItem* item) const;
  VarFrameRoot* findFrame(int frameNo, int threadNo) const;
  WatchRoot* findWatch();
  void setCurrentThread(int currentThread)     { currentThread_ = currentThread; }

  // Remove items that are not active
  void trim();
  void trimExcessFrames();

  void emitExpandItem(VarItem* item)            { emit expandItem(item); }
  void emitExpandUserItem(VarItem* item, const QCString& request)
                                                { emit expandUserItem(item, request);}
  void emitSetLocalViewState(bool localsOn, int frameNo, int threadNo);

public slots:
  void slotAddWatchVariable(const QString& watchVar);

private slots:
  void slotRightButtonClicked(  QListViewItem* selectedItem, const QPoint &, int);
  void slotToggleWatchpoint();
  void slotRemoveWatchVariable();

signals:
  void toggleWatchVariable(const QString& varName);
  void toggleWatchpoint(const QString& varName);
  void expandItem(VarItem* item);
  void expandUserItem(VarItem* item, const QCString& userRequest);
  void setLocalViewState(bool localsOn);
  void selectFrame(int frameNo, int threadNo);

private:
  int activeFlag_;
  int currentThread_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class TrimmableItem : public QListViewItem
{
public:

  TrimmableItem(VarTree* parent);
  TrimmableItem(TrimmableItem* parent);

  virtual ~TrimmableItem();

  virtual void trim();
  virtual QString getName() const         { return QString(text(VarNameCol)); }
  virtual TrimmableItem* findMatch(const QString& match, DataType type) const;
  QListViewItem* lastChild() const;
  int  rootActiveFlag() const;
  void setActive()                        { activeFlag_ = rootActiveFlag(); }
  bool isActive() const                   { return activeFlag_ == rootActiveFlag(); }
  QString getValue() const                { return QString(text(ValueCol)); }
  bool isTrimmable() const;
  void waitingForData ()                  { waitingForData_ = true; }

  virtual void updateValue(char* /* buf */);
  virtual DataType getDataType() const;

  virtual void setCache(const QCString& value);
  virtual QCString getCache();
  virtual QString key ( int column, bool ascending ) const;

private:
  int activeFlag_;
  bool waitingForData_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarItem : public TrimmableItem
{
public:

  VarItem( TrimmableItem* parent, const QString& varName, DataType dataType);

  virtual ~VarItem();

  QString varPath() const;
  QString fullName() const;
  DataType getDataType() const;

  void updateValue(char* data);

  void setCache(const QCString& value);
  QCString getCache();

  void setOpen(bool open);
  void setText (int column, const QString& text);

private:
  void checkForRequests();
  void paintCell( QPainter * p, const QColorGroup & cg,
                          int column, int width, int align );

private:
  QCString  cache_;
  DataType  dataType_;
  bool      highlight_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class VarFrameRoot : public TrimmableItem
{
public:
  VarFrameRoot(VarTree* parent, int frameNo, int threadNo);
  virtual ~VarFrameRoot();

  void setLocals(char* locals);
  void setParams(const QCString& params);

  void setOpen(bool open);

//  int  getFrameNo() const                     { return frameNo_; }
  void setFrameName(const QString& frameName)
            { setText(VarNameCol, frameName); setText(ValueCol, ""); }

  bool needLocals() const                     { return needLocals_; }
  bool matchDetails(int frameNo, int threadNo);

private:
  bool    needLocals_;
  int     frameNo_;
  int     threadNo_;
  QCString params_;
  QCString locals_;
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

class WatchRoot : public TrimmableItem
{
public:
  WatchRoot(VarTree* parent);
  virtual ~WatchRoot();
  
  void requestWatchVars();
};

/***************************************************************************/
/***************************************************************************/
/***************************************************************************/

#endif

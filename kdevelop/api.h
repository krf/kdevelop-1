#ifndef _API_H_
#define _API_H_


#include "kdevapi.h"


class ClassStore;


class API : public KDevApi
{
public:

  virtual KDevTopLevel *topLevel();
  virtual KDevPartController *partController();
  virtual KDevCore *core();
  virtual ClassStore *classStore();
  virtual ClassStore *ccClassStore();

  
  static API *getInstance();

  ~API();

  
protected:

  API();

  
private:

  static API *s_instance;

  ClassStore *m_classStore, *m_ccClassStore;

};


#endif

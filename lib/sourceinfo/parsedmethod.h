/***************************************************************************
                          parsedmethod.h  -  description
                             -------------------
    begin                : Mon Mar 15 1999
    copyright            : (C) 1999 by Jonas Nordin
    email                : jonas.nordin@syncom.se
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   * 
 *                                                                         *
 ***************************************************************************/

#ifndef _PARSEDMETHOD_H_
#define _PARSEDMETHOD_H_

#include <qlist.h>
#include "parsedattribute.h"
#include "parsedargument.h"


/**
 * This object is the general abstraction for a method that
 * has been parsed by the classparser.
 * @author Jonas Nordin
 */
class ParsedMethod : public ParsedAttribute
{
public:
    ParsedMethod();
    ~ParsedMethod();
    
public:
    
    /** The methods arguments(if any) */
    QList<ParsedArgument> arguments;
    
    /** Is this methods declared virtual? */
    bool isVirtual;
    
    /** Is this method declared as a pure virtual method(method()=0) */
    bool isPure;
    
    /** Is this a slot? */
    bool isSlot;
    
    /** Is this a signal? */
    bool isSignal;
    
    /** Is this a constructor? */
    bool isConstructor;
    
    /** Is this a destructor? */
    bool isDestructor;
    
public:
    
    /** Adds an argument to this method. */
    void addArgument(ParsedArgument *anArg);
    
    /**
     * Sets the status if this is a virtual method.
     * @param aState The new state.
     */
    void setIsVirtual(bool aState=true)
    { isVirtual = aState; }

    /**
     * Sets the status if this is a pure virtual method.
     * @param aState The new state. 
     */
    void setIsPure(bool aState=true)
    { isPure = aState; }
    
    /**
     * Sets the status if this is a slot. 
     * @param aState The new state.
     */
    void setIsSlot(bool aState=true)
    { isSlot = aState; }
    
    /**
     * Sets the status if this is a signal. 
     * @param aState The new state.
     */
    void setIsSignal(bool aState=true)
    { isSignal = aState; }
    
    /**
     * Sets this method as a constructor.
     * @param aState The new state.
     */
    void setIsConstructor(bool aState=true)
    { isConstructor = aState; }
    
    /**
     * Sets this method as a destructor.
     * @param aState The new state.
     */
    void setIsDestructor(bool aState=true)
    { isDestructor = aState; }
    
    /** Returns the object as a string(for tooltips etc) */
    QString asString();
    
    /** Outputs this object to stdout */
    void out();
    
    /**
     * Makes this object a copy of the supplied object. 
     * @param aMethod Method to copy.
     */
    virtual void copy(ParsedMethod *aMethod);
    
    /** Returns a string made for persistant storage. */
    virtual QString asPersistantString();
    
    /**
     * Is the supplied method equal to this one
     * (regarding type, name and signature)? */
    bool isEqual(ParsedMethod *method);
    
};

#endif

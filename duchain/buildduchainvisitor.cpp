/*
 *    This file is part of KDevelop
 *
 *    Copyright 2013 Olivier de Gaalon <olivier.jg@gmail.com>
 *
 *    This library is free software; you can redistribute it and/or
 *    modify it under the terms of the GNU Library General Public
 *    License as published by the Free Software Foundation; either
 *    version 2 of the License, or (at your option) any later version.
 *
 *    This library is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *    Library General Public License for more details.
 *
 *    You should have received a copy of the GNU Library General Public License
 *    along with this library; see the file COPYING.LIB.  If not, write to
 *    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 *    Boston, MA 02110-1301, USA.
 */

#include "buildduchainvisitor.h"
#include "declarationbuilder.h"
#include "contextbuilder.h"
#include "clangtypes.h"

QDebug &operator<<(QDebug &dbg, CXCursor cursor)
{
    if (clang_Cursor_isNull(cursor))
        dbg << "CXCursor (NULL)";
    else
        dbg << "CXCursor"
            << ClangString(clang_getCursorKindSpelling(clang_getCursorKind(cursor)))
            << ClangRange(clang_Cursor_getSpellingNameRange(cursor, 0, 0)).toDocumentRange()
            << ClangString(clang_getCursorSpelling(cursor));
    return dbg;
}


using namespace KDevelop;

namespace {

enum CursorBuildType
{
    CBT_Declaration,
    CBT_Context,
    CBT_Use,
    CBT_CtxtDecl,
    CBT_IdCtxtDecl
};

template<CXCursorKind, CursorBuildType> struct CursorBuilder {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        Q_ASSERT(false);
        return CXChildVisit_Break;
    }
};
template<CXCursorKind kind> struct CursorBuilder<kind, CBT_Declaration> {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        Identifier id(IndexedString(ClangString(clang_getCursorSpelling(cursor))));
        DeclarationBuilder::build<kind>(cursor, id, parentContext);
        return CXChildVisit_Continue; ///Hmm...
    }
};
template<CXCursorKind kind> struct CursorBuilder<kind, CBT_Context> {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        ContextBuilder::build<kind>(cursor, parentContext);
        return CXChildVisit_Continue;
    }
};
template<CXCursorKind kind> struct CursorBuilder<kind, CBT_Use> {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        //TODO...
        return CXChildVisit_Continue;
    }
};
template<CXCursorKind kind> struct CursorBuilder<kind, CBT_CtxtDecl> {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        Identifier id(IndexedString(ClangString(clang_getCursorSpelling(cursor))));
        DeclarationBuilder::build<kind>(cursor, id,
            ContextBuilder::build<kind>(cursor, parentContext), parentContext);
        return CXChildVisit_Continue;
    }
};
template<CXCursorKind kind> struct CursorBuilder<kind, CBT_IdCtxtDecl> {
    static CXChildVisitResult build(CXCursor cursor, DUContext *parentContext) {
        Identifier id(IndexedString(ClangString(clang_getCursorSpelling(cursor))));
        DeclarationBuilder::build<kind>(cursor, id,
            ContextBuilder::build<kind>(cursor, id, parentContext), parentContext);
        return CXChildVisit_Continue;
    }
};

}

static CXChildVisitResult buildUseForCursor(CXCursor cursor, DUContext *parentContext)
{
    auto cursorKind = clang_getCursorKind(cursor);
    auto isRefExpr = cursorKind == CXCursor_DeclRefExpr || cursorKind == CXCursor_MemberRefExpr;
    if (!clang_isReference(cursorKind) && !isRefExpr)
        return CXChildVisit_Break;

    auto referenced = clang_getCursorReferenced(cursor);
    auto refLoc = clang_getCursorLocation(referenced);
    CXFile file;
    clang_getFileLocation(refLoc, &file, nullptr, nullptr, nullptr);
    auto url = IndexedString(ClangString(clang_getFileName(file)));
    auto refCursor = CursorInRevision(ClangLocation(refLoc));

    //TODO: handle uses of declarations in other topContexts
    DUChainWriteLocker lock;
    TopDUContext *top = parentContext->topContext();
    if (DUContext *local = top->findContextAt(refCursor)) {
        if (Declaration *used = local->findDeclarationAt(refCursor)) {
            auto usedIndex = top->indexForUsedDeclaration(used);
            auto useRange = ClangRange(clang_getCursorReferenceNameRange(cursor, CXNameRange_WantSinglePiece, 0));
            parentContext->createUse(usedIndex, useRange.toRangeInRevision());
        }
    }

    return isRefExpr ? CXChildVisit_Recurse : CXChildVisit_Continue;
}

CXChildVisitResult visit(CXCursor cursor, CXCursor /*parent*/, CXClientData d)
{
    auto parentContext = static_cast<DUContext*>(d);

    CXChildVisitResult useResult = buildUseForCursor(cursor, parentContext);
    if (useResult != CXChildVisit_Break)
        return useResult;

    //Use to map cursor kinds to build profiles
    #define UseCursorKind(CursorKind, CursorBuildType)\
    case CursorKind: return CursorBuilder<CursorKind, CursorBuildType>::build(cursor, parentContext);

    //Use to map cursor kinds that can be either declarations or definitions to alternate profiles
    #define UseCursorDeclDef(CursorKind, CursorBuildTypeDecl, CursorBuildTypeDef)\
    case CursorKind: return clang_isCursorDefinition(cursor) ?\
        CursorBuilder<CursorKind, CursorBuildTypeDef>::build(cursor, parentContext) :\
        CursorBuilder<CursorKind, CursorBuildTypeDecl>::build(cursor, parentContext);
    switch (clang_getCursorKind(cursor))
    {
    UseCursorKind(CXCursor_UnexposedDecl, CBT_Declaration);
    UseCursorDeclDef(CXCursor_StructDecl, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorDeclDef(CXCursor_UnionDecl, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorDeclDef(CXCursor_ClassDecl, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorDeclDef(CXCursor_EnumDecl, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorKind(CXCursor_FieldDecl, CBT_Declaration);
    UseCursorKind(CXCursor_EnumConstantDecl, CBT_Declaration);
    UseCursorDeclDef(CXCursor_FunctionDecl, CBT_Declaration, CBT_CtxtDecl);
    UseCursorKind(CXCursor_VarDecl, CBT_Declaration);
    UseCursorKind(CXCursor_ParmDecl, CBT_Declaration);
    UseCursorKind(CXCursor_TypedefDecl, CBT_Declaration);
    UseCursorDeclDef(CXCursor_CXXMethod, CBT_Declaration, CBT_CtxtDecl);
    UseCursorKind(CXCursor_Namespace, CBT_IdCtxtDecl);
    UseCursorDeclDef(CXCursor_Constructor, CBT_Declaration, CBT_CtxtDecl);
    UseCursorDeclDef(CXCursor_Destructor, CBT_Declaration, CBT_CtxtDecl);
    UseCursorDeclDef(CXCursor_ConversionFunction, CBT_Declaration, CBT_CtxtDecl);
    UseCursorKind(CXCursor_TemplateTypeParameter, CBT_Declaration);
    UseCursorKind(CXCursor_NonTypeTemplateParameter, CBT_Declaration);
    UseCursorKind(CXCursor_TemplateTemplateParameter, CBT_Declaration);
    UseCursorDeclDef(CXCursor_FunctionTemplate, CBT_Declaration, CBT_CtxtDecl);
    UseCursorDeclDef(CXCursor_ClassTemplate, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorDeclDef(CXCursor_ClassTemplatePartialSpecialization, CBT_Declaration, CBT_IdCtxtDecl);
    UseCursorKind(CXCursor_NamespaceAlias, CBT_Declaration);
    UseCursorKind(CXCursor_UsingDirective, CBT_Declaration); //Should we make a declaration or just a use?
    UseCursorKind(CXCursor_UsingDeclaration, CBT_Declaration); //Should we make a declaration or just a use?
    UseCursorKind(CXCursor_TypeAliasDecl, CBT_Declaration);
    default: return CXChildVisit_Recurse;
    }
}

void BuildDUChainVisitor::visit(CXTranslationUnit unit, ReferencedTopDUContext top)
{
    auto cursor = clang_getTranslationUnitCursor(unit);
    clang_visitChildren(cursor, &::visit, top.data());
}

#ifndef ASTINDEXER_H
#define ASTINDEXER_H

#include "IndexerContext.h"

#include "clang/AST/RecursiveASTVisitor.h"

class ASTIndexer : clang::RecursiveASTVisitor<ASTIndexer> {
public:
  ASTIndexer(IndexerContext &context,
             clang::SourceManager &sourceManager)
      : Context(context), SourceManager(sourceManager) {}

  void indexDecl(clang::Decl *d) { TraverseDecl(d); }

private:
  typedef clang::RecursiveASTVisitor<ASTIndexer> base;
  friend class clang::RecursiveASTVisitor<ASTIndexer>;

  bool TraverseDecl(clang::Decl *d);
  bool VisitDeclRefExpr(clang::DeclRefExpr *e);
  void RecordDeclRefExpr(clang::NamedDecl *d, clang::SourceLocation loc,
                         clang::Expr *e);
  bool VisitDecl(clang::Decl *d);
  void RecordDeclRef(clang::NamedDecl *d, clang::SourceLocation beginLoc,
                     bool isDefinition);

  IndexerContext &Context;
  clang::SourceManager &SourceManager;
};

#endif /* ASTINDEXER_H */

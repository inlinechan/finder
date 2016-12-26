#include "ASTIndexer.h"

bool ASTIndexer::TraverseDecl(clang::Decl *d) { return base::TraverseDecl(d); }

bool ASTIndexer::VisitDeclRefExpr(clang::DeclRefExpr *e) {
  RecordDeclRefExpr(e->getDecl(), e->getLocation(), e);
  return true;
}

void ASTIndexer::RecordDeclRefExpr(clang::NamedDecl *d,
                                   clang::SourceLocation loc, clang::Expr *e) {
  assert(d != nullptr);

  if (llvm::isa<clang::FunctionDecl>(*d)) {
    Context.getIndexerArchive().recordRef(
        SourceManager.getFilename(loc).str(), SourceManager.getFileOffset(loc),
        d->getQualifiedNameAsString(), ReferenceType::RT_FUNCTION_REF);
  }
}

bool ASTIndexer::VisitDecl(clang::Decl *d) {
  if (clang::NamedDecl *nd = llvm::dyn_cast<clang::NamedDecl>(d)) {
    clang::SourceLocation loc = nd->getLocation();
    if (clang::FunctionDecl *fd = llvm::dyn_cast<clang::FunctionDecl>(d)) {
      if (fd->getTemplateInstantiationPattern() != NULL) {
      } else {
        bool isDefinition = fd->isThisDeclarationADefinition();
        // bool isMethod = llvm::isa<clang::CXXMethodDecl>(fd);
        RecordDeclRef(nd, loc, isDefinition);
      }
    }
  }
  return true;
}

void ASTIndexer::RecordDeclRef(clang::NamedDecl *d,
                               clang::SourceLocation beginLoc,
                               bool isDefinition) {
  assert(d != NULL);
  Context.getIndexerArchive().recordRef(
      SourceManager.getFilename(beginLoc).str(),
      SourceManager.getFileOffset(beginLoc), d->getQualifiedNameAsString(),
      isDefinition ? ReferenceType::RT_FUNCTION_DEFI
                   : ReferenceType::RT_FUNCTION_DECL);
}


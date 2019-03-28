#include "clang/AST/ASTNodeTraverser.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/AST/Stmt.h"
#include "clang/AST/TextNodeDumper.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

#include <queue>
#include <stack>
#include <utility>

using namespace clang::tooling;
using namespace clang;

#define DISPATCH_STMT(X)                                                       \
  case Stmt::StmtClass::X##Class:                                              \
    return Visit##X((X *)stmt);

#define DISPATCH_DECL(X)                                                       \
  case Decl::Kind::X:                                                          \
    return Visit##X((X##Decl *)decl);

#define IGNORE_DECL(X)                                                         \
  case Decl::Kind::X:                                                          \
    return;

#define RECURSE_CHILDREN_STMT(X)                                               \
  do {                                                                         \
    for (auto it : X->children())                                              \
      DispatchStmt(it);                                                        \
  } while (0)

void printType(QualType t) { llvm::outs() << "\"" << t.getAsString() << "\""; }

class SexpVisitor {
public:
  void VisitTranslationUnit(TranslationUnitDecl *tu) {
    llvm::outs() << '(' << tu->Decl::getDeclKindName();

    for (auto dec : tu->decls())
      DispatchDecl(dec);

    llvm::outs() << ')';
  }

  void VisitDecl(Decl *decl) {
    llvm::outs() << '(' << decl->getDeclKindName() << ' ';

    if (decl->hasBody())
      DispatchStmt(decl->getBody());

    llvm::outs() << ')';
  }

  void VisitFunction(FunctionDecl *f) {
    if (!f->hasBody())
      return;

    llvm::outs() << "(Function " << f->getNameAsString() << ' ';

    printType(f->getType());
    for (auto param : f->parameters())
      VisitParmVarDecl((ParmVarDecl *)param);

    DispatchStmt(f->getBody());

    llvm::outs() << ')';
  }

  void VisitFunctionTemplate(FunctionTemplateDecl *ft) {
    llvm::outs() << "(FunctionTemplate " << ft->getNameAsString() << ' ';
    VisitFunction(ft->getTemplatedDecl());
    llvm::outs() << ')';
  }

  void VisitParmVarDecl(ParmVarDecl *p) {
    llvm::outs() << "(ParmVarDecl " << p->getNameAsString() << ' ';
    printType(p->getType());
    llvm::outs() << ')';
  }

  void VisitTypedef(TypedefDecl *td) {
    llvm::outs() << "(Typedef " << td->getNameAsString() << ' ';
    printType(td->getUnderlyingType());
    llvm::outs() << ')';
  }

  void DispatchDecl(Decl *decl) {
    if (!decl)
      return;

    if (!decl->getLocation().isValid())
      return;

    if (_sourceManager->isInSystemMacro(decl->getLocation()))
      return;

    if (_sourceManager->isInSystemHeader(decl->getLocation()))
      return;

    switch (decl->getKind()) {
      DISPATCH_DECL(Function)
      DISPATCH_DECL(FunctionTemplate)
      DISPATCH_DECL(Typedef)
      IGNORE_DECL(LinkageSpec)
    default:
      return VisitDecl(decl);
    }
  }

  void DispatchStmt(Stmt *stmt) {
    if (!stmt)
      return;

    switch (stmt->getStmtClass()) {
      DISPATCH_STMT(BinaryOperator)
      DISPATCH_STMT(UnaryOperator)
      DISPATCH_STMT(IntegerLiteral)
      DISPATCH_STMT(StringLiteral)
      DISPATCH_STMT(DeclStmt)
      DISPATCH_STMT(DeclRefExpr)
    default:
      return VisitStmt(stmt);
    }
  }

  void VisitDeclStmt(DeclStmt *stmt) {
    if (stmt->isSingleDecl()) {
      llvm::outs() << "(DeclStmt ";
      if (auto *var = dyn_cast<VarDecl>(stmt->getSingleDecl())) {
        llvm::outs() << var->getNameAsString() << ' ';
        printType(var->getType());
      } else
        llvm::outs() << stmt->getSingleDecl()->getDeclKindName();

      RECURSE_CHILDREN_STMT(stmt);

      llvm::outs() << ')';
    }
  }

  void VisitIntegerLiteral(IntegerLiteral *i) {
    llvm::outs() << "(IntegerLiteral " << i->getValue().getLimitedValue()
                 << ' ';
    printType(i->getType());
    llvm::outs() << ")";
  }

  void VisitStringLiteral(StringLiteral *s) {
    llvm::outs() << "(StringLiteral ";
    s->outputString(llvm::outs());
    llvm::outs() << ")";
  }

  void VisitDeclRefExpr(DeclRefExpr *ref) {
    llvm::outs() << "(DeclRefExpr " << ref->getDecl()->getNameAsString() << ')';
  }

  void VisitUnaryOperator(UnaryOperator *op) {
    llvm::outs() << "(UnaryOperator "
                 << UnaryOperator::getOpcodeStr(op->getOpcode()).data();
    DispatchStmt(op->getSubExpr());
    llvm::outs() << ')';
  }

  void VisitBinaryOperator(BinaryOperator *op) {
    llvm::outs() << "(BinaryOperator " << op->getOpcodeStr().data() << ' ';

    DispatchStmt(op->getLHS());
    DispatchStmt(op->getRHS());

    llvm::outs() << ')';
  }

  void VisitStmt(Stmt *stmt) {
    llvm::outs() << '(' << stmt->getStmtClassName();

    RECURSE_CHILDREN_STMT(stmt);

    llvm::outs() << ')';
  }

  void setSourceManager(SourceManager *SM) { _sourceManager = SM; }

private:
  SourceManager *_sourceManager;
};

class SexpConsumer : public ASTConsumer {
public:
  virtual void HandleTranslationUnit(ASTContext &Context) {
    _visitor.setSourceManager(&Context.getSourceManager());
    _visitor.VisitTranslationUnit(Context.getTranslationUnitDecl());
  }

private:
  SexpVisitor _visitor;
};

class SexpAction : public ASTFrontendAction {
public:
  virtual std::unique_ptr<ASTConsumer>
  CreateASTConsumer(CompilerInstance &Compiler, StringRef InFile) {
    (void)Compiler;
    (void)InFile;
    return std::unique_ptr<ASTConsumer>(new SexpConsumer);
  }
};

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory ClangSexpCategory("clang-sexpression options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static llvm::cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static llvm::cl::extrahelp
    MoreHelp("\nThis tool converts clang ASTs to S-Expressions\n");

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, ClangSexpCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());
  Tool.run(newFrontendActionFactory<SexpAction>().get());
}

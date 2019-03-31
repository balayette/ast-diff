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
    return Visit##X((const X *)stmt);

#define DISPATCH_DECL(X)                                                       \
  case Decl::Kind::X:                                                          \
    return Visit##X((const X##Decl *)decl);

#define TRANSPARENT_STMT(X)                                                    \
  case Stmt::StmtClass::X##Class:                                              \
    return VisitTransparentStmt(stmt);

#define IGNORE_DECL(X)                                                         \
  case Decl::Kind::X:                                                          \
    return;

#define RECURSE_CHILDREN_STMT(X)                                               \
  do {                                                                         \
    for (const auto *it : X->children())                                       \
      DispatchStmt(it);                                                        \
  } while (0)

void printType(QualType t) { llvm::outs() << "\"" << t.getAsString() << "\""; }

class SexpVisitor {
public:
  void VisitTranslationUnit(const TranslationUnitDecl *tu) {
    llvm::outs() << '(' << tu->Decl::getDeclKindName();

    for (auto dec : tu->decls())
      DispatchDecl(dec);

    llvm::outs() << ')';
  }

  void test(const ReturnStmt *s)
  {
    RECURSE_CHILDREN_STMT(s);
  }

  void VisitTransparentStmt(const Stmt *stmt) { RECURSE_CHILDREN_STMT(stmt); }

  void VisitDecl(const Decl *decl) {
    llvm::outs() << '(' << decl->getDeclKindName() << ' ';

    if (decl->hasBody())
      DispatchStmt(decl->getBody());

    llvm::outs() << ')';
  }

  void VisitFunction(const FunctionDecl *f) {
    if (!f->hasBody())
      return;

    llvm::outs() << "(Function " << f->getNameAsString() << ' ';

    printType(f->getType());
    for (auto param : f->parameters())
      VisitParmVarDecl((ParmVarDecl *)param);

    DispatchStmt(f->getBody());

    llvm::outs() << ')';
  }

  void VisitFunctionTemplate(const FunctionTemplateDecl *ft) {
    llvm::outs() << "(FunctionTemplate " << ft->getNameAsString() << ' ';
    VisitFunction(ft->getTemplatedDecl());
    llvm::outs() << ')';
  }

  void VisitParmVarDecl(const ParmVarDecl *p) {
    llvm::outs() << "(ParmVarDecl " << p->getNameAsString() << ' ';
    printType(p->getType());
    llvm::outs() << ')';
  }

  void VisitTypedef(const TypedefDecl *td) {
    llvm::outs() << "(Typedef " << td->getNameAsString() << ' ';
    printType(td->getUnderlyingType());
    llvm::outs() << ')';
  }

  void VisitRecord(const RecordDecl *rd) {
    llvm::outs() << "(Record " << rd->getNameAsString() << ' ';
    for (auto *field : rd->fields())
      DispatchDecl(field);
    llvm::outs() << ')';
  }

  void VisitField(const FieldDecl *fd) {
    llvm::outs() << '(' << fd->getNameAsString() << ' ';
    printType(fd->getType());
    llvm::outs() << ')';
  }

  void VisitEnum(const EnumDecl *ed) {
    llvm::outs() << "(Enum " << ed->getNameAsString();
    for (auto *field : ed->enumerators())
      DispatchDecl(field);
    llvm::outs() << ')';
  }

  void VisitEnumConstant(const EnumConstantDecl *ecd) {
    llvm::outs() << '(' << ecd->getNameAsString() << ' '
                 << ecd->getInitVal().getExtValue() << ')';
  }

  void VisitVar(const VarDecl *vd) {
    llvm::outs() << "(VarDecl " << vd->getNameAsString() << ' ';
    DispatchStmt(vd->getInit());
    printType(vd->getType());
    llvm::outs() << ')';
  }

  void DispatchDecl(const Decl *decl) {
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
      DISPATCH_DECL(Record)
      DISPATCH_DECL(Field)
      DISPATCH_DECL(Enum)
      DISPATCH_DECL(EnumConstant)
      DISPATCH_DECL(Var)
      IGNORE_DECL(LinkageSpec)
    default:
      return VisitDecl(decl);
    }
  }

  void VisitSwitchStmt(const SwitchStmt *ss) {
    llvm::outs() << "(SwitchStmt ";
    RECURSE_CHILDREN_STMT(ss);
    llvm::outs() << ')';
  }

  void DispatchStmt(const Stmt *stmt) {
    if (!stmt)
      return;

    switch (stmt->getStmtClass()) {
      DISPATCH_STMT(BinaryOperator)
      DISPATCH_STMT(UnaryOperator)
      DISPATCH_STMT(IntegerLiteral)
      DISPATCH_STMT(StringLiteral)
      DISPATCH_STMT(DeclStmt)
      DISPATCH_STMT(DeclRefExpr)
      DISPATCH_STMT(MemberExpr)
      DISPATCH_STMT(SwitchStmt)
      TRANSPARENT_STMT(ParenExpr)
      TRANSPARENT_STMT(ImplicitCastExpr)
    default:
      return VisitStmt(stmt);
    }
  }

  void VisitMemberExpr(const MemberExpr *me) {
    llvm::outs() << "(MemberExpr ";
    RECURSE_CHILDREN_STMT(me);
    llvm::outs() << me->getMemberDecl()->getNameAsString() << ')';
  }

  void VisitDeclStmt(const DeclStmt *stmt) {
    if (stmt->isSingleDecl()) {
      llvm::outs() << "(DeclStmt ";
      if (auto *var = dyn_cast<VarDecl>(stmt->getSingleDecl()))
        DispatchDecl(var);
      else
        llvm::outs() << stmt->getSingleDecl()->getDeclKindName();

      llvm::outs() << ')';
    }
  }

  void VisitIntegerLiteral(const IntegerLiteral *i) {
    llvm::outs() << "(IntegerLiteral " << i->getValue().getLimitedValue()
                 << ' ';
    printType(i->getType());
    llvm::outs() << ")";
  }

  void VisitStringLiteral(const StringLiteral *s) {
    llvm::outs() << "(StringLiteral ";
    s->outputString(llvm::outs());
    llvm::outs() << ")";
  }

  void VisitDeclRefExpr(const DeclRefExpr *ref) {
    llvm::outs() << '(' << ref->getDecl()->getNameAsString() << ')';
  }

  void VisitUnaryOperator(const UnaryOperator *op) {
    llvm::outs() << "(UnaryOperator "
                 << UnaryOperator::getOpcodeStr(op->getOpcode()).data();
    DispatchStmt(op->getSubExpr());
    llvm::outs() << ')';
  }

  void VisitBinaryOperator(const BinaryOperator *op) {
    llvm::outs() << "(BinaryOperator " << op->getOpcodeStr().data() << ' ';

    DispatchStmt(op->getLHS());
    DispatchStmt(op->getRHS());

    llvm::outs() << ')';
  }

  void VisitStmt(const Stmt *stmt) {
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

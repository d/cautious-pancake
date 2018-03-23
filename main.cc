// Declares clang::SyntaxOnlyAction.
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
// Declares llvm::cl::extrahelp.
#include "llvm/Support/CommandLine.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"

using namespace clang::tooling;
using namespace llvm;

// Apply a custom category to all command-line options so that they are the
// only ones displayed.
static llvm::cl::OptionCategory MyToolCategory("my-tool options");

// CommonOptionsParser declares HelpMessage with a description of the common
// command-line options related to the compilation database and input files.
// It's nice to have this help message in all tools.
static cl::extrahelp CommonHelp(CommonOptionsParser::HelpMessage);

// A help message for this specific tool can be added afterwards.
static cl::extrahelp MoreHelp("\nMore help text...");

using namespace clang;
using namespace clang::ast_matchers;

StatementMatcher LoopMatcher =
    forStmt(hasLoopInit(declStmt(hasSingleDecl(varDecl(
        hasInitializer(ignoringImplicit(integerLiteral(equals(0))))))))).bind("forLoop");

DeclarationMatcher EnumMatcher = clang::ast_matchers::enumDecl().bind("enum");

class LoopPrinter : public MatchFinder::MatchCallback {
public :
  virtual void run(const MatchFinder::MatchResult &Result) {
    if (const ForStmt *FS = Result.Nodes.getNodeAs<clang::ForStmt>("forLoop"))
      FS->printPretty(llvm::outs(), nullptr, Result.Context->getPrintingPolicy());
  }
};

struct EnumPrinter : MatchFinder::MatchCallback {
  void run(const MatchFinder::MatchResult &Result) override {
    if (const EnumDecl *enumDecl = Result.Nodes.getNodeAs<EnumDecl>("enum")) {
      llvm::raw_ostream &out = llvm::outs();
      enumDecl->printQualifiedName(out);
      out << '\n';
      for (const EnumConstantDecl *constantDecl : enumDecl->enumerators()) {
        out << "  " << constantDecl->getQualifiedNameAsString() << '='
            << constantDecl->getInitVal() << '\n';
      }
      out << '\n';
    }
  }
};

int main(int argc, const char **argv) {
  CommonOptionsParser OptionsParser(argc, argv, MyToolCategory);
  ClangTool Tool(OptionsParser.getCompilations(),
                 OptionsParser.getSourcePathList());

  EnumPrinter Printer;
  MatchFinder Finder;
  Finder.addMatcher(EnumMatcher, &Printer);

  return Tool.run(newFrontendActionFactory(&Finder).get());
}

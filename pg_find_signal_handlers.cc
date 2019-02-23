#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "llvm/Support/CommandLine.h"

using clang::ast_matchers::callee;
using clang::ast_matchers::callExpr;
using clang::ast_matchers::declRefExpr;
using clang::ast_matchers::functionDecl;
using clang::ast_matchers::hasArgument;
using clang::ast_matchers::hasName;
using clang::ast_matchers::MatchFinder;
using clang::ast_matchers::namedDecl;
using clang::ast_matchers::StatementMatcher;
using clang::ast_matchers::to;

static llvm::cl::OptionCategory my_tool_category(
    "pg_find_signal_handlers options");

StatementMatcher signal_handler_matcher =
    callExpr(callee(namedDecl(hasName("pqsignal"))),
             hasArgument(1,
                         declRefExpr(to(functionDecl().bind("SIGHDL")))
                             .bind("referenced here")));

struct SignalHandlerCollector : MatchFinder::MatchCallback {
  void run(const MatchFinder::MatchResult& Result) override {
    if (const auto* const function_decl =
            Result.Nodes.getNodeAs<clang::FunctionDecl>("SIGHDL")) {
      llvm::raw_ostream& out = llvm::outs();
      clang::ASTContext& context = *Result.Context;
      clang::DiagnosticsEngine& diagnostics = context.getDiagnostics();
      const auto diagnostic_id = diagnostics.getCustomDiagID(
          clang::DiagnosticsEngine::Remark, "Handler");

      const clang::DiagnosticBuilder &diagnostic_builder = diagnostics.Report(function_decl->getBeginLoc(), diagnostic_id);
      const clang::SourceRange source_range = function_decl->getSourceRange();
      diagnostic_builder.AddSourceRange(clang::CharSourceRange::getCharRange(source_range));
    }
  }
};

int main(int argc, const char** argv) {
  clang::tooling::CommonOptionsParser OptionsParser(argc, argv,
                                                    my_tool_category);
  clang::tooling::ClangTool Tool(OptionsParser.getCompilations(),
                                 OptionsParser.getSourcePathList());
  MatchFinder finder;
  SignalHandlerCollector signal_handler_collector;
  finder.addMatcher(signal_handler_matcher, &signal_handler_collector);

  Tool.run(clang::tooling::newFrontendActionFactory(&finder).get());
}


#include "module.h"
#include "context.h"
#include "options.h"

using namespace hilti;

Module::Module(shared_ptr<CompilerContext> ctx, shared_ptr<ID> id, const string& path,
               const Location& l)
    : ast::Module<AstInfo>(id, path, l)
{
    assert(id);

    _context = ctx;

    shared_ptr<hilti::statement::Block> body(new hilti::statement::Block(nullptr, nullptr, l));
    body->scope()->setID(id);
    setBody(body);

    // Implicitly always import the internal libhilti module.
    if ( id->name() != "hlt" )
        import("libhilti");

    if ( ctx->options().profile && util::strtolower(id->name()) != "hilti" )
        // Need this for Hilti::ProfileStyle.
        import("Hilti");

    // Implicitly always export Main::run().
    if ( util::strtolower(id->name()) == "main" )
        exportID("run", true);
}

shared_ptr<CompilerContext> Module::compilerContext() const
{
    return _context;
}

shared_ptr<type::Context> Module::executionContext() const
{
    auto expr = body()->scope()->lookupUnique(std::make_shared<ID>("Context"), false);

    if ( ! expr )
        return nullptr;

    auto texpr = ast::rtti::checkedCast<expression::Type>(expr);
    auto ctx = ast::rtti::checkedCast<type::Context>(texpr->typeValue());

    return ctx;
}

shared_ptr<passes::CFG> Module::cfg() const
{
    return _cfg;
}

shared_ptr<passes::Liveness> Module::liveness() const
{
    return _liveness;
}

void Module::setPasses(shared_ptr<passes::CFG> cfg, shared_ptr<passes::Liveness> liveness)
{
    _cfg = cfg;
    _liveness = liveness;
}

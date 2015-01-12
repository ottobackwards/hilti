
#ifndef BINPAC_CODEGEN_PARSER_BUILDER_H
#define BINPAC_CODEGEN_PARSER_BUILDER_H

#include <ast/visitor.h>

#include "../common.h"
#include "cg-visitor.h"

namespace binpac {

namespace type { namespace unit { namespace item { class Field; } } }

namespace codegen {

class ParserState;

/// Generates code to parse input according to a grammar.
class ParserBuilder : public CGVisitor<shared_ptr<hilti::Expression>, shared_ptr<type::unit::item::Field>>
{
public:
    ParserBuilder(CodeGen* cg);
    virtual ~ParserBuilder();

    /// Returns the type of the currently parsed unit. The method must only
    /// be called when parsing is in progress.
    shared_ptr<type::Unit> unit() const;

    /// Generates the function to parse input according to a unit's grammar.
    ///
    /// u: The unit to generate the parser for.
    ///
    /// Returns: The generated HILTI function with the parsing code.
    shared_ptr<hilti::Expression> hiltiCreateParseFunction(shared_ptr<type::Unit> u);

    /// Generates the externally visible functions for parsing a unit type.
    ///
    /// u: The unit type to export via functions.
    void hiltiExportParser(shared_ptr<type::Unit> unit);

    // Returns the HILTI struct type for a unit's parse object.
    shared_ptr<hilti::Type> hiltiTypeParseObject(shared_ptr<type::Unit> unit);

    // Creates the host-facing parser function.
    //
    // unit: The unit to create the host function for.
    //
    // sink: If true, we generate a slightly different version for internal
    // use with sinks.
    shared_ptr<hilti::Expression> hiltiCreateHostFunction(shared_ptr<type::Unit> unit, bool sink);

    // Returns the new() function that instantiates a new parser object. In
    // addition to the type parameters, the returned function receives three:
    // parameters: the user cookie; a ``hlt_sink *`` with the sink the parser
    // is connected to (NULL if none); and a ``bytes`` object with the MIME
    // type associated with the input (empty if None). The function also runs
    // the %init hook.
    shared_ptr<hilti::Expression> hiltiFunctionNew(shared_ptr<type::Unit> unit);

    /// Writes a new chunk of data into a field's sinks.
    ///
    /// field: The field.
    ///
    /// data: The data to write into the sinks.
    void hiltiWriteToSinks(shared_ptr<type::unit::item::Field> field, shared_ptr<hilti::Expression> data);

    /// Writes a new chunk of data into a sink.
    ///
    /// sink: The sink to write to.
    ///
    /// data: The data to write into the sink.
    void hiltiWriteToSink(shared_ptr<hilti::Expression> sink, shared_ptr<hilti::Expression> data);

protected:
    /// Parse a given entity. This a wrapper around processOne() that adds
    /// additional logic around it.
    bool parse(shared_ptr<Node> node, shared_ptr<hilti::Expression>* result = nullptr, shared_ptr<type::unit::item::Field> field = nullptr);

    /// Returns the current parsing state.
    shared_ptr<ParserState> state() const;

    typedef std::function<void ()> unpack_callback;

    /// Generates a HILTI ``unpack`` instruction wrapped in error handling
    /// code. The error handling code will do "the right thing" when either a
    /// parsing error occurs or insufficient input is found. In the latter
    /// case, it will execute the code generated by
    /// _hiltiInsufficientInputHandler.
    ///
    /// target_type: The type being unpacked.
    ///
    /// op1, op2, op3: Same as with the regular ``unpack`` instruction.
    ///
    /// callback: function - A function that will be called for
    /// generating additional code for the case of insufficient input.
    ///
    /// Returns: The result of the unpack.
    shared_ptr<hilti::Expression> hiltiUnpack(shared_ptr<Type> target_type,
                                              shared_ptr<hilti::Expression> op1,
                                              shared_ptr<hilti::Expression> op2,
                                              shared_ptr<hilti::Expression> op3 = nullptr,
                                              unpack_callback callback = nullptr);

    /// Pushes a new parsing state onto the stack.
    void pushState(shared_ptr<ParserState> state);

    /// Pops the current parsing state from the stack.
    void popState();

    void visit(constant::Address* a) override;
    void visit(constant::Bitset* b) override;
    void visit(constant::Bool* b) override;
    void visit(constant::Double* d) override;
    void visit(constant::Enum* e) override;
    void visit(constant::Integer* i) override;
    void visit(constant::Interval* i) override;
    void visit(constant::Network* n) override;
    void visit(constant::Port* p) override;
    void visit(constant::String* s) override;
    void visit(constant::Time* t) override;

    void visit(ctor::Bytes* b) override;
    void visit(ctor::RegExp* r) override;

    void visit(expression::Ctor* c) override;
    void visit(expression::Constant* c) override;
    void visit(expression::Type* c) override;

    void visit(Production* p) override;

    void visit(production::Boolean* b) override;
    void visit(production::ChildGrammar* c) override;
    void visit(production::Enclosure* e) override;
    void visit(production::Counter* c) override;
    void visit(production::ByteBlock* c) override;
    void visit(production::Epsilon* e) override;
    void visit(production::Literal* l) override;
    void visit(production::LookAhead* l) override;
    void visit(production::NonTerminal* n) override;
    void visit(production::Sequence* s) override;
    void visit(production::Switch* s) override;
    void visit(production::Terminal* t) override;
    void visit(production::Variable* v) override;
    void visit(production::While* w) override;
    void visit(production::Loop* l) override;

    void visit(type::Address* a) override;
    void visit(type::Bitfield* b) override;
    void visit(type::Bitset* b) override;
    void visit(type::Bool* b) override;
    void visit(type::Bytes* b) override;
    void visit(type::Double* d) override;
    void visit(type::Enum* e) override;
    void visit(type::EmbeddedObject* o) override;
    void visit(type::Integer* i) override;
    void visit(type::Interval* i) override;
    void visit(type::List* l) override;
    void visit(type::Network* n) override;
    void visit(type::Port* p) override;
    void visit(type::Sink* s) override;
    void visit(type::String* s) override;
    void visit(type::Set* s) override;
    void visit(type::Time* t) override;
    void visit(type::Unit* u) override;
    void visit(type::Void* v) override;
    void visit(type::unit::Item* i) override;
    void visit(type::unit::item::Field* f) override;
    void visit(type::unit::item::field::Constant* c) override;
    void visit(type::unit::item::field::Ctor* r) override;
    void visit(type::unit::item::field::Switch* s) override;
    void visit(type::unit::item::field::AtomicType* t) override;
    void visit(type::unit::item::field::Unit* t) override;
    void visit(type::unit::item::field::switch_::Case* c) override;
    void visit(type::unit::item::Variable* v) override;
    void visit(type::unit::item::Property* p) override;
    void visit(type::unit::item::GlobalHook* h) override;
    void visit(type::Vector* v) override;

private:
    typedef std::list<std::pair<shared_ptr<hilti::Expression>, shared_ptr<Type>>> hilti_expression_type_list;

    // Pushes an empty parse function with the right standard signature. If
    // value_type is given, the function return tuple will contain an
    // additional element of that type for passing back the parsed value.
    //
    // Returns: An expression referencing the function.
    shared_ptr<hilti::Expression> _newParseFunction(const string& name, shared_ptr<type::Unit> unit, shared_ptr<hilti::Type> value_type = nullptr);

    // Finishes a functions tarted with _newParseFunction().
    void _finishParseFunction(bool finalize_pobj);

    /// Generates the body code for parsing a given node. Same parameters as
    /// parse().
    bool _hiltiParse(shared_ptr<Node> node, shared_ptr<hilti::Expression>* result, shared_ptr<type::unit::item::Field> f);

    // Allocates and initializes a new parse object.
    shared_ptr<hilti::Expression> _allocateParseObject(shared_ptr<Type> unit, bool store_in_self);

    // Initializes the current parse object before starting the parsing
    // process.
    void _prepareParseObject(const hilti_expression_type_list& params, shared_ptr<hilti::Expression> cur, shared_ptr<hilti::Expression> sink = nullptr, shared_ptr<hilti::Expression> mimetype = nullptr);

    // Finalizes the current parser when the parsing process has finished.
    void _finalizeParseObject(bool success);

    // Called just before a production is being parsed.
    void _startingProduction(shared_ptr<Production> p, shared_ptr<type::unit::item::Field> field);

    // Called just after a production has been parsed.
    void _finishedProduction(shared_ptr<Production> p);

    // Callen when parsing a production leads to a new value to be assigned
    // to a user-visible field of the current parse object. This method is
    // doing the actual assignment and triggering the corresponding hook. If
    // value is nullptr, the fields current value is taken to trigger the
    // hook.
    void _newValueForField(shared_ptr<Production> p, shared_ptr<type::unit::item::Field> field, shared_ptr<hilti::Expression> value);

    // Calls a parse function with the current parsing state.
    shared_ptr<hilti::Expression> _hiltiCallParseFunction(shared_ptr<binpac::type::Unit> unit, shared_ptr<hilti::Expression> func, bool catch_parse_error, shared_ptr<hilti::Type> presult_value_type);

    // Prints the given message to the binpac debug stream.
    void _hiltiDebug(const string& msg);

    // Prints the given message to the binpac verbose debug stream.
    void _hiltiDebugVerbose(const string& msg);

    // Prints the given token to binpac-verbose.
    void _hiltiDebugShowToken(const string& tag, shared_ptr<hilti::Expression> token);

    // Prints the upcoming input bytes to binpac-verbose.
    void _hiltiDebugShowInput(const string& tag, shared_ptr<hilti::Expression> cur);

<<<<<<< HEAD
    // Helper producing the "binpac" debugging output for a bitfield.
    void _hiltiDebugBitfield(shared_ptr<hilti::Expression> value, shared_ptr<type::Integer> type);
=======
    // Executes a hook. \a self is the self parameter to pass to the hook. \a
    // id is the full path to the hooked element, including the module. \a
    // foreach must be true if this is a \c forach hook. \a dolllardollar is
    // the value for the \a $$ identifier within the hook, if it takes one
    // (or null). If \a foreach is true, returns a boolean expression that is
    // true if the hook has called "hook.stop true". If \a foreach is false,
    // returns null.
    shared_ptr<hilti::Expression> _hiltiRunHook(shared_ptr<binpac::type::Unit> unit, shared_ptr<hilti::Expression> self, shared_ptr<ID> id, shared_ptr<type::unit::Item> item, bool foreach, shared_ptr<hilti::Expression> dollardollar = nullptr);

    // Defines a hook's implementation. <id> is the full path to the hooked
    // element, including the module. <forach> is true if this is a \c
    // &foreach hook. The ID of \a hook is ignored. \a dollardollar, if
    // given, is the type for the \a $$ identifier within the hook, if it
    // takes one. If <debug> is true, the hook will only be compiled in at
    // non-zero debugging levels, and it will only be executed at run-time if
    // explicitly enabled via libbinpac.
    void _hiltiDefineHook(shared_ptr<ID> id, shared_ptr<type::unit::Item> item, bool foreach, bool debug, shared_ptr<type::Unit> unit, shared_ptr<Statement> block, shared_ptr<Type> dollardollar = nullptr, int priority = 0);

    // Returns the full path ID for the hook referecing a unit item.
    shared_ptr<ID> _hookForItem(shared_ptr<type::Unit>, shared_ptr<type::unit::Item> item, bool foreach, bool private_);

    // Returns the full path ID for the hook referecing a unit-global hook.
    shared_ptr<ID> _hookForUnit(shared_ptr<type::Unit>, const string& name);

    // Computes the canonical hook name, given the full path. The returned
    // boolean indicates whether the hook is a local one (i.e., within the
    // same module; true) or cross-module (false).
    std::pair<bool, string> _hookName(const string& path);
>>>>>>> 014df6ac6bd5b6fe506bb97d1baf65b131d1d4c6

    // Scans for a look-ahead symbol out of an expected set and sets the
    // state()->lah* accordingly if found. Raises a parse error if not.
    void _hiltiGetLookAhead(shared_ptr<Production> prod, const std::list<shared_ptr<production::Terminal>>& terms, bool must_find);

    // Generates HILTI code to initialize the matching state for finding the
    // next token.
    shared_ptr<hilti::Expression> _hiltiMatchTokenInit(const string& name, const std::list<shared_ptr<production::Terminal>>& terms);

    // Performs the matching of the next token. Throws execeptions if the matching fails.
    shared_ptr<hilti::Expression> _hiltiMatchTokenAdvance(shared_ptr<hilti::Expression> mstate);

    // Adds the standard error cases for a switch statement switching on the match result.
    shared_ptr<hilti::builder::BlockBuilder> _hiltiAddMatchTokenErrorCases(shared_ptr<Production> prod,
                                                                           hilti::builder::BlockBuilder::case_list* cases,
                                                                           shared_ptr<hilti::builder::BlockBuilder> repeat,
                                                                           std::list<shared_ptr<production::Terminal>> expected,
                                                                           shared_ptr<hilti::builder::BlockBuilder> cont = nullptr
                                                                          );

    // Raises a ParseError exception.
    void _hiltiParseError(const string& msg);

    // Raises a ParseError exception given an existing exception.
    void _hiltiParseError(shared_ptr<hilti::Expression> excpt);

    // Gnerates the HILTI code to handle an out-of-input situation by
    // retrying next time if possible.
    void _hiltiYieldAndTryAgain(shared_ptr<Production> prod, shared_ptr<hilti::builder::BlockBuilder> cont);

    // Generates the HILTI code to report insufficient input during matching.
    shared_ptr<hilti::Expression> _hiltiInsufficientInputHandler(bool eod_ok = false, shared_ptr<hilti::Expression> iter = nullptr);

    // Disables saving parsed values in a parse objects. This is primarily
    // for parsing container items that aren't directly stored there.
    void disableStoringValues();

    // Renables saving parsed values in a parse objects. This is primarily
    // for parsing container items that aren't directly stored there.
    void enableStoringValues();

    // Returns true if storing values in the parse object is enabled.
    bool storingValues();

    /// Save the current input position in the parser object.
    void _hiltiSaveInputPostion();

    // Update the curren input position from what's stored in the parser object.
    void _hiltiUpdateInputPostion();

    // Trim input bytes to current parsing position if we can.
    void _hiltiTrimInput();

    // Turns a unit's parameters into a HILTI function parameter list, adding
    // to the given list.
    void _hiltiUnitParameters(shared_ptr<type::Unit> unit, hilti::builder::function::parameter_list* args) const;

    // Helper to transparently pipe input through a chain of filters if a
    // parsing object has defined any. It adjust the parsing arguments
    // appropiately so that subsequent code doesn't notice the filtering.
    //
    // resume: True if this method is called after resuming parsing because
    // of having insufficient inout earlier. False if it's the initial
    // parsing step for a newly instantiated parser.
    void _hiltiFilterInput(bool resume);

    // Helper for bytes parsing, implements &chunked.
    void _hiltiCheckChunk(shared_ptr<type::unit::item::Field> field);

    // Synchronize parsing with a given production.
    void _hiltiSynchronize(shared_ptr<Node> n, shared_ptr<ParserState> hook_state, shared_ptr<Production> p);

    // Synchronize parsing by moving ahead a given number of bytes.
    void _hiltiSynchronize(shared_ptr<Node> n, shared_ptr<ParserState> hook_state, shared_ptr<hilti::Expression> i);

    // When parsing a production that can resynchronize on errors, insert
    // code to prepare for that.
    void _hiltiPrepareSynchronize(Production* sync_check, shared_ptr<Production> sync_on, shared_ptr<ParserState> hook_state, shared_ptr<hilti::Expression> cont);

    // When parsing a production that can resynchronize on errors, insert
    // code to finalize that.
    void _hiltiFinishSynchronize(Production* sync_check, shared_ptr<Production> sync_on);

    // Returns an iterator reflecting the end of the data available to the
    // parsing currently.
    shared_ptr<hilti::Expression> _hiltiEod();

    enum EodDataType {
        // Semantics corresponding to operator::End: End of current input
        // reached, independent of whether input is frozen and whether
        // there's a object following.
        EodStandard,
        // Like EodStandard but only if input is also frozen.
        EodIfFrozen
    };

    // Returns a HILTI boolean indicating if the current input stream has been frozen.
    shared_ptr<hilti::Expression> _hiltiIsFrozen();

    // Returns a HILTI boolean indicating if the current input position
    // reflects the end of the input data. The type indicates further
    // constraints to test for.
    shared_ptr<hilti::Expression> _hiltiAtEod(EodDataType type = EodStandard);

    // Returns a HILTI boolean indicating if a given input position reflects
    // the end of the input data. The type indicates further constraints to
    // test for.
    shared_ptr<hilti::Expression> _hiltiAtEod(shared_ptr<hilti::Expression> pos, EodDataType type = EodStandard);

    // Advances the current input position to the given iterator. If the
    // distance between current and new position is known, it can be passed
    // in as a hint in the form of an integer expression; which will make the
    // generated code more efficient. Note that embedded objects don't count.
    void _hiltiAdvanceTo(shared_ptr<hilti::Expression> ncur, shared_ptr<hilti::Expression> distance = nullptr);

    // Advances the current input position by the given number of bytes.
    void _hiltiAdvanceBy(shared_ptr<hilti::Expression> n);

    typedef shared_ptr<hilti::Expression> InputPosition;

    // Saves the current input position so that it can be later restored.
    InputPosition _hiltiSavePosition();

    // Restores a previoysly saved input position.
    void _hiltiRestorePosition(InputPosition pos);

    std::list<shared_ptr<ParserState>> _states;
    shared_ptr<hilti::Expression> _last_parsed_value;
    shared_ptr<production::Literal> _cur_literal;
    int _store_values;
};

}
}

#endif

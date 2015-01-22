#include <iostream>
#include <vector>

class CodeGenContext;
class NStatement;
class NExpression;
class NVariableDeclaration;

typedef std::vector<NStatement*> StatementList;
typedef std::vector<NExpression*> ExpressionList;
typedef std::vector<NVariableDeclaration*> VariableList;

struct Context
{
	std::map<std::string, double> m_variables;
	std::map<std::string, NFunctionDeclaration*> m_functions;
};

#define RetVal double

class Node {
public:
    virtual ~Node() {}
    virtual RetVal exec(Context& ctx) =0;
};

class NExpression : public Node {
};

class NStatement : public Node {
};

class NInteger : public NExpression {
public:
    long long value;
    NInteger(long long value) : value(value) { }
   virtual RetVal exec(Context& ctx) {return value;}
};

class NDouble : public NExpression {
public:
    double value;
    NDouble(double value) : value(value) { }
  virtual RetVal exec(Context& ctx) {return value;}
};

class NIdentifier : public NExpression {
public:
    std::string name;
    NIdentifier(const std::string& name) : name(name) { }
    virtual RetVal exec(Context& ctx) {return 0;}
};

class NMethodCall : public NExpression {
public:
    const NIdentifier& id;
    ExpressionList arguments;
    NMethodCall(const NIdentifier& id, ExpressionList& arguments) :
        id(id), arguments(arguments) { }
    NMethodCall(const NIdentifier& id) : id(id) { }
    virtual RetVal exec(Context& ctx) {
		NFunctionDeclaration *pFtn =(ctx.FindFunction(id)); 
		assert(pFtn && pFtn->nArgs==arguments.size());
		return pFtn->exec(arguments);
	}
};

class NBinaryOperator : public NExpression {
public:
    int op;
    NExpression& lhs;
    NExpression& rhs;
    NBinaryOperator(NExpression& lhs, int op, NExpression& rhs) :
        lhs(lhs), rhs(rhs), op(op) { }
	virtual RetVal exec(Context& ctx) {
		switch(op)
		{
		 case???
		}
	}
};

class NAssignment : public NExpression {
public:
    NIdentifier& lhs;
    NExpression& rhs;
    NAssignment(NIdentifier& lhs, NExpression& rhs) : 
        lhs(lhs), rhs(rhs) { }
	virtual RetVal exec(Context& ctx) {
		double *d= ctx.FindIdentifier(lhs);
		assert(d);
		return *d = rhs.exec(ctx);
	}
};

class NBlock : public NExpression {
public:
    StatementList statements;
    NBlock() { }
   virtual RetVal exec(Context& ctx) {
    RetVal d=0;
	for(int i=0; i<statements.size(); i++)
	{
	 d =statements[i].exec(ctx);
	}
	return d;
   }
};

class NExpressionStatement : public NStatement {
public:
    NExpression& expression;
    NExpressionStatement(NExpression& expression) : 
        expression(expression) { }
  //  virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NVariableDeclaration : public NStatement {
public:
    const NIdentifier& type;
    NIdentifier& id;
    NExpression *assignmentExpr;
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id) :
        type(type), id(id) { }
    NVariableDeclaration(const NIdentifier& type, NIdentifier& id, NExpression *assignmentExpr) :
        type(type), id(id), assignmentExpr(assignmentExpr) { }
  //  virtual llvm::Value* codeGen(CodeGenContext& context);
};

class NFunctionDeclaration : public NStatement {
public:
    const NIdentifier& type;
    const NIdentifier& id;
    VariableList arguments;
    NBlock& block;
    NFunctionDeclaration(const NIdentifier& type, const NIdentifier& id, 
            const VariableList& arguments, NBlock& block) :
        type(type), id(id), arguments(arguments), block(block) { }
  //  virtual llvm::Value* codeGen(CodeGenContext& context);
};
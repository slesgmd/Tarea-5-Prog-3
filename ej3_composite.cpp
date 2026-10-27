#include <iostream>
#include <memory>
#include <string>
#include <sstream>
#include <cmath>

// componente base
class Expression {
public:
    virtual double evaluate(double x) = 0;
    virtual std::shared_ptr<Expression> derivative() = 0;
    virtual std::string toString() = 0;
    // para no usar dynamic_cast en power
    virtual bool isConstant() { return false; }
    virtual ~Expression() {}
};

// hojas
class Constant : public Expression {
    double value;
public:
    Constant(double value) : value(value) {}

    double evaluate(double x) override { return value; }

    std::shared_ptr<Expression> derivative() override {
        return std::make_shared<Constant>(0);
    }

    std::string toString() override {
        // ostringstream para que no salga 2.000
        std::ostringstream ss;
        ss << value;
        return ss.str();
    }

    bool isConstant() override { return true; }
};

class Variable : public Expression {
public:
    double evaluate(double x) override { return x; }

    std::shared_ptr<Expression> derivative() override {
        return std::make_shared<Constant>(1);
    }

    std::string toString() override { return "x"; }
};

//compuestos con dos hijos
class BinaryOperation : public Expression {
protected:
    std::shared_ptr<Expression> left;
    std::shared_ptr<Expression> right;
public:
    BinaryOperation(std::shared_ptr<Expression> left, std::shared_ptr<Expression> right)
        : left(left), right(right) {}
};

class Add : public BinaryOperation {
public:
    Add(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r) : BinaryOperation(l, r) {}

    double evaluate(double x) override {
        return left->evaluate(x) + right->evaluate(x);
    }

    std::shared_ptr<Expression> derivative() override {
        return std::make_shared<Add>(left->derivative(), right->derivative());
    }

    std::string toString() override {
        return "(" + left->toString() + " + " + right->toString() + ")";
    }
};

class Multiply : public BinaryOperation {
public:
    Multiply(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r) : BinaryOperation(l, r) {}

    double evaluate(double x) override {
        return left->evaluate(x) * right->evaluate(x);
    }

    // regla del producto 
    std::shared_ptr<Expression> derivative() override {
        return std::make_shared<Add>(
            std::make_shared<Multiply>(left->derivative(), right),
            std::make_shared<Multiply>(left, right->derivative())
        );
    }

    std::string toString() override {
        return "(" + left->toString() + " * " + right->toString() + ")";
    }
};

// funciones de un solo hijo
class UnaryFunction : public Expression {
protected:
    std::shared_ptr<Expression> arg;
public:
    UnaryFunction(std::shared_ptr<Expression> arg) : arg(arg) {}
};

class Cos;

class Sin : public UnaryFunction {
public:
    Sin(std::shared_ptr<Expression> a) : UnaryFunction(a) {}

    double evaluate(double x) override { return std::sin(arg->evaluate(x)); }

    std::shared_ptr<Expression> derivative() override;

    std::string toString() override { return "sin(" + arg->toString() + ")"; }
};

class Cos : public UnaryFunction {
public:
    Cos(std::shared_ptr<Expression> a) : UnaryFunction(a) {}

    double evaluate(double x) override { return std::cos(arg->evaluate(x)); }

    std::shared_ptr<Expression> derivative() override {
        return std::make_shared<Multiply>(
            std::make_shared<Constant>(-1),
            std::make_shared<Multiply>(std::make_shared<Sin>(arg), arg->derivative())
        );
    }

    std::string toString() override { return "cos(" + arg->toString() + ")"; }
};

std::shared_ptr<Expression> Sin::derivative() {
    return std::make_shared<Multiply>(std::make_shared<Cos>(arg), arg->derivative());
}

class Ln : public UnaryFunction {
public:
    Ln(std::shared_ptr<Expression> a) : UnaryFunction(a) {}

    double evaluate(double x) override { return std::log(arg->evaluate(x)); }

    std::shared_ptr<Expression> derivative() override;

    std::string toString() override { return "ln(" + arg->toString() + ")"; }
};

class Power : public BinaryOperation {
public:
    Power(std::shared_ptr<Expression> l, std::shared_ptr<Expression> r) : BinaryOperation(l, r) {}

    double evaluate(double x) override {
        return std::pow(left->evaluate(x), right->evaluate(x));
    }

    std::shared_ptr<Expression> derivative() override {
        if (right->isConstant()) {
            double n = right->evaluate(0);
            return std::make_shared<Multiply>(
                std::make_shared<Multiply>(
                    std::make_shared<Constant>(n),
                    std::make_shared<Power>(left, std::make_shared<Constant>(n - 1))
                ),
                left->derivative()
            );
        }
        return std::make_shared<Multiply>(
            std::make_shared<Power>(left, right),
            std::make_shared<Add>(
                std::make_shared<Multiply>(right->derivative(), std::make_shared<Ln>(left)),
                std::make_shared<Multiply>(
                    right,
                    std::make_shared<Multiply>(
                        left->derivative(),
                        std::make_shared<Power>(left, std::make_shared<Constant>(-1))
                    )
                )
            )
        );
    }

    std::string toString() override {
        return "(" + left->toString() + " ^ " + right->toString() + ")";
    }
};

std::shared_ptr<Expression> Ln::derivative() {
    return std::make_shared<Multiply>(
        arg->derivative(),
        std::make_shared<Power>(arg, std::make_shared<Constant>(-1))
    );
}

int main() {
    auto x = std::make_shared<Variable>();

    auto expr =
        std::make_shared<Multiply>(
            std::make_shared<Add>(
                std::make_shared<Power>(
                    x,
                    std::make_shared<Constant>(2)
                ),
                std::make_shared<Multiply>(
                    std::make_shared<Constant>(3),
                    x
                )
            ),
            std::make_shared<Sin>(x)
        );

    auto deriv = expr->derivative();

    std::cout << "f(x) = " << expr->toString() << std::endl;
    std::cout << "f'(x) = " << deriv->toString() << std::endl;

    std::cout << "f(2) = " << expr->evaluate(2) << std::endl;
    std::cout << "f'(2) = " << deriv->evaluate(2) << std::endl;
}

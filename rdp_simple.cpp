#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <stdexcept>


static std::string input;
static std::size_t pos = 0;

static double parseExpression();
static double parseTerm();
static double parseFactor();


static char peek() {
    if (pos < input.size()) return input[pos];
    return '\0';
}

static char get() {
    char c = peek();
    if (c != '\0') pos++;
    return c;
}

static void skipSpaces() {
    while (std::isspace(static_cast<unsigned char>(peek()))) pos++;
}

static bool match(char expected) {
    skipSpaces();
    if (peek() == expected) {
        pos++;
        return true;
    }
    return false;
}

[[noreturn]] static void error(const std::string& message) {
    std::cerr << "Error: " << message << " at position " << (pos + 1) << "\n";
    std::cerr << input << "\n";
    for (std::size_t i = 0; i < pos; ++i) std::cerr << (input[i] == '\t' ? '\t' : ' ');
    std::cerr << "^\n";
    std::exit(1);
}

static double parseNumber() {
    skipSpaces();
    std::size_t start = pos;
    bool hasDigit = false;

    while (std::isdigit(static_cast<unsigned char>(peek()))) { get(); hasDigit = true; }

    if (peek() == '.') {
        get();
        while (std::isdigit(static_cast<unsigned char>(peek()))) { get(); hasDigit = true; }
    }

    if (!hasDigit) error("Expected a number");

    std::string token = input.substr(start, pos - start);
    try {
        return std::stod(token);
    } catch (...) {
        error("Invalid number");
    }
}

static double parseFactor() {
    skipSpaces();

    if (match('+')) {
        return parseFactor();
    }
    if (match('-')) {
        return -parseFactor();
    }

    if (match('(')) {
        double value = parseExpression();
        if (!match(')')) error("Expected ')' after expression");
        return value;
    }

    return parseNumber();
}

static double parseTerm() {
    double value = parseFactor();
    for (;;) {
        if (match('*')) {
            value *= parseFactor();
        } else if (match('/')) {
            double right = parseFactor();
            if (right == 0.0) error("Division by zero");
            value /= right;
        } else {
            break;
        }
    }
    return value;
}

static double parseExpression() {
    double value = parseTerm();
    for (;;) {
        if (match('+')) {
            value += parseTerm();
        } else if (match('-')) {
            value -= parseTerm();
        } else {
            break;
        }
    }
    return value;
}

int main(int argc, char** argv) {
    if (argc >= 2) {
        input = argv[1];
    } else {
        std::cout << "Enter an expression: ";
        std::getline(std::cin, input);
    }

    pos = 0;
    double result = parseExpression();

    skipSpaces();
    if (peek() != '\0') {
        error("Unexpected characters after the expression");
    }

    std::cout << result << "\n";
    return 0;
}


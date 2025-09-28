#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>
#include <unistd.h>

// Simple recursive descent parser and evaluator for arithmetic expressions.
// Grammar (with precedence and associativity):
//   Expr   -> Term ((+ | -) Term)*
//   Term   -> Factor ((* | /) Factor)*
//   Factor -> ('+' | '-') Factor | NUMBER | '(' Expr ')'
// Supports integers and floating-point numbers, unary +/- and parentheses.

typedef enum {
    T_END,
    T_NUMBER,
    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_LPAREN,
    T_RPAREN
} TokenKind;

typedef struct {
    TokenKind kind;
    double    number; // valid if kind == T_NUMBER
    size_t    start;  // byte offset in input where token starts
    size_t    end;    // byte offset in input where token ends (one past last char)
} Token;

typedef struct {
    const char *input;
    size_t len;
    size_t pos;
    Token tok; // current token
} Parser;

static void   advance(Parser *p);
static double parse_expr(Parser *p);
static double parse_term(Parser *p);
static double parse_factor(Parser *p);

static void report_error(const Parser *p, size_t where, const char *fmt, ...) {
    if (!p || !p->input) {
        fprintf(stderr, "Parse error: ");
        va_list ap; va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
        fputc('\n', stderr);
        exit(EXIT_FAILURE);
    }

    // Determine line start/end for caret display
    size_t line_start = where;
    while (line_start > 0 && p->input[line_start - 1] != '\n') line_start--;
    size_t line_end = where;
    while (line_end < p->len && p->input[line_end] != '\n') line_end++;

    size_t col = where - line_start + 1; // 1-based column in this line

    fprintf(stderr, "Error at column %zu: ", col);
    va_list ap; va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
    fputc('\n', stderr);

    // Print the line
    fwrite(p->input + line_start, 1, line_end - line_start, stderr);
    fputc('\n', stderr);

    // Print caret under the error column; preserve tabs for alignment
    for (size_t i = line_start; i < where; i++) {
        fputc(p->input[i] == '\t' ? '\t' : ' ', stderr);
    }
    fputs("^\n", stderr);

    exit(EXIT_FAILURE);
}

static Token make_token(TokenKind kind, size_t start, size_t end) {
    Token t; t.kind = kind; t.number = 0.0; t.start = start; t.end = end; return t;
}

static void skip_ws(Parser *p) {
    while (p->pos < p->len && isspace((unsigned char)p->input[p->pos])) p->pos++;
}

static Token lex(Parser *p) {
    skip_ws(p);
    size_t i = p->pos;
    if (i >= p->len) return make_token(T_END, i, i);

    char c = p->input[i];
    switch (c) {
        case '+': p->pos++; return make_token(T_PLUS,  i, i+1);
        case '-': p->pos++; return make_token(T_MINUS, i, i+1);
        case '*': p->pos++; return make_token(T_STAR,  i, i+1);
        case '/': p->pos++; return make_token(T_SLASH, i, i+1);
        case '(': p->pos++; return make_token(T_LPAREN, i, i+1);
        case ')': p->pos++; return make_token(T_RPAREN, i, i+1);
        default: break;
    }

    if (isdigit((unsigned char)c) || c == '.') {
        char *endptr = NULL;
        double val = strtod(p->input + i, &endptr);
        size_t j = (size_t)(endptr - p->input);
        if (j == i) {
            report_error(p, i, "Invalid number");
        }
        p->pos = j;
        Token t = make_token(T_NUMBER, i, j);
        t.number = val;
        return t;
    }

    report_error(p, i, "Unexpected character '%c'", c);
    return make_token(T_END, i, i); // Unreachable
}

static void advance(Parser *p) {
    p->tok = lex(p);
}

static double parse_factor(Parser *p) {
    // Handle unary plus/minus
    if (p->tok.kind == T_PLUS) {
        advance(p);
        return parse_factor(p);
    }
    if (p->tok.kind == T_MINUS) {
        advance(p);
        return -parse_factor(p);
    }

    if (p->tok.kind == T_NUMBER) {
        double v = p->tok.number;
        advance(p);
        return v;
    }

    if (p->tok.kind == T_LPAREN) {
        size_t open_pos = p->tok.start;
        advance(p);
        double v = parse_expr(p);
        if (p->tok.kind != T_RPAREN) {
            report_error(p, open_pos, "Expected ')' to match '(' ");
        }
        advance(p);
        return v;
    }

    report_error(p, p->tok.start, "Expected a number, '(', or unary +/-");
    return 0.0; // Unreachable
}

static double parse_term(Parser *p) {
    double left = parse_factor(p);
    for (;;) {
        if (p->tok.kind == T_STAR) {
            size_t op_pos = p->tok.start;
            advance(p);
            double right = parse_factor(p);
            left *= right;
            (void)op_pos; // silence unused warning if not used further
        } else if (p->tok.kind == T_SLASH) {
            size_t op_pos = p->tok.start;
            advance(p);
            double right = parse_factor(p);
            if (right == 0.0) {
                report_error(p, op_pos, "Division by zero");
            }
            left /= right;
        } else {
            break;
        }
    }
    return left;
}

static double parse_expr(Parser *p) {
    double left = parse_term(p);
    for (;;) {
        if (p->tok.kind == T_PLUS) {
            advance(p);
            double right = parse_term(p);
            left += right;
        } else if (p->tok.kind == T_MINUS) {
            advance(p);
            double right = parse_term(p);
            left -= right;
        } else {
            break;
        }
    }
    return left;
}

static char *read_all_stdin(void) {
    size_t cap = 4096, n = 0;
    char *buf = (char *)malloc(cap + 1);
    if (!buf) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    for (;;) {
        size_t avail = cap - n;
        if (avail == 0) {
            cap *= 2;
            char *tmp = (char *)realloc(buf, cap + 1);
            if (!tmp) {
                free(buf);
                perror("realloc");
                exit(EXIT_FAILURE);
            }
            buf = tmp;
            avail = cap - n;
        }
        size_t r = fread(buf + n, 1, avail, stdin);
        n += r;
        if (r < avail) {
            if (feof(stdin)) break;
            if (ferror(stdin)) {
                free(buf);
                perror("fread");
                exit(EXIT_FAILURE);
            }
        }
    }
    buf[n] = '\0';
    return buf;
}

int main(int argc, char **argv) {
    const char *expr = NULL;
    char *owned = NULL; // if we read from stdin

    if (argc >= 2) {
        expr = argv[1];
    } else {
        if (isatty(STDIN_FILENO)) {
            fprintf(stderr, "Usage: %s \"EXPR\"  (or pipe: echo \"EXPR\" | %s)\n", argv[0], argv[0]);
            return 2;
        }
        owned = read_all_stdin();
        expr = owned;
    }

    Parser p = {0};
    p.input = expr;
    p.len = strlen(expr);
    p.pos = 0;

    advance(&p);
    double result = parse_expr(&p);
    if (p.tok.kind != T_END) {
        report_error(&p, p.tok.start, "Unexpected input after end of expression");
    }

    // Print the result with good precision
    printf("%.17g\n", result);

    free(owned);
    return 0;
}


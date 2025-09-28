%{
int count = 0;
%}

%%

[^\t\n ]+    {
                if (yyleng >= 5 && yyleng <= 10)
                    count++;
             }
.|\n         { /* ignore all other characters */ }

%%

int main() {
    yylex();
    printf("Total words of length 5 to 10: %d\n", count);
    return 0;
}

int yywrap() {
    return 1;
}

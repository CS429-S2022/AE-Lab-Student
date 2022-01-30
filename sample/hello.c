
char* magic = (char*) 0xFFFFFFFFFFFFFFFFULL;

void write_char(const char c) {
    *magic = c;
}

void write_string(const char* s) {
    do {
        char c = *s;
        if (c == 0) return;
        write_char(c);
        s++;
    } while(1);
}
    

void start() {
    write_string("hello\n");
}

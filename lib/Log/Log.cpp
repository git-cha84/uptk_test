#include "Log.h"

namespace logs {
std::mutex consolM;
char* fileName = nullptr;

void Init(char* fName) {
    const std::lock_guard<std::mutex> lock(consolM);
    fileName = fName;
}

void Write(const char* Format, ...) {
    va_list Args;
    va_start(Args, Format);

    std::array<char, 1024> Buffer{0};    // NOLINT
    auto Size = vsprintf(Buffer.data(), Format, Args);

    if (Size > 0) {
        const std::lock_guard<std::mutex> lock(consolM);
        std::array<char, 128> TimeBuffer{0};    // NOLINT
        struct timeval timeVal {};
        gettimeofday(&timeVal, nullptr);
        struct tm* st = gmtime(&timeVal.tv_sec);

        int TimeSize = sprintf(TimeBuffer.data(),
                               " %d:%d:%d : [%02d:%02d:%02d:%06ld] :",
                               st->tm_year + 1900,
                               st->tm_mon + 1,
                               st->tm_mday,
                               st->tm_hour - 4,
                               st->tm_min,
                               st->tm_sec,
                               timeVal.tv_usec);

        if (TimeSize != 0)
            fprintf(stdout, TimeBuffer.data(), 0);
        fprintf(stdout, Buffer.data(), 0);
        fprintf(stdout, "\n");
        FILE* f = nullptr;
        f = fopen(fileName, "a+");
        if (f != nullptr) {
            if (TimeSize != 0)
                fprintf(f, TimeBuffer.data(), 0);
            fprintf(f, Buffer.data(), 0);
            fprintf(f, "\n");
            fclose(f);
        }
    }
    va_end(Args);
}

}    // namespace logs

#include "app.h"

#include <stdio.h>
#include <stdlib.h>

int main() {
    App app = {0};

    APP_Result result;
    app_Run(&app, &result);
    if (result != APP_SUCCESS) {
        perror("APP_Result is Failure");
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

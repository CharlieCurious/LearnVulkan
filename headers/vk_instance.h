#pragma once

#include <app.h>
#include <utils.h>

struct QueueFamilyIndicies {
    OptionalUint32 graphicsFamily;
    OptionalUint32 presentFamily;
};

void app_CreateVkInstance(App *app);
void app_SetupDebugMessenger(App *app);
void app_CreateSurface(App *app);
void app_PickPhysicalDevice(App *app);
void app_CreateLogicalDevice(App *app);

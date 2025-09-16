#pragma once
#include <android/asset_manager.h>

void create();

void update(int width, int height);

void draw();

void initAssetManager(AAssetManager* assetPtr);
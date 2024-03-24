#pragma once

#include "renderer_types.inl"

struct staticMeshData;
struct platformState;

b8 rendererInitialize(const char *application_name,
                       struct platformState *plat_state);
void rendererShutdown();

void rendererOnResized(u16 width, u16 height);

b8 rendererDrawFrame(renderPacket *packet);

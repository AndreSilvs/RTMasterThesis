#pragma once

#include <memory>

#include "OBJloader.h"

#include "Vao.h"

namespace ModelConverter {
    bool convertOBJtoVAO( const OBJModel& model, Vao *outData );
    bool convertOBJtoVAOinitializer( const OBJModel& model, VaoInitializer& vInit );
}

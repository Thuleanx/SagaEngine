#pragma once

#include <bitset>

namespace Saga {

const int MAX_COMPONENTS = 256; //!< Upper bound on number of components. This can be as high as needed, but will linearly increase the speed of ComponentGroup lookups.

/**
 * @brief Used for determining if an object has a certain component type. Signature [i] is true if it does, and false otherwise.
 */
using Signature = std::bitset<MAX_COMPONENTS>; 


} // namespace Saga

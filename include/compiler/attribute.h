#pragma once

#define CHECK_ATTR(attr) static_assert(__has_c_attribute(attr), "No attribute " #attr);

// function attribute

CHECK_ATTR(gnu::__always_inline__)
CHECK_ATTR(gnu::constructor)
CHECK_ATTR(gnu::weak)

CHECK_ATTR(gnu::section)

CHECK_ATTR(gnu::aligned)
CHECK_ATTR(gnu::used)

CHECK_ATTR(gnu::fallthrough)
#pragma once

#define abs(num)                 \
    ({                           \
        auto _num = num;         \
        _num < 0 ? -_num : _num; \
    })

#define max(num1, num2)                \
    ({                                 \
        auto _num1 = num1;             \
        auto _num2 = num2;             \
        _num1 > _num2 ? _num1 : _num2; \
    })

#define min(num1, num2)                \
    ({                                 \
        auto _num1 = (num1);           \
        auto _num2 = (num2);           \
        _num1 < _num2 ? _num1 : _num2; \
    })

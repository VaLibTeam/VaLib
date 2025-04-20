#include <VaLib/Utils/ToString.hpp>

#include <VaLib/Types/BasicTypedef.hpp>

#include <VaLib/Types/ImmutableString.hpp>
#include <VaLib/Types/String.hpp>

#include <algorithm>
#include <cmath>
#include <limits>
#include <ostream>
#include <string>

namespace va {

VaString toString(int64 num) {
    if (num == 0) {
        return "0";
    }

    VaString result = "";
    bool isNegative = false;

    if (num < 0) {
        isNegative = true;
        num = -num;
    }

    while (num > 0) {
        result += static_cast<char>('0' + num % 10);
        num /= 10;
    }

    if (isNegative) {
        result += '-';
    }

    std::reverse(result.begin(), result.end());
    return result;
}

VaString toString(uint64 num) {
    if (num == 0) {
        return "0";
    }

    VaString result = "";

    while (num > 0) {
        result += static_cast<char>('0' + num % 10);
        num /= 10;
    }

    std::reverse(result.begin(), result.end());
    return result;
}

constexpr float64 EPS = 1e-9;

VaString toString(float64 num, int precision) {
    if (std::isnan(num)) return "NaN";
    if (std::isinf(num)) return num > 0 ? "Inf" : "-Inf";

    VaString result;
    bool isNegative = num < 0;
    if (isNegative) {
        result += '-';
        num = -num;
    }

    if (num < std::numeric_limits<float64>::epsilon() && num >= 0) {
        return "0.0";
    }

    float64 integerPart;
    float64 fractionalPart = std::modf(num, &integerPart);

    uint64 intPart = static_cast<uint64>(integerPart);
    if (intPart == 0) {
        result += '0';
    } else {
        VaString intStr;
        while (intPart > 0) {
            intStr += '0' + (intPart % 10);
            intPart /= 10;
        }

        std::reverse(intStr.begin(), intStr.end());
        result += intStr;
    }

    fractionalPart = std::abs(fractionalPart);
    if (fractionalPart > 0) {
        result += '.';

        if (precision ==
            AUTO_PRECISION) { // Automatic precision mode - continue until no fractional part remains
            if (precision == AUTO_PRECISION) {
                constexpr int maxAutoDigits = 8;
                float64 scaled = std::round(fractionalPart * std::pow(10.0, maxAutoDigits));
                uint64 fracInt = static_cast<uint64>(scaled);

                VaString fracStr;
                for (int i = 0; i < maxAutoDigits; ++i) {
                    fracStr +=
                        '0' +
                        (fracInt / static_cast<uint64>(std::pow(10, maxAutoDigits - i - 1)) % 10);
                }

                int lastNonZero = len(fracStr) - 1;
                while (lastNonZero >= 0 && fracStr[lastNonZero] == '0') {
                    --lastNonZero;
                }
                for (int i = 0; i <= lastNonZero; ++i) {
                    result += fracStr[i];
                }
            }
        } else { // Fixed precision mode
            for (int i = 0; i < precision && fractionalPart > 0; ++i) {
                fractionalPart *= 10;
                int digit = static_cast<int>(fractionalPart);
                result += '0' + digit;
                fractionalPart -= digit;
            }
        }
    } else {
        result += ".0";
    }

    return result;
}

} // namespace va
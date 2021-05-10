#include "../lib/ieee754.h"

IEEE_754::IEEE_754(std::bitset<IEEE_754::number_of_bits> ieee_number /* 0 */) : number(ieee_number)
{
}

const std::bitset<IEEE_754::number_of_bits> &IEEE_754::get_number() const
{
    return number;
}

std::string IEEE_754::display_in_decimal() const
{

    int sign = number[IEEE_754::number_of_bits - 1];

    int base = 1;

    auto exponent = this->get_exponent_bits().to_ulong();

    auto mantissa = this->get_mantissa_bits();

    if (exponent == 0 && mantissa.to_ulong() == 0)
    {
        std::cout << "0" << std::endl;
        return "0";
    }
    else if (exponent == 0 && mantissa.to_ulong() != 0)
    {
        exponent = 1;
        base = 0;
    }
    else if (exponent == 255 && mantissa.to_ulong() != 0)
    {
        std::cout << "NaN" << std::endl;
        return "NaN";
    }
    else if (exponent == 255 && mantissa.to_ulong() == 0)
    {
        if (sign == 1)
        {
            std::cout << "-Inf" << std::endl;
            return "-Inf";
        }
        else
        {
            std::cout << "+Inf" << std::endl;
            return "+Inf";
        }
    }

    double value = 0;
    double x = 0.5;
    for (int i = IEEE_754::number_of_mantissa_bits - 1; i >= 0; i--)
    {
        value += x * mantissa[i];
        x /= 2;
    }
    // std::cout << "value  = " << value << std::endl;

    std::cout << (sign ? "-1" : "1") << " * 2^" << (int)(exponent - pow(2, IEEE_754::number_of_exponent_bits - 1) - 1) << " * " << base + value << std::endl;
    return "Normalized Number";
}

std::bitset<IEEE_754::number_of_exponent_bits> IEEE_754::get_exponent_bits() const
{
    std::bitset<IEEE_754::number_of_exponent_bits> exponent;

    for (size_t i = 0; i < IEEE_754::number_of_exponent_bits; i++)
    {
        exponent[i] = this->number[IEEE_754::number_of_mantissa_bits + i];
    }

    return std::move(exponent);
}

std::bitset<IEEE_754::number_of_mantissa_bits> IEEE_754::get_mantissa_bits() const
{

    std::bitset<IEEE_754::number_of_mantissa_bits> mantissa;

    for (size_t i = 0; i < IEEE_754::number_of_mantissa_bits; i++)
    {
        mantissa[i] = this->number[i];
    }

    return std::move(mantissa);
}

void IEEE_754::round_ties_to_even(bool r, bool s)
{
    //in this round method there is no need to check sign

    bool unity_bit = this->number[0];

    if (unity_bit == 0 && r == 1 && s == 1)
        unity_bit = 1;
    else if (unity_bit == 1 && r == 1)
    {
        unity_bit = 0;
        this->number[1] = 1;
    }

    this->number[0] = unity_bit;
}

void IEEE_754::round_ties_to_away(bool r, bool s)
{
    //in this round method there is no need to check sign

    bool unity_bit = this->number[0];

    if (unity_bit == 0 && (r == 1 || s == 1))
        unity_bit = 1;
    else if (unity_bit == 1 && (r == 1 || s == 1))
    {
        unity_bit = 0;
        this->number[1] = 1;
    }

    this->number[0] = unity_bit;
}

template <size_t N1>
std::bitset<N1> IEEE_754::additive_inverse(std::bitset<N1> &mantissa)
{
    //reverse all bits
    std::cout << "before mantissa = " << mantissa << std::endl;

    mantissa.flip();

    unsigned long ulong_mantissa = mantissa.to_ulong();

    ulong_mantissa += 1;

    std::cout << "after mantissa = " << std::bitset<N1>(ulong_mantissa) << std::endl;

    return std::bitset<N1>(ulong_mantissa);
}

template <size_t N1>
void IEEE_754::scale_mantissa_down(unsigned long exponent_diff, std::bitset<N1> &mantissa)
{
    // std::cout << exponent_diff << std::endl;
    mantissa >>= exponent_diff;
    // std::cout << mantissa << std::endl;
}

template <size_t N1, size_t N2>
std::bitset<N1> IEEE_754::get_bits(const std::bitset<N2> &b1, const uint8_t start_index, const uint8_t end_index)
{
    assert(N1 <= N2);
    assert(start_index <= end_index);

    // std::cout << N1 << std::endl;

    std::bitset<N1> result(0);

    for (size_t i = 0; i <= (end_index - start_index); i++)
    {
        result[i] = b1[start_index + 1];
    }

    return std::move(result);
}

template <size_t N1, size_t N2>
void IEEE_754::paste_bits_into(const std::bitset<N1> &src, std::bitset<N2> &dst, const uint8_t dst_start_index, const uint8_t dst_end_index)
{
    // TODO: const uint8_t src_start_index /* 0 */, const uint8_t src_end_index /* dst_end_index - dst_start_index */ using overloads
    assert(N1 <= N2);
    assert(dst_start_index <= dst_end_index);
    assert(dst_start_index >= 0 && dst_end_index >= 0);
    assert(N1 == dst_end_index - dst_start_index + 1);

    for (size_t i = 0; i <= (dst_end_index - dst_start_index); i++)
    {
        dst[dst_start_index + i] = src[i];
    }
}

template <size_t N1, size_t N2>
void IEEE_754::paste_bits_into(const std::bitset<N1> &src, std::bitset<N2> &dst, uint8_t dst_start_index, uint8_t dst_end_index, uint8_t src_start_index, uint8_t src_end_index)
{
    assert(N1 <= N2);
    assert(dst_start_index <= dst_end_index);
    assert(src_start_index <= src_end_index);
    assert(dst_start_index >= 0 && dst_end_index >= 0 && src_start_index >= 0 && src_end_index >= 0);
    assert(dst_end_index - dst_start_index == src_end_index - src_start_index);

    // for (size_t i = 0; i <= (dst_end_index - dst_start_index); i++)
    // {
    //     dst[dst_start_index + i] = src[i];
    // }

    for (size_t i = 0; i <= (dst_end_index - dst_start_index); i++)
    {
        dst[dst_start_index + i] = src[src_start_index + i];
    }
}

template <size_t N1>
AddResult<N1> IEEE_754::add(const std::bitset<N1> &b1, const std::bitset<N1> &b2)
{

    std::bitset<N1> res;

    int c = 0b0;

    for (int i = 0; i < b1.size(); i++)
    {
        res[i] = (b1[i] ^ b2[i]) ^ c;
        c = ((b1[i] & b2[i]) | (c & (b1[i] ^ b2[i])));

        // cout << "c = " << c << endl;
    }
    // std::cout << "overflow? = " << c << std::endl;

    assert(c == 0 || c == 1);

    return {std::move(res), c};
}

IEEE_754 IEEE_754::operator-(const IEEE_754 &different_number)
{

    auto exponent1 = this->get_exponent_bits();
    auto exponent2 = different_number.get_exponent_bits();

    auto mantissa1 = this->get_mantissa_bits();
    auto mantissa2 = different_number.get_mantissa_bits();

    unsigned long exponent1_ulong = exponent1.to_ulong();
    unsigned long exponent2_ulong = exponent2.to_ulong();

    if (exponent1_ulong > exponent2_ulong)
    {
        scale_mantissa_down(exponent1_ulong - exponent2_ulong, mantissa2);
        exponent2_ulong = exponent1_ulong;
    }
    else if (exponent1_ulong < exponent2_ulong)
    {
        scale_mantissa_down(exponent2_ulong - exponent1_ulong, mantissa1);
        exponent1_ulong = exponent2_ulong;
    }

    IEEE_754 result;

    return result;
}

IEEE_754 IEEE_754::operator+(const IEEE_754 &different_number)
{
    //TODO: sign = 1 -> negative numbers

    auto exponent1 = this->get_exponent_bits();
    auto exponent2 = different_number.get_exponent_bits();

    auto mantissa1 = this->get_mantissa_bits();
    auto mantissa2 = different_number.get_mantissa_bits();

    // std::cout << mantissa1 << " " << mantissa2 << std::endl;

    unsigned long exponent1_ulong = exponent1.to_ulong();
    unsigned long exponent2_ulong = exponent2.to_ulong();

    // std::cout << std::endl;
    // std::cout << mantissa1.to_ulong() << std::endl;
    // std::cout << mantissa2.to_ulong() << std::endl;

    // TODO what if exponent == 0 -> denormalized number

    if (exponent1_ulong > exponent2_ulong)
    {
        scale_mantissa_down(exponent1_ulong - exponent2_ulong, mantissa2);
        exponent2_ulong = exponent1_ulong;
    }
    else if (exponent1_ulong < exponent2_ulong)
    {
        scale_mantissa_down(exponent2_ulong - exponent1_ulong, mantissa1);
        exponent1_ulong = exponent2_ulong;
    }

    // std::cout << mantissa1.to_ulong() << std::endl;
    // std::cout << mantissa2.to_ulong() << std::endl;
    // std::cout << std::endl;

    // adding 1 as first bit as all numbers start with 1
    std::bitset<IEEE_754::number_of_mantissa_bits + 1> _mantissa1(mantissa1.to_ulong());
    std::bitset<IEEE_754::number_of_mantissa_bits + 1> _mantissa2(mantissa2.to_ulong());
    _mantissa1[IEEE_754::number_of_mantissa_bits] = 1;
    _mantissa2[IEEE_754::number_of_mantissa_bits] = 1;

    IEEE_754 result;

    // auto mantissa_result = IEEE_754::add(_mantissa1, _mantissa2);

    // if (mantissa_result.carry_bit == 1)
    // {
    //     exponent1_ulong += 1;
    //     scale_mantissa_down(1, mantissa_result.result);
    // }

    // 24 bit bitset
    std::bitset<IEEE_754::number_of_mantissa_bits + 1> mantissa_result(mantissa1.to_ulong() + mantissa2.to_ulong());
    // TODO Round the result
    if (mantissa_result[number_of_mantissa_bits] == 1)
    {
        exponent1_ulong += 1;
        scale_mantissa_down(1, mantissa_result);
    }

    // TODO dodawanie mantys musimy zawierac jedynki z przodu
    // ? void IEEE_754::paste_bits_into<24u, 32u>(std::bitset<24u> const&, std::bitset<32u>&, unsigned char, unsigned char, unsigned char, unsigned char)
    IEEE_754::paste_bits_into(mantissa_result, result.number, 0, IEEE_754::number_of_mantissa_bits - 1, 0, IEEE_754::number_of_mantissa_bits - 1);
    IEEE_754::paste_bits_into(std::bitset<IEEE_754::number_of_exponent_bits>(exponent1_ulong), result.number, IEEE_754::number_of_mantissa_bits, IEEE_754::number_of_mantissa_bits + IEEE_754::number_of_exponent_bits - 1);

    // std::cout << mantissa1 << " " << mantissa2 << std::endl;

    return result;
}

// int main()
// {

//     //additive inverse

//     std::bitset<8> testmantissa(0b00000010);

//     std::cout << testmantissa << std::endl;

//     IEEE_754::additive_inverse(testmantissa);

//     std::cout << testmantissa << std::endl;

//     //symetryczne do parzystej

//     IEEE_754 roundTestNumb(std::bitset<32>(0b00111111110100000000000000000001));
//     std::cout << "RTN  = " << roundTestNumb.get_number() << std::endl;

//     roundTestNumb.round_ties_to_even(1, 1);

//     std::cout << "RTN = " << roundTestNumb.get_number() << std::endl;

//     IEEE_754 roundTestNumb2(std::bitset<32>(0b00111111110100000000000000000000));
//     std::cout << "RTN2 = " << roundTestNumb2.get_number() << std::endl;

//     roundTestNumb2.round_ties_to_away(1, 1);

//     std::cout << "RTN2 = " << roundTestNumb2.get_number() << std::endl;

//     // number1.display_in_decimal();
//     // number2.display_in_decimal();
//     // number3.display_in_decimal();
//     // number4.display_in_decimal();
//     // number5.display_in_decimal();
//     // number6.display_in_decimal();

//     // std::cout << number1.get_number() << std::endl;
//     // std::cout << number2.get_number() << std::endl;

//     // IEEE_754 number7 = number1 + number2;

//     // number7.display_in_decimal();
//     // IEEE_754 number1(std::bitset<32>(0b00111111110100000000000000000000));
//     // IEEE_754 number2(std::bitset<32>(0b00111111110000000000000000000000));
//     // IEEE_754 number3(std::bitset<32>(0b11111111100000000000000000000000));
//     // IEEE_754 number4(std::bitset<32>(0b11111111100001100000000000000000));
//     // IEEE_754 number5(std::bitset<32>(0b00000000011100000000000000000000));
//     // IEEE_754 number6(std::bitset<32>(0b00000000000000000000000000000000));

//     // std::cout << "Number1:  ";
//     // number1.display_in_decimal();
//     // std::cout << "Number2:  ";
//     // number2.display_in_decimal();
//     // IEEE_754 number7 = number1 + number2;

//     // std::cout << "Przyklad dodania Number1 + Number2:  ";
//     // number7.display_in_decimal();

//     // std::cout << "Przyklad ujemnej nieskonczonosci:  ";

//     // number3.display_in_decimal();
//     // std::cout << "Przyklad nieliczby:  ";

//     // number4.display_in_decimal();
//     // std::cout << "Przyklad liczby zdenormalizowanej:  ";

//     // number5.display_in_decimal();
//     // std::cout << "Przyklad zera:  ";
//     // number6.display_in_decimal();

//     // std::cout << number7.get_number() << std::endl;

//     // number1.display_in_decimal();
//     // number2.display_in_decimal();

//     // std::cout << number1.get_number() << std::endl;
//     // std::cout << number2.get_number() << std::endl;

//     return 0;
// }
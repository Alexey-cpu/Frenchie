#include <math.h>

class Complex
{
public:
    Complex(double re, double im)
        : _re(re), _im(im)
    {}
    double modulus() const
    {
        return sqrt(_re * _re + _im * _im);
    }
private:
    double _re;
    double _im;
};

void bar(int i)
{
    static int counter = 0;
    counter += i;
}

namespace Foo
{
namespace Bar
{
void foo(int a, int b)
{
    for (int i = 0; i < a; i++)
    {
        if (i < b)
            bar(i);
        else
        {
            bar(i);
            bar(b);
        }
    }
}
} // namespace Bar
} // namespace Foo
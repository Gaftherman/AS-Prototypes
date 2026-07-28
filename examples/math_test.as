// math_test.as - Demonstrates scriptmath add-on

void main() {
    println("--- AngelScript Math Add-on Test ---");

    float val = 3.14159265f;
    float sinVal = sin(val / 2.0f);
    float cosVal = cos(val / 2.0f);
    float sqrVal = sqrt(16.0f);
    float powVal = pow(2.0f, 10.0f);

    println("pi / 2 = " + (val / 2.0f));
    println("sin(pi / 2) = " + sinVal);
    println("cos(pi / 2) = " + cosVal);
    println("sqrt(16.0)  = " + sqrVal);
    println("pow(2, 10)   = " + powVal);
}

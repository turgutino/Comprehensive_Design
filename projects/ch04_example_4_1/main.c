int main(void)
{
    volatile const signed char a = -100, b = -10;   // Define two signed constants a and b
    volatile signed char x, y;                      // Define two signed variables x and y
    x = a + b;                                      // Implement the addition of two signed numbers a and b
    y = a - b;                                      // Implement the subtraction of two signed numbers a and b
    return 0;
}

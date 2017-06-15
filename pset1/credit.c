#include <stdio.h>
#include <cs50.h>

int main(void){
    
    // Take card number as input from user
    unsigned long long card_number;
    
    do 
    {
        printf("Card Number:\n");
        card_number = GetLongLong();
    }
    while(card_number <= 0);
    
    unsigned long long card_number_copy = card_number;
    int num_digits = 0;
    
    
    // Find length of the card number
    while (card_number >= 1)
    {
        card_number /= 10;
        num_digits++;
        //printf("Curr = %i\n", num_digits);
    }
    
    
    // Store each individual digit of the card number
    long long digits_array[num_digits];
    
    for (int i = num_digits - 1; i >= 0; i--)
    {
        digits_array[i] = (card_number_copy % 10);
        card_number_copy /= 10;
        //printf("Stored digit was: %lli\n", digits_array[i]);
    }
    
    
    // Classify the card as VISA (3), AMEX(1), or MASTERCARD(2)
    int card_flag;
    
    if(digits_array[0] == 3)
        card_flag = 1;
        
    else if(digits_array[0] == 5)
        card_flag = 2;
        
    else
        card_flag = 3;
            
            
    // Multiply appropriate digits
    int counter = num_digits - 2;
    
    while(counter >= 0)
    {
        digits_array[counter] *= 2;
        counter -= 2;
    }
    
    
    // Sum all digits
    long long total = 0;
    long long temp_num1, temp_num2;
    for (int i = 0; i < num_digits; i++)
    {
        if (digits_array[i] >= 10)
        {
            temp_num1 = digits_array[i] % 10;
            temp_num2 = digits_array[i] / 10;
            digits_array[i] = temp_num1 + temp_num2;
        }
        total += digits_array[i];
        //printf("Curr sum = %lli\n", total);
    }
    
    
    // Output INVALID or the card based on number of digits
    if (total % 10)
    
        printf("INVALID\n");
        
    else
    {
        if (card_flag == 1)
            printf("AMEX\n");
        
        else if (card_flag == 2)
            printf("MASTERCARD\n");
            
        else 
            printf("VISA\n");
    }
    
    return 0;
}
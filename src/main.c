
#define MAX_LEN 16
#define MAX_CARDS 5

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*----------------------------------------------------------------------------*/
uchar status;
uchar str[MAX_LEN]; 
uchar serNum[5];
char myString[100];
uchar check,check2;
uchar Key_Cards[MAX_CARDS][5]; //Store the card's data from SDCard
uchar test [5] ={0xfd, 0x1c, 0x29, 0x40, 0x88};
FATFS FatFs;
FIL fil;
FRESULT fr;

//Compare 2 array.
int compareArrays(const uchar *arr1, const uchar *arr2, int size) {
    for (int i = 0; i < size; ++i) {
        if (arr1[i] != arr2[i]) {
            return 0; //Not correct.
        }
    }
    return 1; //Correct.
}

// Show the content of file.
void showFileContent() {
    f_rewind(&fil); 
    char ch;
    UINT bytesRead;
    while (f_read(&fil, &ch, 1, &bytesRead) == FR_OK && bytesRead > 0) { // Read characters and show them.
        USART_SendChar(ch); // Send them throught USART.
    }
}

int main() {
    // Initiation
    SPIRC552_Config(); 
    TIMDelay_Config();  
    UART1_Config(); 
    MFRC522_Init(); 

    // Mount to file system.
    fr = f_mount(&FatFs, "", 1);
    if (fr != FR_OK) {
        return 0;
    }

    // Open "cards.txt" file to read.
    fr = f_open(&fil, "cards.txt", FA_READ);
    if (fr != FR_OK) {
        return 0;
    }

    // Show the content of "cards.txt"
    showFileContent();

    // Read data from file
    for (int i = 0; i < MAX_CARDS; ++i) {
        UINT bytesRead; // Number of bytes
        fr = f_read(&fil, Key_Cards[i], sizeof(serNum), &bytesRead);
        if (fr != FR_OK || bytesRead != sizeof(serNum)) {
            break; // Stop if there's an error or reach EOF.
        }
    }
	
    f_close(&fil);//Close the file.

    while (1) {
        // Waiting for RFID card to read.
        Delay_ms(1000); 
        status = MFRC522_Request(PICC_REQIDL, str);    
        if (status == MI_OK) {	
            sprintf(myString,"Your card's number are: %x, %x, %x, %x, %x \r\n",serNum[0], serNum[1], serNum[2], serNum[3],serNum[4]);
						USART_SendString(myString);
            status = MFRC522_Anticoll(str);
            memcpy(serNum, str, sizeof(serNum));
						check = 1;
						check2 = 1;
            // Set the cardMatched.
            int cardMatched = 0;

            USART_SendString("The ID: ");
            for (int k = 0; k < sizeof(serNum); ++k) {
                sprintf(myString, "%02X ", serNum[k]); // Convert to hex
                USART_SendString(myString);
            }
            USART_SendString("\n");
 
            // Compare the ID of card to all IDs stored in SDCard.
             for (int j = 0; j < MAX_CARDS; ++j) {
                if (compareArrays(serNum, Key_Cards[j], sizeof(serNum))) {
                    
                    sprintf(myString, "ID the khop tai vi tri %d\n", j);
                    USART_SendString(myString);
                    cardMatched = 1;  
                    break; 
                }
								if (!cardMatched) {
                USART_SendString("ID the khong khop voi du lieu SD\n");
								}
            }

            
        }
		if(check == 1||check2 ==1)
				{
				
					check = 0;
					check2 = 0;
					Delay_ms(1000);
					set_pwm(1500);
					Delay_ms(2000);
					set_pwm(500);
					Delay_ms(1000);
				}
		MFRC522_StopCrypto1();
    }
	
    return 0;
}

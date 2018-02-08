/*
 * Breathalyzer.c
 *
 * Created: 11/13/2017 9:35:38 PM
 * Author : nichh
 */ 

#include "main.h"

int main(void)
{
	#if F_CPU == 1000000UL
	CLKPR_SET(CLKPR_1MHZ);
	#elif F_CPU == 8000000UL
	CLKPR_SET(CLKPR_8MHZ);
	#else
	#error "CPU frequency should be either 1 MHz or 8 MHz"
	#endif
	
	//Initialize SSD1306
	_delay_ms(40);
	ssd1306_init();
	
	Initialize();
	printf("\x01");
	PlaySplashScreen();
	printf("\x02");
	PlayWarmUpScreen();
	printf("\x03");
	
	float hiscore;
	
	LoadStorage(&hiscore);
	TransmitReading(&hiscore, true);
	
	float highscore = hiscore;
	float lastscore = 0.0f;
	
    while (1) 
    {
		char hiDisplay[7];
		char currentDisplay[7];
				
		//sprintf(hiDisplay, "%.3f", highscore);
		ftoa(highscore, hiDisplay, 3);
		//sprintf(currentDisplay, "%.3f", lastscore);
		ftoa(lastscore, currentDisplay, 3);
				
		ssd1306_clear();
				
		ssd1306_string_font8x16xy(0, 0, "High: ");
		ssd1306_string_font8x16xy(0, 3, "Last: ");
				
		ssd1306_string_font8x16xy(48, 0, hiDisplay);
		ssd1306_string_font8x16xy(48, 3, currentDisplay);
				
		ssd1306_setpos(88, 1);
		ssd1306_string_font6x8("bac");
		ssd1306_setpos(88, 4);
		ssd1306_string_font6x8("bac");
				
		ssd1306_setpos(0, 7);
		ssd1306_string_font6x8("Ready to sample...");
				
				
				
		char key = 0x00;
		bool sampled = false;
		bool reset = false;
		while(sampled == false && reset == false)
		{
			if(ListenKeyPress(&key))
			{
				if((key & SAMPLEKEYCODE) == SAMPLEKEYCODE)
				{
					lastscore = PlaySampling();
					sampled = true;
				}
				else if((key & RESETKEYCODE) == RESETKEYCODE)
				{
					reset = PlayResetScreen();
				}
			}
		}
				
		if(sampled)
		{
			if(lastscore >= highscore)
			{
				highscore = lastscore;
				SaveStorage(highscore);
				PlayHighScore(lastscore);
			}
			else
			{
				PlayScore(lastscore);
			}
			TransmitReading(&lastscore, lastscore == hiscore);
		}
		else if(reset)
		{
			highscore = 0;
			lastscore = 0;
		}
    }
}


FILE USART0_Stream = FDEV_SETUP_STREAM(USART0SendByte, NULL, _FDEV_SETUP_WRITE);

void Initialize(void)
{
	//////////////////////////////////////////////////////////////////////////
	//Initialize Outputs
	//////////////////////////////////////////////////////////////////////////
	//Set as outputs: PB1 - PB5 (LED 1 - 5)
	DDRB |= 0x3E;
	//Turn off: PB1 - PB5 (LED 1 - 5)
	PORTB &= 0xC0;

	//////////////////////////////////////////////////////////////////////////
	//Initialize Inputs
	//////////////////////////////////////////////////////////////////////////
	//Set as input: PC0 - PC3 (Sample Button, Reset Button, Override Jumper, MQ3 Input)
	DDRC &= ~0x0F;

	//////////////////////////////////////////////////////////////////////////
	//Set up ADC
	//////////////////////////////////////////////////////////////////////////
	//Disable other reference sources. Use AVCC (5V) reference source.
	ADMUX = (1 << REFS0);

	//Set clocking for ADC between 50 and 200kHz by prescalling the system clock (16MHz / 128)
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
	//Enable ADC module
	ADCSRA |= (1 << ADEN);
	
	//////////////////////////////////////////////////////////////////////////
	//Set up USART0
	//////////////////////////////////////////////////////////////////////////
	//Set BAUD rate
	UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
	UBRR0L = (uint8_t)UBRR_VALUE;
	
	//Set frame format to 8 data bits, no parity, 1 stop bit
	UCSR0C |= (1 << UCSZ01) | (1 << UCSZ00);
	
	//Enable transmission and reception
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0);
	//Set up standard output
	stdout = &USART0_Stream;


	//Transmit: Set up completed.
	printf("\xFF");
}

//////////////////////////////////////////////////////////////////////////
//>> SPLASH SCREEN
//Breathalyzer
//By Paradox Computing
void PlaySplashScreen(void)
{
	ssd1306_clear();
	//Writes Splash Screen
	ssd1306_string_font8x16xy(24, 0, "High Score");
	ssd1306_string_font8x16xy(16, 2, "Breathalyzer");
	ssd1306_setpos(0, 5);
	ssd1306_string_font6x8("By Paradox Computing");
	
	_delay_ms(SPLASHDELAY);
}

//////////////////////////////////////////////////////////////////////////
//>> WARM UP SCREEN
//Warming up...
//{0}s remaining
//
//Warm up overridden.
void PlayWarmUpScreen(void)
{
	char text[5];
	bool overridden = false;
	
	ssd1306_clear();
	ssd1306_string_font8x16xy(12, 0, "Warming up...");
	ssd1306_setpos(37, 3);
	ssd1306_string_font6x8("s Remaining.");
	
	for(long i = 0; i < WARMUPDELAY; i += 1000)
	{
		char key;
		if(ListenKeyPress(&key))
		{
			if((key & OVERRIDEKEYCODE) == OVERRIDEKEYCODE)
			{
				//Complete warm up delay.
				i = WARMUPDELAY;
				overridden = true;
			}
		}
		
		//Format text: "Warming up...\n{0:000}s remaining."
		//sprintf(text, "%03d", (int)((WARMUPDELAY - i) / 1000));
		intToStr((int)((WARMUPDELAY - i) / 1000), text, 3);
		
		
		ssd1306_setpos(19, 3);
		ssd1306_string_font6x8(text);
		_delay_ms(1000);
	}
	
	if(overridden)
	{
		ssd1306_clear();
		ssd1306_setpos(0, 0);
		ssd1306_string_font6x8("Warmup overridden.");
		_delay_ms(OVERRIDDENISPLAYDELAY);
	}
}

//////////////////////////////////////////////////////////////////////////
//>> RESET SCREEN
//Reset. Are you sure?
//Hold down to confirm.
//{0}s
//
//Memory reset.
bool PlayResetScreen(void)
{
	char remaining[5];
	bool reset = false;
	for(int i = 0; i < RESETDELAY; i += 1000)
	{
		//sprintf(remaining, "%02ds", (int)((RESETDELAY - i) / 1000));
		intToStr((int)((RESETDELAY - i) / 1000), remaining, 2);
		
		ssd1306_clear();
		ssd1306_setpos(0, 0);
		ssd1306_string_font6x8("Reset. Are you sure?");
		ssd1306_setpos(0, 1);
		ssd1306_string_font6x8("Hold down to confirm.");
		ssd1306_setpos(0, 2);
		ssd1306_string_font6x8(remaining);
		
		char key;
		if(ListenKeyPress(&key))
		{
			if((key & RESETKEYCODE) == RESETKEYCODE)
			{
				i = RESETDELAY;
				reset = true;
			}
		}
		
		_delay_ms(1000);
	}
	
	if(reset)
	{
		//Clear memory
		SaveStorage(0);
		
		ssd1306_clear();
		ssd1306_setpos(0, 0);
		ssd1306_string_font6x8("Memory reset.");
		_delay_ms(RESETDISPLAYDELAY);
		return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
//>> SAMPLING
//Sampling...
//  {0:0.000}
//{1}s remaining.
float PlaySampling(void)
{
	float sumWeightedBAC = 0;
	float sumWeights = 0;
	
	ssd1306_clear();
	ssd1306_string_font8x16xy(20, 0, "Sampling...");
	ssd1306_setpos(72, 3);
	ssd1306_string_font6x8("bac");
	ssd1306_setpos(34, 5);
	ssd1306_string_font6x8("s remaining.");
	
	
	for(int i = 0; i < SAMPLEDELAY; i += SAMPLERESDELAY)
	{
		float t = (float)i / 1000.0f;
		float bac = ReadBAC();
		if(bac < 0)
		{
			bac = 0;
		}
		float coef = 1.0f / M_PI * atan(t - SAMPLEINFLECTIONPOINT) + 1.0f / 2.0f;
		
		bac *= coef;
		sumWeightedBAC += bac;
		sumWeights += coef;
		
		float tempresult = sumWeightedBAC / sumWeights;
		
		
		char currSampleText[7];
		//sprintf(currSampleText, "%.3f", tempresult);
		ftoa(tempresult, currSampleText, 3);
		char remaining[5];
		//sprintf(remaining, "%02d", (SAMPLEDELAY - i) / 1000);
		intToStr((SAMPLEDELAY - i) / 1000, remaining, 2);
		
		
		int leds = (int)floor(((float)i / (float)SAMPLEDELAY) * 5.0) + 1;
		char disp = 0x00;
		for(int j = 0; j <= leds; j++)
		{
			disp |= (1 << j);
		}
		DisplayProgress(disp);
		
		
		ssd1306_string_font8x16xy(32, 2, currSampleText);
		ssd1306_setpos(22, 5);
		ssd1306_string_font6x8(remaining);
		
		
		_delay_ms(SAMPLERESDELAY);
	}
	
	DisplayProgress(0x00);
	
	float result = sumWeightedBAC / sumWeights;
	
	return result;
}

//////////////////////////////////////////////////////////////////////////
//>> SCORE
//Sample:
//  {0:0.000}bac
void PlayScore(float score)
{
	char text[7];
	//sprintf(text, "%.3f", score);
	ftoa(score, text, 3);
	
	ssd1306_clear();
	ssd1306_string_font8x16xy(36, 0, "Sample:");
	ssd1306_string_font8x16xy(35, 2, text);
	ssd1306_setpos(75, 3);
	ssd1306_string_font6x8("bac");
	
	_delay_ms(SCOREDELAY);
}

//////////////////////////////////////////////////////////////////////////
//>> HIGH SCORE (flashing)
//HIGH SCORE!!!
//	{0:0.000}bac
//Congrats! You're drunk!
void PlayHighScore(float hiscore)
{
	char text[7];
	//sprintf(text, "%.3f", hiscore);
	ftoa(hiscore, text, 3);
	
	ssd1306_clear();
	ssd1306_string_font8x16xy(12, 0, "HIGH SCORE!!!");
	ssd1306_setpos(75, 3);
	ssd1306_string_font6x8("bac");
	ssd1306_string_font8x16xy(35, 2, text);
	ssd1306_setpos(37, 6);
	ssd1306_string_font6x8("Congrats!");
	ssd1306_setpos(19, 7);
	ssd1306_string_font6x8("You're a drunk!");
	
	for(int i = 0; i < HIGHSCOREDELAY; i += HIGHSCORERESDELAY)
	{
		float prog = (float)i / (float)HIGHSCORERESDELAY;
		prog = prog / 5.0f * M_PI; //Progress in terms of sin
		prog = (sin(prog) + 1.0f) / 2.0f * 5.0f;
		prog = round(prog);
		
		char led = (1 << (int)prog);
		DisplayProgress(led);
		
		_delay_ms(HIGHSCORERESDELAY);
	}
	
	DisplayProgress(0x00);
}

void LoadStorage(float *hiscore)
{
	eeprom_read_block(hiscore, (float *)EEPROM_MEM_ADDR, 4);
	if(isnan(*hiscore))
	{
		*hiscore = 0.0f;
		SaveStorage(*hiscore);
	}
}
void SaveStorage(float hiscore)
{
	eeprom_write_block((float *)EEPROM_MEM_ADDR, &hiscore, 4);
}
float ReadBAC(void)
{
	double v = (double)ReadADC() * VREF;
	
	double ppm = 150.4351049 * pow(v, 5);
	ppm += -2244.75988 * pow(v, 4);
	ppm += 13308.5139 * pow(v, 3);
	ppm += -39136.08594 * pow(v, 2);
	ppm += 57082.6258 * v;
	ppm += -32982.05333;
	
	float bac = 0;
	if(ppm < MINPPM)
	{
		bac = 0;
	}
	else if(ppm > MAXPPM)
	{
		bac = -1;
	}
	else
	{
		bac = (float)(ppm / 2600.0f);
	}
	
	return bac;
}


void TransmitReading(float *reading, bool isHi)
{
	char initcode[1] = { 0xF0 };
	if(isHi)
	{
		initcode[0] |= 0x01;
	}
	
	printf(initcode);
	
	char transmission[sizeof(reading)];
	memcpy(transmission, &reading, sizeof(reading));
	printf(transmission);
	
	printf("\xFA");
}


bool SampleKeyDown = false;
bool ResetKeyDown = false;
bool OverrideKeyDown = false;
bool ListenKeyPress(char *keyCode)
{
	*keyCode = 0;

	if(PINC & (1 << PC0))
	{
		SampleKeyDown = true;
	}
	else if(SampleKeyDown && (PINC & (1 << PC0)) != (1 << PC0))
	{
		SampleKeyDown = false;
		*keyCode |= SAMPLEKEYCODE;
	}

	if(PINC & (1 << PC1))
	{
		ResetKeyDown = true;
		*keyCode |= RESETKEYCODE;
	}
	else if(ResetKeyDown && (PINC & (1 << PC1)) != (1 << PC1))
	{
		ResetKeyDown = false;
	}

	if(PINC & (1 << PC2))
	{
		OverrideKeyDown = true;
		*keyCode |= OVERRIDEKEYCODE;
	}
	else if(OverrideKeyDown && (PINC & (1 << PC2)) != (1 << PC2))
	{
		OverrideKeyDown = false;
	}

	return *keyCode != 0;
}
double ReadADC(void)
{
	//Select ADC channel with safety mask
	ADMUX = (1 << REFS0) | (3 & 0x0F);
	//Single conversion mode
	ADCSRA |= (1 << ADSC);

	//Wait until ADC conversion is complete
	while(ADCSRA & (1 << ADSC));
	//Convert to ratio
	return (double)ADCW / 1023.0;
}

int USART0SendByte(char data, FILE *stream)
{
	if(data == '\n')
	{
		USART0SendByte('\r', stream);
	}

	//Wait while previous byte is being sent.
	while(!(UCSR0A & (1 << UDRE0)));
	//Transmit data
	UDR0 = data;

	return 0;
}

void DisplayProgress(char selection)
{
	selection &= 0x3E; //Remove unnecessary noise
	PORTB = (PORTB & 0xC1) | selection;
}


void reverse(char *str, int len)
{
	int i=0, j=len-1, temp;
	while (i<j)
	{
		temp = str[i];
		str[i] = str[j];
		str[j] = temp;
		i++; j--;
	}
}
int intToStr(int x, char str[], int d)
{
	int i = 0;
	while (x)
	{
		str[i++] = (x%10) + '0';
		x = x/10;
	}
	
	// If number of digits required is more, then
	// add 0s at the beginning
	while (i < d)
	str[i++] = '0';
	
	reverse(str, i);
	str[i] = '\0';
	return i;
}
void ftoa(float n, char *res, int afterpoint)
{
	// Extract integer part
	int ipart = (int)n;
	
	// Extract floating part
	float fpart = n - (float)ipart;
	
	// convert integer part to string
	int i = intToStr(ipart, res, 0);
	
	if(i == 0)
	{
		i++;
		res[0] = '0';
	}
	// check for display option after point
	if (afterpoint != 0)
	{
		res[i] = '.';  // add dot
		
		// Get the value of fraction part upto given no.
		// of points after dot. The third parameter is needed
		// to handle cases like 233.007
		fpart = fpart * pow(10, afterpoint);
		
		intToStr((int)fpart, res + i + 1, afterpoint);
	}
}
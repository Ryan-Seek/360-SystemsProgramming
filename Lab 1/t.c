#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

int *FP;
typedef unsigned int u32;
char *ctable = "0123456789ABCDEF";
int  BASE = 10;

int main(int argc, char *argv[ ], char *env[ ])
{
      int a,b,c;
      myprintf("enter main\n");
  
      myprintf("&argc=%x argv=%x env=%x\n", &argc, argv, env);
      myprintf("&a=%8x &b=%8x &c=%8x\n", &a, &b, &c);


     myprintf("\n***PART 1.1***\n");
      
      char* myChar;

      for (int i = 1; i < argc; i++) //do we print the first char or the whole string?
      { 
          myChar = argv[i]; 
          myprintf("%s\t", myChar);//prints out each character
      }
  
      putchar('\n');

      a=1; b=2; c=3;
      A(a,b);

      myprintf("\n***PART 2 and 3***\n");
      myTest();

      myprintf("exit main\n");
}

int A(int x, int y)
{
      int d,e,f;
      printf("enter A\n");
      // PRINT ADDRESS OF d, e, f
      d=4; e=5; f=6;
      B(d,e);
      printf("exit A\n");
}

int B(int x, int y)
{
      int g,h,i;
      printf("enter B\n");
      // PRINT ADDRESS OF g,h,i
      g=7; h=8; i=9;
      C(g,h);
      printf("exit B\n");
}

int C(int x, int y)
{
  int u, v, w, i, *p;

  printf("enter C\n");
  // PRINT ADDRESS OF u,v,w,i,p;
  u=10; v=11; w=12; i=13;

  FP = (int *)getebp();

  printf("\n***Part 1.2***\n");
 // (2). Write C code to print the stack frame link list.

  p = FP;
  while(p != 0)
  {
    printf("FP -> %X ->\n", p);
    p = *p;
  }
  printf("FP -> %X\n", p);


  p = (int *)&p;
  printf("\n***Part 1.3***\n");
 // (3). Print the stack contents from p to the frame of main()
  //   YOU MAY JUST PRINT 128 entries of the stack contents.

  i = 0;
  while(i < 128)
  {
    printf("%d(FP) -> %X\n%d(FP) = %d\n\n", i, FP, i, *FP);

    FP++;
    i++;
  }

}


 

int rpu(u32 x)
{  
    char c;
    if (x){
       c = ctable[x % BASE];
       rpu(x / BASE);
       putchar(c);
    }
}

int printu(u32 x)
{
   (x==0)? putchar('0') : rpu(x);
   //putchar(' ');
}

void prints(char *s)
{
    char myChar;
    int i = 0;

    while(myChar != '\0')
    {
      myChar = s[i]; //takes each character in the string and prints it
      putchar(myChar);
      i++;
    }
}


int  printd(int x)
{
  if (x < 0)
  {
    putchar('-');//if it is negative it prints the negatie sign then strips it and sends it to print as unsigned
    printu(-1*x);
  }
  else
    printu(x);
}

int  printx(u32 x)
{
    char hexNum[50];
    int i =0;

    while(x !=0)//loop does the conversion and save it into an array
    {
      int num =0;
      num = x % 16;

      if(num < 10)
        hexNum[i] = num + 48;
      else
        hexNum[i] = num + 55;

      i++;

      x /= 16;
    }

    hexNum[i++] = 'x';//adds the appropriate 0x
    hexNum[i++] = '0';

    i--;
    while(i>=0)
    { 
      putchar(hexNum[i]);//prints each character out in the array 
      i--;
    }
}

int  printo(u32 x) 
{
    int OctNum [100];
    int i = 0;

    while(x != 0)//conversion loop
    {
      OctNum[i] = x % 8;
      x = x / 8;
      i++;
    }

    putchar('0');
    i--;
    while(i >= 0)
    {
      printu(OctNum[i]);//prints each char in the array 
      i--;
    }
}


void myprintf(char *fmt,...)
{
    va_list arg;
    va_start(arg, fmt);

    int i =0;
    unsigned int j =0;
    char *str;
    char trav;

    
    while(*fmt)
    {
         trav = *fmt;

         if( trav == '%')//if there is something that needs to be printed in a special way such as a data type
         {
              fmt++;
              switch(*fmt)//this is the switch statement for when a % is encountered, calls correct function
               {
            case 'c':
                i = va_arg(arg, chars);
                putchar(i);
                break;
            case 's':
                str = va_arg(arg, char *);
                prints(str);
                break;
            case 'u':
                j = va_arg(arg, unsigned int);
                printu(j);
                break;
            case 'd':
                i = va_arg(arg, int);
                printd(i);
                break;
            case 'o':
                j = va_arg(arg, unsigned int);
                printo(j);
                break;
            case 'x':
                j = va_arg(arg, unsigned int);
                printx(j);
                break;
            case '%':
                putchar(*fmt);
                break;
               }
         }

         else//none datatype just text, then it gets printed with putchar
          putchar(*fmt);

        va_end(arg); 
        fmt++;
    }
     
}

void myTest()
{
  myprintf("cha=%c string=%s      dec=%d hex=%x oct=%o neg=%d\n", 
        'A', "this is a test", 100,    100,   100,  -100);
}
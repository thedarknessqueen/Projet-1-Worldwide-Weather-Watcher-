volatile uint8_t aa asm("aa")= 1;
volatile uint8_t bb asm("bb")= 2;
volatile uint8_t rASM asm("rASM") = 0;

void setup() {
  Serial.begin(9600);
  long start = micros();
  int compt = 0;
  int compt2 = 0;
  int limit = 11;
  
  
  
  
  while(compt<limit){
    fiboASM();
    compt++;
  }
  
  long endASM = micros();
  
  while(compt2<limit){
    fibo();
    compt2++;
  }

  long endN = micros();
  Serial.print("ASM time : ");  Serial.println(endASM - start);
  Serial.print("C time : ");    Serial.println(endN - endASM);
}


long fiboASM(){
  
  asm
  (
    "lds [0x12], 1        \n"
    "lds [0x10], (aa)     \n"
    "lds [0x11], (bb)     \n"
    "add [0x12], [0x10]   \n"
    "add [0x12], [0x11]   \n"
    "sts (aa), [0x11]     \n"
    "sts (bb), [0x12]     \n"
    "sts (rASM), [0x12]   \n"
    );
    
    Serial.println(rASM); 
}

long a = 1;
long b = 2;
long fibo(){
  long r = a + b;
  a = b;
  b = r;
  Serial.println(r);
  }

void loop(){
  
}